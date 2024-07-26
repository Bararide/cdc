#include "broker.h"

namespace BROKER 
{

    void Broker::post_message(const std::string& message) 
    {
        std::cout << message << std::endl;
        std::lock_guard<std::mutex> lock(broker_mutex);
        this->producer.produce(cppkafka::MessageBuilder(topic).payload(message));
        this->producer.flush();
    }

    void Broker::post_message(const json& message) 
    {
        try
        {
            std::cout << message.dump(4) << std::endl;
            std::lock_guard<std::mutex> lock(broker_mutex);
            std::string mes = message.dump();
            this->producer.produce(cppkafka::MessageBuilder(topic).payload(mes));
            this->producer.flush();
        }
        catch (const std::exception& e)
        {
            std::cout << e.what() << std::endl;
        }
    }
}