"use strict";

const window = require("../build/Release/window");
const { validateString, validateInt } = require("./util");
const { Worker } = require("worker_threads");
const { inspect } = require("util");
const { join } = require("path");

const windowStyles = Object.entries({
    border: 0x00800000,
    caption: 0x00C00000,
    child: 0x40000000,
    clipChildren: 0x02000000,
    clipSiblings: 0x04000000,
    disabled: 0x08000000,
    dialog: 0x00400000,
    horizontalScroll: 0x00100000,
    maximized: 0x01000000,
    maximizeButton: 0x00010000,
    minimized: 0x20000000,
    minimizeButton: 0x00020000,
    overlapped: 0x00000000,
    popup: 0x80000000,
    sysMenu: 0x00080000,
    tiled: 0x00000000,
    visible: 0x10000000,
    virtualScroll: 0x00200000
});

const extendedWindowStyles = Object.entries({
    acceptFiles: 0x00000010,
    appWindow: 0x00040000,
    clientEdge: 0x00000200,
    composited: 0x02000000,
    contextHelp: 0x00000400,
    controlParent: 0x00010000,
    dialogModalFrame: 0x00000001,
    layered: 0x00080000,
    layoutRT: 0x00400000,
    left: 0x00000000,
    leftScrollBar: 0x00004000,
    mdiChild: 0x00000040,
    noActivate: 0x08000000,
    noInheritLayout: 0x00100000,
    noParentNotify: 0x00000004,
    noRedirectionBitmap: 0x00200000,
    right: 0x00001000,
    RTLreading: 0x00002000,
    staticEdge: 0x00020000,
    toolWindow: 0x00000080,
    topMost: 0x00000008
});

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
    
    type(text) {
        validateString(text);
        window.sendKeyboard(this.#ptr, text);
    }
    
    getStyles() {
        const [ styles, extendedStyles ] = window.getWindowStyles(this.#ptr);
        
        return { ...Object.fromEntries(windowStyles.map(([ K, V ]) => [ K, (styles & V) !== 0 ])), extended: {
            ...Object.fromEntries(extendedWindowStyles.map(([ K, V ]) => [ K, (styles & V) !== 0 ]))
        }};
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
}

module.exports = {
    all:        () => window.enumerateWindows().map(ptr => new Window(ptr)),
    desktop:    () => new Window(window.getDesktop()),
    foreground: () => new Window(window.getForeground()),
    console:    () => new Window(window.console()),
    find:       path => {
                    validateString(path);
                    return window.getHwndFromPath(path).map(x => new Window(x))
                },
    
    Window
};