// NO this is not sponsored by macOS >:(

use super::Window;
use core::slice;
use napi::{Env, Result, Task};
use std::{intrinsics::transmute, mem::MaybeUninit};
use windows_sys::Win32::{Foundation::*, System::Threading::*, UI::WindowsAndMessaging::*};

#[repr(u8)]
pub enum Kind {
  ProcessName,
  WindowTitle,
}

pub struct Finder {
  kind: Kind,
  requested_name: String,
  output: Option<isize>,
}

impl Finder {
  #[inline(always)]
  pub fn new(kind: Kind, requested_name: String) -> Self {
    Self {
      kind,
      requested_name: requested_name.to_lowercase(),
      output: None,
    }
  }
}

unsafe extern "system" fn window_finder_proc(hwnd: isize, lparam: isize) -> i32 {
  if IsWindowVisible(hwnd) == 0 {
    return 1;
  }

  let ptr: *mut Finder = transmute(lparam);

  let mut proc_id = MaybeUninit::uninit().assume_init();
  GetWindowThreadProcessId(hwnd, &mut proc_id);

  let process = OpenProcess(PROCESS_QUERY_INFORMATION, 0, proc_id);
  if process == 0 {
    return 1;
  }

  match (*ptr).kind {
    Kind::ProcessName => {
      let mut image_name_ptr: [u16; (MAX_PATH + 1) as _] = MaybeUninit::uninit().assume_init();
      let mut image_name_size = MAX_PATH;

      if QueryFullProcessImageNameW(
        process,
        0,
        image_name_ptr.as_mut_ptr(),
        &mut image_name_size,
      ) == 0
        || ((*ptr).requested_name.len() as u32) > image_name_size
      {
        CloseHandle(process);
        return 1;
      }

      CloseHandle(process);

      match String::from_utf16_lossy(slice::from_raw_parts(
        image_name_ptr.as_ptr(),
        image_name_size as _,
      ))
      .split(&['/', '\\'])
      .last()
      {
        Some(s) => {
          if s.to_lowercase() == (*ptr).requested_name {
            (*ptr).output.replace(hwnd);
            0
          } else {
            1
          }
        }

        None => 1,
      }
    }

    Kind::WindowTitle => {
      CloseHandle(process);

      let title_len = GetWindowTextLengthW(hwnd);
      let mut bytes = Vec::with_capacity(title_len as _);

      GetWindowTextW(hwnd, bytes.as_mut_ptr(), title_len);

      if String::from_utf16_lossy(&bytes)
        .to_lowercase()
        .contains(&(*ptr).requested_name)
      {
        (*ptr).output.replace(hwnd);

        0
      } else {
        1
      }
    }
  }
}

#[napi]
impl Task for Finder {
  type Output = ();
  type JsValue = Option<Window>;

  fn compute(&mut self) -> Result<Self::Output> {
    unsafe {
      EnumWindows(Some(window_finder_proc), transmute(self as *mut Self));
    }
    Ok(())
  }
  fn resolve(&mut self, _env: Env, _output: Self::Output) -> Result<Self::JsValue> {
    Ok(unsafe { transmute(self.output) })
  }
}
