"use strict";

const cursor = require("../build/Release/cursor");
const { validateInt } = require("./util");
const Window = require("./windowImpl");

module.exports = {
    move: (x, y) => {
        validateInt(x, y);
        cursor.moveCursor(x, y);
    },
    
    position: cursor.getCursorPos,
    
    leftHold:       () => void cursor.sendCursorEvent(0, 0),
    leftRelease:    () => void cursor.sendCursorEvent(0, 1),
    leftClick:      () => void cursor.sendCursorEvent(0, 2),
                    
    rightHold:      () => void cursor.sendCursorEvent(1, 0),
    rightRelease:   () => void cursor.sendCursorEvent(1, 1),
    rightClick:     () => void cursor.sendCursorEvent(1, 2),
                    
    middleHold:     () => void cursor.sendCursorEvent(2, 0),
    middleRelease:  () => void cursor.sendCursorEvent(2, 1),
    middleClick:    () => void cursor.sendCursorEvent(2, 2),
    
    hoveredWindows: () => cursor.isHoveringOn().map(x => new Window(x))
};