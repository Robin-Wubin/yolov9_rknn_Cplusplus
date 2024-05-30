#ifndef _RKNN_COMMON_H_
#define _RKNN_COMMON_H_

#include <stdio.h>
#include <stdlib.h>

#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

#include "rga.h"
#include "RgaUtils.h"
#include "im2d.h"

#include "rknn_api.h"

typedef struct
{
    int x_pad;
    int y_pad;
    float scale;
} letterbox_t;

typedef struct
{
    rknn_context rknn_ctx;
    rknn_input_output_num io_num;
    rknn_tensor_attr *input_attrs;
    rknn_tensor_attr *output_attrs;
    int model_channel;
    int model_width;
    int model_height;
    bool is_quant;
} app_context_t;

int read_data_from_file(const char *path, char **out_data);

void dump_tensor_attr(rknn_tensor_attr *attr);

int deal_image(app_context_t *app_ctx, cv::Mat *src_image, rknn_input *inputs, bool *resize);

/**
 * @brief Image pixel format
 *
 */
typedef enum
{
    IMAGE_FORMAT_GRAY8,
    IMAGE_FORMAT_RGB888,
    IMAGE_FORMAT_RGBA8888,
    IMAGE_FORMAT_YUV420SP_NV21,
    IMAGE_FORMAT_YUV420SP_NV12,
} image_format_t;

/**
 * @brief Image buffer
 *
 */
typedef struct
{
    int width;
    int height;
    int width_stride;
    int height_stride;
    image_format_t format;
    unsigned char *virt_addr;
    int size;
    int fd;
} image_buffer_t;

/**
 * @brief Image rectangle
 *
 */
typedef struct
{
    int left;
    int top;
    int right;
    int bottom;
} image_rect_t;

#endif