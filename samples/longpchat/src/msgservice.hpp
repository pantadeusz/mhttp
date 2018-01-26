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
	void send_message( const message_t &m, const std::string &to_id = "" ) {
		std::lock_guard<std::mutex> lock( messages_m_ );
		if ( to_id == "" ) { // if we broadcast
			for ( auto &l : messages_ ) {
				if ( l.second.size() > 100 ) l.second.pop_front();
				l.second.push_back( m );
			}
		} else { // we send to one recipient
			if ( messages_[to_id].size() > 100 ) messages_[to_id].pop_front();
			messages_[to_id].push_back( m );
		}
		cv_.notify_all();
	}

	std::list<message_t> wait_message( const std::string &id_, int timeout = 10000 ) {
		std::unique_lock<std::mutex> lk( cv_m_ );
		cv_.wait_for( lk, std::chrono::milliseconds( timeout ), [this, id_] {
			std::lock_guard<std::mutex> lock( messages_m_ );
			return messages_[id_].size() > 0;
		} );
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
