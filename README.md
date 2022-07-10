# js-macro [![npm][npm-image]][npm-url] [![downloads][downloads-image]][downloads-url]

[npm-image]: https://img.shields.io/npm/v/js-macro.svg
[npm-url]: https://npmjs.org/package/js-macro
[downloads-image]: https://img.shields.io/npm/dm/js-macro.svg
[downloads-url]: https://npmjs.org/package/js-macro

A npm package that lets you automate your windows desktop.

## Examples

### Simple cursor usage

```js
const { mouse } = require('js-macro');

mouse.position();
// { x: 679, y: 0 }

mouse.move(0, 0);
// cursor is now at the very left top of the screen

mouse.leftClick();
```

### Typing something on notepad

```js
const { exec } = require('node:child_process');
const { Window, keyboard } = require('js-macro');

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
const { clipboard } = require('js-macro');

clipboard.copy('Hello, World!');

clipboard.paste();
// returns 'Hello, World!'
```

### Click-logging!

```js
const { clicklogger } = require('js-macro');

// add a listener on every left click
// we support left, middle, and right clicks!
clicklogger.on('left', coord => console.log(coord));

// start logging!
clicklogger.start();

// do stuff here... then
// while not required, this is highly recommended
clicklogger.stop();
```

### Keylogging!

```js
const { keylogger } = require('js-macro');

// add a listener on every word (after space is pressed)
// we support raw keys, characters, words, and lines!

// please note that this listens directly to your keyboard
// which means that it can't read things like keyboard shortcuts (e.g: clipboard)
// or shifted keys directly.
keylogger.on('word', text => console.log(text));

// start logging!
keylogger.start();

// do stuff here... then
// while not required, this is highly recommended
keylogger.stop();
```

### Screenshotting a specific location

```js
const { Screenshot } = require('js-macro');

const screenshot = new Screenshot({
  x: 0,
  y: 0, 
  width: 150,
  height: 150
});

// take screenshot and write it to a file
await screenshot.file('file.png');
```

### Screenshotting your desktop (like print-screen!)

```js
const { Screenshot } = require('js-macro');

// providing no arguments will screenshot the current monitor/screen
const screenshot = new Screenshot();

// take screenshot and write it to a Buffer in a PNG format
// the format can be 'png' or 'jpeg'
const buf = await screenshot.buffer('png');
```
