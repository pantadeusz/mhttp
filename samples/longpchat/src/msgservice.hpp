#ifndef __MSG_SERVICE_HPP_TP_____
#define  __MSG_SERVICE_HPP_TP_____

#include <list>
#include <functional>
#include <string>
#include <map>
#include <mutex>
#include <thread>
#include <atomic>
#include <condition_variable>
#include <iostream>
#include <chrono>

namespace tp {
namespace services {


template<class message_t>
class message_service {
protected:
	std::mutex messages_m_;
	std::mutex cv_m_;
	std::condition_variable cv_;
public:
	std::map < std::string, std::list < message_t > > messages_; // multiple message queues, for every registered listener
	void send_message( const message_t &m ) {
		std::lock_guard<std::mutex> lock( messages_m_ );
		for ( auto &l : messages_ ) {
			if ( l.second.size() > 100 ) l.second.pop_front();
			l.second.push_back( m );
		}
		cv_.notify_all();
	}

	std::list<message_t> wait_message( const std::string &id_, int timeout = 10000 ) {
		{
			std::lock_guard<std::mutex> lock( messages_m_ );
			if ( messages_[id_].size() > 0 ) {
				auto ret = messages_[id_];
				messages_[id_].clear();
				return ret;
			}
		}
		std::unique_lock<std::mutex> lk( cv_m_ );
		cv_.wait_for( lk, std::chrono::milliseconds( timeout ) );
		{
			std::lock_guard<std::mutex> lock( messages_m_ );
			auto ret = messages_[id_];
			messages_[id_].clear();
			return ret;
		}
	}

	message_service() {
	}

	virtual ~message_service() {
		cv_.notify_all();
	}

};


}
}

#endif
