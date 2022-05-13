import { virtualKeyCodes } from './constants.js';
import { createRequire } from 'node:module';

const require: NodeRequire = createRequire(import.meta.url);

// eslint-disable-next-line
const keyboard = require('../build/Release/keyboard.node');

export type EmptyFunction = () => void;

export interface KeyboardButton extends EmptyFunction {
  hold: EmptyFunction;
  release: EmptyFunction;
}

const keys: Record<string, KeyboardButton> = Object.fromEntries(
  virtualKeyCodes.map(([K, V]) => [
    K,
    Object.assign(() => void keyboard.type(V, 42069), {
      hold: () => void keyboard.type(V, 0),
      release: () => void keyboard.type(V, 2)
    })
  ])
);

export default {
  type: keyboard.type,
  ...keys
};
