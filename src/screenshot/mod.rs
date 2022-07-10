use crate::window::Boundaries;
use image::ImageFormat;
use napi::{bindgen_prelude::AsyncTask, Error, Result, Status};
use windows_sys::Win32::UI::WindowsAndMessaging::*;

mod img;
mod writer;
pub(crate) use img::Image;

#[napi(js_name = "screenBoundaries")]
pub unsafe fn screen_boundaries() -> Boundaries {
  let width = GetSystemMetrics(SM_CXSCREEN);
  let height = GetSystemMetrics(SM_CYSCREEN);

  Boundaries { width, height }
}

#[napi(object)]
pub struct ScreenshotOptions {
  pub x: Option<i32>,
  pub y: Option<i32>,
  pub width: Option<i32>,
  pub height: Option<i32>,
}

fn get_format(s: &str) -> Result<ImageFormat> {
  match s {
    "png" => Ok(ImageFormat::Png),
    "jpeg" | "jpg" => Ok(ImageFormat::Jpeg),
    _ => Err(Error::new(
      Status::InvalidArg,
      String::from("Invalid format. expected formats: 'png', 'jpeg', 'jpg'"),
    )),
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
    let boundaries = unsafe { screen_boundaries() };

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
  pub fn array_buffer(&mut self, format: String) -> Result<AsyncTask<writer::ArrayBuffer>> {
    Ok(AsyncTask::new(writer::ArrayBuffer::new(
      self,
      get_format(&format)?,
    )))
  }

  #[napi]
  pub fn buffer(&mut self, format: String) -> Result<AsyncTask<writer::Buffer>> {
    Ok(AsyncTask::new(writer::Buffer::new(
      self,
      get_format(&format)?,
    )))
  }
}
