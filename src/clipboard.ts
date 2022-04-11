import { resolve, join } from "node:path";
import { Buffer } from "node:buffer";
import { fileURLToPath } from "node:url";

// eslint-disable-next-line
const clipboard = require(fileURLToPath(join(import.meta.url, "..", "..", "build", "Release", "clipboard.node")));

// rust <3
export type Option<T> = T | undefined | null;

const WORKER_FILE_NAME: string = fileURLToPath(join(import.meta.url, "..", "pasteFiles.js"));

export function empty(): void {
  clipboard.empty();
}

export function copy(str: string): void {
  clipboard.copy(`${str.replace(/\r?\n/g, "\r\n").replace(/\0/g, "\uFFFD")}\0`);
}

export function copyFiles(...files: string[]): void {
  clipboard.copyFiles(...files.map(x => `${resolve(x.replace(/\0/g, "\uFFFD"))}\0`));
}

export function copyHTML(html: string, url: Option<string>): void {
  html = html.replace(/^\s*(?:<html>)?\s*(?:<body>)?\s*/i, "").replace(/\s*(?:<\/body>)?\s*(?:<\/html>)?\s*$/i, "");

  if (typeof url === "string" && url.length) {
    clipboard.copyHTML(html, url);
  } else {
    clipboard.copyHTML(html);
  }
}

export function paste(): void {
  clipboard.paste(13);
}

export function pasteFiles(dir: string): Promise<void> {
  return new Promise(promiseResolve => clipboard.paste(15, resolve(dir), promiseResolve));
}

export function pasteAll(): void {
  clipboard.paste(Buffer.allocUnsafe);
}

export interface HTML {
  version: string;
  html: string;
  url?: string;
}

export function pasteHTML(): HTML {
  return clipboard.pasteHTML();
}
