const { series, watch, dest } = require('gulp');
const ts = require('gulp-typescript');
const rename = require('gulp-rename');
const replace = require('gulp-just-replace');
const del = require('del');

function watchFiles() {
  return watch(
    ['src/*', 'tsconfig.json', 'package.json'],
    series(clean, build)
  );
}

function clean() {
  return del('dist/*');
}

function build() {
  const tsProject = ts.createProject('tsconfig.json');
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
      ])
    )
    .pipe(dest('dist'));
}

exports.watch = series(clean, build, watchFiles);

exports.default = series(clean, build);
