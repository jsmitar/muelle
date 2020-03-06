const fs = require('fs');
const path = require('path');
const { exec } = require('child_process');

function listFiles(pathname = `${__dirname}/packages`, files = []) {
  return fs
    .readdirSync(pathname, { withFileTypes: true })
    .reduce((files, file) => {
      if (file.isDirectory()) {
        files.concat(listFiles(`${pathname}/${file.name}`, files));
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

function generateQrc(from, paths = [], patttern = /.qml|.js|.mjs/) {
  return qrc`${paths
    .flatMap(pathname => listFiles(`${__dirname}/${pathname}`))
    .filter(file => {
      const { ext, name } = path.parse(file);
      return (patttern.test(ext) && /\w/.test(name)) || /qmldir/.test(name);
    })
    .map(to => path.relative(from, to))}`;
}

function createCommand(command) {
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

const clear = createCommand(`clear`);

exports.generateQrc = generateQrc;
exports.listFiles = listFiles;
exports.createCommand = createCommand;
exports.clear = clear;
