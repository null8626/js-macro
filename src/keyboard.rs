#[napi]
pub mod keyboard {
  use napi::{bindgen_prelude::ToNapiValue, Error, JsString, Result, Status};
  use std::mem::{size_of, MaybeUninit};
  use windows_sys::Win32::UI::Input::KeyboardAndMouse::*;

  #[napi]
  pub enum KeyCode {
    Backspace = 8,
    Tab = 9,
    Return = 13,
    Shift = 16,
    Control = 17,
    Alt = 18,
    Pause = 19,
    CapsLock = 20,
    Escape = 27,
    SpaceBar = 32,
    PageUp = 33,
    PageDown = 34,
    End = 35,
    Home = 36,
    LeftArrow = 37,
    UpArrow = 38,
    RightArrow = 39,
    DownArrow = 40,
    Select = 41,
    Print = 42,
    Execute = 43,
    PrintScreen = 44,
    Insert = 45,
    Delete = 46,
    Help = 47,
    LeftWindows = 91,
    RightWindows = 92,
    F1 = 112,
    F2 = 113,
    F3 = 114,
    F4 = 115,
    F5 = 116,
    F6 = 117,
    F7 = 118,
    F8 = 119,
    F9 = 120,
    F10 = 121,
    F11 = 122,
    F12 = 123,
    F13 = 124,
    F14 = 125,
    F15 = 126,
    F16 = 127,
    F17 = 128,
    F18 = 129,
    F19 = 130,
    F20 = 131,
    F21 = 132,
    F22 = 133,
    F23 = 134,
    F24 = 135,
    NumLock = 144,
    ScrollLock = 145,
    LeftShift = 160,
    RightShift = 161,
    LeftControl = 162,
    Rightontrol = 163,
    LeftAlt = 164,
    RightAlt = 165,
    BrowserBack = 166,
    BrowserForward = 167,
    VolumeMute = 173,
    VolumeDown = 174,
    VolumeUp = 175,
    MediaNextTrack = 176,
    MediaPreviousTrack = 177,
    MediaStop = 178,
    MediaPlayPause = 179,
  }

  impl KeyCode {
    fn into_input(self, flags: u32) -> INPUT {
      unsafe {
        let mut input: INPUT = MaybeUninit::zeroed().assume_init();
        let mut input_union: INPUT_0 = MaybeUninit::zeroed().assume_init();

        input.r#type = INPUT_KEYBOARD;
        input_union.ki.wVk = self as u16;
        input_union.ki.dwFlags = flags;
        input.Anonymous = input_union;

        input
      }
    }
  }

  #[napi]
  pub unsafe fn hold(code: KeyCode) {
    let input = code.into_input(0);
    SendInput(1, &input, size_of::<INPUT>() as _);
  }

  #[napi]
  pub unsafe fn release(code: KeyCode) {
    let input = code.into_input(KEYEVENTF_KEYUP);
    SendInput(1, &input, size_of::<INPUT>() as _);
  }

  #[napi(js_name = "type")]
  pub unsafe fn type_text(text: JsString) -> Result<()> {
    let mut input: INPUT = MaybeUninit::zeroed().assume_init();
    let mut ki: INPUT_0 = MaybeUninit::zeroed().assume_init();

    let codes = text.into_utf16()?;

    let inputs_iter = codes.as_slice().into_iter();

    let inputs = inputs_iter
      .filter(|&&x| (x > 31 && x < 127) || (x > 159))
      .map(|&code| {
        ki.ki.dwFlags = KEYEVENTF_UNICODE;
        ki.ki.wScan = code;

        input.r#type = INPUT_KEYBOARD;
        input.Anonymous = ki;

        input
      })
      .collect::<Vec<_>>();

    if inputs.len() > (u32::MAX as usize) {
      return Err(Error::new(
        Status::InvalidArg,
        String::from("String length must not exceed the 32 unsigned integer limit."),
      ));
    } else if inputs.is_empty() {
      return Err(Error::new(
        Status::InvalidArg,
        String::from("Cannot type empty strings."),
      ));
    }

    SendInput(inputs.len() as _, inputs.as_ptr(), size_of::<INPUT>() as _);

    Ok(())
  }
}
