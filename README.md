#JMEGame

使用JMEngine编写网游服务器框架,包含如下内容
所有测试代码省略一下代码片段
while (1)
{
	string cmd;
	cin >> cmd;

	if (!cmd.compare("quit"))
	{
		break;
	}
}

消息分发模块
DispatchHandler.h

#include "JME_Dispatcher.h"
typedef JMEngine::game::JME_Dispatcher<int, google::protobuf::Message*> MessageDispatcher;

class DispatchHandler : 
	public MessageDispatcher
{
public:
	static void init();

private:
	static void requestLogin(int net, const google::protobuf::Message* params);
};

DispatchHandler.cpp 

#include "DispatchHandler.h"

void DispatchHandler::init()
{
	regMessageHandler(10002, boost::bind(&DispatchHandler::requestLogin, _1, _2));
}

void DispatchHandler::requestLogin(int net, const google::protobuf::Message* params)
{
	
}


游戏事件模块
main.cpp

#include "JME_GameEventCenter.h"

void cb3(const string& evt, JMEngine::game::GameEventCenter<string>::Observer::EventConnectorPtr conn)
{
	std::cout << "event [ " << evt << " ] callback 3" << std::endl; 
	conn->disconnect(); 
}

int main()
{
	auto game_mgr = new JMEngine::game::GameEventCenter<string>;
	auto cb1 = [](const string& evt){ std::cout << "event [ " << evt << " ] callback 1" << std::endl; };
	auto cb2 = [](const string& evt){ std::cout << "event [ " << evt << " ] callback 2" << std::endl; };
	
	game_mgr->regEventHandler("test", cb1);
	game_mgr->regEventHandler("test", cb2);
	
	JMEngine::game::GameEventCenter<string>::Observer::EventConnectorPtr conn(new JMEngine::game::GameEventCenter<string>::Observer::EventConnector);
	game_mgr->regEventHandler("test", boost::bind(cb3, _1, conn), conn);
	
	game_mgr->eventCallback("test");
	game_mgr->removeEventHandler("test", cb1);
	game_mgr->eventCallback("test");
	game_mgr->removeEventHandler("test", cb2);
	game_mgr->eventCallback("test");
	
	return 0;
}
run result:
event [ test ] callback 1
event [ test ] callback 2
event [ test ] callback 3
event [ test ] callback 2

时间相关函数

定时器模块
main.cpp 

#include "JME_TimerTask.h"

void cb4(boost::shared_ptr<boost::asio::deadline_timer> dt, const string& para)
{
	LOGT("%s callback", para);
};

int main()
{
	LOGT("==============================");
	JMEngine::game::JME_TimerTaskCenter::callbackFromNow(20, boost::bind(cb4, _1, "callbackFromNow"));
	JMEngine::game::JME_TimerTaskCenter::callbackAtTime(time(NULL) + 20, boost::bind(cb4, _1, "callbackAtTime"));
	JMEngine::game::JME_TimerTaskCenter::callbackByInterval(5, boost::bind(cb4, _1, "callbackByInterval"));

	return 0;
}
run result:
[2015-Nov-05 11:35:24] [f:\work\jmengine\testservers\client\src\main.cpp:101:main:00F88670] [TRACE] ==============================
[2015-Nov-05 11:35:29] [f:\work\jmengine\testservers\client\src\main.cpp:79:cb4:00F86258] [TRACE] callbackByInterval callback
[2015-Nov-05 11:35:34] [f:\work\jmengine\testservers\client\src\main.cpp:79:cb4:00F86258] [TRACE] callbackByInterval callback
[2015-Nov-05 11:35:39] [f:\work\jmengine\testservers\client\src\main.cpp:79:cb4:00F86258] [TRACE] callbackByInterval callback
[2015-Nov-05 11:35:44] [f:\work\jmengine\testservers\client\src\main.cpp:79:cb4:00F86258] [TRACE] callbackAtTime callback
[2015-Nov-05 11:35:44] [f:\work\jmengine\testservers\client\src\main.cpp:79:cb4:00F86258] [TRACE] callbackFromNow callback
[2015-Nov-05 11:35:44] [f:\work\jmengine\testservers\client\src\main.cpp:79:cb4:00F86258] [TRACE] callbackByInterval callback

共享内存
