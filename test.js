const { window } = require("./src/index");

for (const windowHandle of window.all()) {
    console.log(windowHandle);
}