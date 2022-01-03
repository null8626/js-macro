"use strict";

const window = require("../build/Release/window");
const { validateString } = require("./util");
const { inspect } = require("util");

class ChildWindow {
    #ptr;
    #jsClassName;
    
    constructor(ptr, name = "ChildWindow") {
        this.#ptr = ptr;
        this.#jsClassName = name;
    }
    
    [inspect.custom]() {
        return `${this.#jsClassName}(0x${this.#ptr.toString(16).padStart(8, '0')})`;
    }
    
    get memoryLocation() {
        return this.#ptr;
    }
    
    getText() {
        return window.getTitle(this.#ptr);
    }
    
    getClassName() {
        return window.getClass(this.#ptr);
    }
    
    getChild() {
        return window.enumerateWindows(this.#ptr).map(x => new ChildWindow(x));
    }
    
    type(text) {
        validateString(text);
        window.sendKeyboard(this.#ptr, text);
    }
}

class Window extends ChildWindow {
    constructor(ptr) {
        super(ptr, "Window");
    }
    
    focus() {
        window.setForeground(this.memoryLocation);
    }
    
    close() {
        window.close(this.memoryLocation);
    }
}

module.exports = {
    all:        () => window.enumerateWindows().map(ptr => new Window(ptr)),
    desktop:    () => new Window(window.find()),
    foreground: window.getForeground,
    console:    window.console,
    find:       str => {
                    validateString(str);
                    
                    const hwnd = window.find(str);
                    return hwnd ? new Window(hwnd) : null;
                }
};