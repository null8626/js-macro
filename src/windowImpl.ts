import { validateInt } from "./util.js";
import { windowStyles, extendedWindowStyles } from "./constants.js";
import { join } from "node:path";
import { inspect } from "node:util";
import { Buffer } from "node:buffer";
import { fileURLToPath } from "node:url";

// eslint-disable-next-line
const window = require(fileURLToPath(join(import.meta.url, "..", "..", "build", "Release", "window.node")));

// rust <3
export type Option<T> = T | null;

export interface Boundaries {
  width: number;
  height: number;
}

interface MaybeBoundaries {
  width: Option<number>;
  height: Option<number>;
}

export interface WindowStyles extends Record<string, boolean | Record<string, boolean>> {
  extended: Record<string, boolean>;
}

class ChildWindow {
  private ptr: BigInt;
  private jsClassName: string;

  constructor(ptr: BigInt, name: string = "ChildWindow") {
    this.ptr = ptr;
    this.jsClassName = name;
  }

  [inspect.custom](): string {
    return `${this.jsClassName}(0x${this.ptr.toString(16).padStart(8, "0")})`;
  }

  get memoryLocation(): BigInt {
    return this.ptr;
  }

  getText(): string {
    return window.getTitle(this.ptr);
  }

  getClassName(): string {
    return window.getClass(this.ptr);
  }

  getChild(): ChildWindow[] {
    return window.enumerateWindows(this.ptr).map((x: BigInt) => new ChildWindow(x));
  }

  getBoundaries(): Boundaries {
    return window.boundaries(this.ptr);
  }

  setPosition(x: number, y: number): void {
    validateInt(x, y);
    window.setWindowPos(this.ptr, x, y, 0, 0);
  }

  setBoundaries(width: number, height: number): void {
    validateInt(width, height);
    window.setWindowPos(this.ptr, 0, 0, width, height);
  }

  getStyles(): WindowStyles {
    const [styles, extendedStyles]: [number, number] = window.getWindowStyles(this.ptr);

    return {
      ...Object.fromEntries(windowStyles.map(([K, V]) => [K, (styles & V) !== 0])),
      extended: {
        ...Object.fromEntries(extendedWindowStyles.map(([K, V]) => [K, (extendedStyles & V) !== 0]))
      }
    };
  }
}

export default class Window extends ChildWindow {
  constructor(ptr: BigInt) {
    super(ptr, "Window");
  }

  focus(): void {
    window.setForeground(this.memoryLocation);
  }

  close(): void {
    window.close(this.memoryLocation);
  }

  screenshot(x: number, y: number, width: Option<number> = null, height: Option<number> = null, file: Option<string> = null): Promise<void | Buffer> {
    if (typeof width === "string" && arguments.length === 3) {
      file = width;
      width = null;
    }

    let boundaries: MaybeBoundaries = { width, height };

    if (boundaries.width === null || boundaries.height === null) {
      boundaries = this.getBoundaries();
    }

    validateInt(x, y, boundaries.width!, boundaries.height!);

    if (file !== null && (typeof file !== "string" || !file.length || /[\u0100-\uffff\0]/g.test(file))) {
      throw new TypeError("Invalid string.");
    }

    return new Promise(resolve => {
      if (file === null) {
        window.screenshot(x, y, boundaries.width, boundaries.height, (buf: number[]) => resolve(Buffer.from(buf)));
      } else {
        window.screenshot(x, y, boundaries.width, boundaries.height, resolve, file);
      }
    });
  }

  hide(): void {
    window.showWindow(this.memoryLocation, 0);
  }

  restore(): void {
    window.showWindow(this.memoryLocation, 9);
  }

  show(): void {
    window.showWindow(this.memoryLocation, 5);
  }

  maximize(): void {
    window.showWindow(this.memoryLocation, 3);
  }

  minimize(activate: boolean = false): void {
    window.showWindow(this.memoryLocation, activate ? 7 : 2);
  }
}
