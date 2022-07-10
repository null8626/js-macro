mod sys;

use std::ffi::c_void;

use napi::{
  threadsafe_function::{ErrorStrategy, ThreadsafeFunction},
  Error, Result, Status,
};
pub(crate) use sys::*;

trait Context: Sized {
  type Data: Sized;

  fn new() -> Self;
  unsafe fn valid(this: *mut Self, data: *mut Self::Data) -> bool;
  unsafe fn callback(this: *mut Self, data: *mut Self::Data);
}

unsafe fn get_context<C: Context>(dp: DataPointer) -> Result<*mut C> {
  if dp.is_null() {
    return Err(Error::new(
      Status::GenericFailure,
      String::from("This hook is already running."),
    ));
  } else if (*dp).is_null() {
    *dp = Box::leak(Box::new(C::new())) as *mut C as *mut c_void;
  }

  Ok(*dp as _)
}

unsafe extern "C" fn callback<C: Context>(this: *mut c_void, data: *mut c_void) -> bool {
  C::valid(this as _, data as _)
}

unsafe extern "C" fn async_callback<C: Context>(this: *mut c_void, data: *mut c_void) {
  C::callback(this as _, data as _)
}

unsafe extern "C" fn cleanup_callback<C: Context>(ptr: *mut c_void) {
  let _ = Box::from_raw(ptr as *mut C);
}

pub mod keyboard;
pub mod mouse;
