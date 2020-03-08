const { series, parallel, watch, dest, src } = require('gulp');
const ts = require('gulp-typescript');
const rename = require('gulp-rename');
const replace = require('gulp-just-replace');
const { generateQrc, createCommand, clear } = require('./gulputils');

const fs = require('fs');
const { spawn } = require('child_process');

const development = process.env.NODE_ENV === 'development';

const dirs = {
  tsconfig: 'tsconfig.json',
  CMakeLists: 'CMakeLists.txt',
  packages: 'packages/',
  muelle: 'packages/muelle',
  shared: 'packages/shared',
  shell: 'packages/shell',
  dist: 'packages/dist',
  build: development ? 'build' : 'build-release',
  bin: 'build/muelle',
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
      watch: [
        'CMakeLists.txt',
        'packages/muelle/**/*.(hpp|cpp)',
        '!packages/muelle/dockconfig.hpp',
      ],
    },
  },
};

function cleanDist(cb) {
  createCommand(`find ${dirs.dist} -type f -not -name 'qml.rcc' -delete`)(cb);
}

const mkdirBuild = createCommand(`mkdir -p ${dirs.build}`);

const runCMake = createCommand(
  [
    'cmake',
    '-Wno-dev',
    '-G Ninja',
    '-S .',
    `-B ${dirs.build}`,
    development ? `-DSHELL_RCC=${dirs.resources.shell.qrc.rcc}` : '',
  ].join(' ')
);

const runNinja = createCommand(`ninja -C ${dirs.build}`);

const runNinjaInstall = createCommand(`ninja install -C ${dirs.build}`);

const runMuelle = (() => {
  let muelle;
  const log = fs.createWriteStream('out.log');

  return function muelleApp(restart, cb) {
    if (restart && muelle) {
      muelle.on('close', () => ((muelle = null), muelleApp(false, cb)));
      muelle.kill('SIGTERM');
      return;
    }

    if (muelle == null) {
      log.write(`${'\u001b[1;35m'}
╓───────────────────────────────────╖
║          STARTING MUELLE          ║
╙───────────────────────────────────╜
${'\u001b[0m'}`);
      muelle = spawn(`${dirs.bin}`, dirs.binArgs, {
        cwd: __dirname,
        env: { ...process.env, QML_IMPORT_TRACE: 0 },
      });
      muelle.unref();
      muelle
        .on('exit', () => {
          muelle = null;
        })
        .on('close', (_, signal) => log.write(`\n[END] Signal: ${signal}\n\n`));

      muelle.stdout.pipe(log, { end: false });
      muelle.stderr.pipe(log, { end: false });
    }
    if (cb) cb();
  };
})();

function buildQml(cb) {
  const tsProject = ts.createProject(dirs.tsconfig);

  return series(
    parallel(
      function transpileTs() {
        return tsProject
          .src()
          .pipe(tsProject())
          .js.pipe(
            rename(path => {
              path.extname = '.mjs';
            })
          )
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
      function copyShellQmlResources() {
        return src(['packages/**/*.qml', 'packages/**/*.js'])
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
    function createShellQrc(cb) {
      [dirs.resources.shell].forEach(resource => {
        const qrc = generateQrc(resource.qrc.from, resource.qrc.input);
        fs.writeFileSync(resource.qrc.output, qrc, { flag: 'w' }, () => {});
      });
      cb();
    },
    function createShellQrcQtCreator(cb) {
      [dirs.resources.shell].forEach(resource => {
        const qrc = generateQrc(
          resource.qrcQtCreator.from,
          resource.qrcQtCreator.input,
          /.qml|.js|.ts/
        );
        fs.writeFile(
          resource.qrcQtCreator.output,
          qrc,
          { flag: 'w' },
          () => {}
        );
      });
      cb();
    },
    function createShellRcc(cb) {
      [dirs.resources.shell].forEach(resource => {
        createCommand(
          `rcc-qt5 --no-compress --binary -o ${resource.qrc.rcc} ${resource.qrc.output}`
        )(cb);
      });
    }
  )(cb);
}

function watchTask() {
  return series(() =>
    watch(
      [...dirs.resources.muelle.watch, ...dirs.resources.watchQmlPackages],
      { ignoreInitial: false, delay: 500, useFsEvents: true },
      series(clear, buildTask(), function START(cb) {
        runMuelle(true, cb);
      })
    )
  );
}

function buildTask() {
  return parallel(
    series(mkdirBuild, runCMake, runNinja),
    series(cleanDist, buildQml)
  );
}

function install() {
  return series(runNinjaInstall);
}

exports.watch = watchTask();
exports.build = buildTask();
exports.install = install();
