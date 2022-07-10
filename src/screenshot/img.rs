use core::slice;
use std::{
  convert::AsRef,
  ffi::c_void,
  io::Cursor,
  mem::{size_of, MaybeUninit},
  ops::Deref,
  path::Path,
};

use super::Screenshot;
use image::{ImageBuffer, ImageFormat, ImageResult, Rgba};
use napi::{Error, Result, Status};
use windows_sys::Win32::{Graphics::Gdi::*, System::Memory::*};

#[derive(Copy, Clone)]
pub(crate) struct RawPixel {
  pub(crate) blue: u8,
  pub(crate) green: u8,
  pub(crate) red: u8,
  _unused: u8,
}

pub(crate) struct Pixels {
  ptr: *const u8,
  size: usize,
}

impl Pixels {
  #[inline(always)]
  pub(crate) fn new(size: usize) -> Self {
    unsafe {
      Self {
        ptr: HeapAlloc(GetProcessHeap(), 0, size) as _,
        size,
      }
    }
  }

  #[inline(always)]
  pub(crate) fn as_ptr(&mut self) -> *mut c_void {
    self.ptr as _
  }

  #[inline(always)]
  pub(crate) fn as_pixels(&mut self) -> &mut [RawPixel] {
    unsafe { slice::from_raw_parts_mut(self.ptr as *mut RawPixel, self.size / 4) }
  }

  #[inline(always)]
  pub(crate) fn as_slice(&self) -> &[u8] {
    unsafe { slice::from_raw_parts(self.ptr as _, self.size) }
  }
}

impl Deref for Pixels {
  type Target = [u8];

  #[inline(always)]
  fn deref(&self) -> &Self::Target {
    self.as_slice()
  }
}

impl Drop for Pixels {
  #[inline(always)]
  fn drop(&mut self) {
    unsafe {
      HeapFree(GetProcessHeap(), 0, self.ptr as _);
    }
  }
}

pub(crate) struct Image {
  image: ImageBuffer<Rgba<u8>, Pixels>,
}

impl Image {
  pub(crate) fn new(sc: &mut Screenshot) -> Result<Self> {
    if sc.consumed {
      return Err(Error::new(
        Status::InvalidArg,
        String::from("Screenshot object already consumed."),
      ));
    }

    unsafe {
      let hdc = CreateCompatibleDC(0);
      let bitmap = CreateCompatibleBitmap(GetDC(0), sc.width, sc.height);

      SelectObject(hdc, bitmap);

      BitBlt(
        hdc,
        0,
        0,
        sc.width,
        sc.height,
        GetDC(0),
        sc.x,
        sc.y,
        SRCCOPY,
      );

      let mut bmp: BITMAP = MaybeUninit::uninit().assume_init();
      let mut info: BITMAPINFO = MaybeUninit::zeroed().assume_init();

      let current_dc = CreateCompatibleDC(0);
      SelectObject(current_dc, bitmap);

      GetObjectA(
        bitmap,
        size_of::<BITMAP>() as _,
        &mut bmp as *mut BITMAP as *mut c_void,
      );

      bmp.bmWidth = sc.width;
      bmp.bmHeight = -sc.height;

      info.bmiHeader.biSize = size_of::<BITMAPINFOHEADER>() as u32;
      info.bmiHeader.biWidth = bmp.bmWidth;
      info.bmiHeader.biHeight = bmp.bmHeight;
      info.bmiHeader.biCompression = BI_RGB as u32;
      info.bmiHeader.biPlanes = 1;
      info.bmiHeader.biBitCount = 32;
      info.bmiHeader.biSizeImage = (sc.width * sc.height * 4) as u32;

      let mut pixels = Pixels::new(info.bmiHeader.biSizeImage as _);

      GetDIBits(
        current_dc,
        bitmap,
        0,
        sc.height as _,
        pixels.as_ptr(),
        &mut info,
        DIB_RGB_COLORS,
      );

      DeleteDC(hdc);
      DeleteDC(current_dc);
      DeleteObject(bitmap);

      sc.consumed = true;

      for p in pixels.as_pixels() {
        let blue = p.blue;

        p.blue = p.red;
        p.red = blue;
      }

      match ImageBuffer::from_raw(sc.width as _, sc.height as _, pixels) {
        Some(image) => Ok(Self { image }),

        None => Err(Error::new(
          Status::GenericFailure,
          String::from("Cannot generate image from screenshot buffer."),
        )),
      }
    }
  }

  #[inline(always)]
  pub(crate) fn file<P: AsRef<Path>>(&self, p: P, format: ImageFormat) -> ImageResult<()> {
    self.image.save_with_format(p, format)
  }

  pub(crate) fn buffer(&self, format: ImageFormat) -> ImageResult<Vec<u8>> {
    let mut buf = Vec::new();
    self.image.write_to(&mut Cursor::new(&mut buf), format)?;

    Ok(buf)
  }
}
