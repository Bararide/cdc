#ifndef CONSUMER_H
#define CONSUMER_H

#include "parser.h"
#include "Drivers/baseConnector.h"
#include "Broker/broker.h"

#include "Logger/Logger.h"

#include <iostream>
#include <cppkafka/cppkafka.h>
#include <nlohmann/json.hpp>
#include <memory>

#include <atomic>
#include <mutex>
#include <thread>

#include <future>

#include <map>

namespace CONSUMER
{
	template<typename T, typename N>
	class Consumer
	{
	private:
		cppkafka::Configuration config;
		cppkafka::Consumer consumer;

		std::string topic;

		std::mutex consumer_mutex;

		std::unique_ptr<PARSER::Parser> parser;
		std::shared_ptr<CONNECTOR::BaseConnector<T>> conn;
		std::shared_ptr<LOGGER::Logger<N>> logger;
		std::shared_ptr<BROKER::Broker> broker;
		std::shared_ptr<BROKER::Broker> servbroker;

        std::map<std::string, json (*)(const std::vector<std::string>, json&)> vecops {
            {"COLUMNS", [](std::vector<std::string> words, json& response) {
                response["COLUMNS"] = json::array();
                for (const auto& word : words) 
                {
                    response["COLUMNS"].push_back(word);
                }
                return response;
            }}
        };
        
		void delegate_message(int oper, const std::string& msg, const json& json_response)
		{
		    bool success = false;
		    std::string log_prefix;

		    switch (oper)
		    {
		        case 0:
		            {
		            	std::cout << "0" << std::endl;
		                std::vector<std::string> res = conn->select(msg);

		                std::cout << res.empty() << std::endl;
		                if (!res.empty())
		                {
		                    success = true;
		                    log_prefix = "[OKEY]";
		                    json response;

		                    vecops["COLUMNS"](res, response);
		                    res.clear();

		                    std::cout << response.dump(4) << std::endl;

		                    broker->post_message(response);
		                } 
		                else 
		                {
		                    log_prefix = "[ERROR]";
		                }
		                break;
		            }
		        case 1:
		        	std::cout << "1" << std::endl;
		            success = conn->update(msg);
		            log_prefix = success ? "[OKEY]" : "[ERROR]";
		            break;
		        case 2:
		        	std::cout << "2" << std::endl;
		            success = conn->insert(msg);
		            log_prefix = success ? "[OKEY]" : "[ERROR]";
		            break;
		        default:
		            log_prefix = "[ERROR]";
		            break;
		    }

		    if (success) 
		    {
		        broker->post_message(json_response);
		    }

		    std::thread log_thread(&LOGGER::Logger<N>::post_message, logger, log_prefix + " " + msg, msg, success);

		    if (log_thread.joinable()) 
		    {
		        log_thread.join();
		    } 
		    else 
		    {
		        std::cerr << "Ошибка создания потока для логирования" << std::endl;
		    }
		}

	public:
		Consumer(std::shared_ptr<CONNECTOR::BaseConnector<T>>& conn, 
				 std::shared_ptr<BROKER::Broker>& broker, 
				 std::shared_ptr<BROKER::Broker>& servbroker, 
				 std::shared_ptr<LOGGER::Logger<N>>& logger, 
				 std::string topic)
			: config{
			    {"metadata.broker.list", "localhost:9092"},
			    {"enable.auto.commit", "false"},
			    {"group.id", "test_group"}
			},
			consumer(config),
			topic(topic),
			logger(logger),
			parser(std::make_unique<PARSER::Parser>()),
			broker(broker),
			servbroker(servbroker),
			conn(conn.get())
		{}

	    void parserThread(std::unique_ptr<PARSER::Parser> parser, std::string request)
		{
			parser->Request_parse(request);
		}

	    void get_message()
	    {
	    	try
	    	{
		        this->consumer.subscribe({topic});

		        while(true)
		        {
		            cppkafka::Message msg = this->consumer.poll();

		            if (msg)
		            {
		            	std::lock_guard<std::mutex> lock(consumer_mutex);
		                if (msg.get_error()) 
		                {
		                    std::cout << "[" << "\033[31m" << "Error" << "\033[0m" << "] get kafka message" << std::endl;
		                }
		                else 
		                {
		                    std::cout << msg.get_payload() << std::endl;
		                    SUB::Third<json, bool, int> result = parser->Request_parse(msg.get_payload());

		                    if(result.second)
		                    {
		                    	delegate_message(result.third, msg.get_payload(), result.first);
		                    }
		                }
		            }
		        }
	    	}
	    	catch (const std::exception& e)
	    	{
	    		std::cout << "1: " << e.what() << std::endl;
	    	}
	    }
	};
}

#endif