use crate::screenshot::{Image, Format, Screenshot};
use napi::{Env, Error, JsArrayBuffer, JsBuffer, Result, Status, Task};
use std::intrinsics::transmute;

pub struct ArrayBuffer<'a> {
  sc: &'a mut Screenshot,
  format: Format,
}

impl<'a> ArrayBuffer<'a> {
  #[inline(always)]
  pub fn new(sc: &'a mut Screenshot, format: Format) -> Self {
    Self { sc, format }
  }
}

impl Task for ArrayBuffer<'_> {
  type Output = Vec<u8>;
  type JsValue = JsArrayBuffer;

  fn compute(&mut self) -> Result<Self::Output> {
    let img = Image::new(self.sc)?;

    match img.buffer(self.format) {
      Ok(r) => Ok(r),
      Err(_) => Err(Error::new(Status::GenericFailure, String::from("Cannot write screenshot to buffer."))),
    }
  }

  #[inline(always)]
  fn resolve(&mut self, env: Env, output: Self::Output) -> Result<Self::JsValue> {
    Ok(env.create_arraybuffer_with_data(output)?.into_raw())
  }
}

pub struct Buffer<'a> {
  sc: &'a mut Screenshot,
  format: Format,
}

impl<'a> Buffer<'a> {
  #[inline(always)]
  pub fn new(sc: &'a mut Screenshot, format: Format) -> Self {
    unsafe {
      transmute(ArrayBuffer::new(sc, format))
    }
  }
}

impl<'a> Task for Buffer<'a> {
  type Output = Vec<u8>;
  type JsValue = JsBuffer;

  #[inline(always)]
  fn compute(&mut self) -> Result<Self::Output> {
    Task::compute(unsafe { transmute::<_, &mut ArrayBuffer<'a>>(self) })
  }

  #[inline(always)]
  fn resolve(&mut self, env: Env, output: Self::Output) -> Result<Self::JsValue> {
    Ok(env.create_buffer_with_data(output)?.into_raw())
  }
}

pub struct File<'a> {
  sc: &'a mut Screenshot,
  path: String,
  format: Format,
}

impl<'a> File<'a> {
  pub fn new(sc: &'a mut Screenshot, path: String) -> Result<Self> {
    match Format::from_path(&path) {
      Some(f) => Ok(Self {
        sc,
        path,
        format: f,
      }),

      None => Err(Error::new(
        Status::InvalidArg,
        String::from("Invalid file extension."),
      )),
    }
  }
}

impl Task for File<'_> {
  type Output = ();
  type JsValue = ();

  fn compute(&mut self) -> Result<Self::Output> {
    let img = Image::new(self.sc)?;

    if let Err(_) = img.file(&self.path, self.format) {
      Err(Error::new(
        Status::Unknown,
        String::from("Cannot write screenshot to file."),
      ))
    } else {
      Ok(())
    }
  }

  #[inline(always)]
  fn resolve(&mut self, _env: Env, _output: Self::Output) -> Result<Self::JsValue> {
    Ok(())
  }

  #[inline(always)]
  fn reject(&mut self, _env: Env, err: Error) -> Result<Self::JsValue> {
    Err(err)
  }
}
