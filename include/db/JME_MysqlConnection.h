#define _CRT_SECURE_NO_WARNINGS

#ifndef JME_MysqlConnection_h__
#define JME_MysqlConnection_h__

#include <string>
#include "JME_DBPool.h"

#ifdef WIN32
	#include "winsock2.h"
	#pragma comment(lib,"ws2_32")
#endif

#ifdef WIN32
	#include "mysql.h"
	#include "errmsg.h"
#else
	#include <mysql.h>
	#include <errmsg.h>
#endif

using namespace std;
using namespace JMEngine::db;

class MysqlConnector
{
public:
	
public:
	MysqlConnector();
	~MysqlConnector();

	bool connect();
	bool lockTable( const char* table, const char* priority );
	bool unlockTable();
	bool query( const char *sql );
	bool isResEmpty();
	bool connected();

	MYSQL_RES* genRes();
	MYSQL* get_mysql();
	MYSQL_RES* getRes() { return m_queryRes; }

public:
	void setConfig(const string& ip, const string& userName, const string& pwd, const string& dbName, const int port);

private:
	MYSQL						m_mysql;
	MYSQL_ROW					m_row;
	MYSQL_RES					*m_queryRes;
	MYSQL_FIELD					*m_field;

	string m_ip;
	string m_userName;
	string m_pwd;
	string m_dbName;
	unsigned short m_port;
};

#endif // JME_MysqlConnection_h__