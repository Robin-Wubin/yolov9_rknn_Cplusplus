#include "main.h"
#include "rknnprocess.h"
#include "yolov8.h"

// 使用简化的命名空间
using boost::asio::ip::tcp;

std::queue<std::shared_ptr<tcp::socket>> requestQueue;
std::mutex queueMutex;
std::condition_variable queueCondition;

char model_path[256] = "model/yolov8n.rknn";

void handleRequest()
{

    rknn_app_context_t rknn_app_ctx;

    int ret;

    ret = init_yolov8_model(model_path, &rknn_app_ctx);
    if (ret != 0)
    {
        printf("init_yolov8_model fail! ret=%d model_path=%s\n", ret, model_path);
        return;
    }

    while (true)
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        queueCondition.wait(lock, []
                            { return !requestQueue.empty(); });

        auto socket = requestQueue.front();
        tcp::socket *raw_socket = socket.get();
        requestQueue.pop();
        lock.unlock();
        
        // 读取base64字符串
        boost::asio::streambuf buffer;
        boost::asio::read_until(*socket, buffer, "\n");
        std::string base64_string = boost::beast::buffers_to_string(buffer.data());

        
        std::size_t decoded_size = base64_string.size() * 3 / 4;
        std::vector<unsigned char> img_buffer(decoded_size);
        boost::beast::detail::base64::decode(img_buffer.data(), base64_string.data(), base64_string.size());

            // 获取socket传输的内容，并传给rknnprocess进行分析
            
        object_detect_result_list od_results;

        ret = inference_yolov8_model(&rknn_app_ctx, &img_buffer, &od_results);
        

        // 响应 "ok" 字符串
        // std::string response = "ok";
        // boost::asio::write(*socket, boost::asio::buffer(response));

        // 关闭连接
        socket->close();
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