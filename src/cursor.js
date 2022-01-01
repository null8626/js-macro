"use strict";

const cursor = require("../build/Release/cursor");

module.exports = {
    move:          cursor.moveCursor,
    leftHold:      () => cursor.sendCursorEvent(0, 0),
    leftRelease:   () => cursor.sendCursorEvent(0, 1),
    leftClick:     () => cursor.sendCursorEvent(0, 2),
    rightHold:     () => cursor.sendCursorEvent(1, 0),
    rightRelease:  () => cursor.sendCursorEvent(1, 1),
    rightClick:    () => cursor.sendCursorEvent(1, 2),
    middleHold:    () => cursor.sendCursorEvent(2, 0),
    middleRelease: () => cursor.sendCursorEvent(2, 1),
    middleClick:   () => cursor.sendCursorEvent(2, 2)
};