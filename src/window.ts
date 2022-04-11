import { join } from "node:path";
import { fileURLToPath } from "node:url";
import Window from "./windowImpl.js";

// eslint-disable-next-line
const window = require(fileURLToPath(join(import.meta.url, "..", "..", "build", "Release", "window.node")));

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
  return window.getHwndFromPath(path.replace(/\0/g, "\uFFFD")).map((x: BigInt) => new Window(x));
}
