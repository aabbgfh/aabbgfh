#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <thread>
#include <chrono>
#include <ctime>
#include"start.h"
#include <mysql.h>
#include <windows.h>
#include <tlhelp32.h>
#include <locale>
#include <codecvt>



const std::vector<std::string> qmtPaths = { "C:\\Program Files\\Tencent\\WeChat\\WeChat.exe", "C:\\Users\\changhui.luo\\AppData\\Local\\Programs\\Microsoft VS Code\\Code.exe" };
const std::vector<std::string> orderProgramPaths = { "C:\\Program Files\\Tencent\\WeChat\\WeChat.exe", "C:\\Users\\changhui.luo\\AppData\\Local\\Programs\\Microsoft VS Code\\Code.exe" };
const std::string logFilePath = "C:\\dayhistory";
const std::string mqLogFilePath = "C:\\dayhistory";

bool isProcessRunning(const std::string& processName)
{
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        std::cout << "CreateToolhelp32Snapshot error: " << GetLastError() << std::endl;
        return false;
    }

    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);

    if (Process32First(hSnapshot, &entry)) {
        do {
            std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
            std::string exeFile = converter.to_bytes(entry.szExeFile);
           
            if (processName == exeFile) {
                CloseHandle(hSnapshot);
                return true;
            }
        } while (Process32Next(hSnapshot, &entry));
    }

    CloseHandle(hSnapshot);
    return false;
}

bool isQMTRunning()
{
    std::vector<std::string> processNames = { "C:\\Program Files\\Tencent\\WeChat\\WeChat.exe" ,"C:\\Users\\changhui.luo\\AppData\\Local\\Programs\\Microsoft VS Code\\Code.exe" };

    for (const std::string& processName : processNames)
    {
        if (isProcessRunning(processName))
        {
            return true;
        }
    }

    return false;
}

bool isOrderProgramRunning()
{
    std::vector<std::string> processNames = { "C:\\Program Files\\Tencent\\WeChat\\WeChat.exe" ,"C:\\Users\\changhui.luo\\AppData\\Local\\Programs\\Microsoft VS Code\\Code.exe" };

    for (const std::string& processName : processNames)
    {
        if (isProcessRunning(processName))
        {
            return true;
        }
    }

    return false;
}




void startQMT(const std::string& qmtPath, int timeParam) {
    std::string command = "\"" + qmtPath + "\" " + std::to_string(timeParam);

    // ����QMT
    system(command.c_str());

    // ��¼��־
    std::ofstream logFile("C:\dayhistory", std::ios_base::app);
    logFile << "Started QMT: " << qmtPath << ", Time Parameter: " << timeParam << std::endl;
    logFile.close();
}

void startOrderProgram(const std::string& orderProgramPath, int timeParam) {
    std::string command = "\"" + orderProgramPath + "\" " + std::to_string(timeParam);

    // �����µ�����
    system(command.c_str());

    // ��¼��־
    std::ofstream logFile("C:\dayhistory", std::ios_base::app);
    logFile << "Started Order Program: " << orderProgramPath << ", Time Parameter: " << timeParam << std::endl;
    logFile.close();
}

void startProgramAtTime(const std::string& programPath, int timeParam, const std::chrono::system_clock::time_point& startTime) {
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    if (now < startTime) { 
        std::this_thread::sleep_until(startTime);
    }

    if (programPath.find("QMT") != std::string::npos) { // �ж��Ƿ���QMT����
        startQMT(programPath, timeParam);
    }
    else {
        startOrderProgram(programPath, timeParam);
    }
}

bool isLoginSuccess() {
    std::ifstream mqLogFile(mqLogFilePath);
    std::string line;

    while (std::getline(mqLogFile, line)) {
        if (line.find("Login success") != std::string::npos) {
            mqLogFile.close();
            return true;
        }
    }

    mqLogFile.close();
    return false;
}

bool isOnOrderAccepted() {
    std::ifstream mqLogFile(mqLogFilePath);
    std::string line;

    while (std::getline(mqLogFile, line)) {
        if (line.find("[StkA001]OnOrder Accepted") != std::string::npos) {
            mqLogFile.close();
            return true;
        }
    }

    mqLogFile.close();
    return false;
}

void monitoQMT(const std::string& Path) {
    const int QMT_TIME_PARAM = 1; // ����QMT��ʱ�����

    while (true) {
        if (!isQMTRunning()) {
            for (const std::string& qmtPath : qmtPaths) {
                startQMT(qmtPath, QMT_TIME_PARAM);
            }
        }

        bool isLoginSuccessFlag = false;
        bool isOnOrderAcceptedFlag = false;

        // ��ش���
        std::ifstream mqLogFile(mqLogFilePath);
        std::string line;

        while (std::getline(mqLogFile, line)) {
            // �ж��Ƿ����"Login success"
            if (!isLoginSuccessFlag && line.find("Login success") != std::string::npos) {
                isLoginSuccessFlag = true;
                return  ;
            }

            // �ж��Ƿ����"[StkA001]OnOrder Accepted"
            if (!isOnOrderAcceptedFlag && line.find("[StkA001]OnOrder Accepted") != std::string::npos) {
                isOnOrderAcceptedFlag = true;
                return ;
            }

            // ����Ѿ��ҵ�������������ʾQMT�������
            if (isLoginSuccessFlag && isOnOrderAcceptedFlag) {
                // ��¼��־
                std::ofstream logFile(logFilePath, std::ios_base::app);
                logFile << "QMT monitoring is normal" << std::endl;
                logFile.close();

                break; // ����ѭ�����������QMT
            }
        }

        mqLogFile.close();

        // �ȴ�һ��ʱ����ٽ�����һ�μ��
        std::this_thread::sleep_for(std::chrono::seconds(10));
    }

}

void monitorOrderProgram(const std::string& Path) {
    const int ORDER_PROGRAM_TIME_PARAM = 2; // �����µ������ʱ�����

    while (true) {
        if (!isOrderProgramRunning()) {
            for (const std::string& orderProgramPath : orderProgramPaths) {
                startOrderProgram(orderProgramPath, ORDER_PROGRAM_TIME_PARAM);
            }
        }

        // �ȴ�һ��ʱ����ٽ�����һ�μ��
        std::this_thread::sleep_for(std::chrono::seconds(10));
    }
}

void autoQuit(int timeInSeconds) {
    std::this_thread::sleep_for(std::chrono::seconds(timeInSeconds));

    // ��¼��־
    std::ofstream logFile(logFilePath, std::ios_base::app);
    logFile << "Automatically quit after " << timeInSeconds << " seconds" << std::endl;
    logFile.close();

    // �˳�����
    exit(0);
}



