{
  "targets": [
    {
      "target_name": "addon",
      "sources": [ "./src/addon.cc", "./src/common.cc", "./src/image_utils.cc", "./src/yolov8.cc", "./src/postprocess.cc", "./src/retinaface.cc" ],
      "cflags!": [ "-fno-rtti", "-fno-exceptions" ],
      "cflags_cc!": [ "-fno-rtti", "-fno-exceptions" ],
      "cflags": [ "-std=c++14", "-fexceptions", "-fPIC" ], 
      "include_dirs": [
        "<!(node -e \"require('nan')\")",
        "../3rdparty/rga/RK356X/include",
        "../3rdparty/rk_mpi_mmz/include",
        "../3rdparty/stb",
        "../3rdparty/jpeg_turbo/include",
        "./include",
        "/usr/include/opencv4",
        "/usr/lib/include",
        "/usr/local/include"
      ],
      "libraries": [
        "<!@(pkg-config --libs opencv4)",
        "/usr/lib/librga.so",
        "/usr/lib/librknnrt.so",
        "/root/yolov9_rknn_Cplusplus/examples/3rdparty/jpeg_turbo/Linux/aarch64/libturbojpeg.a"
      ],
    }
  ]
}