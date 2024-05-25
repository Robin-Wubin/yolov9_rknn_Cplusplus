#include "postprocess.h"
#include <algorithm>
#include <math.h>

#define ZQ_MAX(a, b) ((a) > (b) ? (a) : (b))
#define ZQ_MIN(a, b) ((a) < (b) ? (a) : (b))

// 这个函数的目的是提供一个比标准库函数 exp 更快的实现，用于计算自然指数函数 e^x。
static inline float fast_exp(float x)
{
    // return exp(x);
    union
    {
        uint32_t i;
        float f;
    } v;
    v.i = (12102203.1616540672 * x + 1064807160.56887296);
    return v.f;
}

// 用于计算两个矩形的交并比（Intersection over Union，IoU）
static inline float IOU(float XMin1, float YMin1, float XMax1, float YMax1, float XMin2, float YMin2, float XMax2, float YMax2)
{
    float Inter = 0;
    float Total = 0;
    float XMin = 0;
    float YMin = 0;
    float XMax = 0;
    float YMax = 0;
    float Area1 = 0;
    float Area2 = 0;
    float InterWidth = 0;
    float InterHeight = 0;

    XMin = ZQ_MAX(XMin1, XMin2);
    YMin = ZQ_MAX(YMin1, YMin2);
    XMax = ZQ_MIN(XMax1, XMax2);
    YMax = ZQ_MIN(YMax1, YMax2);

    InterWidth = XMax - XMin;
    InterHeight = YMax - YMin;

    InterWidth = (InterWidth >= 0) ? InterWidth : 0;
    InterHeight = (InterHeight >= 0) ? InterHeight : 0;

    Inter = InterWidth * InterHeight;

    Area1 = (XMax1 - XMin1) * (YMax1 - YMin1);
    Area2 = (XMax2 - XMin2) * (YMax2 - YMin2);

    Total = Area1 + Area2 - Inter;

    return float(Inter) / float(Total);
}

// 将量化的值（quantized value）转换回浮点数。
static float DeQnt2F32(int8_t qnt, int zp, float scale)
{
    return ((float)qnt - (float)zp) * scale;
}

/****** yolov9 ****/
GetResultRectYolov9::GetResultRectYolov9()
{
}

GetResultRectYolov9::~GetResultRectYolov9()
{
}

float GetResultRectYolov9::sigmoid(float x)
{
    return 1 / (1 + fast_exp(-x));
}

int GetResultRectYolov9::GenerateMeshgrid()
{
    int ret = 0;
    if (headNum == 0)
    {
        printf("=== yolov9 Meshgrid  Generate failed! \n");
    }

    for (int index = 0; index < headNum; index++)
    {
        for (int i = 0; i < mapSize[index][0]; i++)
        {
            for (int j = 0; j < mapSize[index][1]; j++)
            {
                meshgrid.push_back(float(j + 0.5));
                meshgrid.push_back(float(i + 0.5));
            }
        }
    }

    printf("=== yolov9 Meshgrid  Generate success! \n");

    return ret;
}

int GetResultRectYolov9::GetConvDetectionResult(int8_t **pBlob, std::vector<int> &qnt_zp, std::vector<float> &qnt_scale, std::vector<float> &DetectiontRects)
{
    int ret = 0;
    if (meshgrid.empty())
    {
        ret = GenerateMeshgrid();
    }

    int gridIndex = -2;
    float xmin = 0, ymin = 0, xmax = 0, ymax = 0;
    float cls_val = 0;
    float cls_max = 0;

    int quant_zp_cls = 0, quant_zp_reg = 0;
    float quant_scale_cls = 0, quant_scale_reg = 0;

    float sfsum = 0;
    float locval = 0;
    float locvaltemp = 0;

    DetectRect temp;
    std::vector<DetectRect> detectRects;

    int row = mapSize[0][0];
    int col = mapSize[0][1];

    printf("=== Detection result: row:%d, col:%d\n", row, col);

    int zp = qnt_zp[0];
    float scale = qnt_scale[0];

    for (int y = 0; y < col; y++)
    {
        int xrow = 0;
        int yrow = 1;
        int wrow = 2;
        int hrow = 3;
        int cx = DeQnt2F32(pBlob[0][xrow * col + y], zp, scale);
        int cy = DeQnt2F32(pBlob[0][yrow * col + y], zp, scale);
        int w = DeQnt2F32(pBlob[0][wrow * col + y], zp, scale);
        int h = DeQnt2F32(pBlob[0][hrow * col + y], zp, scale);

        if (w <= 0 && h <= 0)
        {
            continue;
        }

        float maxClassVal = -1;
        int cls_index = -1;
        for (int i = 4; i < row; i++)
        {
            float val = pBlob[0][i * col + y];
            if (val > maxClassVal)
            {
                maxClassVal = val;
                cls_index = i;
            }
        }
        if (maxClassVal <= 0)
        {
            continue;
        }

        printf("row: %d, cx: %d, cy: %d, w: %d, h: %d, class_id: %d, conf: %d\n, ", y, cx, cy, w, h, cls_index - 4, maxClassVal);
    }

    std::sort(detectRects.begin(), detectRects.end(), [](DetectRect &Rect1, DetectRect &Rect2) -> bool
              { return (Rect1.score > Rect2.score); });

    std::cout << "NMS Before num :" << detectRects.size() << std::endl;
    for (int i = 0; i < detectRects.size(); ++i)
    {
        float xmin1 = detectRects[i].xmin;
        float ymin1 = detectRects[i].ymin;
        float xmax1 = detectRects[i].xmax;
        float ymax1 = detectRects[i].ymax;
        int classId = detectRects[i].classId;
        float score = detectRects[i].score;

        if (classId != -1)
        {
            // 将检测结果按照classId、score、xmin1、ymin1、xmax1、ymax1 的格式存放在vector<float>中
            DetectiontRects.push_back(float(classId));
            DetectiontRects.push_back(float(score));
            DetectiontRects.push_back(float(xmin1));
            DetectiontRects.push_back(float(ymin1));
            DetectiontRects.push_back(float(xmax1));
            DetectiontRects.push_back(float(ymax1));

            for (int j = i + 1; j < detectRects.size(); ++j)
            {
                float xmin2 = detectRects[j].xmin;
                float ymin2 = detectRects[j].ymin;
                float xmax2 = detectRects[j].xmax;
                float ymax2 = detectRects[j].ymax;
                float iou = IOU(xmin1, ymin1, xmax1, ymax1, xmin2, ymin2, xmax2, ymax2);
                if (iou > nmsThresh)
                {
                    detectRects[j].classId = -1;
                }
            }
        }
    }

    return ret;
}
