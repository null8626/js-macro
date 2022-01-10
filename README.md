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
- Screenshotting a window, or your desktop (like print-screen!)
> The buffers will ALWAYS be in a **PNG** format.
```js
const { window } = require("js-macro");
const desktop = window.desktop();

desktop.screenshot(0, 0, "file.png").then(() => console.log("screenshotted!"));
desktop.screenshot(0, 0, 500, 500).then(buf => /* ... */);
```