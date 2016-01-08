#include "game/JME_RPCManager.h"
#include "json/json.h"
#include "util/JME_String.h"

namespace JMEngine
{
	namespace game
	{

		void RPCManager::initRpcClient(const Json::Value& conf)
		{
			const auto& clientConf = conf["client"];
			for (const auto& c : clientConf)
			{
				auto rpc = RpcClient::create(c["ip"].asString(), c["port"].asString(), c["reconnect"].asUInt(), c["buffer_size"].asUInt());
				_rpcClient[c["name"].asString()] = rpc;
			}
		}

		RpcClient::RpcClientPtr RPCManager::getRpcClient(const char* server)
		{
			auto rpc = _rpcClient.find(server);
			if (rpc == _rpcClient.end())
			{
				string err = JMEngine::tools::createString("Can't find rpc [ %s ]", server);
				throw RPCException(err);
			}

			return rpc->second;
		}

		RpcClient::RpcClientPtr RPCManager::getRpcClient(const string& server)
		{
			return getRpcClient(server.c_str());
		}

		void RPCManager::initRpcServer(const Json::Value& conf)
		{
			if (conf.isMember("server"))
			{
				auto& serverConf = conf["server"];
				_rpcServer = RpcServer::create(RpcHandlerInterface::create(), serverConf["port"].asUInt(), serverConf["buffer_size"].asUInt());
			}
		}

		JMEngine::game::RPCChannel::RPCChannelPtr RPCManager::getRpcChannel(const string& server)
		{
			auto channel = boost::make_shared<RPCChannel>();
			for (auto it = _rpcClient.begin(); it != _rpcClient.end(); ++it)
			{
				if (it->first.find(server))
					continue;
				channel->_clients.push_back(it->second);
			}
			return channel;
		}


		bool RPCChannel::callRpcMethod(const char* method, const google::protobuf::Message* params)
		{
			for (auto client : _clients)
			{
				client->callRpcMethod(method, params);
			}
			return true;
		}

		bool RPCChannel::callRpcMethod(const char* method, const google::protobuf::Message* params, RpcCallback::RpcHandler cb)
		{
			for (auto client : _clients)
			{
				client->callRpcMethod(method, params, cb);
			}
			return true;
		}

		bool RPCChannel::callRpcMethod(const char* method, const google::protobuf::Message* params, RpcCallback::RpcHandler cb, size_t dt, RpcCallback::RpcDeadHandler dcb)
		{
			for (auto client : _clients)
			{
				client->callRpcMethod(method, params, cb, dt, dcb);
			}
			return true;
		}

	}
}