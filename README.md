# macro.js
A npm package that lets you automate your windows desktop.
> [Beta] not on npm yet!

## Example
- Having fun with your cursor
```
const { cursor } = require("macro.js");

cursor.position();
// { x: 679, y: 0 }

cursor.move(0, 0);

cursor.leftClick();
```
- Closing this very node window
```
const { window } = require("macro.js");

process.title = "my title";

const win = window.find("my title");

if (win) {
  win.close();
}
```
