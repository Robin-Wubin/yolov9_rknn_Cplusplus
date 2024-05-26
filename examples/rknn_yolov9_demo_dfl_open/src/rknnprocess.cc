
// #include "rknnprocess.h"

// char yolo_model_path[256] = "model/yolov8n.rknn";

// int yolo_model_data_size = 0;
// unsigned char *yolo_model_data;
// rknn_input_output_num yolo_io_num;
// rknn_sdk_version yolo_version;
// int yolo_channel = 3;
// int yolo_width = 0;
// int yolo_height = 0;

// rknn_app_context_t rknn_app_ctx;

// #define LABEL_NALE_TXT_PATH "./model/coco_80_labels_list.txt"

// static char *labels[OBJ_CLASS_NUM];

// static char *readLine(FILE *fp, char *buffer, int *len)
// {
//     int ch;
//     int i = 0;
//     size_t buff_len = 0;

//     buffer = (char *)malloc(buff_len + 1);
//     if (!buffer)
//         return NULL; // Out of memory

//     while ((ch = fgetc(fp)) != '\n' && ch != EOF)
//     {
//         buff_len++;
//         void *tmp = realloc(buffer, buff_len + 1);
//         if (tmp == NULL)
//         {
//             free(buffer);
//             return NULL; // Out of memory
//         }
//         buffer = (char *)tmp;

//         buffer[i] = (char)ch;
//         i++;
//     }
//     buffer[i] = '\0';

//     *len = buff_len;

//     // Detect end
//     if (ch == EOF && (i == 0 || ferror(fp)))
//     {
//         free(buffer);
//         return NULL;
//     }
//     return buffer;
// }

// static int readLines(const char *fileName, char *lines[], int max_line)
// {
//     FILE *file = fopen(fileName, "r");
//     char *s;
//     int i = 0;
//     int n = 0;

//     if (file == NULL)
//     {
//         printf("Open %s fail!\n", fileName);
//         return -1;
//     }

//     while ((s = readLine(file, s, &n)) != NULL)
//     {
//         lines[i++] = s;
//         if (i >= max_line)
//             break;
//     }
//     fclose(file);
//     return i;
// }

// static int loadLabelName(const char *locationFilename, char *label[])
// {
//     printf("load lable %s\n", locationFilename);
//     readLines(locationFilename, label, OBJ_CLASS_NUM);
//     return 0;
// }

// int init_post_process()
// {
//     int ret = 0;
//     ret = loadLabelName(LABEL_NALE_TXT_PATH, labels);
//     if (ret < 0)
//     {
//         printf("Load %s failed!\n", LABEL_NALE_TXT_PATH);
//         return -1;
//     }
//     return 0;
// }

// static unsigned char *load_data(FILE *fp, size_t ofst, size_t sz)
// {
//     unsigned char *data;
//     int ret;

//     data = NULL;

//     if (NULL == fp)
//     {
//         return NULL;
//     }

//     ret = fseek(fp, ofst, SEEK_SET);
//     if (ret != 0)
//     {
//         printf("blob seek failure.\n");
//         return NULL;
//     }

//     data = (unsigned char *)malloc(sz);
//     if (data == NULL)
//     {
//         printf("buffer malloc failure.\n");
//         return NULL;
//     }
//     ret = fread(data, 1, sz, fp);
//     return data;
// }

// static unsigned char *load_model(const char *filename, int *model_size)
// {
//     FILE *fp;
//     unsigned char *data;

//     fp = fopen(filename, "rb");
//     if (NULL == fp)
//     {
//         printf("Open file %s failed.\n", filename);
//         return NULL;
//     }

//     fseek(fp, 0, SEEK_END);
//     int size = ftell(fp);

//     data = load_data(fp, 0, size);

//     fclose(fp);

//     *model_size = size;
//     return data;
// }

// double __get_us(struct timeval t) { return (t.tv_sec * 1000000 + t.tv_usec); }

