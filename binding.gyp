{
  "targets": [
    {
      "target_name": "cursor",
      "sources": [
        "<(PRODUCT_DIR)/../lib/main.cpp",
        "<(PRODUCT_DIR)/../lib/cursor.cpp"
      ]
    },
    {
      "target_name": "window",
      "sources": [
        "<(PRODUCT_DIR)/../lib/main.cpp",
        "<(PRODUCT_DIR)/../lib/screenshot.cpp",
        "<(PRODUCT_DIR)/../lib/window.cpp"
      ],
      "include_dirs": [ "<(PRODUCT_DIR)/../deps/include" ],
      "libraries": [
        "<(PRODUCT_DIR)/../deps/zlib.lib",
        "<(PRODUCT_DIR)/../deps/libpng.lib",
        "gdi32.lib",
        "user32.lib"
      ],
      "configurations": {
        "Release": {
          "msvs_settings": {
            "VCCLCompilerTool": {
              "RuntimeLibrary": "2"
            }
          }
        }
      }
    }
  ],
  
}
