#include "network.h"

namespace NETWORK
{
	void Session::start()
	{
		boost::asio::async_read_until(
			socket,
			streambuf,
			'\n',
			[self = shared_from_this()] (
				boost::system::error_code error,
				std::size_t bytes_transferred) {
					std::cout << std::istream(&self->streambuf).rdbuf();
			});
	}

	void Server::async_accept()
	{
		socket.emplace(io_context);

		acceptor.async_accept(*socket, [&](boost::system::error_code error) {
			std::make_shared<Session>(std::move(*socket))->start();
			async_accept();
		});
	}
}