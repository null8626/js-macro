{
  "variables": {
    "root_dir%": "<!(cd)"
  },
  "targets": [
    {
      "target_name": "cursor",
      "sources": [
        "<(root_dir)/lib/main.cpp",
        "<(root_dir)/lib/cursor.cpp"
      ]
    },
    {
      "target_name": "window",
      "sources": [
        "<(root_dir)/lib/main.cpp",
        "<(root_dir)/lib/screenshot.cpp",
        "<(root_dir)/lib/window.cpp"
      ],
      "include_dirs": [ "deps/include" ],
      "libraries": [
        "-l<(root_dir)/deps/zlib.lib",
        "-l<(root_dir)/deps/libpng.lib",
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
