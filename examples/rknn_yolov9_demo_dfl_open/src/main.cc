#include "main.h"
#include "rknnprocess.h"
#include "yolov8.h"
#include "face_landmarks.h"

// 使用简化的命名空间
using boost::asio::ip::tcp;

std::queue<std::shared_ptr<tcp::socket>> requestQueue;
std::mutex queueMutex;
std::condition_variable queueCondition;

char model_path[256] = "model/yolov8n.rknn";

void handleRequestThread(tcp::socket *socket)
{

    rknn_app_context_t rknn_app_ctx;

    int ret;

    ret = init_yolov8_model(model_path, &rknn_app_ctx);
    if (ret != 0)
    {
        printf("init_yolov8_model fail! ret=%d model_path=%s\n", ret, model_path);
        return;
    }

    ret = init_face_detector();
    if (ret != 0)
    {
        printf("init_face_detector fail! ret=%d\n", ret);
        return;
    }

    // 发送 "ready" 字符串
    std::string response = "ready";
    boost::asio::write(*socket, boost::asio::buffer(response));

    // 循环读取socket穿过来的图像buf数据（以"--bonary\n"分割），并进行分析
    while (true)
    {
        // 读取图像数据
        boost::asio::streambuf buf;
        boost::asio::read_until(*socket, buf, "--bonary\n");

        // buf中移除尾部的“--bonary\n”
        buf.consume(9);

        // buf 转成 string
        std::string data(boost::asio::buffers_begin(buf.data()), boost::asio::buffers_end(buf.data()));

        // 打印字符串
        printf("data.size()=%s\n", data);

        printf("buf.size()=%d\n", buf.size());

        // 将 streambuf 转换为 std::vector<unsigned char>
        // Convert streambuf to std::vector
        std::vector<unsigned char> buffer(boost::asio::buffer_cast<const unsigned char *>(buf.data()), boost::asio::buffer_cast<const unsigned char *>(buf.data()) + buf.size());

        printf("buffer.size()=%d\n", buffer.size());
        // Convert std::vector to cv::Mat
        cv::Mat img_data = cv::Mat(buffer);

        printf("img_data.size()=%d\n", img_data.size());

        object_detect_result_list od_results;

        int ret = inference_yolov8_model(&rknn_app_ctx, &img_data, &od_results);

        // 响应 "ok" 字符串
        // std::string response = "ok";
        // boost::asio::write(*socket, boost::asio::buffer(response));
    }
}

void handleRequest()
{

    while (true)
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        queueCondition.wait(lock, []
                            { return !requestQueue.empty(); });

        auto socket = requestQueue.front();
        tcp::socket *raw_socket = socket.get();
        requestQueue.pop();
        lock.unlock();

        // 新建一个线程，并把raw_socket传到线程内进行处理
        std::thread t1(handleRequestThread, raw_socket);

        t1.detach();
    }
}

void startAccept(boost::asio::io_context &io_context, tcp::acceptor &acceptor)
{
    auto socket = std::make_shared<tcp::socket>(io_context);
    acceptor.async_accept(*socket,
                          [&io_context, &acceptor, socket](const boost::system::error_code &error)
                          {
                              if (!error)
                              {
                                  {
                                      std::lock_guard<std::mutex> lock(queueMutex);
                                      requestQueue.push(socket);
                                  }
                                  queueCondition.notify_one();
                              }
                              startAccept(io_context, acceptor);
                          });
}

int main()
{
    try
    {

        init_post_process();

        boost::asio::io_context io_context;

        // 监听端口 12345
        tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 12345));

        // 启动接受连接
        startAccept(io_context, acceptor);

        // 启动工作线程处理请求
        std::thread workerThread(handleRequest);

        // 运行 io_context
        io_context.run();

        // 确保工作线程结束
        workerThread.join();
    }
    catch (std::exception &e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}