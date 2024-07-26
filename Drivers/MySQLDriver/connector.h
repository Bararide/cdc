#ifndef MYSQLDRIVER_H
#define MYSQLDRIVER_H

#include <mysqlx/xdevapi.h>
#include <iostream>
#include <vector>

using namespace ::mysqlx;

namespace MYSQLCONN
{
	class Mysqlconn
	{
	private:
		Session session;
		Schema schema;
		Collection collection;

	public:
		Mysqlconn(std::string host, std::string user, std::string password, std::string database);

//		bool insert(const std::string& data);
		
//		void update(const std::string& update_query);
//		void remove(const std::string& remove_query);

//		std::vector<std::string> select(const std::string& select_query);
	};
}

#endif