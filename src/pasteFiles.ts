import { join } from "node:path";
import { fileURLToPath } from "node:url";
import { workerData } from "node:worker_threads";

// eslint-disable-next-line
const { paste } = require(fileURLToPath(join(import.meta.url, "..", "..", "build", "Release", "clipboard.node")));

paste(workerData.dir);
