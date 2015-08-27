#include "JME_MysqlConnection.h"
#include <stdio.h>
#include <sstream>

MysqlConnector::MysqlConnector()
{
	m_queryRes = NULL;
	m_field = NULL;
	MYSQL *mysql = mysql_init( &m_mysql );

	if ( mysql )
	{
		LOGI("Call mysql_init call succeeded");
	}
	else
	{
		LOGE("Call mysql_init call failed");
	}
}

MysqlConnector::~MysqlConnector()
{
	if( m_queryRes )
		mysql_free_result( m_queryRes );
	mysql_close( &m_mysql );
}

///////////////////////////////////////////////////////////////////////////////
// Connector::connect
///////////////////////////////////////////////////////////////////////////////
bool MysqlConnector::connect()
{
	MYSQL *mysql = mysql_real_connect( &m_mysql, m_ip.c_str(), m_userName.c_str(), m_pwd.c_str(), m_dbName.c_str(), m_port, NULL, 0 );

	if( mysql )
	{
		LOGI("Connect to mysql [ %s:%d==>%s ] succeed", m_ip, m_port, m_dbName);
		// Set the utf8 query charset
		int ret = mysql_real_query(&m_mysql,"set NAMES'utf8'",(unsigned int) strlen("set NAMES'utf8'"));
		if( ret )
		{
			int qerrno = mysql_errno( &m_mysql );
			LOGE("Set utf8 query charset failed, error ==> [ %d ]", qerrno);			
		}
	}
	else
	{
		LOGE("Connect to mysql [ %s:%d==>%s ] failed, error ==> [ %d ]", m_ip, m_port, m_dbName, mysql_errno(&m_mysql));
		
		return false;
	}
	return true;
}
///////////////////////////////////////////////////////////////////////////////
// Connector::lockTable
///////////////////////////////////////////////////////////////////////////////
bool MysqlConnector::lockTable( const char* table, const char* priority )
{
	ostringstream oss;
	oss << "LOCK TABLES " << table << " " << priority;
	if( mysql_query( &m_mysql, oss.str().c_str() ) )
		return false;
	return true;
}

///////////////////////////////////////////////////////////////////////////////
// Connector::unlockTable
///////////////////////////////////////////////////////////////////////////////
bool MysqlConnector::unlockTable()
{
	if( mysql_query( &m_mysql, "UNLOCK TABLES" ) )
		return true;
	return false;
}


///////////////////////////////////////////////////////////////////////////////
// Connector::query
///////////////////////////////////////////////////////////////////////////////
bool MysqlConnector::query( const char *sql )
{
	//boost::system_time startTime = boost::get_system_time();
	int ret = mysql_real_query( &m_mysql, sql, strlen( sql ) );
	//boost::system_time endTime = boost::get_system_time();
	//boost::int64_t elapse = (endTime - startTime).total_milliseconds();
	//LogT << "+++++++++++> db querey consume time : " << (int)elapse << "for sql : " << sql <<  LogEnd;

	if( ret )
	{
		int qerrno = mysql_errno( &m_mysql );
		LOGE("Query [ %s ] call failed, error ==> [ %d ]", sql, qerrno);		
		if( qerrno == CR_SERVER_GONE_ERROR ) // MYSQL server timeout
		{
			LOGT("Trying to connect the server again..");
			if(connect())
			{
				ret = mysql_real_query( &m_mysql, sql, strlen( sql ) );
				if( ret )
				{
					qerrno = mysql_errno( &m_mysql );
					LOGE("Query [ %s ] call failed, error ==> [ %d ]", sql, qerrno);		
					return false;
				}
				else
					return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}
	else
	{
	}
	return true;
}

///////////////////////////////////////////////////////////////////////////////
// Connector::getRes
///////////////////////////////////////////////////////////////////////////////
MYSQL_RES* MysqlConnector::genRes()
{
	if( m_queryRes )
		mysql_free_result( m_queryRes );

	m_queryRes = mysql_store_result( &m_mysql );
	if( !m_queryRes )
	{
		LOGE("Call mysql_store_result failed");
		return NULL;
	}
	return m_queryRes;
}

///////////////////////////////////////////////////////////////////////////////
// Connector::getRes
///////////////////////////////////////////////////////////////////////////////
bool MysqlConnector::isResEmpty()
{
	if( !genRes() )
		return true;

	return !m_queryRes->row_count;
}

MYSQL* MysqlConnector::get_mysql()
{
	return &m_mysql;
}

bool MysqlConnector::connected()
{
	int qerrno = mysql_errno( &m_mysql );
	if( qerrno == CR_SERVER_GONE_ERROR ) // MYSQL server timeout
	{
		return false;
	}
	return true;	
}

void MysqlConnector::setConfig( const string& ip, const string& userName, const string& pwd, const string& dbName, const int port )
{
	m_ip = ip;
	m_userName = userName;
	m_pwd = pwd;
	m_dbName = dbName;
	m_port = port;
}
