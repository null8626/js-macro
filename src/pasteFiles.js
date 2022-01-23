"use strict";

const { workerData: { dir } } = require("worker_threads");
require("../build/Release/clipboard").paste(15, dir);