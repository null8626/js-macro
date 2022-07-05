# js-macro [![npm][npm-image]][npm-url] [![downloads][downloads-image]][downloads-url]

[npm-image]: https://img.shields.io/npm/v/js-macro.svg
[npm-url]: https://npmjs.org/package/js-macro
[downloads-image]: https://img.shields.io/npm/dm/js-macro.svg
[downloads-url]: https://npmjs.org/package/js-macro

A npm package that lets you automate your windows desktop.

## Examples

### Simple cursor usage

```js
import { mouse } from 'js-macro';

mouse.pos();
// { x: 679, y: 0 }

moouse.move(0, 0);
// cursor is now at the very left top of the screen

mouse.leftClick();
```

### Typing something on notepad

```js
import { exec } from 'node:child_process';
import { Window, keyboard } from 'js-macro';

exec('notepad.exe');

setTimeout(async () => {
  let notepad = await Window.fromProcess('notepad.exe');

  if (!notepad) {
    return console.error('error: cannot find notepad :(');
  }

  notepad.focus();

  keyboard.type('Hello, World!');
}, 1000);
```

### Copying and pasting programmatically!

```js
import { clipboard } from 'js-macro';

clipboard.copy('Hello, World!');

clipboard.paste();
// returns "Hello, World!"
```

### Screenshotting a specific location

```js
import { Screenshot } from "js-macro";

const screenshot = new Screenshot({
  x: 0,
  y: 0, 
  width: 150,
  height: 150
});

// take screenshot and write it to a file
await screenshot.file("file.png");
```

### Screenshotting your desktop (like print-screen!)

```js
import { Screenshot, Format } from "js-macro";

// providing no arguments will screenshot the current monitor/screen
const screenshot = new Screenshot();

// take screenshot and write it to a Buffer in a PNG format
// p.s: we also support JPEG!
const buf = await screenshot.buffer(Format.Png);
```