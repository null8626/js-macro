# js-macro [![CI][ci-image]][ci-url] [![npm][npm-image]][npm-url] [![downloads][downloads-image]][downloads-url] [![prettier][prettier-image]][prettier-url]

[ci-image]: https://github.com/null8626/js-macro/actions/workflows/lint.yml/badge.svg?branch=main
[ci-url]: https://github.com/null8626/js-macro/actions/workflows/lint.yml
[prettier-image]: https://img.shields.io/badge/code_style-prettier-ff69b4.svg?style=flat-square
[prettier-url]: https://github.com/prettier/prettier
[npm-image]: https://img.shields.io/npm/v/js-macro.svg
[npm-url]: https://npmjs.org/package/js-macro
[downloads-image]: https://img.shields.io/npm/dm/js-macro.svg
[downloads-url]: https://npmjs.org/package/js-macro

A npm package that lets you automate your windows desktop.

```
npm i js-macro
```

## Examples

- Simple cursor usage

```js
import { cursor } from 'js-macro';

cursor.position();
// { x: 679, y: 0 }

cursor.move(0, 0);
// cursor is now at the very left top of the screen

cursor.leftClick();
```

- Typing something on notepad

```js
import { exec } from 'node:child_process';
import { window, keyboard } from 'js-macro';

void exec('notepad.exe');
setTimeout(() => {
  let notepad = window.find('notepad.exe');

  if (!notepad.length) {
    return console.error('error: cannot find notepad :(');
  }

  // window.find returns an array - use the first element
  notepad[0].focus();

  keyboard.type('Hello, World!');
}, 1000);
```

- Copying and pasting programmatically!

```js
import { clipboard } from 'js-macro';

clipboard.copy('Hello, World!');

clipboard.paste();
// returns "Hello, World!"
```

- Screenshotting a window, or your desktop (like print-screen!)
  > The buffers will ALWAYS be in a **PNG** format.

```js
import { window } from "js-macro";
const desktop = window.desktop();

desktop.screenshot({
  x: 0,
  y: 0,
  file: "file.png"
}).then(() => console.log("screenshotted!"));

desktop.screenshot({
  x: 0,
  y: 0,
  width: 500,
  height: 500
}).then(buf => /* ... */);
```

# Building locally

```cmd
git clone https://github.com/null8626/js-macro.git
cd js-macro
npm install --save-dev --ignore-scripts
npm run install
```
