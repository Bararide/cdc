#ifndef LOGGER_H
#define LOGGER_H

#include "../Broker/broker.h"
#include "../Drivers/baseConnector.h"

#include <mutex>
#include <thread>
#include <vector>
#include <memory>
#include <ctime>

namespace LOGGER
{
	template<typename T>
	class Logger
	{
	private:
		std::mutex logger_mutex;

		std::string query;

		std::shared_ptr<BROKER::Broker> broker;

		std::shared_ptr<CONNECTOR::BaseConnector<T>> conn;

	public:
		Logger(std::shared_ptr<BROKER::Broker> broker, std::shared_ptr<CONNECTOR::BaseConnector<T>> conn) : 
			broker(broker),
			conn(conn)
		{};

		void post_message(std::string message, std::string request, bool status)
		{
		    std::lock_guard<std::mutex> lock(logger_mutex);

		    this->broker->post_message(message);
		    this->conn->post_logger_message(request, status);
		}
	};
}

#endif