// buffer_example.cpp
#include <nan.h>
#include <queue>
#include <mutex>
#include "yolov8.h"

using namespace Nan;
using namespace v8;

rknn_app_context_t rknn_app_ctx;

char model_path[256] = "model/yolov8n.rknn";

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

    printf("%d:%d\n", size, len);


    object_detect_result_list od_results;

    int ret = inference_yolov8_model(&rknn_app_ctx, data, size, &od_results);

    // 获取 Buffer 对象的数据指针和长度

    // 将结果转换为 V8 字符串并返回给 JavaScript
    info.GetReturnValue().Set(Nan::New("result").ToLocalChecked());
}

// 初始化扩展模块
NAN_MODULE_INIT(Initialize)
{

    int ret;

    ret = init_yolov8_model(model_path, &rknn_app_ctx);
    if (ret != 0)
    {
        return Nan::ThrowTypeError("init_yolov8_model fail!");
    }

    // 导出算法调用的方法
    Nan::Set(target, Nan::New("algorithm").ToLocalChecked(), Nan::GetFunction(Nan::New<v8::FunctionTemplate>(AlgorithmMethod)).ToLocalChecked());
}

// 声明扩展模块
NODE_MODULE(addon, Initialize)
