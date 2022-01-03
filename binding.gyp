{
  "targets": [
    {
      "target_name": "cursor",
      "sources": [
        "lib/main.cpp",
        "lib/cursor.cpp"
      ]
    },
    {
      "target_name": "window",
      "sources": [
        "lib/main.cpp",
        "lib/screenshot.cpp",
        "lib/window.cpp"
      ],
      "include_dirs": [ "deps/include" ],
      "libraries": [
        "../deps/zlib.lib",
        "../deps/libpng.lib",
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