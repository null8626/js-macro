import { execSync } from "node:child_process";
import { existsSync, readdirSync } from "node:fs";

(() => {
  if (existsSync("./build/Release/")) {
    const required_files = ["window.node", "cursor.node", "keyboard.node", "clipboard.node"];
    const files = readdirSync("./build/Release").filter(x => x.endsWith(".node"));
  
    if (files.length === 4 && files.every(x => required_files.includes(x))) {
      return console.info("binary already made. skipping.");
    }
  }
  
  void execSync("node-gyp rebuild", { stdio: "inherit" });
})();

void execSync("npm run test && npx tsc --project tsconfig.json", { stdio: "inherit" });