/* tslint:disable */
/* eslint-disable */

/* auto-generated by NAPI-RS */

export function screenBoundaries(): Boundaries;
export interface ScreenshotOptions {
  x?: number;
  y?: number;
  width?: number;
  height?: number;
}
export const enum Format {
  Png = 0,
  Jpeg = 1
}
export interface Boundaries {
  width: number;
  height: number;
}
export interface Coordinates {
  x: number;
  y: number;
}
export class Screenshot {
  constructor(options?: ScreenshotOptions | undefined | null);
  save(s: string): Promise<unknown>;
  arrayBuffer(format: Format): Promise<unknown>;
  buffer(format: Format): Promise<unknown>;
}
export class Window {
  static desktop(): Window | null;
  static active(): Window | null;
  static console(): Window | null;
  static fromProcess(name: string): Promise<Window>;
  static fromTitle(name: string): Promise<Window>;
  static all(): Promise<Array<Window>>;
  screenshot(): Screenshot;
  position(): Coordinates;
  hide(): void;
  restore(): void;
  show(): void;
  maximize(): void;
  minimize(): void;
  boundaries(): Boundaries;
  resize(width: number, height: number): void;
  setCoordinates(x: number, y: number): void;
  focus(): void;
  close(): void;
}
export namespace clipboard {
  export function copy(text: string): void;
  export function paste(): string | null;
  export function empty(): void;
}
export namespace keyboard {
  export const enum KeyCode {
    Backspace = 8,
    Shift = 16,
    Control = 17,
    Alt = 18,
    CapsLock = 20,
    Escape = 27,
    PageUp = 33,
    PageDown = 34,
    End = 35,
    Home = 36,
    LeftArrow = 37,
    UpArrow = 38,
    RightArrow = 39,
    DownArrow = 40,
    Select = 41,
    Print = 42,
    Execute = 43,
    PrintScreen = 44,
    Insert = 45,
    Delete = 46,
    Help = 47,
    LeftWindows = 91,
    RightWindows = 92,
    F1 = 112,
    F2 = 113,
    F3 = 114,
    F4 = 115,
    F5 = 116,
    F6 = 117,
    F7 = 118,
    F8 = 119,
    F9 = 120,
    F10 = 121,
    F11 = 122,
    F12 = 123,
    F13 = 124,
    F14 = 125,
    F15 = 126,
    F16 = 127,
    F17 = 128,
    F18 = 129,
    F19 = 130,
    F20 = 131,
    F21 = 132,
    F22 = 133,
    F23 = 134,
    F24 = 135,
    VolumeMute = 173,
    VolumeDown = 174,
    VolumeUp = 175
  }
  export function hold(code: KeyCode): void;
  export function release(code: KeyCode): void;
  export function type(text: string): void;
}
export namespace mouse {
  export function move(x: number, y: number): void;
  export function pos(): Coordinates;
  export function leftHold(): void;
  export function leftRelease(): void;
  export function leftClick(): void;
  export function middleHold(): void;
  export function middleRelease(): void;
  export function middleClick(): void;
  export function rightHold(): void;
  export function rightRelease(): void;
  export function rightClick(): void;
}
