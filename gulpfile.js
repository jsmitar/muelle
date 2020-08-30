const { series, parallel, watch, dest, src } = require('gulp');
const ts = require('gulp-typescript');
const rename = require('gulp-rename');
const replace = require('gulp-just-replace');
const {
  generate_qrc,
  create_command,
  clear,
  noop,
  time_end,
  time,
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
  binArgs: [
    '-qmljsdebugger=port:5000,services:CanvasFrameRate,EngineControl,DebugMessages',
  ],
  resources: {
    watchQmlPackages: [
      'packages/shell/**/*.(qml|ts)',
      'packages/shared/**/*.(qml|ts)',
    ],
    shell: {
      qrc: {
        from: 'packages/dist',
        input: ['packages/dist/shell', 'packages/dist/shared'],
        output: 'packages/dist/shell.qrc',
        rcc: 'packages/resources/shell.rcc',
      },
      qrcQtCreator: {
        from: 'packages',
        input: ['packages/shell', 'packages/shared'],
        output: 'packages/shell.qrc',
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

const clean_dist = cb =>
  create_command(
    `if [ -f '${dirs.dist}' ]; then find ${dirs.dist} -type f -not -name 'qml.rcc' -delete; fi`
  )(cb);

const mkdir_build = create_command(`mkdir -p ${dirs.build}`);

const kill_muelle = create_command(
  'killall -s KILL muelle &> /dev/null; exit 0'
);

const run_CMake = create_command(
  [
    'cmake',
    '-Wno-dev',
    '-G Ninja',
    '-S .',
    `-B ${dirs.build}`,
    development ? `-DMUELLE_DATADIR=${dirs.resources.muelle.datadir}` : '',
  ].join(' ')
);

const run_Ninja = create_command(`ninja -C ${dirs.build}`);

const run_Ninja_install = create_command(`ninja install -C ${dirs.build}`);

const run_muelle = (() => {
  const log = fs.createWriteStream('out.log');

  return function muelle(cb) {
    log.write(`${'\u001b[1;35m'}
╓───────────────────────────────────╖
║          STARTING MUELLE          ║
╙───────────────────────────────────╜
${'\u001b[0m'}`);
    const muelle = spawn(`${dirs.bin}`, dirs.binArgs, {
      detached: true,
      cwd: __dirname,
      env: dirs.env,
    });
    muelle.unref();
    muelle.stdout.pipe(log, { end: false });
    muelle.stderr.pipe(log, { end: false });
    muelle.on('close', (_, signal) =>
      log.write(`\n[END] Signal: ${signal}\n\n`)
    );
    cb();
  };
})();

const tsProject = ts.createProject(dirs.tsconfig);

function build_qml(cb) {
  return series(
    parallel(
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
      },
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
    ),
    parallel(
      function create_shell_qrc(cb) {
        [dirs.resources.shell].forEach(resource => {
          const qrc = generate_qrc(resource.qrc.from, resource.qrc.input);
          fs.writeFileSync(resource.qrc.output, qrc, { flag: 'w' }, noop);
        });
        cb();
      },
      function create_shell_qtcreator_qrc(cb) {
        [dirs.resources.shell].forEach(resource => {
          const qrc = generate_qrc(
            resource.qrcQtCreator.from,
            resource.qrcQtCreator.input,
            /.qml|.js|.ts/
          );
          fs.writeFile(resource.qrcQtCreator.output, qrc, { flag: 'w' }, noop);
        });
        cb();
      }
    ),
    function create_shell_rcc(cb) {
      [dirs.resources.shell].forEach(resource => {
        create_command(
          `rcc-qt5 --no-compress --binary -o ${resource.qrc.rcc} ${resource.qrc.output}`
        )(cb);
      });
    }
  )(cb);
}

function _watch() {
  /**
   * @type {import('gulp').WatchOptions}
   */
  const options = {
    ignoreInitial: false,
    delay: 500,
    useFsEvents: true,
    queue: false,
  };
  return series(
    mkdir_build,
    clean_dist,
    parallel(
      function cmake_project_watcher() {
        watch(
          dirs.resources.muelle.watch,
          options,
          series(
            clear,
            time('cmake_project_watcher'),
            parallel(kill_muelle, series(run_CMake, run_Ninja)),
            run_muelle,
            time_end('cmake_project_watcher')
          )
        );
      },
      function qml_project_watcher() {
        watch(
          dirs.resources.watchQmlPackages,
          options,
          series(
            clear,
            time('qml_project_watcher'),
            parallel(kill_muelle, build_qml),
            run_muelle,
            time_end('qml_project_watcher')
          )
        );
      }
    )
  );
}

function _build() {
  return parallel(
    series(mkdir_build, run_CMake, run_Ninja),
    series(clean_dist, build_qml)
  );
}

function _install() {
  return series(run_Ninja_install);
}

exports.watch = _watch();
exports.build = _build();
exports.install = _install();
