#ifndef NETWORK_H
#define NETWORK_H

#include <boost/asio.hpp>
#include <boost/program_options.hpp>
#include <iostream>
#include <optional>

namespace opt = boost::program_options;

namespace NETWORK
{
	class Session: public std::enable_shared_from_this<Session>
	{
	public:
		Session(boost::asio::ip::tcp::socket&& socket) : socket(std::move(socket)) {}

		void start();

	private:
		boost::asio::ip::tcp::socket socket;
		boost::asio::streambuf streambuf;
	};

	class Server
	{
	public:
		Server(boost::asio::io_context& io_context, std::uint16_t port) :
			io_context(io_context),
			acceptor(
					io_context,
					boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)
				) {}

		void async_accept();

	private:
		boost::asio::io_context& io_context;
		boost::asio::ip::tcp::acceptor acceptor;
		std::optional<boost::asio::ip::tcp::socket> socket;
	};
}

#endif