
const fs = require("fs");
const addon = require("../rknn_yolov9_demo_dfl_open/build/Release/addon.node");

// 读取图片文件
fs.readFile("./1008.jpeg", (err, data) => {
    if (err) throw err;

    const dataSize = data.length;
    // addon.algorithm(data, dataSize);
    // addon.algorithm(data, dataSize);
    // addon.algorithm(data, dataSize);
    // addon.algorithm(data, dataSize);
    // addon.algorithm(data, dataSize);
    // addon.algorithm(data, dataSize);
    // addon.algorithm(data, dataSize);
    const res = addon.algorithm(data, dataSize);
    console.log(res);
  
    // // for (let i = 0; i < 8; i++) {
    // // 创建TCP客户端
    // const client = net.createConnection({ host: "192.168.0.118", port: 12345, keepAlive: true }, () => {
    //   // 'connect' listener
    //   console.log("connected to server!");
    //   // 发送Base64图片数据
    // });
  
    // client.on("data", data => {
    //   const command = data.toString();
    //   console.log(command, data.toString());
  
    //   if (command === "ready") {
    //     for (let i = 0; i < 8; i++) {
    //       client.write(data + "--bonary\n");
    //       console.log("send data size:" + data.length);
    //     }
    //   }
    // });
  
    // client.on("end", () => {
    //   console.log("disconnected from server");
    // });
  
    // }
  });
