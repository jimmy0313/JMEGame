#include "JME_MongoConnection.h"
#include "JME_GLog.h"

namespace JMEngine
{
	namespace db
	{


		bool JME_MongoConnection::connect( const string& addr, string* err )
		{
			return _conn.connect(addr, *err);
		}

		bool JME_MongoConnection::auth( const string& dbName, const string& userName, const string& pwd, string* err, bool digestPassword /*= true*/ )
		{
			return _conn.auth(dbName, userName, pwd, *err, digestPassword);
		}

		void JME_MongoConnection::selectJson( const string& dbName, const mongo::BSONObj& key, Json::Value* res, const mongo::BSONObj* fileds /*= NULL*/ )
		{
			mongo::BSONObj b = _conn.findOne(dbName, key, fileds);

			if(b.isEmpty())
				return;

			Json::Reader reader;
			reader.parse(b.jsonString(), *res);
		}

		void JME_MongoConnection::insertBsonObj( const string& dbName, const mongo::BSONObj& obj )
		{
			_conn.insert(dbName, obj);
			string err = _conn.getLastError();

			if (!err.empty())
			{
				LOGE(err);
			}
		}

		void JME_MongoConnection::updateBsonVal( const string& dbName, const mongo::BSONObj& key, const mongo::BSONObj& val, bool upsert /*= true*/, bool multi /*= false*/ )
		{
			_conn.update(dbName, key, val, upsert, multi);
			string err = _conn.getLastError();

			if (!err.empty())
			{
				LOGE(err);
			}
		}

		int JME_MongoConnection::createAutoIncId( const string& dbName, const string& key, int init /*= 1*/ )
		{
			mongo::BSONObj b;  

			mongo::BSONObjBuilder bj;
			bj.append("findAndModify","AutoIncrement");  
			bj.append("query",BSON("_id"<<key));
			bj.append("update",BSON("$inc"<<BSON("CurrentIdValue"<<1)));  
			bj.append("upsert", true);

			_conn.runCommand(dbName, bj.obj(), b);  
			string err = _conn.getLastError();  
			if(!err.empty())  
				return 0;  

			mongo::BSONElement bel = b.getField("value");  
  
			if(bel.eoo()) 
				return 0;  

			mongo::BSONType t = bel.type();

			if (t == mongo::jstNULL)
				return init;

			if(bel.type() == mongo::NumberDouble)  
				return (int)bel["CurrentIdValue"].Double() + 1;  
			else  
				return bel["CurrentIdValue"].Int() + 1; 
		}

		void JME_MongoConnection::removeBson( const string& dbName, const mongo::BSONObj& key, bool justOne /*= true*/ )
		{
			_conn.remove(dbName, key, justOne);
			string err = _conn.getLastError();
			if ( !err.empty() )
			{
				LOGE(err);
			}
		}

		void JME_MongoConnection::findAndModify(const string& dbName, const string& table, const mongo::BSONObj& key, const mongo::BSONObj& query, bool upsert /*= true*/)
		{
			mongo::BSONObj b;
			mongo::BSONObjBuilder bj;  
			bj.append("findAndModify", table);  
			bj.append("query", key);

			bj.append("update", query);  
			bj.append("upsert", upsert);

			_conn.runCommand(dbName, bj.obj(), b);
		}

		void JME_MongoConnection::insertJsonObj(const string& dbName, const Json::Value& val)
		{
			string strValue = val.toStyledString();
			int	len = strValue.length();

			try
			{
				mongo::BSONObj obj = mongo::fromjson(strValue.c_str(),&len);
				insertBsonObj(dbName, obj);
			}
			catch(mongo::MsgAssertionException& e)
			{
				LOGE("Serialize json [ %s ] to bson failed ==> [ %s ]", strValue, e.what());
			}
		}

		void JME_MongoConnection::updateJsonVal(const string& dbName, const mongo::BSONObj& key, const Json::Value& val, bool upsert /*= true*/, bool multi /*= false*/)
		{
			string strValue = val.toStyledString();
			int	len = strValue.length();

			try
			{
				mongo::BSONObj obj = mongo::fromjson(strValue.c_str(),&len);
				updateBsonVal(dbName, key, obj, upsert, multi);
			}
			catch(mongo::MsgAssertionException& e)
			{
				LOGE("Serialize json [ %s ] to bson failed ==> [ %s ]", strValue, e.what());
			}
		}

		void JME_MongoConnection::selectMoreJson(const string& dbName, const mongo::BSONObj& key, Json::Value* res)
		{
			mongo::auto_ptr<mongo::DBClientCursor> cursor = _conn.query(dbName, key);
			while (cursor->more()) {
				mongo::BSONObj obj = cursor->next();
				string str = obj.jsonString();
				Json::Reader reader;
				Json::Value val;
				reader.parse(str, val);
				res->append( val );
			}
		}

		void JME_MongoConnection::dropCollection(const string& collection)
		{
			_conn.dropCollection(collection);
		}

	}
}