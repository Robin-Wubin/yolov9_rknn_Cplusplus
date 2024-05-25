#ifndef _RKNN_PROCESS_H_
#define _RKNN_PROCESS_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <vector>
#include "rknn_api.h"
#include "opencv2/core/core.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include "RgaUtils.h"
#include "im2d.h"
#include "postprocess.h"
#include "main.h"
#include "rga.h"
#include "rknn_api.h"
#include <dirent.h>


void process_connection(boost::asio::ip::tcp::socket socket);

#endif
