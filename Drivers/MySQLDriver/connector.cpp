#include "connector.h"

namespace MYSQLCONN
{
	Mysqlconn::Mysqlconn(std::string&& host, std::string&& user, std::string&& password, std::string&& database)
	{
	    try
	    {
	        session = Session("localhost", host, user, password);
	        schema = session.createSchema(database, "test");
	        //collection = schema.createCollection("your_collection_name", true);
	    }
	    catch (const std::exception& e)
	    {
	        std::cout << e.what() << std::endl;
	    }
	}

	/*bool Mysqlconn::insert(const std::string& data)
	{
		try
	    {
	        Result add = collection.add(data).execute();
	        return add.getGeneratedIds().size() > 0;
	    }
	    catch (const std::exception& e)
	    {
	        std::cout << e.what() << std::endl;
	        return false;
	    }
	}

	std::vector<std::string> Mysqlconn::select(const std::string& select_query)
	{
	    std::vector<std::string> results;

	    try
	    {
	        DocResult docs = collection.find(select_query).execute();

	        for (DbDoc doc : docs)
	        {
	            std::string result = "Document: " + doc.toString();
	            results.push_back(result);
	        }
	    }
	    catch (const std::exception& e)
	    {
	        std::cout << e.what() << std::endl;
	    }

	    return results;
	}

    void Mysqlconn::update(const std::string& update_query)
    {
        try
        {
            collection.update(update_query).execute();
        }
        catch (const std::exception& e)
        {
            std::cout << e.what() << std::endl;
        }
    }

    void Mysqlconn::remove(const std::string& remove_query)
    {
        try
        {
            collection.remove(remove_query).execute();
        }
        catch (const std::exception& e)
        {
            std::cout << e.what() << std::endl;
        }
    }*/
}