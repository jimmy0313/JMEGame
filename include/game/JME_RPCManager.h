#ifndef JME_RPCManager_h__
#define JME_RPCManager_h__

#include "JME_TcpSession.h"
#include "JME_NetHandler.h"
#include "JME_Dispatcher.h"
#include "JME_Message.h"
#include "JME_RpcClient.h"
#include "JME_RpcServer.h"

#include "JME_Singleton.h"

#include <string>
#include "google/protobuf/message.h"

using namespace std;
using namespace JMEngine;
using namespace JMEngine::net;
using namespace JMEngine::rpc;
using namespace JMEngine::game;

namespace Json
{
	class Value;
}

#define GETRPC(server) JMEngine::game::JME_RPCManager::getInstance()->getRpcClient(server)
#define CALLMETHOD(server, method, rpc) JMEngine::game::JME_RPCManager::getInstance()->callServersMethod(server, method, rpc)
#define CALLMETHODCB(server, method, rpc, cb) JMEngine::game::JME_RPCManager::getInstance()->callServersMethod(server, method, rpc, cb)
namespace JMEngine
{
	namespace game
	{
		class JME_RPCException
		{
		public:
			explicit JME_RPCException(const string& err):
			_err(err)
			{
			}
			explicit JME_RPCException(const char* err):
			_err(err)
			{
			}

			const string& what() const { return _err; }
		public:
			string _err;
		};
		class JME_RPCManager
		{
		public:
			CreateSingletonClass(JME_RPCManager);

			//************************************
			// Method:    initRpcClient
			// FullName:  JMEngine::game::JME_RPCManager::initRpcClient
			// Access:    public 
			// Returns:   void
			// Qualifier: 为了方便起见， 初始化rpc客户端放在服务器初始化第一步， 尝试连接rpc服务器
			// Parameter: const Json::Value & conf
			//************************************
			void initRpcClient(const Json::Value& conf);


			//************************************
			// Method:    initRpcServer
			// FullName:  JMEngine::game::JME_RPCManager::initRpcServer
			// Access:    public 
			// Returns:   void
			// Qualifier: rpc服务器监听 放在服务器初始化第三步， 在rpc函数注册完毕后进行
			// Parameter: const Json::Value & conf
			//************************************
			void initRpcServer(const Json::Value& conf);

			JME_RpcClient::JME_RpcClientPtr getRpcClient(const char* server);
			JME_RpcClient::JME_RpcClientPtr getRpcClient(const string& server);

			void callServersMethod(const char* server, const char* method, const google::protobuf::Message* rpc);
			void callServersMethod(const char* server, const char* method, const google::protobuf::Message* rpc, JMEngine::rpc::JME_RpcCallback::RpcHandler cb);
		private:
			map<string, JME_RpcClient::JME_RpcClientPtr> _rpcClient;	//rpc客户端, 用于调用远程服务

			JME_RpcServer::JME_RpcServerPtr _rpcServer;
		};
	}
}
#endif // JME_RPCManager_h__
