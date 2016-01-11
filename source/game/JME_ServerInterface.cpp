#include "game/JME_ServerInterface.h"
#include "JME_JsonFile.h"

namespace JMEngine
{
	namespace game
	{


		void ServerInterface::onInit()
		{
			_config = JMEngine::file::load_jsonfile_val("./config/conf.json");

			const auto& _baseConf = _config["base"];
			_server_id = _baseConf["server_id"].asInt();
			_server_name = _baseConf["server_name"].asString();

			const auto& rpc = _config["rpc"];

			RPCManager::getInstance()->initRpcClient(rpc);

			init();

			init_();
		}

		void ServerInterface::init_()
		{
			const auto& rpc = _config["rpc"];
			RPCManager::getInstance()->initRpcServer(rpc);

			const auto& _baseConf = _config["base"];

			if (_baseConf.isMember("acceptor"))
			{
				const auto& acceptConf = _baseConf["acceptor"];

				unsigned short port = acceptConf["port"].asUInt();
				_acceptor_ptr = JMEngine::net::TcpAcceptor::create(_client_handler, port);

				size_t bufferSize = acceptConf["buffer_size"].asUInt();
				_acceptor_ptr->accept(0, bufferSize);

				LOGI("Start accept connector on port[ %s ]", port);
			}

			LOGI("Start server [ %s ] complete", _server_name);
		}

		void ServerInterface::init()
		{

		}
	}
}