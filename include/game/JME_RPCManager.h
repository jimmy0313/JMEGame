#ifndef JME_RPCManager_h__
#define JME_RPCManager_h__

#include "net/JME_TcpSession.h"
#include "net/JME_NetHandler.h"
#include "net/JME_Message.h"
#include "rpc/JME_RpcClient.h"
#include "rpc/JME_RpcServer.h"
#include "game/JME_Dispatcher.h"
#include "game/JME_Singleton.h"

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
		class RPCException
		{
		public:
			explicit RPCException(const string& err):
			_err(err)
			{
			}
			explicit RPCException(const char* err):
			_err(err)
			{
			}

			const string& what() const { return _err; }
		public:
			string _err;
		};

		class RPCManager;
		class JME_RPCChannel
		{
		public:
			friend class RPCManager;

			typedef boost::shared_ptr<JME_RPCChannel> JME_RPCChannelPtr;
		public:
			bool callRpcMethod(const char* method, const google::protobuf::Message* params);
			bool callRpcMethod(const char* method, const google::protobuf::Message* params, RpcCallback::RpcHandler cb);	//����ֵΪ�� ��ʾ����
			bool callRpcMethod(const char* method, const google::protobuf::Message* params, RpcCallback::RpcHandler cb, size_t dt, RpcCallback::RpcDeadHandler dcb);	//����ֵΪ�� ��ʾ����

		protected:
			vector<RpcClient::JME_RpcClientPtr> _clients;
		};
		class RPCManager
		{
		public:
			CreateSingletonClass(RPCManager);

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

			RpcClient::JME_RpcClientPtr getRpcClient(const char* server);
			RpcClient::JME_RpcClientPtr getRpcClient(const string& server);

			JME_RPCChannel::JME_RPCChannelPtr getRpcChannel(const string& server);
		private:
			map<string, RpcClient::JME_RpcClientPtr> _rpcClient;	//rpc�ͻ���, ���ڵ���Զ�̷���

			RpcServer::RpcServerPtr _rpcServer;
		};
	}
}
#endif // JME_RPCManager_h__
