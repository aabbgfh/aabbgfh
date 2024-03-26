#include"pch.h"
#include"public.h"
#include"Connection.h"
#include<iostream>
using namespace std;


Connection::Connection() : _conn(mysql_init(nullptr)), _alivetime(0) {
	// 其他初始化操作
}
Connection::~Connection()
	{
		if (_conn != nullptr)
			mysql_close(_conn);
	}

	bool Connection::connect(string ip, unsigned port, string user, string password, string dbname) {
		MYSQL* p = mysql_real_connect(_conn, ip.c_str(), user.c_str(), password.c_str(), dbname.c_str(), port, nullptr, 0);
		return p != nullptr;
	}

	bool Connection::update(string sql) {
		if (mysql_query(_conn, sql.c_str())) {
			LOG("更新失败: " + sql);
			return false;
		}
		return true;
	}

	MYSQL_RES* Connection::query(string sql) {
		if (mysql_query(_conn, sql.c_str())) {
			LOG("查询失败: " + sql);
			return nullptr;
		}
		return mysql_use_result(_conn);
	}

