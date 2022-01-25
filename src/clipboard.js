"use strict";

const clipboard = require("../build/Release/clipboard");
const { Worker } = require("worker_threads");
const { resolve, join } = require("path");

const WORKER_FILE_NAME = join(__dirname, "pasteFiles.js");
const BUFFER_ALLOCUNSAFE = Buffer.allocUnsafe;

module.exports = {
    copy: str => void clipboard.copy(str.replace(/\r?\n/g, "\r\n").replace(/\0/g, "\uFFFD") + "\0"),
    copyFiles: (...files) => void clipboard.copyFiles(...files.map(x => resolve(x.replace(/\0/g, "\uFFFD")) + "\0")),
    copyHTML: (html, url) => {
        html = html
            ?.replace(/^(\s+)?(<html>)?(\s+)?(<body>)?(\s+)?/i, "")
            ?.replace(/(\s+)?(<\/body>)?(\s+)?(<\/html>)?(\s+)?$/i, "");
        
        if (typeof html !== 'string' || !html.length) {
            throw new TypeError("Invalid HTML.");
        }
        
        if (typeof url === 'string' && url.length) {
            clipboard.copyHTML(html, url);
        } else {
            clipboard.copyHTML(html);
        }
    },
    
    clear: clipboard.empty,
    
    paste: () => clipboard.paste(13),
    pasteFiles: dir => new Promise(promiseResolve => {
        const worker = new Worker(WORKER_FILE_NAME, { workerData: { dir: resolve(dir) } });
        worker.on("exit", () => promiseResolve());
    }),
	pasteHTML: clipboard.pasteHTML,
    
    pasteAll: () => clipboard.paste(BUFFER_ALLOCUNSAFE),
};
