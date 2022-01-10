"use strict";

const window = require("../build/Release/window");
const Window = require("./windowImpl");

module.exports = {
    all:        () => window.enumerateWindows().map(ptr => new Window(ptr)),
    desktop:    () => new Window(window.getDesktop()),
    foreground: () => new Window(window.getForeground()),
    console:    () => new Window(window.console()),
    find:       path => {
                    validateString(path);
                    return window.getHwndFromPath(path).map(x => new Window(x))
                }
};
