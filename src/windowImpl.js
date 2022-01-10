"use strict";

const { window } = require("../build/Release/window");
const { validateString, validateInt } = require("./util");
const { windowStyles, extendedWindowStyles } = require("./constants");
const { Worker } = require("worker_threads");
const { inspect } = require("util");
const { join } = require("path");

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
    
    getBoundaries() {
        return window.boundaries(this.#ptr);
    }
    
    setPosition(x, y) {
        validateInt(x, y);
        window.setWindowPos(this.#ptr, x, y, 0, 0);
    }
    
    setBoundaries(width, height) {
        validateInt(width, height);
        window.setWindowPos(this.#ptr, 0, 0, width, height);
    }
/*
    type(text) {
        validateString(text);
        window.sendKeyboard(this.#ptr, text);
    }
*/    
    getStyles() {
        const [ styles, extendedStyles ] = window.getWindowStyles(this.#ptr);
        
        return { ...Object.fromEntries(windowStyles.map(([ K, V ]) => [ K, (styles & V) !== 0 ])), extended: {
            ...Object.fromEntries(extendedWindowStyles.map(([ K, V ]) => [ K, (extendedStyles & V) !== 0 ]))
        }};
    }
}

module.exports = class Window extends ChildWindow {
    constructor(ptr) {
        super(ptr, "Window");
    }
    
    focus() {
        window.setForeground(this.memoryLocation);
    }
    
    close() {
        window.close(this.memoryLocation);
    }
    
    screenshot(x, y, width = null, height = null, file = null) {
        if (typeof width === 'string' && arguments.length === 3) {
            file = width;
            width = null;
        }
        
        let boundaries = { width, height };
        
        if (boundaries.width === null || boundaries.height === null) {
            boundaries = this.getBoundaries();
        }
        
        validateInt(x, y, boundaries.width, boundaries.height);
        
        if (file !== null) {
            validateString(file);
        }
        
        return new Promise(resolve => {
            const worker = new Worker(join(__dirname, "screenshot.js"), {
                workerData: { ptr: this.memoryLocation.toString(), x, y, file, ...boundaries }
            });
            
            if (file === null) {
                worker.on("message", buf => resolve(Buffer.from(buf)));
            } else {
                worker.on("exit", () => resolve());
            }
        });
    }
    
    hide() {
        window.showWindow(this.memoryLocation, 0);
    }
    
    restore() {
        window.showWindow(this.memoryLocation, 9);
    }
    
    show() {
        window.showWindow(this.memoryLocation, 5);
    }
    
    maximize() {
        window.showWindow(this.memoryLocation, 3);
    }
    
    minimize(activate = false) {
        window.showWindow(this.memoryLocation, activate ? 7 : 2);
    }
}