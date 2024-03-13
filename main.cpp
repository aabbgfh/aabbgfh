#include <iostream>
#include"start.h"
#include <fstream>
#include <thread>
#include <string>
#include <filesystem>


int main() {
 
    connect();
    // ��ȡ��ǰʱ��
    time_t now = time(0);
    tm ltm;
    // ʹ�� localtime_s ����
    localtime_s(&ltm, &now);
    // ��ȡ��ǰСʱ���ͷ�����
    int currentHour = ltm.tm_hour;
    int currentMinute = ltm.tm_min;

    // ���ü�ؽ���ʱ��Ϊ9��30��
    int endHour = 9;
    int endMinute = 30;

    // �жϵ�ǰʱ���Ƿ񳬹���ؽ���ʱ��
    if (currentHour > endHour || (currentHour == endHour && currentMinute >= endMinute)) {
        std::cout << "״̬����ѽ���" << std::endl;
    }
    else {
        std::cout << "״̬������ڽ�����" << std::endl;
    }
    // �����־�ļ�
    std::ofstream logFile(logFilePath);
    logFile.close();
    std::string  path = "data";
    // ����QMT����̺߳��µ��������߳�
    std::thread qmtThread(monitoQMT,path);
    std::thread orderProgramThread(monitorOrderProgram,path);

    // �����Զ��˳��̣߳�30���Ӻ��Զ��˳���
    std::thread autoQuitThread(autoQuit, 30 * 60);

    // �ȴ������߳̽���
    qmtThread.join();
    orderProgramThread.join();
    autoQuitThread.join();

    return 0;
}