// int yolo_rknn_init()
// {
//     int ret;
//     yolo_model_data = load_model(yolo_model_path, &yolo_model_data_size);
//     /* Create the neural network */
//     ret = rknn_init(&yolo_ctx, yolo_model_data, yolo_model_data_size, 0, NULL);
//     if (ret < 0)
//     {
//         printf("rknn_init error ret=%d\n", ret);
//         return -1;
//     }

//     ret = rknn_query(yolo_ctx, RKNN_QUERY_SDK_VERSION, &yolo_version, sizeof(rknn_sdk_version));
//     if (ret < 0)
//     {
//         printf("rknn_init error ret=%d\n", ret);
//         return -1;
//     }
//     printf("sdk version: %s driver version: %s\n", yolo_version.api_version, yolo_version.drv_version);

//     ret = rknn_query(yolo_ctx, RKNN_QUERY_IN_OUT_NUM, &yolo_io_num, sizeof(yolo_io_num));
//     if (ret < 0)
//     {
//         printf("rknn_init error ret=%d\n", ret);
//         return -1;
//     }
//     printf("model input num: %d, output num: %d\n", yolo_io_num.n_input, yolo_io_num.n_output);

//     rknn_tensor_attr input_attrs[yolo_io_num.n_input];
//     memset(input_attrs, 0, sizeof(input_attrs));
//     for (int i = 0; i < yolo_io_num.n_input; i++)
//     {
//         input_attrs[i].index = i;
//         ret = rknn_query(yolo_ctx, RKNN_QUERY_INPUT_ATTR, &(input_attrs[i]), sizeof(rknn_tensor_attr));
//         if (ret < 0)
//         {
//             printf("rknn_init error ret=%d\n", ret);
//             return -1;
//         }
//         dump_tensor_attr(&(input_attrs[i]));
//     }

//     if (input_attrs[0].fmt == RKNN_TENSOR_NCHW)
//     {
//         printf("model is NCHW input fmt\n");
//         yolo_channel = input_attrs[0].dims[1];
//         yolo_height = input_attrs[0].dims[2];
//         yolo_width = input_attrs[0].dims[3];
//     }
//     else
//     {
//         printf("model is NHWC input fmt\n");
//         yolo_height = input_attrs[0].dims[1];
//         yolo_width = input_attrs[0].dims[2];
//         yolo_channel = input_attrs[0].dims[3];
//     }

//     printf("model input height=%d, width=%d, channel=%d\n", yolo_height, yolo_width, yolo_channel);
//     return 0;
// }

// int detect(cv::Mat *src_image, std::vector<float> &DetectiontRects)
// {

//     int ret;
//     rknn_tensor_attr output_attrs[yolo_io_num.n_output];
//     memset(output_attrs, 0, sizeof(output_attrs));
//     for (int i = 0; i < yolo_io_num.n_output; i++)
//     {
//         output_attrs[i].index = i;
//         ret = rknn_query(yolo_ctx, RKNN_QUERY_OUTPUT_ATTR, &(output_attrs[i]), sizeof(rknn_tensor_attr));
//         dump_tensor_attr(&(output_attrs[i]));
//     }

//     if (ret < 0)
//     {
//         printf("rknn_init error ret=%d\n", ret);
//         return -1;
//     }

//     int status = 0;
//     char *model_name = NULL;
//     size_t actual_size = 0;
//     int img_width = 0;
//     int img_height = 0;
//     int img_channel = 0;
//     struct timeval start_time, stop_time;

//     rga_buffer_t src;
//     rga_buffer_t dst;
//     im_rect src_rect;
//     im_rect dst_rect;
//     memset(&src_rect, 0, sizeof(src_rect));
//     memset(&dst_rect, 0, sizeof(dst_rect));
//     memset(&src, 0, sizeof(src));
//     memset(&dst, 0, sizeof(dst));

//     printf("Read img ...\n");
//     if (!src_image->data)
//     {
//         printf("cv::imread fail!\n");
//         return -1;
//     }

//     img_width = src_image->cols;
//     img_height = src_image->rows;

//     printf("img width = %d, img height = %d\n", img_width, img_height);

