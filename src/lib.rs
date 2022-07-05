#![allow(dead_code)]
#![deny(clippy::all)]

#[macro_use]
extern crate napi_derive;

extern crate image;
extern crate napi;
extern crate windows_sys;

mod clipboard;
mod keyboard;
mod mouse;
mod screenshot;
mod window;

#[napi(object)]
pub struct Boundaries {
  pub width: i32,
  pub height: i32,
}

#[napi(object)]
pub struct Coordinates {
  pub x: i32,
  pub y: i32,
}
