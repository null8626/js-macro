#[napi(js_name = "mouse")]
pub mod mouse_namespace {
  use crate::Coordinates;
  use napi::{Error, Result, Status};
  use std::mem::MaybeUninit;
  use windows_sys::Win32::UI::{Input::KeyboardAndMouse::*, WindowsAndMessaging::*};

  #[napi(js_name = "move")]
  pub fn move_cursor(x: i32, y: i32) -> Result<()> {
    if x < 0 || y < 0 {
      return Err(Error::new(
        Status::InvalidArg,
        String::from("Both values of X and Y must be higher than 0."),
      ));
    }

    unsafe {
      SetCursorPos(x, y);
    }

    Ok(())
  }

  #[napi]
  pub fn pos() -> Coordinates {
    unsafe {
      let mut point = MaybeUninit::uninit().assume_init();
      GetCursorPos(&mut point);

      Coordinates {
        x: point.x,
        y: point.y,
      }
    }
  }

  #[napi(js_name = "leftHold")]
  pub fn left_hold() {
    unsafe {
      mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
    }
  }

  #[napi(js_name = "leftRelease")]
  pub fn left_release() {
    unsafe {
      mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
    }
  }

  #[napi(js_name = "leftClick")]
  pub fn left_click() {
    unsafe {
      mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
      mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
    }
  }

  #[napi(js_name = "middleHold")]
  pub fn middle_hold() {
    unsafe {
      mouse_event(MOUSEEVENTF_MIDDLEDOWN, 0, 0, 0, 0);
    }
  }

  #[napi(js_name = "middleRelease")]
  pub fn middle_release() {
    unsafe {
      mouse_event(MOUSEEVENTF_MIDDLEUP, 0, 0, 0, 0);
    }
  }

  #[napi(js_name = "middleClick")]
  pub fn middle_click() {
    unsafe {
      mouse_event(MOUSEEVENTF_MIDDLEDOWN, 0, 0, 0, 0);
      mouse_event(MOUSEEVENTF_MIDDLEUP, 0, 0, 0, 0);
    }
  }

  #[napi(js_name = "rightHold")]
  pub fn right_hold() {
    unsafe {
      mouse_event(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, 0);
    }
  }

  #[napi(js_name = "rightRelease")]
  pub fn right_release() {
    unsafe {
      mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, 0, 0);
    }
  }

  #[napi(js_name = "rightClick")]
  pub fn right_click() {
    unsafe {
      mouse_event(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, 0);
      mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, 0, 0);
    }
  }
}
