mod collector;
mod finder;

use crate::screenshot::Screenshot;
use collector::Collector;
use finder::Finder;
use napi::{bindgen_prelude::AsyncTask, Error, Result, Status};
use std::mem::MaybeUninit;
use windows_sys::Win32::{System::Console::GetConsoleWindow, UI::WindowsAndMessaging::*};

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

#[derive(Clone)]
#[napi]
pub struct Window {
  handle: isize,
}

#[napi]
impl Window {
  #[napi]
  pub fn desktop() -> Option<Window> {
    unsafe {
      let handle = GetDesktopWindow();

      if handle == 0 {
        None
      } else {
        Some(Window { handle })
      }
    }
  }

  #[napi]
  pub fn active() -> Option<Window> {
    unsafe {
      let handle = GetForegroundWindow();

      if handle == 0 {
        None
      } else {
        Some(Window { handle })
      }
    }
  }

  #[napi]
  pub fn console() -> Option<Window> {
    unsafe {
      let handle = GetConsoleWindow();

      if handle == 0 {
        None
      } else {
        Some(Window { handle })
      }
    }
  }

  #[napi(js_name = "fromProcess")]
  pub fn from_process(name: String) -> AsyncTask<finder::Finder> {
    AsyncTask::new(Finder::new(finder::Kind::ProcessName, name))
  }

  #[napi(js_name = "fromTitle")]
  pub fn from_title(name: String) -> AsyncTask<Finder> {
    AsyncTask::new(Finder::new(finder::Kind::WindowTitle, name))
  }

  #[napi]
  pub fn all() -> AsyncTask<Collector> {
    AsyncTask::new(Collector::new())
  }

  #[napi]
  pub fn screenshot(&self) -> Screenshot {
    unsafe {
      let mut rect = MaybeUninit::uninit().assume_init();
      GetWindowRect(self.handle, &mut rect);

      Screenshot {
        consumed: false,
        x: rect.left,
        y: rect.bottom,
        width: rect.right - rect.left,
        height: rect.bottom - rect.top,
      }
    }
  }

  #[napi]
  pub fn position(&self) -> Coordinates {
    unsafe {
      let mut rect = MaybeUninit::uninit().assume_init();
      GetWindowRect(self.handle, &mut rect);

      Coordinates {
        x: rect.left,
        y: rect.top,
      }
    }
  }

  #[napi]
  pub fn hide(&self) {
    unsafe {
      ShowWindowAsync(self.handle, 0);
    }
  }

  #[napi]
  pub fn restore(&self) {
    unsafe {
      ShowWindowAsync(self.handle, 9);
    }
  }

  #[napi]
  pub fn show(&self) {
    unsafe {
      ShowWindowAsync(self.handle, 5);
    }
  }

  #[napi]
  pub fn maximize(&self) {
    unsafe {
      ShowWindowAsync(self.handle, 3);
    }
  }

  #[napi]
  pub fn minimize(&self) {
    unsafe {
      ShowWindowAsync(self.handle, 2);
    }
  }

  #[napi]
  pub fn boundaries(&self) -> Boundaries {
    unsafe {
      let mut rect = MaybeUninit::uninit().assume_init();
      GetWindowRect(self.handle, &mut rect);

      Boundaries {
        width: rect.right - rect.left,
        height: rect.bottom - rect.top,
      }
    }
  }

  #[napi]
  pub fn resize(&self, width: i32, height: i32) -> Result<()> {
    unsafe {
      if width < 0 || height < 0 {
        return Err(Error::new(
          Status::InvalidArg,
          String::from("Both values must not be negative."),
        ));
      }

      let mut rect = MaybeUninit::uninit().assume_init();
      GetWindowRect(self.handle, &mut rect);
      MoveWindow(self.handle, rect.left, rect.top, width, height, 1);

      Ok(())
    }
  }

  #[napi(js_name = "setCoordinates")]
  pub fn set_position(&self, x: i32, y: i32) -> Result<()> {
    unsafe {
      if x < 0 || y < 0 {
        return Err(Error::new(
          Status::InvalidArg,
          String::from("Both values must not be negative."),
        ));
      }

      let mut rect = MaybeUninit::uninit().assume_init();
      GetWindowRect(self.handle, &mut rect);

      MoveWindow(
        self.handle,
        x,
        y,
        rect.right - rect.left,
        rect.bottom - rect.top,
        1,
      );

      Ok(())
    }
  }

  #[napi]
  pub fn focus(&self) {
    unsafe {
      SetForegroundWindow(self.handle);
    }
  }

  #[napi]
  pub fn close(&self) {
    unsafe {
      CloseWindow(self.handle);
    }
  }
}
