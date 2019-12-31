const { series, parallel, watch, dest, src } = require('gulp');
const gulpUtils = require('./gulpUtils');
const ts = require('gulp-typescript');
const rename = require('gulp-rename');
const replace = require('gulp-just-replace');
const execa = require('gulp-execa');
const fs = require('fs');

const dirs = {
  tsconfig: 'tsconfig.json',
  CMakeLists: 'CMakeLists.txt',
  packages: 'packages/',
  muelle: 'packages/muelle',
  shared: 'packages/shared',
  shell: 'packages/shell',
  dist: 'packages/dist',
  buildDev: 'build-dev',
  resources: {
    shell: [
      'packages/dist/shell/**/*.ts',
      'packages/dist/shell/**/*.qml',
      'packages/dist/shared/**/*.ts',
      'packages/dist/shared/**/*.qml',
    ],
  },
};

const tasks = {
  cleanDist(cb) {
    fs.rmdir(dirs.dist, cb);
  },
  async runCMake() {
    await execa.exec(`cmake -S . -B ${dirs.buildDev}`);
  },
  async runMake() {
    await execa.exec(`make -C ${dirs.buildDev}`);
  },
  async runMuelle() {
    await tasks.runMake();
    if (tasks.process) tasks.process.cancel();
    tasks.process = await execa.exec(`${dirs.buildDev}/muelle`);
  },
  buildQml() {
    const tsProject = ts.createProject(dirs.tsconfig);

    return parallel(
      () =>
        tsProject
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
            ])
          )
          .pipe(dest(dirs.dist)),
      () => {
        src(`${dirs.shell}/**/*.qml`).pipe(dest(dirs.dist));
      }
    );
  },
  async watchApp() {
    const silent = (...tasks) => {
      try {
        return series(...tasks)();
      } finally {
      }
    };

    async function cmake() {
      silent(tasks.runCMake, tasks.runMuelle);
    }

    async function muelle() {
      silent(tasks.runMuelle);
    }

    async function shell() {
      silent(tasks.cleanDist, tasks.buildQml);
    }

    await cmake();

    watch([dirs.CMakeLists], cmake);
    watch([`${dirs.muelle}/*`], muelle);
    watch(dirs.resources.shell, shell);
  },
};

function watchTask() {
  return series(tasks.watchApp);
}
function buildTask() {
  return series(tasks.runCMake, tasks.cleanDist, tasks.buildQml);
}

exports.watch = watchTask.call(tasks);
exports.build = buildTask.call(tasks);
