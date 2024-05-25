#ifndef _POSTPROCESS_H_
#define _POSTPROCESS_H_

#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <vector>

typedef signed char int8_t;
typedef unsigned int uint32_t;

typedef struct
{
    float xmin;
    float ymin;
    float xmax;
    float ymax;
    float score;
    int classId;
} DetectRect;

// yolov9
class GetResultRectYolov9
{
public:
    GetResultRectYolov9();

    ~GetResultRectYolov9();

    int GenerateMeshgrid();

    int GetConvDetectionResult(int8_t **pBlob, std::vector<int> &qnt_zp, std::vector<float> &qnt_scale, std::vector<float> &DetectiontRects);

    float sigmoid(float x);

private:
    std::vector<float> meshgrid;

    const int class_num = 80;
    int headNum = 1;

    int input_w = 640;
    int input_h = 640;
    int strides[3] = {8, 16, 32};
    int mapSize[1][2] = {{84, 8400}};

    std::vector<float> regdfl;
    float regdeq[16] = {0};

    float nmsThresh = 0.45;
    float objectThresh = 0.5;
};

#endif
