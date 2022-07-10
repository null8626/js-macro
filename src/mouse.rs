#[napi]
pub mod mouse {
  use crate::window::Coordinates;
  use napi::{Error, Result, Status};
  use std::mem::MaybeUninit;
  use windows_sys::Win32::UI::{Input::KeyboardAndMouse::*, WindowsAndMessaging::*};

  #[napi(js_name = "move")]
  pub unsafe fn move_cursor(x: i32, y: i32) -> Result<()> {
    if x < 0 || y < 0 {
      return Err(Error::new(
        Status::InvalidArg,
        String::from("Both values of X and Y must be higher than 0."),
      ));
    }

    SetCursorPos(x, y);

    Ok(())
  }

  #[napi]
  pub unsafe fn position() -> Coordinates {
    let mut point = MaybeUninit::uninit().assume_init();
    GetCursorPos(&mut point);

    Coordinates {
      x: point.x,
      y: point.y,
    }
  }

  #[napi(js_name = "leftHold")]
  pub unsafe fn left_hold() {
    mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
  }

  #[napi(js_name = "leftRelease")]
  pub unsafe fn left_release() {
    mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
  }

  #[napi(js_name = "leftClick")]
  pub unsafe fn left_click() {
    mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
    mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
  }

  #[napi(js_name = "middleHold")]
  pub unsafe fn middle_hold() {
    mouse_event(MOUSEEVENTF_MIDDLEDOWN, 0, 0, 0, 0);
  }

  #[napi(js_name = "middleRelease")]
  pub unsafe fn middle_release() {
    mouse_event(MOUSEEVENTF_MIDDLEUP, 0, 0, 0, 0);
  }

  #[napi(js_name = "middleClick")]
  pub unsafe fn middle_click() {
    mouse_event(MOUSEEVENTF_MIDDLEDOWN, 0, 0, 0, 0);
    mouse_event(MOUSEEVENTF_MIDDLEUP, 0, 0, 0, 0);
  }

  #[napi(js_name = "rightHold")]
  pub unsafe fn right_hold() {
    mouse_event(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, 0);
  }

  #[napi(js_name = "rightRelease")]
  pub unsafe fn right_release() {
    mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, 0, 0);
  }

  #[napi(js_name = "rightClick")]
  pub unsafe fn right_click() {
    mouse_event(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, 0);
    mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, 0, 0);
  }
}
