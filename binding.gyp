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
      "include_dirs": [ "<(root_dir)/include" ],
      "libraries": [
        "-l<(root_dir)/zlib.lib",
        "-l<(root_dir)/libpng.lib",
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
