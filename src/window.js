"use strict";

const window = require("../build/Release/window");
const { validateString } = require("./util");

class Window {
    #ptr;
    
    constructor(ptr) {
        this.#ptr = ptr;
    }
    
    focus() {
        window.setForeground(this.#ptr);
    }
    
    close() {
        window.close(this.#ptr);
    }
    /*
    type(text) {
        validateString(text);
        window.sendKeyboard(this.#ptr, text);
    }
    */
}

module.exports = {
    getAll:     () => window.enumerateWindows().filter(ptr => ptr).map(ptr => new Window(ptr)),
    desktop:    () => new Window(window.find()),
    foreground: window.getForeground,
    find:       str => {
        validateString(str);
        
        const hwnd = window.find(str);
        return hwnd ? new Window(hwnd) : null;
    }
};