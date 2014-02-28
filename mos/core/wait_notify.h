#ifndef WAIT_NOTIFY_H_
#define WAIT_NOTIFY_H_

#include <thread>
#include <mutex>
#include <condition_variable>

class wait_notify
{
public:
	bool m_notifyed;
//*
	std::mutex  m_mutex;
	//std::condition_variable m_cond;
	void wait(){
		//std::unique_lock<std::mutex> locker(m_mutex);
		while(!m_notifyed)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
			//m_cond.wait(locker);
		}
		m_notifyed = false;
	}
	void notify(){
		//std::unique_lock<std::mutex> locker(m_mutex);
		m_notifyed = true;
		//m_cond.notify_one();
	}
/**/	
/*
	void wait(){
		while(!m_notifyed)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
		m_notifyed = false;
	}
	void notify(){
		m_notifyed = true;
	}
*/
};


#endif