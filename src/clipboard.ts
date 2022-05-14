import { resolve } from 'node:path';
import { Buffer } from 'node:buffer';
import { createRequire } from 'node:module';

const require: NodeRequire = createRequire(import.meta.url);

// eslint-disable-next-line
const clipboard = require('../build/Release/clipboard.node');

// rust <3
export type Option<T> = T | undefined | null;

export function empty(): void {
  clipboard.empty();
}

export function copy(str: string): void {
  clipboard.copy(`${str.replace(/\r?\n/g, '\r\n').replace(/\0/g, '\uFFFD')}\0`);
}

export function copyFiles(...files: string[]): void {
  clipboard.copyFiles(...files.map(x => `${resolve(x.replace(/\0/g, '\uFFFD'))}\0`));
}

export function copyHTML(html: string, url: Option<string>): void {
  html = html.replace(/^\s*(?:<html>)?\s*(?:<body>)?\s*/i, '').replace(/\s*(?:<\/body>)?\s*(?:<\/html>)?\s*$/i, '');

  if (typeof url === 'string' && url.length) {
    clipboard.copyHTML(html, url);
  } else {
    clipboard.copyHTML(html);
  }
}

export function paste(): string {
  return clipboard.paste(13);
}

export function pasteFiles(dir: string): Promise<void> {
  return new Promise(promiseResolve => clipboard.paste(15, resolve(dir), promiseResolve));
}

export function pasteAll(): Record<string, Option<Buffer>> {
  return clipboard.paste();
}

export interface HTML {
  version: string;
  html: string;
  url?: string;
}

export function pasteHTML(): HTML {
  return clipboard.pasteHTML();
}
