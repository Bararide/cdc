#ifndef BASECONNECTOR_H
#define BASECONNECTOR_H

#include <iostream>
#include <memory>
#include <vector>

namespace CONNECTOR
{
	template<typename T>
	class BaseConnector
	{
	private:
		std::shared_ptr<T> conn;

	public:
		BaseConnector(std::shared_ptr<T> conn) : conn(conn) {}

		bool insert(const std::string data)
		{
			try
			{
				return conn->insert(data);
			}
			catch(const std::exception& e)
			{
				std::cout << e.what() << std::endl;
				return false;
			}
		}

		bool update(const std::string data)
		{
			try
			{
				return conn->update(data);
			}
			catch(const std::exception& e)
			{
				std::cout << e.what() << std::endl;
				return false;
			}
		}

		std::vector<std::string> select(const std::string select_query)
		{
			try
			{
				return conn->select(select_query);
			}
			catch (const std::exception& e)
			{
				std::cout << e.what() << std::endl;
				return {};
			}
		}

		void post_logger_message(const std::string& request, bool status)
		{
			try
			{
				return conn->post_logger_message(request, status);
			}
			catch(const std::exception& e)
			{
				std::cout << e.what() << std::endl;
			}
		}
	};
}

#endif