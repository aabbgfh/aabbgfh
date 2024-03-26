#pragma once
//ʵ��MySQL���ݿ����ɾ�Ĳ����
#include<mysql.h>
#include<string>
#include<ctime>

using namespace std;

class Connection {
public:
	Connection();

	~Connection();
	

	//�������ݿ�
	bool connect(string ip,
		unsigned port,
		string user,
		string password,
		string dbname);

	//���²���
	bool update(string sql);

	MYSQL_RES* query(string sql);

	
	//ˢ��һ�����ӵ���ʼ�Ŀ���ʱ��
	void refreshAliveTime() { _alivetime = clock(); }
	//���ش��ʱ��
	clock_t getAliceTime() { return clock() - _alivetime; }

private:
	MYSQL* _conn;//��ʾ��MySQL Server��һ������
	clock_t _alivetime;//��¼�������״̬��Ĵ��ʱ��
};