#pragma once
//实现MySQL数据库的增删改查操作
#include<mysql.h>
#include<string>
#include<ctime>

using namespace std;

class Connection {
public:
	Connection();

	~Connection();
	

	//连接数据库
	bool connect(string ip,
		unsigned port,
		string user,
		string password,
		string dbname);

	//更新操作
	bool update(string sql);

	MYSQL_RES* query(string sql);

	
	//刷新一下连接的起始的空闲时间
	void refreshAliveTime() { _alivetime = clock(); }
	//返回存活时间
	clock_t getAliceTime() { return clock() - _alivetime; }

private:
	MYSQL* _conn;//表示和MySQL Server的一条连接
	clock_t _alivetime;//记录进入空闲状态后的存活时间
};