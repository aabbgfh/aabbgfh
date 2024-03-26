#include <iostream>
#include"pch.h"
#include"CommonConnectionPool.h"
using namespace std;
#include"Connection.h"
int main()
{
	/*Connection conn;
	char sql[1024] = { 0 };
	sprintf(sql, "insert into user(name,age,sex) values('%s',%d,'%s')", "zhangsan", 20, "male");
	conn.connect("127.0.0.1", 3306, "lch", "200222", "chat");
	conn.update(sql);*/


	/*ConnectionPool* cp = ConnectionPool::getConnectionPool();*/
	//cp->loadConfigFile();
	
	clock_t begin = clock();
	
	thread t1([]() {
		ConnectionPool* cp = ConnectionPool::getConnectionPool(); //获取线程池
		
		for (int i = 0; i < 2500; ++i) {
			char sql[1024] = { 0 };
			sprintf(sql, "insert into user(name,age,sex) values('%s',%d,'%s')", "zhangsan", 20, "male");
			shared_ptr<Connection>sp = cp->getConnection();
			sp->update(sql);
		}
		});
	thread t2([]() {
		ConnectionPool* cp = ConnectionPool::getConnectionPool(); //获取线程池

		for (int i = 0; i < 2500; ++i) {
			char sql[1024] = { 0 };
			sprintf(sql, "insert into user(name,age,sex) values('%s',%d,'%s')", "zhangsan", 20, "male");
			shared_ptr<Connection>sp = cp->getConnection();
			sp->update(sql);
		}
		});
	thread t3([]() {
		ConnectionPool* cp = ConnectionPool::getConnectionPool(); //获取线程池

		for (int i = 0; i <2500; ++i) {
			char sql[1024] = { 0 };
			sprintf(sql, "insert into user(name,age,sex) values('%s',%d,'%s')", "zhangsan", 20, "male");
			shared_ptr<Connection>sp = cp->getConnection();
			sp->update(sql);
		}
		});
	thread t4([]() {
		ConnectionPool* cp = ConnectionPool::getConnectionPool(); //获取线程池

		for (int i = 0; i < 2500; ++i) {
			char sql[1024] = { 0 };
			sprintf(sql, "insert into user(name,age,sex) values('%s',%d,'%s')", "zhangsan", 20, "male");
			shared_ptr<Connection>sp = cp->getConnection();
			sp->update(sql);
		}
		});

	t1.join();
	t2.join();
	t3.join();
	t4.join();
	clock_t end = clock();
	cout << (end - begin )<< "ms" << endl;


#if 0
	for (int i = 0; i < 10000; ++i) {
		/*Connection conn;
		char sql[1024] = { 0 };
		sprintf(sql, "insert into user(name,age,sex) values('%s',%d,'%s')", "zhangsan", 20, "male");
		conn.connect("127.0.0.1", 3306, "lch", "200222", "chat");
		conn.update(sql);*/

		shared_ptr<Connection>sp = cp->getConnection();
		char sql[1024] = { 0 };
		sprintf(sql, "insert into user(name,age,sex) values('%s',%d,'%s')", "zhangsan", 20, "male");
		sp->update(sql);
	}

#endif

	return 0;
}