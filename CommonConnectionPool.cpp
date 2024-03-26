#include "pch.h"
#include"CommonConnectionPool.h"
#include"public.h"
#include<iostream>
using namespace std;

ConnectionPool* ConnectionPool::getConnectionPool()//消费者线程
{
	static ConnectionPool pool;
	return &pool;
}

bool ConnectionPool::loadConfigFile() {
	FILE* pf = fopen("mysql.ini", "r");
	if (pf == nullptr) {
		LOG("mysql.ini file is not exist!");
		return false;
	}
	while (!feof(pf))
	{
		char line[1024] = { 0 };
		fgets(line, 1024, pf);
		string str = line;
		int idx = str.find('=', 0);
		if (idx == -1) {
			continue;
		}
		int endidx = str.find('\n', idx);
		string key = str.substr(0, idx);
		string value = str.substr(idx + 1, endidx - idx - 1);
		if (key == "ip") {
			_ip = value;
		}
		else if (key == "port") {
			_port = atoi(value.c_str());
		}
		else if (key == "username") {
			_username = value;
		}
		else if (key == "dbname") {
			_dbname = value;
		}
		else if (key == "password") {
			_password = value;
		}
		else if (key == "initSize") {
			_initSize = atoi(value.c_str());
		}
		else if (key == "MaxSize") {
			_MaxSize = atoi(value.c_str());
		}
		else if (key == "MaxIdleTime") {
			_MaxIdleTime = atoi(value.c_str());
		}
		else if (key == "connectionTimeOut") {
			_connectionTimeOut = atoi(value.c_str());
		}
	}
	return true;
}

//线程池的构造
ConnectionPool::ConnectionPool() {
	if (!loadConfigFile()) {
		return;
	}
	//创建初始数量的连接
	for (int i = 0; i < _initSize; ++i) {
		Connection* p = new Connection();
		p->connect(_ip, _port, _username, _password, _dbname);
		p->refreshAliveTime(); //刷新一下开始空闲的起始时间
		_connectionqueue.push(p);
		_connectionCnt++;
	}
	//启动一个新线程，作为连接的生产者
	thread produce(std::bind(&ConnectionPool::produceConnectionTask, this));
	produce.detach();
	//启动一个新的定时线程，扫描超过maxIdleTime时间的空闲连接，进行多余的连接回收
	thread scanner(std::bind(&ConnectionPool::scannerConnectionTask, this));
	scanner.detach();
}

void ConnectionPool::produceConnectionTask() {
	for (;;) {
		unique_lock<mutex> lock(_queueMutex);
		while (!_connectionqueue.empty()) {
			cv.wait(lock);//队列不空，此处生成线程进入等待状态
		}
		//连接数量没有到达上线，继续创建新的连接
		if (_connectionCnt < _MaxSize) {
			Connection* p = new Connection();
			p->connect(_ip, _port, _username, _password, _dbname);
			p->refreshAliveTime(); //刷新一下开始空闲的起始时间
			_connectionqueue.push(p);
			_connectionCnt++;
		}
		//通知消费者线程可以消费连接了
		cv.notify_all();
	}
}
//获取一个连接
shared_ptr<Connection> ConnectionPool::getConnection() {
	unique_lock<mutex> lock(_queueMutex);
	while (_connectionqueue.empty())
	{

		if (cv_status::timeout == cv.wait_for(lock, chrono::milliseconds(_connectionTimeOut))) {

			if (_connectionqueue.empty()) {
				LOG("获取空闲连接超时了...获取连接失败！");
				return nullptr;
			}
		}
	}

	shared_ptr<Connection>sp(_connectionqueue.front(), [&](Connection*pcon) {
		unique_lock<mutex> lock(_queueMutex);
		pcon->refreshAliveTime(); //刷新一下开始空闲的起始时间
		_connectionqueue.push(pcon);
		});

	_connectionqueue.pop();
	if (_connectionqueue.empty()) {
		cv.notify_all();//消费完后，通知生产者线程检查一下，如果队列为空了，赶快生产连接
	}
	return sp;
}

void  ConnectionPool::scannerConnectionTask() {
	for (;;) {
		//通过sleep模拟定时效果
		this_thread::sleep_for(chrono::seconds(_MaxIdleTime));
		//扫描整个队列，释放多余的连接
		unique_lock<mutex> lock(_queueMutex);
		while (_connectionCnt > _initSize) {
			Connection* p = _connectionqueue.front();
			if (p->getAliceTime() > _MaxIdleTime*1000) {
				_connectionqueue.pop();
				_connectionCnt--;
				delete p;
			}
			else {
				break;
			}
		}
	}
}