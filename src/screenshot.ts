import { join } from "node:path";
import { fileURLToPath } from "node:url";
import { parentPort, workerData } from "node:worker_threads";

// eslint-disable-next-line
const { screenshot } = require(fileURLToPath(join(import.meta.url, "..", "..", "build", "Release", "screenshot.node")));

if (workerData.file !== null) {
  screenshot(BigInt(workerData.ptr), workerData.x, workerData.y, workerData.width, workerData.height, workerData.file);
} else {
  parentPort!.postMessage(screenshot(BigInt(workerData.ptr), workerData.x, workerData.y, workerData.width, workerData.height));
}
