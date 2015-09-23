#include "JME_Time.h"
#include "boost/date_time/posix_time/posix_time.hpp"

namespace JMEngine
{
	namespace game
	{

		bool JME_Time::isTimeToday(time_t t)
		{
			auto ct = boost::posix_time::from_time_t(t);
			auto nt = boost::posix_time::second_clock::local_time();

			return ct.date() == nt.date();
		}

		int JME_Time::todayYearday()
		{
			auto nt = boost::posix_time::second_clock::local_time();
			return nt.date().day_of_year();
		}

		tm JME_Time::localTime()
		{
			return boost::posix_time::to_tm(boost::posix_time::second_clock::local_time());
		}

		tm JME_Time::localTime(time_t t)
		{
			tm * timeinfo = localtime(&t);
			return *timeinfo;
		}

	}
}