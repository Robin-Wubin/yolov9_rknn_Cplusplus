{
  "targets": [
    {
      "target_name": "addon",
      "sources": [ "./src/addon.cc", "./src/yolov8.cc", "./src/postprocess.cc" ],
      "cflags!": [ "-fno-rtti", "-fno-exceptions" ],
      "cflags_cc!": [ "-fno-rtti", "-fno-exceptions" ],
      "cflags": [ "-std=c++14", "-fexceptions", "-fPIC" ], 
      "include_dirs": [
        "<!(node -e \"require('nan')\")",
        "<!@(pkg-config --cflags opencv4)",
        "../3rdparty/opencv/opencv-linux-aarch64/include",
        "../3rdparty/rga/RK356X/include",
        "../3rdparty/rk_mpi_mmz/include",
        "./include",
        "/usr/lib/include",
        "/usr/local/include"
      ],
      "libraries": [
        "<!@(pkg-config --libs opencv4)",
        "/usr/lib/librga.so",
        "/usr/lib/librknnrt.so"
      ],
    }
  ]
}