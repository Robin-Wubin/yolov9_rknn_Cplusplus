#ifndef _RKNN_DEMO_MOBILENET_H_
#define _RKNN_DEMO_MOBILENET_H_

#include "rknn_api.h"
#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include "common.h"

typedef struct box_rect_t
{
    int left;   ///< Most left coordinate
    int top;    ///< Most top coordinate
    int right;  ///< Most right coordinate
    int bottom; ///< Most bottom coordinate
} box_rect_t;

typedef struct ponit_t
{
    int x;
    int y;
} ponit_t;

typedef struct retinaface_object_t
{
    int cls;
    box_rect_t box;
    float score;
    ponit_t ponit[5];
} retinaface_object_t;

typedef struct
{
    int count;
    retinaface_object_t object[128];
} retinaface_result;

int init_retinaface_model(const char *model_path, app_context_t *app_ctx);

int release_retinaface_model(app_context_t *app_ctx);

int inference_retinaface_model(app_context_t *app_ctx, cv::Mat *src_image, retinaface_result *out_result);

int post_process_retinaface(app_context_t *app_ctx, int width, int height, rknn_output outputs[], retinaface_result *result, letterbox_t *letter_box);

#endif //_RKNN_DEMO_MOBILENET_H_