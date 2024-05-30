// Copyright (c) 2023 by Rockchip Electronics Co., Ltd. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef _RKNN_YOLOV8_H_
#define _RKNN_YOLOV8_H_

#include "rknn_api.h"
#include <sys/time.h>
#include "common.h"

#define OBJ_CLASS_NUM 80
#define OBJ_NAME_MAX_SIZE 64
#define OBJ_NUMB_MAX_SIZE 128

#define NMS_THRESH 0.45
#define BOX_THRESH 0.25

typedef struct
{
    int left;
    int top;
    int right;
    int bottom;
} image_rect_t;

typedef struct
{
    image_rect_t box;
    float prop;
    int cls_id;
} object_detect_result;

typedef struct
{
    int id;
    int count;
    object_detect_result results[OBJ_NUMB_MAX_SIZE];
} object_detect_result_list;

int init_yolov8_model(const char *model_path, app_context_t *app_ctx);

int release_yolov8_model(app_context_t *app_ctx);

int inference_yolov8_model(app_context_t *app_ctx, unsigned char *data, int size, object_detect_result_list *od_results);

#endif //_RKNN_DEMO_YOLOV8_H_