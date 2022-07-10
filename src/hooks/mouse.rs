#[napi]
pub mod clicklogger {
  use super::super::*;
  use crate::window::Coordinates;
  use napi::{
    threadsafe_function::{ThreadSafeCallContext, ThreadsafeFunctionCallMode},
    Error, JsFunction, Result, Status,
  };
  use std::{collections::HashMap, str::FromStr};
  use windows_sys::Win32::UI::WindowsAndMessaging::*;

  #[napi(object)]
  pub struct Data {
    pub coordinates: Coordinates,
    pub left: bool,
  }

  #[derive(Hash, Eq, PartialEq)]
  #[repr(u8)]
  enum ListenerKind {
    Left,
    Middle,
    Right,
  }

  impl From<usize> for ListenerKind {
    fn from(wparam: usize) -> Self {
      match wparam as u32 {
        WM_LBUTTONUP => Self::Left,
        WM_MBUTTONUP => Self::Middle,
        WM_RBUTTONUP => Self::Right,
        _ => unimplemented!(),
      }
    }
  }

  impl FromStr for ListenerKind {
    type Err = Error;

    fn from_str(s: &str) -> Result<Self> {
      match s {
        "left" => Ok(Self::Left),
        "middle" => Ok(Self::Middle),
        "right" => Ok(Self::Right),
        _ => Err(Error::new(
          Status::InvalidArg,
          String::from("Invalid listener type. Accepted types: 'left', 'middle', 'right'"),
        )),
      }
    }
  }

  struct LocalContext {
    callbacks: HashMap<ListenerKind, Vec<ThreadsafeFunction<Coordinates, ErrorStrategy::Fatal>>>,
  }

  impl LocalContext {
    #[inline(always)]
    fn contains_callbacks(&self) -> bool {
      self.callbacks.len() > 0
    }

    fn insert(
      &mut self,
      kind: ListenerKind,
      data: ThreadsafeFunction<Coordinates, ErrorStrategy::Fatal>,
    ) {
      if self.callbacks.contains_key(&kind) {
        let vec = unsafe { self.callbacks.get_mut(&kind).unwrap_unchecked() };

        vec.push(data);
      } else {
        self.callbacks.insert(kind, vec![data]);
      }
    }
  }

  impl Context for LocalContext {
    type Data = MOUSE_DATA;

    #[inline(always)]
    fn new() -> Self {
      Self {
        callbacks: HashMap::new(),
      }
    }

    #[inline(always)]
    unsafe fn valid(this: *mut Self, data: *mut Self::Data) -> bool {
      let value = ListenerKind::from((*data).wParam);

      (*this).callbacks.contains_key(&value)
    }

    #[inline(always)]
    unsafe fn callback(this: *mut Self, data: *mut Self::Data) {
      let value = ListenerKind::from((*data).wParam);

      for cb in (*this).callbacks.get(&value).unwrap_unchecked().iter() {
        cb.call(
          Coordinates {
            x: (*data).pt.x,
            y: (*data).pt.y,
          },
          ThreadsafeFunctionCallMode::Blocking,
        );
      }
    }
  }

  #[napi(js_name = "onClick")]
  pub unsafe fn on_click(typ: String, callback: JsFunction) -> Result<()> {
    let data = get_context::<LocalContext>(MouseHookData())?;
    let kind = typ.parse()?;
    let tsfn = callback
      .create_threadsafe_function(0, |ctx: ThreadSafeCallContext<Coordinates>| {
        Ok(vec![ctx.value])
      })?;

    (*data).insert(kind, tsfn);

    Ok(())
  }

  #[napi]
  pub unsafe fn start() -> Result<()> {
    let data = get_context::<LocalContext>(MouseHookData())?;

    if !(*data).contains_callbacks() {
      return Err(Error::new(
        Status::InvalidArg,
        String::from("The hook contains no callbacks as of now."),
      ));
    }

    MouseHookStart(
      Some(callback::<LocalContext>),
      Some(async_callback::<LocalContext>),
      Some(cleanup_callback::<LocalContext>),
    );

    Ok(())
  }

  #[napi]
  pub unsafe fn stop() {
    MouseHookStop();
  }
}
