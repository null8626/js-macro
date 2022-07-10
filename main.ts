const { existsSync } = require('node:fs');
const repl = require('node:repl');

if (process.platform !== 'win32') {
  throw new Error(
    `This operating system (${process.platform}) is not supported. It must be Windows.`
  );
}

const platforms: Record<string, string> = {
  x64: 'win32-x64-msvc',
  ia32: 'win32-ia32-msvc',
  arm64: 'win32-arm64-msvc'
};

if (platforms[process.arch] == null) {
  throw new Error(`This architecture (${process.arch}) is not supported.`);
}

const mod: any = require(existsSync(`./js-macro.${platforms[process.arch]}.node`)
  ? `./js-macro.${platforms[process.arch]}.node`
  : `@vierofernando/js-macro-${platforms[process.arch]}`);

mod.init();

const cleanup: () => void = mod.cleanup;

process.on('exit', cleanup);

if (repl) {
  repl.repl.on('exit', cleanup);
}

delete mod.init;
delete mod.cleanup;

module.exports = mod;