//     rknn_input inputs[1];
//     memset(inputs, 0, sizeof(inputs));
//     inputs[0].index = 0;
//     inputs[0].type = RKNN_TENSOR_UINT8;
//     inputs[0].size = yolo_width * yolo_height * yolo_channel;
//     inputs[0].fmt = RKNN_TENSOR_NHWC;
//     inputs[0].pass_through = 0;

//     // You may not need resize when src resulotion equals to dst resulotion
//     void *resize_buf = nullptr;

//     if (img_width != yolo_width || img_height != yolo_height)
//     {
//         printf("resize with RGA!\n");
//         resize_buf = malloc(yolo_height * yolo_width * yolo_channel);
//         memset(resize_buf, 0x00, yolo_height * yolo_width * yolo_channel);

//         src = wrapbuffer_virtualaddr((void *)src_image->data, img_width, img_height, RK_FORMAT_RGB_888);
//         dst = wrapbuffer_virtualaddr((void *)resize_buf, yolo_width, yolo_height, RK_FORMAT_RGB_888);
//         ret = imcheck(src, dst, src_rect, dst_rect);
//         if (IM_STATUS_NOERROR != ret)
//         {
//             printf("%d, check error! %s", __LINE__, imStrError((IM_STATUS)ret));
//             return -1;
//         }
//         IM_STATUS STATUS = imresize(src, dst);
//         inputs[0].buf = resize_buf;
//     }
//     else
//     {
//         inputs[0].buf = (void *)src_image->data;
//     }

//     gettimeofday(&start_time, NULL);
//     rknn_inputs_set(yolo_ctx, yolo_io_num.n_input, inputs);

//     rknn_output outputs[yolo_io_num.n_output];
//     memset(outputs, 0, sizeof(outputs));
//     for (int i = 0; i < yolo_io_num.n_output; i++)
//     {
//         outputs[i].want_float = 0;
//     }

//     ret = rknn_run(yolo_ctx, NULL);
//     ret = rknn_outputs_get(yolo_ctx, yolo_io_num.n_output, outputs, NULL);

//     gettimeofday(&stop_time, NULL);
//     printf("model once run use %f ms\n", (__get_us(stop_time) - __get_us(start_time)) / 1000);

//     // post process
//     std::vector<float> out_scales;
//     std::vector<int32_t> out_zps;
//     for (int i = 0; i < yolo_io_num.n_output; ++i)
//     {
//         out_scales.push_back(output_attrs[i].scale);
//         out_zps.push_back(output_attrs[i].zp);
//     }

//     int8_t *pblob[2];
//     for (int i = 0; i < yolo_io_num.n_output; ++i)
//     {
//         pblob[i] = (int8_t *)outputs[i].buf;
//     }

//     // // 打印outputs的内容
//     // for (int i = 0; i < yolo_io_num.n_output; i++)
//     // {
//     //     printf("outputs[%d].buf=%p, size=%d\n", i, outputs[i].buf, outputs[i].size);
//     //     // 输出其中的buf内容
//     //     for (int j = 0; j < outputs[i].size; j++)
//     //     {
//     //         printf("%d ", pblob[i][j]);
//     //     }
//     //     printf("\n");
//     // }

//     // 将检测结果按照classId、score、xmin1、ymin1、xmax1、ymax1 的格式存放在vector<float>中
//     GetResultRectYolov9 PostProcess;

//     gettimeofday(&start_time, NULL);
//     PostProcess.GetConvDetectionResult(pblob, out_zps, out_scales, DetectiontRects);

//     gettimeofday(&stop_time, NULL);
//     printf("postprocess once run use %f ms\n", (__get_us(stop_time) - __get_us(start_time)) / 1000);

//     // 将 cv::Mat 对象转换为 cv::InputOutputArray
//     // cv::InputOutputArray imageInputOutputArray(*src_image);

//     // for (int i = 0; i < DetectiontRects.size(); i += 6)
//     // {
//     //     int classId = int(DetectiontRects[i + 0]);
//     //     float conf = DetectiontRects[i + 1];
//     //     int xmin = int(DetectiontRects[i + 2] * float(img_width) + 0.5);
//     //     int ymin = int(DetectiontRects[i + 3] * float(img_height) + 0.5);
//     //     int xmax = int(DetectiontRects[i + 4] * float(img_width) + 0.5);
//     //     int ymax = int(DetectiontRects[i + 5] * float(img_height) + 0.5);

