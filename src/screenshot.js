"use strict";

const { parentPort, workerData: { ptr, x, y, width, height, file } } = require("worker_threads");
const { screenshot } = require("../build/Release/window");
const BUFFER_ALLOCUNSAFE = Buffer.allocUnsafe;

if (file !== null) {
    screenshot(BigInt(ptr), x, y, width, height, file);
} else {
    parentPort.postMessage(screenshot(BigInt(ptr), x, y, width, height, BUFFER_ALLOCUNSAFE));
}