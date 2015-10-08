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
#include <vector>
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
#define GETRPCCHANNEL(server) JMEngine::game::JME_RPCManager::getInstance()->getRpcChannel(server)

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

		class JME_RPCManager;
		class JME_RPCChannel
		{
		public:
			friend class JME_RPCManager;

			typedef boost::shared_ptr<JME_RPCChannel> JME_RPCChannelPtr;
		public:
			bool callRpcMethod(const char* method, const google::protobuf::Message* params);
			bool callRpcMethod(const char* method, const google::protobuf::Message* params, JME_RpcCallback::RpcHandler cb);	//����ֵΪ�� ��ʾ����
			bool callRpcMethod(const char* method, const google::protobuf::Message* params, JME_RpcCallback::RpcHandler cb, size_t dt, JME_RpcCallback::RpcDeadHandler dcb);	//����ֵΪ�� ��ʾ����

		protected:
			vector<JME_RpcClient::JME_RpcClientPtr> _clients;
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
			// Qualifier: Ϊ�˷�������� ��ʼ��rpc�ͻ��˷��ڷ�������ʼ����һ���� ��������rpc������
			// Parameter: const Json::Value & conf
			//************************************
			void initRpcClient(const Json::Value& conf);


			//************************************
			// Method:    initRpcServer
			// FullName:  JMEngine::game::JME_RPCManager::initRpcServer
			// Access:    public 
			// Returns:   void
			// Qualifier: rpc���������� ���ڷ�������ʼ���������� ��rpc����ע����Ϻ����
			// Parameter: const Json::Value & conf
			//************************************
			void initRpcServer(const Json::Value& conf);

			JME_RpcClient::JME_RpcClientPtr getRpcClient(const char* server);
			JME_RpcClient::JME_RpcClientPtr getRpcClient(const string& server);

			JME_RPCChannel::JME_RPCChannelPtr getRpcChannel(const string& server);

// 			void callServersMethod(const char* server, const char* method, const google::protobuf::Message* rpc);
// 			void callServersMethod(const char* server, const char* method, const google::protobuf::Message* rpc, JMEngine::rpc::JME_RpcCallback::RpcHandler cb);
		private:
			map<string, JME_RpcClient::JME_RpcClientPtr> _rpcClient;	//rpc�ͻ���, ���ڵ���Զ�̷���

			JME_RpcServer::JME_RpcServerPtr _rpcServer;
		};
	}
}
#endif // JME_RPCManager_h__
