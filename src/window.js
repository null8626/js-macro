"use strict";

const window = require("../build/Release/window");
const { validateString } = require("./util");
const { inspect } = require("util");

class Window {
    #ptr;
    #title;
    
    constructor(ptr) {
        this.#ptr = ptr;
        this.#title = null;
    }
    
    [inspect.custom]() {
        return `Window[0x${this.#ptr.toString(16)}] ` + inspect({
            title: this.title
        });
    }
    
    get memoryLocation() {
        return this.#ptr;
    }
    
    get title() {
        return (this.#title ?? (this.#title = window.getTitle(this.#ptr)));
    }
    
    focus() {
        window.setForeground(this.#ptr);
    }
    
    close() {
        window.close(this.#ptr);
    }
    
    type(text) {
        validateString(text);
        window.sendKeyboard(this.#ptr, text);
    }
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