
#include "common.h"

int init_post_process();

char *coco_cls_to_name(int cls_id);


int post_process(app_context_t *app_ctx, void *outputs, letterbox_t *letter_box, float conf_threshold, float nms_threshold, object_detect_result_list *od_results);