//     //     char text1[256];
//     //     sprintf(text1, "%d:%.2f", classId, conf);
//     //     rectangle(imageInputOutputArray, cv::Point(xmin, ymin), cv::Point(xmax, ymax), cv::Scalar(255, 0, 0), 2);
//     //     putText(imageInputOutputArray, text1, cv::Point(xmin, ymin + 15), cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(0, 0, 255), 2);
//     // }

//     // imwrite(save_image_path, src_image);

//     // printf("== obj: %d \n", int(float(DetectiontRects.size()) / 6.0));

//     // // release
//     // ret = rknn_destroy(ctx);

//     // if (model_data)
//     // {
//     //     free(model_data);
//     // }

//     // if (resize_buf)
//     // {
//     //     free(resize_buf);
//     // }

//     return 0;
// }

// void process_connection(boost::asio::ip::tcp::socket *socket)
// {

//     memset(&rknn_app_ctx, 0, sizeof(rknn_app_context_t));
//     // 读取base64字符串
//     boost::asio::streambuf buffer;
//     boost::asio::read_until(*socket, buffer, "\n");
//     std::string base64_string = boost::beast::buffers_to_string(buffer.data());

//     // 打印接收到的base64字符串
//     printf("Received base64 string length: %d\n", base64_string.length());

//     // 解码base64字符串

//     // 创建一个足够大的缓冲区来存储解码后的数据
//     // 计算解码后的大小，这通常是Base64字符串长度的3/4
//     std::size_t decoded_size = base64_string.size() * 3 / 4;
//     std::vector<unsigned char> img_buffer(decoded_size);
//     boost::beast::detail::base64::decode(img_buffer.data(), base64_string.data(), base64_string.size());

//     // 将数据转换为OpenCV图像
//     cv::Mat image = cv::imdecode(cv::Mat(img_buffer), cv::IMREAD_COLOR);

//     std::vector<float> DetectiontRects;

//     detect(&image, DetectiontRects);

//     // 将 cv::Mat 对象转换为 cv::InputOutputArray
//     cv::InputOutputArray imageInputOutputArray(image);

//     // 将DetectiontRects转换成JSON格式并发送回客户端
//     std::string json_string = "[";
//     for (int i = 0; i < DetectiontRects.size(); i += 6)
//     {
//         int classId = int(DetectiontRects[i + 0]);
//         float conf = DetectiontRects[i + 1];
//         int xmin = int(DetectiontRects[i + 2] * float(image.cols) + 0.5);
//         int ymin = int(DetectiontRects[i + 3] * float(image.rows) + 0.5);
//         int xmax = int(DetectiontRects[i + 4] * float(image.cols) + 0.5);
//         int ymax = int(DetectiontRects[i + 5] * float(image.rows) + 0.5);

//         char text1[256];
//         sprintf(text1, "%d:%.2f", classId, conf);
//         rectangle(imageInputOutputArray, cv::Point(xmin, ymin), cv::Point(xmax, ymax), cv::Scalar(255, 0, 0), 2);
//         putText(imageInputOutputArray, text1, cv::Point(xmin, ymin + 15), cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(0, 0, 255), 2);

//         json_string += "{\"classId\":" + std::to_string(classId) + ",";
//         json_string += "\"conf\":" + std::to_string(conf) + ",";
//         json_string += "\"xmin\":" + std::to_string(xmin) + ",";
//         json_string += "\"ymin\":" + std::to_string(ymin) + ",";
//         json_string += "\"xmax\":" + std::to_string(xmax) + ",";
//         json_string += "\"ymax\":" + std::to_string(ymax) + "},";
//     }
//     json_string.pop_back();
//     json_string += "]";

//     imwrite("./out.jpeg", image);

//     printf("== obj: %d \n", int(float(DetectiontRects.size()) / 6.0));

//     boost::asio::write(*socket, boost::asio::buffer(json_string));
// }
