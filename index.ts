import { existsSync } from 'node:fs';
import { createRequire } from 'node:module';

if (process.platform !== 'win32') {
  throw new Error(
    `This operating system (${process.platform}) is not supported. It must be Windows.`
  );
}

const require: NodeRequire = createRequire(import.meta.url);

const platforms: Record<string, string> = {
  x64: 'win32-x64-msvc',
  ia32: 'win32-ia32-msvc',
  arm64: 'win32-arm64-msvc'
};

if (platforms[process.arch] == null) {
  throw new Error(`This architecture (${process.arch}) is not supported.`);
}

export default require(existsSync(`./js-macro.${platforms[process.arch]}.node`)
  ? `./js-macro.${platforms[process.arch]}.node`
  : `@vierofernando/js-macro-${platforms[process.arch]}`);
