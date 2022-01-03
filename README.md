# macro.js
A npm package that lets you automate your windows desktop.
> [Beta] not on npm yet!

## Example
- Simple cursor usage
```js
const { cursor } = require("macro.js");

cursor.position();
// { x: 679, y: 0 }

cursor.move(0, 0);
// cursor is now at the very left top of the screen

cursor.leftClick();
```
- Automatically typing something on the console
```js
const { window } = require("macro.js");

const consoleWindow = window.console();

consoleWindow.type("Hello, World!");
```
