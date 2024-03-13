
#include <mysql.h>
#include<iostream>
#include"start.h"
#include <vector>
#include <fstream>
#include<string>

void connect() {
    MYSQL mysql; // ���ݿ���
    MYSQL_RES* res; // ��ѯ�����
    MYSQL_ROW row; // ��¼�����

    mysql_init(&mysql);    // ��ʼ�����ݿ�

    mysql_options(&mysql, MYSQL_SET_CHARSET_NAME, "gbk");    // �����ַ�����

    std::ifstream configFile("C:\\Users\\changhui.luo\\source\\repos\\Project3\\Project3\\config\\config.txt"); // �����ļ���Ϊ "config.txt"���ļ����ݸ�ʽ���£�
    // host=127.0.0.1
    // username=root
    // password=123456Qq
    // database=test

    std::string host, username, password, database;
    std::string line;

    while (std::getline(configFile, line)) {
        size_t delimiterPos = line.find('=');
        if (delimiterPos != std::string::npos) {
            std::string key = line.substr(0, delimiterPos);
            std::string value = line.substr(delimiterPos + 1);
            if (key == "host") {
                host = value;
            }
            else if (key == "username") {
                username = value;
            }
            else if (key == "password") {
                password = value;
            }
            else if (key == "database") {
                database = value;
            }
        }
    }
    configFile.close();

    // �������ݿ�
    if (mysql_real_connect(&mysql, host.c_str(), username.c_str(), password.c_str(), database.c_str(), 3306, NULL, 0) == NULL) {
        std::cout << "���ݿ�����ʧ�ܣ�����ԭ��" << mysql_error(&mysql) << std::endl;
        exit(-1);
    }
    else {
        std::cout << "���ݿ����ӳɹ���������" << std::endl;
    }



    // �������״̬��
    std::string createTableQuery = "CREATE TABLE IF NOT EXISTS monitoring (id INT AUTO_INCREMENT PRIMARY KEY, monitoring_time TIMESTAMP DEFAULT CURRENT_TIMESTAMP, category VARCHAR(255), status VARCHAR(255), txetfilepath VARCHAR(255), programpath VARCHAR(255))";
    if (mysql_query(&mysql, createTableQuery.c_str()) == 0) {
        std::cout << "����ɹ�" << std::endl;
    }
    else {
        std::cout << "����ʧ��" << std::endl;
    }
    

    std::string filePath = "C:\\\\dayhistory\\\\123.log.txt";
    std::vector<std::string> autoStartupPaths = { "C:\\\\Program Files\\\\Tencent\\\\WeChat\\\\WeChat.exe", "C:\\\\Users\\\\changhui.luo\\\\AppData\\\\Local\\\\Programs\\\\Microsoft VS Code\\\\Code.exe2" };

    for (const auto& path : autoStartupPaths) {
        // ��ȡ��ǰʱ��
        time_t currentTime = time(nullptr);
        struct tm* timeInfo = localtime(&currentTime);

        // ��ʱ���ʽ��Ϊ�ַ���
        char timestamp[20];
        strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", timeInfo);

        std::string category = "Startup";
        std::string status = "Running";

        // ��ÿ��·���Ͷ�Ӧ��ʱ�䡢״̬������������ݿ�
        std::string insertQuery = "INSERT INTO monitoring (monitoring_time, category, status, textfilepath, programpath) VALUES ('" + std::string(timestamp) + "', '" + category + "', '" + status + "', '" + filePath + "', '" + path + "')";
        if (mysql_query(&mysql, insertQuery.c_str()) != 0) {
            std::cout << "��¼����ʧ�ܣ�" << mysql_error(&mysql) << std::endl;
        }
    }


    // ��ȡ�ı��ļ�����
    std::string fileorderPath = "C:\\dayhistory\\123.log.txt";
    std::ifstream file(filePath);
    if (!file) {
        std::cout << "�޷����ļ�" << std::endl;
        mysql_close(&mysql);
        return;
    }
    else std::cout << "���Դ��ļ�" << std::endl;

    
    std::string line1;
    bool successFound = false;
    bool orderAccepted = false;

    while (std::getline(file, line)) {
        if (line.find("Login success") != std::string::npos) {
            std::cout <<"successFound = true" <<std:: endl;
        }
        if (line.find("[StkA001]OnOrder") != std::string::npos && line.find("Accepted") != std::string::npos) {
            std::cout << "orderAccepted = true" << std::endl;
        }
    }

    bool result = successFound && orderAccepted;
    
    mysql_close(&mysql);

    // �ر�MySQL����

}