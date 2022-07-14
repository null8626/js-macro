#![allow(
  non_snake_case,
  non_upper_case_globals,
  non_camel_case_types,
  dead_code
)]
#![deny(clippy::all)]

use napi::{Env, Result};
use windows_sys::Win32::System::Console::*;

#[macro_use]
extern crate napi_derive;

extern crate image;
extern crate napi;
extern crate windows_sys;

mod clipboard;
mod hooks;
mod keyboard;
mod mouse;
mod screenshot;
mod window;

static mut INITIALIZED: bool = false;

#[napi]
unsafe fn cleanup() {
  if INITIALIZED {
    INITIALIZED = false;
    SetConsoleCtrlHandler(Some(ctrl_handler), 0);
    hooks::CleanupHooks();
  }
}

unsafe extern "system" fn ctrl_handler(typ: u32) -> i32 {
  match typ {
    CTRL_C_EVENT | CTRL_CLOSE_EVENT | CTRL_LOGOFF_EVENT | CTRL_SHUTDOWN_EVENT => {
      cleanup();
    }

    _ => {}
  }

  0
}

#[napi]
fn init(mut env: Env) -> Result<()> {
  unsafe {
    if !INITIALIZED {
      INITIALIZED = true;

      SetConsoleCtrlHandler(Some(ctrl_handler), 1);
      env.add_env_cleanup_hook((), |_| cleanup())?;
    }

    hooks::InitHooks();

    Ok(())
  }
}
