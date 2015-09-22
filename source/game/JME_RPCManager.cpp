#include "JME_RPCManager.h"
#include "json/json.h"
#include "JME_String.h"

namespace JMEngine
{
	namespace game
	{

		void JME_RPCManager::initRpcClient(const Json::Value& conf)
		{
			const auto& clientConf = conf["client"];
			for (const auto& c : clientConf)
			{
				auto rpc = JME_RpcClient::create(c["ip"].asString(), c["port"].asString(), c["reconnect"].asUInt(), c["buffer_size"].asUInt());
				_rpcClient[c["name"].asString()] = rpc;
			}
		}

		JME_RpcClient::JME_RpcClientPtr JME_RPCManager::getRpcClient(const char* server)
		{
			auto rpc = _rpcClient.find(server);
			if (rpc == _rpcClient.end())
			{
				string err = JMEngine::tools::createString("Can't find rpc [ %s ]", server);
				throw JME_RPCException(err);
			}

			return rpc->second;
		}

		JME_RpcClient::JME_RpcClientPtr JME_RPCManager::getRpcClient(const string& server)
		{
			return getRpcClient(server.c_str());
		}

		void JME_RPCManager::initRpcServer(const Json::Value& conf)
		{
			if (conf.isMember("server"))
			{
				auto& serverConf = conf["server"];
				_rpcServer = JME_RpcServer::create(JME_RpcHandler::create(), serverConf["port"].asUInt(), serverConf["buffer_size"].asUInt());
			}
		}

		void JME_RPCManager::callServersMethod(const char* server, const char* method, const google::protobuf::Message* rpc)
		{
			for (auto it = _rpcClient.begin(); it != _rpcClient.end(); ++it)
			{
				if (it->first.find(server))
					continue;
				it->second->callRpcMethod(method, rpc);
			}
		}

	}
}