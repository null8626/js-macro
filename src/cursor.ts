import { validateInt } from "./util.js";
import Window from "./windowImpl.js";
import { createRequire } from "node:module";

const require: NodeRequire = createRequire(import.meta.url);

// eslint-disable-next-line
const cursor = require("../build/Release/cursor.node");

export function move(x: number, y: number): void {
  validateInt(x, y);
  cursor.moveCursor(x, y);
}

export interface CursorPosition {
  x: number;
  y: number;
}

export function position(): CursorPosition {
  return cursor.getCursorPos();
}

export function leftHold(): void {
  cursor.sendCursorEvent(0, 0);
}

export function leftRelease(): void {
  cursor.sendCursorEvent(0, 1);
}

export function leftClick(): void {
  cursor.sendCursorEvent(0, 2);
}

export function rightHold(): void {
  cursor.sendCursorEvent(1, 0);
}

export function rightRelease(): void {
  cursor.sendCursorEvent(1, 1);
}

export function rightClick(): void {
  cursor.sendCursorEvent(1, 2);
}

export function middleHold(): void {
  cursor.sendCursorEvent(2, 0);
}

export function middleRelease(): void {
  cursor.sendCursorEvent(2, 1);
}

export function middleClick(): void {
  cursor.sendCursorEvent(2, 2);
}

export function hoveredWindows(): Window[] {
  return cursor.isHoveringOn().map((x: BigInt) => new Window(x));
}
