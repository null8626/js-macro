use super::Window;
use napi::{Env, Result, Task};
use std::intrinsics::transmute;
use windows_sys::Win32::UI::WindowsAndMessaging::*;

pub struct Collector {
  output: Vec<Window>,
}

impl Collector {
  #[inline(always)]
  pub fn new() -> Self {
    Self { output: Vec::new() }
  }
}

impl Default for Collector {
  #[inline(always)]
  fn default() -> Self {
    Self::new()
  }
}

unsafe extern "system" fn window_collector_proc(hwnd: isize, lparam: isize) -> i32 {
  if IsWindowVisible(hwnd) != 0 {
    let ptr: *mut Collector = transmute(lparam);

    (*ptr).output.push(transmute(hwnd));
  }

  1
}

#[napi]
impl Task for Collector {
  type Output = ();
  type JsValue = Vec<Window>;

  fn compute(&mut self) -> Result<Self::Output> {
    unsafe {
      EnumWindows(Some(window_collector_proc), transmute(self as *mut Self));
    }

    Ok(())
  }
  fn resolve(&mut self, _env: Env, _output: Self::Output) -> Result<Self::JsValue> {
    Ok(self.output.clone())
  }
}
