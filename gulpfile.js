const { series, parallel, watch, dest, src } = require('gulp');
const { generateQrc } = require('./gulpUtils');
const ts = require('gulp-typescript');
const rename = require('gulp-rename');
const replace = require('gulp-just-replace');
const execa = require('gulp-execa');
const fs = require('fs');

let process;

const dirs = {
  tsconfig: 'tsconfig.json',
  CMakeLists: 'CMakeLists.txt',
  packages: 'packages/',
  muelle: 'packages/muelle',
  shared: 'packages/shared',
  shell: 'packages/shell',
  dist: 'packages/dist',
  buildDev: 'build',
  resources: {
    shell: {
      watch: [
        'packages/shell/**/*.qml',
        'packages/shell/**/*.js',
        'packages/shared/**/*.ts',
      ],
      qrc: 'packages/dist/shell/qml.qrc',
      rcc: 'packages/dist/shell/qml.rcc',
    },
    muelle: {
      watch: [
        'CMakeLists.txt',
        'packages/muelle/**/*.cpp',
        'packages/muelle/**/*.hpp',
      ],
    },
  },
};

async function cleanDist() {
  try {
    await execa.exec(`rm -rf ${dirs.dist}`);
  } catch {}
}
async function runCMake() {
  await execa.exec(`cmake -Wno-dev -G Ninja -S . -B ${dirs.buildDev}`);
}
async function runMake() {
  await execa.exec(`ninja -C ${dirs.buildDev}`);
}
async function runMuelle() {
  await runMake();
  if (process) process.cancel();
  process = await execa.exec(`${dirs.buildDev}/muelle`);
}
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
                search: /import (.*) from '(.*)'/g,
                replacement: "import $1 from '$2.mjs'",
              },
              {
                search: /\/\/\s*@pragma-library/g,
                replacement: '.pragma library',
              },
            ])
          )
          .pipe(dest(dirs.dist));
      },
      function copyShellQmlResources() {
        return src(['packages/**/*.qml'])
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
      const qrc = generateQrc(dirs.shell, [dirs.shell, dirs.shared]);
      fs.writeFile(dirs.resources.shell.qrc, qrc, { flag: 'w' }, cb);
    },
    async function createShellRcc() {
      await execa.exec(
        `rcc-qt5 --no-compress --binary -o ${dirs.resources.shell.rcc} ${dirs.resources.shell.qrc}`
      );
    }
  )(cb);
}
async function watchApp() {
  const silent = (...tasks) => {
    try {
      return series(...tasks)();
    } catch (e) {
      console.log(e);
    } finally {
    }
  };

  async function muelle() {
    silent(runCMake, runMuelle);
  }

  async function qml() {
    silent(cleanDist, buildQml);
  }

  await muelle();
  await qml();

  watch(dirs.resources.shell.watch, { delay: 500 }, qml);
  watch(dirs.resources.muelle.watch, { delay: 500 }, muelle);
}

function watchTask() {
  return series(watchApp);
}

function buildTask() {
  return parallel(series(runCMake, runMake), series(cleanDist, buildQml));
}

exports.watch = watchTask();
exports.build = buildTask();
