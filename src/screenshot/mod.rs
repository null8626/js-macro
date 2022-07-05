use crate::Boundaries;
use image::ImageFormat;
use napi::{
  bindgen_prelude::{AsyncTask, ToNapiValue},
  Error, Result, Status,
};
use std::path::Path;
use windows_sys::Win32::UI::WindowsAndMessaging::*;

mod img;
mod writer;
pub(crate) use img::Image;

#[napi(js_name = "screenBoundaries")]
pub fn screen_boundaries() -> Boundaries {
  unsafe {
    let width = GetSystemMetrics(SM_CXSCREEN);
    let height = GetSystemMetrics(SM_CYSCREEN);

    Boundaries { width, height }
  }
}

#[napi(object)]
pub struct ScreenshotOptions {
  pub x: Option<i32>,
  pub y: Option<i32>,
  pub width: Option<i32>,
  pub height: Option<i32>,
}

#[napi]
pub enum Format {
  Png,
  Jpeg,
}

impl Format {
  pub fn from_path(path: &str) -> Option<Self> {
    match Path::new(path).extension()?.to_str()? {
      "png" => Some(Format::Png),
      "jpg" | "jpeg" => Some(Format::Jpeg),
      _ => None,
    }
  }

  pub const fn image_format(self) -> ImageFormat {
    match self {
      Self::Png => ImageFormat::Png,
      Self::Jpeg => ImageFormat::Jpeg,
    }
  }
}

#[derive(Copy, Clone)]
#[napi]
pub struct Screenshot {
  pub(crate) consumed: bool,
  pub(crate) x: i32,
  pub(crate) y: i32,
  pub(crate) width: i32,
  pub(crate) height: i32,
}

#[napi]
impl Screenshot {
  #[napi(constructor)]
  pub fn new(options: Option<ScreenshotOptions>) -> Result<Self> {
    let boundaries = screen_boundaries();

    match options {
      Some(opt) => {
        let x = opt.x.unwrap_or(0);
        let y = opt.y.unwrap_or(0);

        let mut width = opt.width.unwrap_or(boundaries.width);
        let mut height = opt.height.unwrap_or(boundaries.height);

        if x < 0 || y < 0 || width < 0 || height < 0 {
          return Err(Error::new(
            Status::InvalidArg,
            String::from("All numbers from the argument must be positive."),
          ));
        } else if x >= boundaries.width || y >= boundaries.height {
          return Err(Error::new(
            Status::InvalidArg,
            String::from("X or Y is out of range from the monitor's boundaries."),
          ));
        }

        if (x + width) > boundaries.width {
          width = boundaries.width - x;
        }

        if (y + height) > boundaries.height {
          height = boundaries.height - y;
        }

        if x >= width || y >= height {
          return Err(Error::new(
            Status::InvalidArg,
            String::from("X or Y is out of range from the requested Width or Height"),
          ));
        }

        Ok(Self {
          consumed: false,
          x,
          y,
          width,
          height,
        })
      }

      None => Ok(Self {
        consumed: false,
        x: 0,
        y: 0,
        width: boundaries.width,
        height: boundaries.height,
      }),
    }
  }

  #[napi]
  pub fn save(&mut self, s: String) -> Result<AsyncTask<writer::File>> {
    Ok(AsyncTask::new(writer::File::new(self, s)?))
  }

  #[napi(js_name = "arrayBuffer")]
  pub fn array_buffer(&mut self, format: Format) -> AsyncTask<writer::ArrayBuffer> {
    AsyncTask::new(writer::ArrayBuffer::new(self, format))
  }

  #[napi]
  pub fn buffer(&mut self, format: Format) -> AsyncTask<writer::Buffer> {
    AsyncTask::new(writer::Buffer::new(self, format))
  }
}
