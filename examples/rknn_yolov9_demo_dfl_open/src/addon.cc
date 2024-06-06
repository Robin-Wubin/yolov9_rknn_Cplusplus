// buffer_example.cpp
#include <nan.h>
#include <queue>
#include <mutex>
#include <iostream>
#include "yolov8.h"
#include "retinaface.h"
#include "postprocess.h"
#include "image_utils.h"

using namespace Nan;
using namespace v8;

app_context_t rknn_app_ctx;
app_context_t face_app_ctx;

char yolov_path[256] = "model/yolov8n.rknn";
char face_path[256] = "model/RetinaFace.rknn";

// 示例：从图像缓冲区中裁剪特定区域并获取裁剪区域的缓冲区数据
void cropAndGetBuffer(std::vector<unsigned char> *imageData, cv::Rect *cropRect, std::vector<unsigned char> *outputBuffer)
{

    cv::Mat img_data = cv::Mat(*imageData);
    // // 将数据转换为OpenCV图像
    cv::Mat image = cv::imdecode(img_data, cv::IMREAD_COLOR);

    printf("load image, %d， %d\n", image.cols, image.rows);

    // 将图像缓冲区数据转换为 cv::Mat 对象
    //     cv::Mat image(height, width, channels == 3 ? CV_8UC3 : CV_8UC1, const_cast<unsigned char *>(imageData));

    // // 检查矩形框是否在图像范围内
    if (cropRect->x < 0 || cropRect->y < 0 ||
        cropRect->x + cropRect->width > image.cols ||
        cropRect->y + cropRect->height > image.rows)
    {
        std::cerr << "Crop rectangle is out of image bounds" << std::endl;
        return;
    }

    // // 裁剪图像的特定区域
    cv::Mat croppedImage = image(*cropRect);

    // // 将裁剪后的图像数据转换为缓冲区数据
    // // 这里我们假设裁剪后的图像是 BGR 格式的 JPEG 格式保存
    std::vector<int> compression_params = {cv::IMWRITE_JPEG_QUALITY, 95};
    cv::imencode(".jpg", croppedImage, *outputBuffer, compression_params);
}

// 定义算法函数，接受图像的 buffer 并返回结果

// 定义算法调用的回调函数
NAN_METHOD(AlgorithmMethod)
{
    // 检查传入的参数是否是 Buffer 对象
    if (!info[0]->IsUint8Array())
    {
        Nan::ThrowTypeError("Argument must be a Buffer object.");
        return;
    }
    // 检查第二个参数是不是整数
    if (!info[1]->IsUint32())
    {
        Nan::ThrowTypeError("Argument must be a Uint32.");
        return;
    }

    // 获取第一个参数并转换为 V8 的 Buffer 对象
    v8::Local<v8::Uint8Array> bufferObj = info[0].As<v8::Uint8Array>();
    v8::Local<v8::ArrayBuffer> arrayBuffer = bufferObj->Buffer();
    std::shared_ptr<v8::BackingStore> backingStore = arrayBuffer->GetBackingStore();
    unsigned char *data = static_cast<unsigned char *>(backingStore->Data());

    int len = backingStore->ByteLength();

    // 获取输入的整形为size
    int size = Nan::To<int>(info[1]).FromJust();

    image_buffer_t src_image;
    memset(&src_image, 0, sizeof(image_buffer_t));
    int ret = read_image(&src_image, data, size);

    // // 打印buffer
    std::vector<unsigned char> png_data(data, data + size);

    if (ret < 0)
    {
        Nan::ThrowTypeError("The Buffer is not image.");
        return;
    }

    object_detect_result_list od_results;

    ret = inference_yolov8_model(&rknn_app_ctx, &src_image, &od_results);

    // 遍历od_results并打印里面的内容
    for (int i = 0; i < od_results.count; i++)
    {

        object_detect_result det_result = od_results.results[i];
        if (det_result.cls_id == 0)
        {
            int x = det_result.box.left;
            int y = det_result.box.top;
            int width = det_result.box.right - det_result.box.left;
            int height = det_result.box.bottom - det_result.box.top;
            cv::Rect personRect(x, y, width, height);

            std::vector<unsigned char> out_img;

            cropAndGetBuffer(&png_data, &personRect, &out_img);
            cv::Mat img = cv::imdecode(out_img, cv::IMREAD_COLOR);

            // 保存out_img为图片
            // cv::imwrite("person.jpg", img);

            retinaface_result face_results;

            ret = inference_retinaface_model(&face_app_ctx, &img, &face_results);
        }
        printf("%s @ (%d %d %d %d) %.3f\n", coco_cls_to_name(det_result.cls_id),
               det_result.box.left, det_result.box.top,
               det_result.box.right, det_result.box.bottom,
               det_result.prop);
    }

    // 获取 Buffer 对象的数据指针和长度

    // 将结果转换为 V8 字符串并返回给 JavaScript
    info.GetReturnValue().Set(Nan::New("result").ToLocalChecked());
}

// 初始化扩展模块
NAN_MODULE_INIT(Initialize)
{

    int ret;

    init_post_process();

    ret = init_yolov8_model(yolov_path, &rknn_app_ctx);
    if (ret != 0)
    {
        return Nan::ThrowTypeError("init_yolov8_model fail!");
    }

    ret = init_retinaface_model(face_path, &face_app_ctx);
    if (ret != 0)
    {
        return Nan::ThrowTypeError("init_retinaface_model fail!");
    }

    // 导出算法调用的方法
    Nan::Set(target, Nan::New("algorithm").ToLocalChecked(), Nan::GetFunction(Nan::New<v8::FunctionTemplate>(AlgorithmMethod)).ToLocalChecked());
}

// 声明扩展模块
NODE_MODULE(addon, Initialize)
