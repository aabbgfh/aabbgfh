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

//实现连接池功能模块
class ConnectionPool {
public:
	//获取连接池对象实例
	static ConnectionPool* getConnectionPool();
private:
	//单例1，构造函数私有化
	ConnectionPool();
	//运行在独立线程中，专门负责生成新线程
	void produceConnectionTask();
	//扫描超过maxIdleTime时间的空闲连接，进行多余的连接回收
	void scannerConnectionTask();
public:
	bool loadConfigFile();
	//给外部提供一个接口，从连接池中获取一个可用的空闲连接
	shared_ptr<Connection> getConnection();

	string _ip;
	unsigned short _port;
	string _username;
	string _password;
	int _initSize;   //连接池的初始连接量
	int _MaxSize;    //最大连接量
	string _dbname;   //连接的数据库名称
	int _MaxIdleTime;   //连接池最大空闲时间
	int _connectionTimeOut;   //连接池获取连接的超时时间
	queue<Connection*>_connectionqueue;   //存储mysql连接的队列
	mutex _queueMutex;
	atomic_int _connectionCnt;//记录连接所创建的connection连接的总数量
	condition_variable cv;//设置条件变量用于连接生产线程和连接消费线程的通信
};