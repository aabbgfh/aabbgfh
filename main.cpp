#include <iostream>
#include"start.h"
#include <fstream>
#include <thread>
#include <string>
#include <filesystem>


int main() {
 
    connect();
    // 获取当前时间
    time_t now = time(0);
    tm ltm;
    // 使用 localtime_s 函数
    localtime_s(&ltm, &now);
    // 获取当前小时数和分钟数
    int currentHour = ltm.tm_hour;
    int currentMinute = ltm.tm_min;

    // 设置监控结束时间为9点30分
    int endHour = 9;
    int endMinute = 30;

    // 判断当前时间是否超过监控结束时间
    if (currentHour > endHour || (currentHour == endHour && currentMinute >= endMinute)) {
        std::cout << "状态监控已结束" << std::endl;
    }
    else {
        std::cout << "状态监控仍在进行中" << std::endl;
    }
    // 清空日志文件
    std::ofstream logFile(logFilePath);
    logFile.close();
    std::string  path = "data";
    // 启动QMT监控线程和下单程序监控线程
    std::thread qmtThread(monitoQMT,path);
    std::thread orderProgramThread(monitorOrderProgram,path);

    // 启动自动退出线程（30分钟后自动退出）
    std::thread autoQuitThread(autoQuit, 30 * 60);

    // 等待所有线程结束
    qmtThread.join();
    orderProgramThread.join();
    autoQuitThread.join();

    return 0;
}