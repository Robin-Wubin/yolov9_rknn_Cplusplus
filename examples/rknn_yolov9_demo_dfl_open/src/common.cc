
#include "common.h"

int read_data_from_file(const char *path, char **out_data)
{
    FILE *fp = fopen(path, "rb");
    if (fp == NULL)
    {
        printf("fopen %s fail!\n", path);
        return -1;
    }
    fseek(fp, 0, SEEK_END);
    int file_size = ftell(fp);
    char *data = (char *)malloc(file_size + 1);
    data[file_size] = 0;
    fseek(fp, 0, SEEK_SET);
    if (file_size != fread(data, 1, file_size, fp))
    {
        printf("fread %s fail!\n", path);
        free(data);
        fclose(fp);
        return -1;
    }
    if (fp)
    {
        fclose(fp);
    }
    *out_data = data;
    return file_size;
}

void dump_tensor_attr(rknn_tensor_attr *attr)
{
    printf("  index=%d, name=%s, n_dims=%d, dims=[%d, %d, %d, %d], n_elems=%d, size=%d, fmt=%s, type=%s, qnt_type=%s, "
           "zp=%d, scale=%f\n",
           attr->index, attr->name, attr->n_dims, attr->dims[0], attr->dims[1], attr->dims[2], attr->dims[3],
           attr->n_elems, attr->size, get_format_string(attr->fmt), get_type_string(attr->type),
           get_qnt_type_string(attr->qnt_type), attr->zp, attr->scale);
}

int deal_image(app_context_t *app_ctx, cv::Mat *src_image, rknn_input *inputs, bool *resize)
{

    int ret;

    int width = app_ctx->model_width;
    int height = app_ctx->model_height;
    int channel = app_ctx->model_channel;
    printf("input width = %d, img height = %d, img channel = %d\n", width, height, channel);

    int img_width = 0;
    int img_height = 0;
    int img_channel = 0;

    rga_buffer_t src;
    rga_buffer_t dst;

    memset(&src, 0, sizeof(src));
    memset(&dst, 0, sizeof(dst));

    im_rect src_rect;
    im_rect dst_rect;
    memset(&src_rect, 0, sizeof(src_rect));
    memset(&dst_rect, 0, sizeof(dst_rect));

    img_width = src_image->cols;
    img_height = src_image->rows;

    printf("img width = %d, img height = %d\n", img_width, img_height);

    inputs[0].index = 0;
    inputs[0].type = RKNN_TENSOR_UINT8;
    inputs[0].fmt = RKNN_TENSOR_NHWC;
    inputs[0].size = width * height * channel;

    if (img_width != width || img_height != height)
    {
        printf("resize with RGA! size: %d\n", width * height * channel);
        void *resize_buf = malloc(width * height * channel);
        memset(resize_buf, 0x00, width * height * channel);

        src = wrapbuffer_virtualaddr((void *)src_image->data, img_width, img_height, RK_FORMAT_RGB_888);
        dst = wrapbuffer_virtualaddr(resize_buf, width, height, RK_FORMAT_RGB_888);
        ret = imcheck(src, dst, src_rect, dst_rect);
        if (IM_STATUS_NOERROR != ret)
        {
            printf("%d, check error! %s", __LINE__, imStrError((IM_STATUS)ret));
            return -1;
        }
        IM_STATUS STATUS = imresize(src, dst);
        inputs[0].buf = resize_buf;
        *resize = true;
    }
    else
    {
        inputs[0].buf = (void *)src_image->data;
    }

    return ret;
}
