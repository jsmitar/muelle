const { series, parallel, watch, dest, src } = require('gulp');
const ts = require('gulp-typescript');
const rename = require('gulp-rename');
const replace = require('gulp-just-replace');
const { generateQrc, createCommand, clear } = require('./gulpUtils');

const fs = require('fs');
const { spawn } = require('child_process');

const dirs = {
  tsconfig: 'tsconfig.json',
  CMakeLists: 'CMakeLists.txt',
  packages: 'packages/',
  muelle: 'packages/muelle',
  shared: 'packages/shared',
  shell: 'packages/shell',
  dist: 'packages/dist',
  buildDev: 'build',
  bin: 'build/muelle',
  resources: {
    shell: {
      watch: [`packages/shell/**/*.(qml|ts)`, 'packages/shared/**/*.ts'],
      qrc: {
        from: 'packages/dist/shell/',
        input: ['packages/dist/shell/', 'packages/dist/shared'],
        output: 'packages/dist/shell/qml.qrc',
        rcc: 'packages/resources/shell.rcc',
      },
    },
    muelle: {
      watch: ['CMakeLists.txt', 'packages/muelle/**/*.(hpp|cpp)'],
    },
  },
};

function cleanDist(cb) {
  createCommand(`find ${dirs.dist} -type f -not -name 'qml.rcc' -delete`)(cb);
}

const runCMake = createCommand(
  `cmake -Wno-dev -G Ninja -S . -B ${dirs.buildDev}`
);
const runMake = createCommand(`ninja -C ${dirs.buildDev}`);

const runMuelle = (() => {
  let muelle;
  const log = fs.createWriteStream('out.log');

  return function(restart, cb) {
    if (restart && muelle) {
      muelle.on('exit', () => runMuelle());
      muelle.kill('SIGTERM');
    }

    if (muelle == null) {
      log.write(`
╓───────────────────────────────────╖
║          STARTING MUELLE          ║
╙───────────────────────────────────╜
`);
      muelle = spawn(`${dirs.bin}`, { cwd: __dirname });
      muelle.unref();
      muelle
        .on('exit', () => (muelle = null))
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
                search: /import (.*) from '(.*)'/g,
                replacement: "import $1 from '$2.mjs'",
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
      const qrc = generateQrc(
        dirs.resources.shell.qrc.from,
        dirs.resources.shell.qrc.input
      );
      fs.writeFile(dirs.resources.shell.qrc.output, qrc, { flag: 'w' }, cb);
    },
    function createShellRcc(cb) {
      createCommand(
        `rcc-qt5 --no-compress --binary -o ${dirs.resources.shell.qrc.rcc} ${dirs.resources.shell.qrc.output}`
      )(cb);
    }
  )(cb);
}
async function watchApp() {
  function muelle(cb) {
    return series(clear, runCMake, runMake, cb => runMuelle(true, cb))(cb);
  }

  function qml(cb) {
    return series(cleanDist, buildQml, cb => runMuelle(false, cb))(cb);
  }

  muelle(() => {
    qml();
  });

  watch(dirs.resources.muelle.watch, muelle);
  watch(dirs.resources.shell.watch, qml);
}

function watchTask() {
  return series(watchApp);
}

function buildTask() {
  return parallel(series(runCMake, runMake), series(cleanDist, buildQml));
}

exports.watch = watchTask();
exports.build = buildTask();
