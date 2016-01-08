#include "game/JME_TimerTask.h"
#include "net/JME_Core.h"

#include "boost/make_shared.hpp"

namespace JMEngine
{
	namespace game
	{

		boost::shared_ptr<boost::asio::deadline_timer> TimerTaskCenter::callbackFromNow(size_t t, JME_TimerTaskHandler cb)
		{
			auto dt = boost::make_shared<boost::asio::deadline_timer>(JMECore.getLogicioService());
			dt->expires_from_now(boost::posix_time::seconds(t));
			dt->async_wait(boost::bind(TimerTaskCenter::timerTaskCallback, boost::asio::placeholders::error, dt, cb));
			return dt;
		}

		void TimerTaskCenter::timerTaskCallback(const boost::system::error_code& err, boost::shared_ptr<boost::asio::deadline_timer> dt, JME_TimerTaskHandler cb)
		{
			if (!err)
			{
				cb(dt);
			}
			else
			{
				//该任务被取消
			}
		}

		boost::shared_ptr<boost::asio::deadline_timer> TimerTaskCenter::callbackAtTime(size_t t, JME_TimerTaskHandler cb)
		{
			auto dt = boost::make_shared<boost::asio::deadline_timer>(JMECore.getLogicioService(), boost::posix_time::from_time_t(t));
			dt->async_wait(boost::bind(TimerTaskCenter::timerTaskCallback, boost::asio::placeholders::error, dt, cb));
			return dt;
		}

		boost::shared_ptr<boost::asio::deadline_timer> TimerTaskCenter::callbackByInterval(size_t t, JME_TimerTaskHandler cb)
		{
			auto dt = boost::make_shared<boost::asio::deadline_timer>(JMECore.getLogicioService());
			dt->expires_from_now(boost::posix_time::seconds(t));
			dt->async_wait(boost::bind(TimerTaskCenter::cycleTaskCallback, t, boost::asio::placeholders::error, dt, cb));

			return dt;
		}

		void TimerTaskCenter::cycleTaskCallback(const size_t t, const boost::system::error_code& err, boost::shared_ptr<boost::asio::deadline_timer> dt, JME_TimerTaskHandler cb)
		{
			if (!err)
			{
				dt->expires_from_now(boost::posix_time::seconds(t));
				dt->async_wait(boost::bind(TimerTaskCenter::cycleTaskCallback, t, boost::asio::placeholders::error, dt, cb));

				cb(dt);
			}
			else
			{
				//该任务被取消
			}
		}
	}
}
