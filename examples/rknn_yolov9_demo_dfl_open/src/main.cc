// Copyright (c) 2021 by Rockchip Electronics Co., Ltd. All Rights Reserved.
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

#include "main.h"
#include "rknnprocess.h"

//请实现一个线程池

void process_images()
{
    boost::asio::io_service io_service;
    boost::asio::ip::tcp::acceptor acceptor(io_service, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), 12345));
    // 打印启动成功
    printf("Server started successfully\n");

    while (true)
    {
        boost::asio::ip::tcp::socket socket(io_service);
        acceptor.accept(socket);

        std::thread t(process_connection, std::move(socket));
        t.detach();
    }
}

int main(int argc, char **argv)
{
    std::thread t(process_images);
    t.join();
    return 0;
}
