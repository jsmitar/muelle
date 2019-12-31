const fs = require('fs');
const path = require('path');

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
  return `
<!DOCTYPE RCC><RCC>
  <qresource prefix="/">
    ${files.map(file => `<file>${file}</file>`).join('\n    ')}
  </qresource>
</RCC>
  `;
}

function generateQrc(from, paths = []) {
  return qrc`${paths
    .flatMap(pathname => listFiles(pathname))
    .filter(file => {
      const { ext, name } = path.parse(file);
      return /.qml|.js/.test(ext) && /\w/.test(name);
    })
    .map(to => path.relative(from, to))}`;
}

exports.generateQrc = generateQrc;
exports.listFiles = listFiles;

// console.log(
//   generateQrc(`${__dirname}/packages/shell`, [
//     `${__dirname}/packages/shell`,
//     `${__dirname}/packages/shared`,
//   ])
// );
// console.timeEnd('qrc');
