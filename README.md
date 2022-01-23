# js-macro
A npm package that lets you automate your windows desktop.
```
npm i js-macro
```

## Examples
- Simple cursor usage
```js
const { cursor } = require("js-macro");

cursor.position();
// { x: 679, y: 0 }

cursor.move(0, 0);
// cursor is now at the very left top of the screen

cursor.leftClick();
```
- Typing something on notepad
```js
const { Worker, isMainThread } = require("worker_threads");
const { execSync } = require("child_process");
const { window, keyboard } = require("js-macro");

if (!isMainThread) {
    execSync("notepad.exe");
} else {
    // we don't want for execSync to wait for notepad to exit,
    // so we should use a worker instead
    void new Worker(__filename);
    
    // wait for notepad to start
    setTimeout(() => {
        let notepad = window.find("notepad.exe");
        
        if (!notepad.length) {
            return console.error("error: cannot find notepad :(");
        }
        
        // window.find returns an array - use the first element
        notepad[0].focus();
        
        keyboard.type("Hello, World!");
    }, 1000);
}
```
- Copying and pasting programmatically!
```js
const { clipboard } = require("js-macro");

clipboard.copy("Hello, World!");

clipboard.paste();
// returns "Hello, World!"
```
- Screenshotting a window, or your desktop (like print-screen!)
> The buffers will ALWAYS be in a **PNG** format.
```js
const { window } = require("js-macro");
const desktop = window.desktop();

desktop.screenshot(0, 0, "file.png").then(() => console.log("screenshotted!"));
desktop.screenshot(0, 0, 500, 500).then(buf => /* ... */);
```