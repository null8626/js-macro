"use strict";

const keyboard = require("../build/Release/keyboard");
const { virtualKeyCodes } = require("./constants");
const { validateString } = require("./util");

module.exports = {
	...Object.fromEntries(virtualKeyCodes.map(([K, V]) => [ K, {
		press:   () => void keyboard.type(V, 42069),
		hold:    () => void keyboard.type(V, 0),
		release: () => void keyboard.type(V, 2)
	}])),
	
	type: str => {
		validateString(str);
		keyboard.type(str);
	}
}