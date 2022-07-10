#[napi]
pub mod clipboard {
  use core::slice;
  use napi::{Error, JsString, Result, Status};
  use windows_sys::Win32::{
    Foundation::SetLastError,
    System::{DataExchange::*, Memory::*},
  };

  #[napi]
  pub unsafe fn copy(text: JsString) -> Result<()> {
    let codes_input = text.into_utf16()?;

    let codes = codes_input
      .into_iter()
      .filter(|&&x| (x > 31 && x < 127) || (x > 159))
      .map(|&x| x)
      .collect::<Vec<_>>();

    if codes.is_empty() {
      return Err(Error::new(
        Status::InvalidArg,
        String::from("Cannot type empty strings."),
      ));
    }

    let handle = GlobalAlloc(GMEM_MOVEABLE, codes.len() * 2);
    GlobalLock(handle).copy_from_nonoverlapping(codes.as_ptr() as _, codes.len());
    GlobalUnlock(handle);

    OpenClipboard(0);
    EmptyClipboard();

    SetClipboardData(13, handle);
    CloseClipboard();

    Ok(())
  }

  #[napi]
  pub unsafe fn paste() -> Result<Option<String>> {
    OpenClipboard(0);

    let handle = GetClipboardData(13);

    if handle == 0 {
      CloseClipboard();
      SetLastError(0);

      return Ok(None);
    }

    let out = String::from_utf16(slice::from_raw_parts(
      GlobalLock(handle) as *const u16,
      (GlobalSize(handle) / 2) - 1,
    ));

    GlobalUnlock(handle);
    CloseClipboard();

    match out {
      Ok(res) => Ok(Some(res)),
      Err(_) => Err(Error::new(
        Status::GenericFailure,
        String::from("Cannot encode string encoded in clipboard to UTF-16."),
      )),
    }
  }

  #[napi]
  pub unsafe fn empty() {
    OpenClipboard(0);
    EmptyClipboard();
    CloseClipboard();
  }
}
