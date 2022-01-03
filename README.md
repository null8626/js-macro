# macro.js
A npm package that lets you automate your windows desktop.
> [Beta] not on npm yet!

## Example
- Having fun with your cursor
```js
const { cursor } = require("macro.js");

cursor.position();
// { x: 679, y: 0 }

cursor.move(0, 0);
// cursor is now at the very left top of the screen

cursor.leftClick();
```
- Automatically typing on the console
```js
const { window } = require("./src/index");

const consoleWindow = window.console();

consoleWindow.type("Hello, World!");
```
