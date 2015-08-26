#ifndef JME_ServerInterface_h__
#define JME_ServerInterface_h__

#include "json/json.h"

#include "JME_TcpSession.h"
#include "JME_NetHandler.h"
#include "JME_Dispatcher.h"
#include "JME_Message.h"
#include "JME_RpcClient.h"
#include "JME_RpcServer.h"

#include "JME_RPCManager.h"


using namespace JMEngine;
using namespace JMEngine::net;
using namespace JMEngine::rpc;
using namespace JMEngine::game;

namespace JMEngine
{
	namespace game
	{
		class JME_ServerInterface
		{
		public:
			void onInit();

		private:
			//************************************
			// Method:    init
			// FullName:  JMEngine::game::JME_ServerInterface::init
			// Access:    virtual public 
			// Returns:   void
			// Qualifier: �߼�����������һЩ��ʼ���� �������ݿ��
			//************************************
			virtual void init();

			//************************************
			// Method:    init_
			// FullName:  JMEngine::game::JME_ServerInterface::init_
			// Access:    public 
			// Returns:   void
			// Qualifier: ��ʼ���������ӣ� ������ ����handler��ʼ����Ϻ����
			//************************************
			void init_();
		public:

			size_t _serverId;
			string _serverName;	//�������� ���� name + server_id + idx, ps: "game-server-1-01", "chat-server-1"

		protected:
			Json::Value _config;

			// ��Ҫ�������ط������� ���ܿͻ�������
			JME_TcpAcceptor::JME_TcpAcceptorPtr _acceptorPtr;
			JME_NetHandler::JME_NetHandlerPtr _clientHandler;	//���������ҵġ��ͻ��ˡ���������Ϣ����
		};
	}
}
#endif // JME_ServerInterface_h__