#include "Drivers/baseConnector.h"

#include "Drivers/PostgreSQLDriver/connector.h"
#include "Drivers/SQLiteDriver/connector.h"
#include "Drivers/ClickHouseDriver/connector.h"

#include "Broker/broker.h"
#include "Network/network.h"

#include "Logger/Logger.h"
#include "consumer.h"

using json = nlohmann::json;

template<typename T, typename N>
void consumerThread(std::unique_ptr<CONSUMER::Consumer<T, N>> consumer);

int main(int argc, char** argv)
{
    try
    {
        opt::options_description desc("All options");

        desc.add_options()
        	("network,n",     "Connect to the network")
        	("port,p",        opt::value<int>(), "Port number")
        	("ip",            opt::value<std::string>(), "IP address")
        	("local,l",       "Working on the local machine")
            ("dbtype",        opt::value<std::string>()->default_value("postgresql"), "postgresql, clickhouse, mysql, sqlite, mongodb")
            ("dbname",        opt::value<std::string>()->default_value("test"), "Name of you main database")
            ("logger_dbname", opt::value<std::string>()->default_value("logger"), "Name of your logger database")
            ("user",          opt::value<std::string>()->default_value("bararide"), "Name of database user")
            ("password",      opt::value<std::string>()->default_value("0642q"), "Database password")
            ("hostaddr",      opt::value<std::string>()->default_value("127.0.0.1"), "Host address of database")
            ("dbport",        opt::value<std::string>()->default_value("5432"), "Port of database")
            ("help",          "print all commands");

        opt::variables_map vm;

        opt::store(opt::parse_command_line(argc, argv, desc), vm);
        opt::notify(vm);

        if(vm.count("help"))
        {
            std::cout << desc << std::endl;
        }
        else
        {
        	if(vm.count("network"))
        	{
        		if(vm.count("port") && vm.count("ip"))
        		{
        			int port       = vm["port"].as<int>();
        			std::string ip = vm["ip"].as<std::string>();

        			boost::asio::io_context io_context;
        			std::unique_ptr<NETWORK::Server> server = std::make_unique<NETWORK::Server>(io_context, port);
        			server->async_accept();
        			io_context.run();
        		}
        		else
        		{
	                std::cout << "Error: Both port and IP address are required for network connection." << std::endl;
	                return 1;
        		}
        	}

            if(vm.count("dbtype"))
            {
                if(vm["dbtype"].as<std::string>() == "postgresql")
                {
                    if(vm.count("dbname") && vm.count("user") && vm.count("password") && vm.count("hostaddr") && vm.count("dbport"))
                    {
                        std::string config = "dbname=" + vm["logger_dbname"].as<std::string>() +
                                                " user=" + vm["user"].as<std::string>() +
                                                " password=" + vm["password"].as<std::string>() + 
                                                " hostaddr=" + vm["hostaddr"].as<std::string>() + 
                                                " port=" + vm["dbport"].as<std::string>();

                        std::shared_ptr<CONNECTOR::BaseConnector<CONNECTION::PGConnection>> logbd = std::make_shared<CONNECTOR::BaseConnector<CONNECTION::PGConnection>>(std::make_shared<CONNECTION::PGConnection>(config));

                        std::string servprotopic = "servpost";
                        std::string postopic = "post";
                        std::string contopic = "data";
                        std::string logtopic = "log";

                        config = "dbname=" + vm["dbname"].as<std::string>() +
                                    " user=" + vm["user"].as<std::string>() +
                                    " password=" + vm["password"].as<std::string>() + 
                                    " hostaddr=" + vm["hostaddr"].as<std::string>() + 
                                    " port=" + vm["dbport"].as<std::string>();

                        std::shared_ptr<CONNECTOR::BaseConnector<CONNECTION::PGConnection>> conn = std::make_shared<CONNECTOR::BaseConnector<CONNECTION::PGConnection>>(std::make_shared<CONNECTION::PGConnection>(config));

                        std::shared_ptr<BROKER::Broker> broker          = std::make_shared<BROKER::Broker>(std::move(postopic));
                        std::shared_ptr<BROKER::Broker> logbroker       = std::make_shared<BROKER::Broker>(std::move(logtopic));
                        std::shared_ptr<BROKER::Broker> serbroker       = std::make_shared<BROKER::Broker>(std::move(servprotopic));

                        std::shared_ptr<LOGGER::Logger<CONNECTION::PGConnection>> logger = std::make_shared<LOGGER::Logger<CONNECTION::PGConnection>>(logbroker, logbd);

                        std::unique_ptr<CONSUMER::Consumer<CONNECTION::PGConnection, CONNECTION::PGConnection>> consumer = std::make_unique<CONSUMER::Consumer<CONNECTION::PGConnection, CONNECTION::PGConnection>>(std::ref(conn), std::ref(broker), std::ref(serbroker), std::ref(logger), std::move(contopic));

                        std::future<void> consumerFuture = std::async(std::launch::async, consumerThread<CONNECTION::PGConnection, CONNECTION::PGConnection>, std::move(consumer));
                        //std::future<void> producerFuture = std::async(std::launch::async, producerThread, std::move(broker), "hello, kafka");

                        consumerFuture.get();
                    }
                }
                else if(vm["dbtype"].as<std::string>() == "mysql")
                {

                }
                else if(vm["dbtype"].as<std::string>() == "sqlite")
                {
                    if(vm.count("dbname"))
                    {
                        std::shared_ptr<SQLITECONN::Sqliteconn> lite = std::make_shared<SQLITECONN::Sqliteconn>(
                            vm["dbname"].as<std::string>()
                        );

                        std::string config = "dbname=" + vm["logger_dbname"].as<std::string>() +
                                            " user=" + vm["user"].as<std::string>() +
                                            " password=" + vm["password"].as<std::string>() + 
                                            " hostaddr=" + vm["hostaddr"].as<std::string>() + 
                                            " port=" + vm["dbport"].as<std::string>();

                        std::shared_ptr<CONNECTOR::BaseConnector<CONNECTION::PGConnection>> logbd = std::make_shared<CONNECTOR::BaseConnector<CONNECTION::PGConnection>>(std::make_shared<CONNECTION::PGConnection>(config));

                        std::string servprotopic = "servpost";
                        std::string postopic = "post";
                        std::string contopic = "data";
                        std::string logtopic = "log";

                        std::shared_ptr<BROKER::Broker> broker       = std::make_shared<BROKER::Broker>(std::move(postopic));
                        std::shared_ptr<BROKER::Broker> logbroker    = std::make_shared<BROKER::Broker>(std::move(logtopic));
                        std::shared_ptr<BROKER::Broker> serbroker    = std::make_shared<BROKER::Broker>(std::move(servprotopic));

                        std::shared_ptr<CONNECTOR::BaseConnector<SQLITECONN::Sqliteconn>> conn = std::make_shared<CONNECTOR::BaseConnector<SQLITECONN::Sqliteconn>>(lite);

                        std::shared_ptr<LOGGER::Logger<CONNECTION::PGConnection>> logger = std::make_shared<LOGGER::Logger<CONNECTION::PGConnection>>(logbroker, logbd);

                        std::unique_ptr<CONSUMER::Consumer<SQLITECONN::Sqliteconn, CONNECTION::PGConnection>> consumer = std::make_unique<CONSUMER::Consumer<SQLITECONN::Sqliteconn, CONNECTION::PGConnection>>(std::ref(conn), std::ref(broker), std::ref(serbroker), std::ref(logger), std::move(contopic));

                        std::future<void> consumerFuture = std::async(std::launch::async, consumerThread<SQLITECONN::Sqliteconn, CONNECTION::PGConnection>, std::move(consumer));
                        //std::future<void> producerFuture = std::async(std::launch::async, producerThread, std::move(broker), "hello, kafka");

                        consumerFuture.get();
                    }
                }
                else if(vm["dbtype"].as<std::string>() == "clickhouse")
                {
                	if(vm.count("dbname") && vm.count("hostaddr"))
                	{
                		std::shared_ptr<CLICKHOUSECONN::ClickHouseConn> click = std::make_shared<CLICKHOUSECONN::ClickHouseConn>(
                			vm["hostaddr"].as<std::string>(), vm["dbname"].as<std::string>()
                		);

                        std::string config = "dbname=" + vm["logger_dbname"].as<std::string>() +
                                            " user=" + vm["user"].as<std::string>() +
                                            " password=" + vm["password"].as<std::string>() + 
                                            " hostaddr=" + vm["hostaddr"].as<std::string>() + 
                                            " port=" + vm["dbport"].as<std::string>();

                		std::shared_ptr<CONNECTOR::BaseConnector<CONNECTION::PGConnection>> logbd = std::make_shared<CONNECTOR::BaseConnector<CONNECTION::PGConnection>>(std::make_shared<CONNECTION::PGConnection>(config));

                        std::string servprotopic = "servpost";
                        std::string postopic = "post";
                        std::string contopic = "data";
                        std::string logtopic = "log";

                        std::shared_ptr<BROKER::Broker> broker       = std::make_shared<BROKER::Broker>(std::move(postopic));
                        std::shared_ptr<BROKER::Broker> logbroker    = std::make_shared<BROKER::Broker>(std::move(logtopic));
                        std::shared_ptr<BROKER::Broker> serbroker    = std::make_shared<BROKER::Broker>(std::move(servprotopic));

                        std::shared_ptr<CONNECTOR::BaseConnector<CLICKHOUSECONN::ClickHouseConn>> conn = std::make_shared<CONNECTOR::BaseConnector<CLICKHOUSECONN::ClickHouseConn>>(click);

                        std::shared_ptr<LOGGER::Logger<CONNECTION::PGConnection>> logger = std::make_shared<LOGGER::Logger<CONNECTION::PGConnection>>(logbroker, logbd);

                        std::unique_ptr<CONSUMER::Consumer<CLICKHOUSECONN::ClickHouseConn, CONNECTION::PGConnection>> consumer = std::make_unique<CONSUMER::Consumer<CLICKHOUSECONN::ClickHouseConn, CONNECTION::PGConnection>>(std::ref(conn), std::ref(broker), std::ref(serbroker), std::ref(logger), std::move(contopic));

                        std::future<void> consumerFuture = std::async(std::launch::async, consumerThread<CLICKHOUSECONN::ClickHouseConn, CONNECTION::PGConnection>, std::move(consumer));

                        consumerFuture.get();
                	}
                }
            }
        }
        //producerFuture.get();
        // json ex1 = json::parse(R"(
    	// 		{
    	// 			"pi": 3.141,
    	// 			"happy": true
    	// 		}
    	// 	)");
    } 
    catch (const std::exception& e) 
    {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}

template<typename T, typename N>
void consumerThread(std::unique_ptr<CONSUMER::Consumer<T, N>> consumer)
{
    consumer->get_message();
}