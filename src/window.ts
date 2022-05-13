import Window from './windowImpl.js';
import { createRequire } from 'node:module';

const require: NodeRequire = createRequire(import.meta.url);

// eslint-disable-next-line
const window = require('../build/Release/window.node');

export function all(): Window[] {
  return window.enumerateWindows().map((ptr: BigInt) => new Window(ptr));
}

export function desktop(): Window {
  return new Window(window.getDesktop());
}

export function foreground(): Window {
  return new Window(window.getForeground());
}

export function console(): Window {
  return new Window(window.console());
}

export function find(path: string): Window[] {
  return window.getHwndFromPath(path.replace(/\0/g, '\uFFFD')).map((x: BigInt) => new Window(x));
}
