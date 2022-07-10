use std::ffi::c_void;
use windows_sys::Win32::Foundation::POINT;

#[repr(C)]
pub(crate) struct KEYBOARD_DATA {
  pub(crate) dwCode: u32,
}

#[repr(C)]
pub(crate) struct MOUSE_DATA {
  pub(crate) pt: POINT,
  pub(crate) wParam: usize,
}

pub(crate) type Callback = unsafe extern "C" fn(*mut c_void, *mut c_void) -> bool;
pub(crate) type AsyncCallback = unsafe extern "C" fn(*mut c_void, *mut c_void);
pub(crate) type CleanupCallback = unsafe extern "C" fn(*mut c_void);
pub(crate) type DataPointer = *mut *mut c_void;

#[link(name = "hooks", kind = "static")]
extern "C" {
  pub(crate) fn InitHooks();
  pub(crate) fn CleanupHooks();

  pub(crate) fn KeyboardHookStart(
    lpfnCallback: Option<Callback>,
    lpfnAsyncCallback: Option<AsyncCallback>,
    lpfnCleanupCallback: Option<CleanupCallback>,
  );
  pub(crate) fn KeyboardHookData() -> DataPointer;
  pub(crate) fn KeyboardHookStop();

  pub(crate) fn MouseHookStart(
    lpfnCallback: Option<Callback>,
    lpfnAsyncCallback: Option<AsyncCallback>,
    lpfnCleanupCallback: Option<CleanupCallback>,
  );
  pub(crate) fn MouseHookData() -> DataPointer;
  pub(crate) fn MouseHookStop();
}
