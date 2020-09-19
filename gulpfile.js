const { series, parallel, watch, dest, src } = require('gulp');
const ts = require('gulp-typescript');
const rename = require('gulp-rename');
const replace = require('gulp-just-replace');
const {
  generate_qrc,
  create_command,
  noop,
  time_end,
  time,
  if_,
  not_,
  set_displayName,
  clear_before,
} = require('./gulputils');

const fs = require('fs');
const { spawn } = require('child_process');

process.env.NODE_ENV = process.env.NODE_ENV || 'production';
const development = process.env.NODE_ENV === 'development';

const dirs = {
  tsconfig: 'tsconfig.json',
  CMakeLists: 'CMakeLists.txt',
  packages: 'packages/',
  muelle: 'packages/muelle',
  shared: 'packages/shared',
  shell: 'packages/shell',
  dist: 'packages/dist',
  build: development ? 'build-dev' : 'build',
  env: {
    ...process.env,
    QT_QUICK_CONTROLS_STYLE: 'org.kde.desktop',
  },
  bin: development ? 'build-dev/muelle' : 'build/muelle',
  bin_debug_args: [
    '-qmljsdebugger=port:5000,services:CanvasFrameRate,EngineControl,DebugMessages',
  ],
  resources: {
    watch_qml: [
      'packages/shell/**/*.(qml|ts)',
      'packages/shared/**/*.(qml|ts)',
    ],
    shell: {
      qrc: {
        from: 'packages/dist',
        input: ['packages/dist/shell', 'packages/dist/shared'],
        qrc: 'packages/dist/shell.qrc',
        rcc: 'packages/resources/shell.rcc',
      },
      qrc_qtcreator: {
        from: 'packages',
        input: ['packages/shell', 'packages/shared'],
        qrc: 'packages/shell.qrc',
      },
    },
    muelle: {
      datadir: `${process.cwd()}/packages/resources`,
      watch: [
        'CMakeLists.txt',
        'packages/muelle/**/*.(hpp|cpp)',
        '!packages/muelle/dockconfig.hpp',
      ],
    },
  },
};

const clean_dist = create_command(
  `if [ -f '${dirs.dist}' ]; then find ${dirs.dist} -type f -not -name 'qml.rcc' -delete; fi`
);

const mkdir_build = create_command(`mkdir -p ${dirs.build}`);

const kill_muelle = create_command(
  'killall -s KILL muelle &> /dev/null; exit 0',
  'killall muelle'
);

const run_CMake = create_command(
  [
    'cmake',
    '-Wno-dev',
    '-G Ninja',
    '-S .',
    `-B ${dirs.build}`,
    development ? `-DMUELLE_DATADIR=${dirs.resources.muelle.datadir}` : '',
  ].join(' '),
  `cmake -B ${dirs.build}`
);

const run_Ninja = create_command(`ninja -C ${dirs.build}`);

const run_Ninja_install = create_command(`ninja install -C ${dirs.build}`);

function run_muelle(cb) {
  cb();
  const { stdout } = process;
  stdout.write(`${'\u001b[1;35m'}
╓───────────────────────────────────╖
║          STARTING MUELLE          ║
╙───────────────────────────────────╜
${'\u001b[0m'}`);
  const muelle = spawn(
    `${dirs.bin}`,
    dirs.bin_debug_args,
    {
      detached: true,
      cwd: __dirname,
      env: dirs.env,
      stdio: Array(3).fill(stdout),
    },
    error => {
      if (error) cb(error);
      else cb();
    }
  );
  muelle.unref();
  // muelle.on('close', (_, signal) => {
  //   stdout.write(`\n[END] Signal: ${signal}\n\n`);
  // });
}

const tsProject = ts.createProject(dirs.tsconfig);

