extern crate cc;
extern crate napi_build;

use std::{env, fs, io::Result};

fn main() -> Result<()> {
  napi_build::setup();

  let mut files = fs::read_dir("./src/hooks")?
    .filter_map(|x| Some(x.ok()?.path().to_str()?.to_owned()))
    .filter(|x| x.ends_with(".c"))
    .collect::<Vec<_>>();

  for f in files.iter_mut() {
    println!("cargo:rerun-if-changed={}", f);

    f.pop();
    println!("cargo:rerun-if-changed={}h", f);
    f.push('c');
  }

  let manifest_dir = unsafe { env::var("CARGO_MANIFEST_DIR").unwrap_unchecked() };

  cc::Build::new()
    .files(files)
    .opt_level(2)
    .out_dir(&manifest_dir)
    .compile("hooks");

  println!("cargo:rustc-link-search=native={}", manifest_dir);

  Ok(())
}
