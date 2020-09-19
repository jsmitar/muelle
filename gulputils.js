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

function create_command(command, name) {
  return named_function(name || command, (cb = noop) => {
    exec(
      command,
      { cwd: __dirname, stdio: 'inherit', shell: true },
      (error, stdout) => {
        process.stdout.write(stdout);
        if (error) {
          setImmediate(() => cb(error));
        } else {
          cb();
        }
      }
    ).on('message', message => {
      process.stdout.write(message);
    });
  });
}
const clear = create_command('clear');

/**
 *
 * @type {<Fn = (...args: any[]) => any>(fn: Fn) => Fn}
 */
function clear_before(fn) {
  return named_function(fn.name, cb => {
    clear(error => (error ? cb(error) : fn(cb)));
  });
}

function time(label) {
  return function time(cb) {
    console.time(`           ${label}`);
    setImmediate(cb);
  };
}

function time_end(label) {
  return function time_end(cb) {
    console.timeEnd(`           ${label}`);
    setImmediate(cb);
  };
}

function pass(cb) {
  cb();
}

/**
 * @type {(predicate: () => boolean)
 *  => (then: import('undertaker').Task, otherwise?: import('undertaker').Task)
 *  => import('undertaker').TaskFunction }
 */
const if_ = predicate => (then, otherwise = pass) => {
  const name =
    otherwise === pass
      ? `${then.displayName || then.name} if ${predicate.name}`
      : `${then.displayName || then.name} if ${predicate.name} else ${
          otherwise.displayName || otherwise.name
        }`;

  return named_function(
    name,
    cb => void (predicate() ? then(cb) : otherwise(cb))
  );
};

/**
 * @type {(predicate: () => boolean) => () => boolean
 */
const not_ = predicate => {
  return named_function(`not ${predicate.name}`, () => !predicate());
};

/**
 * @type {<Fn = (...args: any[]) => any>(name: string, fn: Fn) => Fn
 */
function named_function(name, fn) {
  return {
    [name](...args) {
      return fn(...args);
    },
  }[name];
}

/**
 * @type {<T = { displayName: string }>(name: string, obj: T) => T}
 */
function set_displayName(name, obj) {
  return (obj.displayName = name), obj;
}

exports.generate_qrc = generate_qrc;
exports.list_files = list_files;
exports.create_command = create_command;
exports.clear_before = clear_before;
exports.noop = noop;
exports.time = time;
exports.time_end = time_end;
exports.if_ = if_;
exports.not_ = not_;
exports.set_displayName = set_displayName;