function transpile_ts() {
  return tsProject
    .src()
    .pipe(tsProject())
    .js.pipe(rename(path => void (path.extname = '.mjs')))
    .pipe(
      replace([
        {
          search: /import (.*) from 'qt';?/g,
          replacement: '',
        },
        {
          search: /import (.*) from '@qml\/(.*)-(.*)'/g,
          replacement: '',
        },
        {
          search: /'\.import (.*)';/g,
          replacement: '.import $1',
        },
        {
          search: /import (.*) from '(.*)'/g,
          replacement: "import $1 from '$2.mjs'",
        },
        {
          search: /export (.*) from '(.*)'/g,
          replacement: "export $1 from '$2.mjs'",
        },
        {
          search: /'.pragma-library'/g,
          replacement: '.pragma library',
        },
      ])
    )
    .pipe(dest(dirs.dist));
}

function copy_shell_qml_resources() {
  return src(
    ['packages/**/*.qml', 'packages/**/*.js', '!packages/dist/**/*.*'],
    { buffer: true }
  )
    .pipe(
      replace([
        {
          search: /import '(.*).ts' as (.*)/g,
          replacement: "import '$1.mjs' as $2",
        },
      ])
    )
    .pipe(dest(dirs.dist));
}

function create_shell_qrc(cb) {
  [dirs.resources.shell].forEach(resource => {
    const qrc = generate_qrc(resource.qrc.from, resource.qrc.input);
    fs.writeFileSync(resource.qrc.qrc, qrc, { flag: 'w' }, noop);
  });
  cb();
}

function create_shell_qtcreator_qrc(cb) {
  [dirs.resources.shell].forEach(resource => {
    const qrc = generate_qrc(
      resource.qrc_qtcreator.from,
      resource.qrc_qtcreator.input,
      /.qml|.js|.ts/
    );
    fs.writeFile(resource.qrc_qtcreator.qrc, qrc, { flag: 'w' }, noop);
  });
  cb();
}

function create_shell_rcc(cb) {
  [dirs.resources.shell].forEach(resource => {
    create_command(
      `rcc-qt5 --no-compress --binary -o ${resource.qrc.rcc} ${resource.qrc.qrc}`
    )(cb);
  });
}

const target_muelle = set_displayName(
  'target:muelle',
  series(run_CMake, run_Ninja)
);

const target_qml = set_displayName(
  'target:qml',
  series(
    parallel(
      transpile_ts,
      series(
        copy_shell_qml_resources,
        parallel(create_shell_qrc, create_shell_qtcreator_qrc)
      )
    ),
    create_shell_rcc
  )
);

function _watch() {
  /**
   * @type {import('gulp').WatchOptions}
   */
  const options = {
    ignoreInitial: false,
    delay: 250,
    useFsEvents: true,
    queue: false,
  };

  function first_time() {
    let first = true;
    return function first_time() {
      return first ? ((first = false), true) : false;
    };
  }

  return parallel(
    function cmake_project_watcher() {
      watch(
        dirs.resources.muelle.watch,
        options,
        series(
          if_(first_time())(
            set_displayName(
              'build:all',
              series(
                parallel(kill_muelle, clean_dist, mkdir_build),
                parallel(target_muelle, target_qml),
                clear_before(run_muelle)
              )
            ),
            set_displayName(
              'build:muelle',
              series(
                parallel(kill_muelle, target_muelle),
                clear_before(run_muelle)
              )
            )
          )
        )
      );
    },
    function qml_project_watcher() {
      watch(
        dirs.resources.watch_qml,
        options,
        series(
          if_(not_(first_time()))(
            set_displayName(
              'build:qml',
              series(
                parallel(clean_dist, kill_muelle),
                target_qml,
                clear_before(run_muelle)
              )
            )
          )
        )
      );
    }
  );
}

function _build() {
  return parallel(
    parallel(mkdir_build, clean_dist),
    series(run_CMake, run_Ninja),
    parallel(target_muelle, target_qml)
  );
}

function _install() {
  return series(run_Ninja_install);
}

exports.watch = _watch();
exports.build = _build();
exports.install = _install();
