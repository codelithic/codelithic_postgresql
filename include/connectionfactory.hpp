#ifndef BEEBLEBROX_PERSISTENCE__ConnectionFactory_HPP
#define BEEBLEBROX_PERSISTENCE__ConnectionFactory_HPP

#include "postgresql.hpp"

#include <sstream>
#include <map>
#include <memory>
#include <string>

typedef POSTGRESQL CONNECTION;
#define __UNIQUE_CONNECTOR std::unique_ptr<CONNECTION>

class ConnectionFactory {
public:
	ConnectionFactory()= default;

	std::string getConnectionString(const char* user,const char* password, const char* db, const char* host, const char* port  ){

		if (user==nullptr || password==nullptr || db==nullptr || host==nullptr  ){
			throw std::runtime_error("It is impossible to create the connection String, check the connection params");
		}

		std::ostringstream stringStream;
		stringStream << "dbname=" << db << " host=" << host << " password="<<password << " user=" << user;
		
		if (port!=nullptr){
			stringStream << " port=" << port;
		}

		return stringStream.str();
	}

	void setConnectionString(std::string connectionString) {
		this->mConnectionString = std::move(connectionString);
	}

	__UNIQUE_CONNECTOR getUnnamedConnector(bool throwException = false) {
		CONNECTION* connection = new CONNECTION(mConnectionString, throwException);
		if (!connection->getMessageError().empty()) {
//			std::cerr << connection->getMessageError() << std::endl;

			if(throwException)
				throw std::runtime_error(connection->getMessageError());

			return __UNIQUE_CONNECTOR(nullptr);
		}

		return __UNIQUE_CONNECTOR(connection);
	}

	CONNECTION* getConnector(const std::string& connectionName, bool throwException = false) {
		if (mConnectionString.empty() || connectionName.empty()) {
			return nullptr;
		}

		auto connection = mConnectionReleased[connectionName];
		if (connection == nullptr) {
			connection = new CONNECTION(mConnectionString, throwException);
			if (!connection->getMessageError().empty()) {

				if(throwException)
					throw std::runtime_error(connection->getMessageError());

				return nullptr;
			}

			mConnectionReleased[connectionName] = connection;
		}

		return connection;
	}

	void releaseConnection(const std::string& connectionName) {
		auto connection = mConnectionReleased[connectionName];
		if (connection != nullptr) {
			mConnectionReleased.erase(connectionName);
			delete connection;
		}
	}

	void releaseAllConnection() {
		for (auto& m : mConnectionReleased) {
			delete m.second;
		}
		this->mConnectionReleased.clear();
	}

	~ConnectionFactory(){
		releaseAllConnection();
	};

private:
	std::string mConnectionString{ "" };
	std::map<std::string, CONNECTION*> mConnectionReleased{};

public:
	virtual void init(void* ) = 0;
};

#endif // BEEBLEBROX_PERSISTENCE__ConnectionFactory_HPP
