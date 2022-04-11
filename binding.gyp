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
        "<(root_dir)/lib/window.cpp",
        "<(root_dir)/lib/async.cpp"
      ],
      "include_dirs": [ "<(root_dir)/include" ],
      "libraries": [
        "-l<(root_dir)/bin/zlib1.lib",
        "-l<(root_dir)/bin/libpng16_static.lib",
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
    },
	{
      "target_name": "keyboard",
      "sources": [
        "<(root_dir)/lib/main.cpp",
        "<(root_dir)/lib/keyboard.cpp"
      ],
      "libraries": [
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
    },
	{
      "target_name": "clipboard",
      "sources": [
        "<(root_dir)/lib/main.cpp",
        "<(root_dir)/lib/clipboard.cpp",
        "<(root_dir)/lib/async.cpp"
      ],
      "libraries": [
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
  ]
}
