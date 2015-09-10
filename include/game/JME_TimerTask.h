#ifndef JME_TimerTask_h__
#define JME_TimerTask_h__

#include "boost/asio.hpp"
#include "boost/function.hpp"

typedef boost::function<void(boost::shared_ptr<boost::asio::deadline_timer> dt)> JME_TimerTaskHandler;

class JME_TimerTaskCenter
{
public:
	//************************************
	// Method:    callbackFromNow
	// FullName:  JME_TimerTaskCenter::callbackFromNow
	// Access:    public static 
	// Returns:   boost::shared_ptr<boost::asio::deadline_timer> --> used to cancel the callback
	// Qualifier:
	// Parameter: size_t t ==> task will callback at t sec after now
	// Parameter: JME_TimerTaskHandler cb
	//************************************
	static boost::shared_ptr<boost::asio::deadline_timer> callbackFromNow(size_t t, JME_TimerTaskHandler cb);

	//************************************
	// Method:    callbackAtTime
	// FullName:  JME_TimerTaskCenter::callbackAtTime
	// Access:    public static 
	// Returns:   boost::shared_ptr<boost::asio::deadline_timer>
	// Qualifier:
	// Parameter: size_t t ==> task will callback at time t
	// Parameter: JME_TimerTaskHandler cb
	//************************************
	static boost::shared_ptr<boost::asio::deadline_timer> callbackAtTime(size_t t, JME_TimerTaskHandler cb);

	//************************************
	// Method:    callbackByInterval
	// FullName:  JME_TimerTaskCenter::callbackByInterval
	// Access:    public static 
	// Returns:   boost::shared_ptr<boost::asio::deadline_timer>
	// Qualifier:
	// Parameter: size_t t ==> task will callback 1/t s
	// Parameter: JME_TimerTaskHandler cb
	//************************************
	static boost::shared_ptr<boost::asio::deadline_timer> callbackByInterval(size_t t, JME_TimerTaskHandler cb);
private:
	static void timerTaskCallback(const boost::system::error_code& err, boost::shared_ptr<boost::asio::deadline_timer> dt, JME_TimerTaskHandler cb);
	static void cycleTaskCallback(const size_t t, const boost::system::error_code& err, boost::shared_ptr<boost::asio::deadline_timer> dt, JME_TimerTaskHandler cb);
};
#endif // JME_TimerTask_h__
