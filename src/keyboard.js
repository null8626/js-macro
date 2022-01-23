"use strict";

const keyboard = require("../build/Release/keyboard");
const { virtualKeyCodes } = require("./constants");

module.exports = {
    ...Object.fromEntries(virtualKeyCodes.map(([K, V]) => [ K, Object.assign(() => void keyboard.type(V, 42069), {
        hold:    () => void keyboard.type(V, 0),
        release: () => void keyboard.type(V, 2)
    })])),
    
    type: keyboard.type
}