const fs = require('fs');
const path = require('path');
const { exec } = require('child_process');

function noop() {}

function list_files(pathname = `${__dirname}/packages`, files = []) {
  return fs
    .readdirSync(pathname, { withFileTypes: true })
    .reduce((files, file) => {
      if (file.isDirectory()) {
        files.concat(list_files(`${pathname}/${file.name}`, files));
      } else {
        files.push(`${pathname}/${file.name}`);
      }
      return files;
    }, files);
}

function qrc(_, files) {
  return `<!DOCTYPE RCC>
<RCC>
  <qresource prefix="/">
${files.map(file => `    <file>${file}</file>`).join('\n')}
  </qresource>
</RCC>`;
}

function generate_qrc(from, paths = [], patttern = /.qml|.js|.mjs/) {
  return qrc`${paths
    .flatMap(pathname => list_files(`${__dirname}/${pathname}`))
    .filter(file => {
      const { ext, name } = path.parse(file);
      return (patttern.test(ext) && /\w/.test(name)) || /qmldir/.test(name);
    })
    .map(to => path.relative(from, to))}`;
}

function create_command(command) {
  return {
    [command](cb) {
      exec(
        command,
        { cwd: __dirname, stdio: 'inherit', shell: true },
        (error, stdout) => {
          if (stdout) {
            console.log(`${stdout}`);
          }
          error ? cb(error) : cb();
        }
      );
    },
  }[command];
}

const clear = create_command(`clear`);

function time(label) {
  return function timer(cb) {
    console.time(`           ${label}`);
    cb();
  };
}

function time_end(label) {
  return function timer(cb) {
    console.timeEnd(`           ${label}`);
    cb();
  };
}

exports.generate_qrc = generate_qrc;
exports.list_files = list_files;
exports.create_command = create_command;
exports.clear = clear;
exports.noop = noop;
exports.time = time;
exports.time_end = time_end;
