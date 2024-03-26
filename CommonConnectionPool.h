#pragma once
#include<string>
#include<queue>
#include"Connection.h"
#include<mutex>
#include<thread>
#include<condition_variable>
#include<memory>
#include<functional>
#include<atomic>

using namespace std;

//ʵ�����ӳع���ģ��
class ConnectionPool {
public:
	//��ȡ���ӳض���ʵ��
	static ConnectionPool* getConnectionPool();
private:
	//����1�����캯��˽�л�
	ConnectionPool();
	//�����ڶ����߳��У�ר�Ÿ����������߳�
	void produceConnectionTask();
	//ɨ�賬��maxIdleTimeʱ��Ŀ������ӣ����ж�������ӻ���
	void scannerConnectionTask();
public:
	bool loadConfigFile();
	//���ⲿ�ṩһ���ӿڣ������ӳ��л�ȡһ�����õĿ�������
	shared_ptr<Connection> getConnection();

	string _ip;
	unsigned short _port;
	string _username;
	string _password;
	int _initSize;   //���ӳصĳ�ʼ������
	int _MaxSize;    //���������
	string _dbname;   //���ӵ����ݿ�����
	int _MaxIdleTime;   //���ӳ�������ʱ��
	int _connectionTimeOut;   //���ӳػ�ȡ���ӵĳ�ʱʱ��
	queue<Connection*>_connectionqueue;   //�洢mysql���ӵĶ���
	mutex _queueMutex;
	atomic_int _connectionCnt;//��¼������������connection���ӵ�������
	condition_variable cv;//�������������������������̺߳����������̵߳�ͨ��
};