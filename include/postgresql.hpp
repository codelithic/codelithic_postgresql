#ifndef POSTGRESQL_CODELITHIC_H
#define POSTGRESQL_CODELITHIC_H

#include <array>
#include <cstring>
#include <iostream>
#include <libpq-fe.h>
#include <memory>
#include <string>
#include <vector>
#include <sstream>

#include "PersistenceEntity.hpp"
#include "querymask.hpp"

// PERFECT FORWARDING!
#define SQL3_FWD(...) ::std::forward<decltype(__VA_ARGS__)>(__VA_ARGS__)


//#include "t2pc/utils/log.hpp"
//#define INTERNAL_FIELD_SEARCH

class STMT {
public:
	void resetError() {
		messagError.clear();
	}

	const std::string &getError() {
		return messagError;
	}

	void setError(std::string error) {
		messagError.assign(std::move(error));
	}

	PGresult *operator()() {
		return stmt;
	}

	STMT &operator<<(PGresult *ts) {
		stmt = ts;
#ifdef INTERNAL_FIELD_SEARCH
		prepareFieldsName();
#endif
		return *this;
	}

	virtual ~STMT() {
		dispose();
	}

	int getNTuple() {
		if (stmt != nullptr) {
			return PQntuples(stmt);
		}
		return -1;
	}

	void dumpRes() {
		int i, j;
		int nFields = PQnfields(stmt);
		for (i = 0; i < PQntuples(stmt); i++) {
			for (j = 0; j < nFields; j++) {
				printf("%-15s = %-15s %d  \n", PQfname(stmt, j), PQgetvalue(stmt, i, j),
					   PQfnumber(stmt, PQfname(stmt, j)));
			}
			printf("\n");
		}

//		const char* ID={"ID"};
//		std::cout << ">>" <<  PQfnumber(stmt,ID) << "\n";
	}


//#include <algorithm>
//	// change each array element to the second power
//	std::for_each(std::begin(array1), std::end(array1), [](int& v) { v=v*v; });
//	std::for_each(std::begin(array1), std::end(array1), [](int v){ std::cout << v << " "; });
//	std::cout << std::endl;
//
//	std::cout << std::endl;

#ifdef INTERNAL_FIELD_SEARCH
	std::map<std::string,int> fieldsTupla{};
	void prepareFieldsName(){
		int nFields = PQnfields(stmt);
		if (nFields >0){
			for (int j = 0; j < nFields; j++) {
				fieldsTupla[ std::string(PQfname(stmt, j))  ]= j;
			}
		}
	}
#endif

	void getFieldName(std::vector<char *> &arr) {
		int nFields = PQnfields(stmt);
		if (nFields > 0) {
			for (int i = 0; i < nFields; i++) {
				arr.push_back(PQfname(stmt, i));
			}
		}
	}

	void getResult(int p1, PersistenceEntity *entity) {
		if (entity != nullptr) {
			if(entity->isAutoClean())
				entity->__MAPPER_RESET();

			entity->__MAPPER_RUN();
			int fieldId{-1};
			auto m1 = entity->getMapperVar();
			if (m1.size())
				for (auto &v : m1) { // TODO: DUPLICATED CODE--------------INI
#ifdef INTERNAL_FIELD_SEARCH
					auto F=fieldsTupla.find(v.first);
					if (F!=fieldsTupla.end()){
						(*entity)(PQgetvalue(stmt, p1, F->second), v.second, v.first);
					}
#else
					fieldId = PQfnumber(stmt, v.first.c_str());
					if (fieldId != -1) {
						(*entity)(PQgetvalue(stmt, p1, fieldId), v.second, v.first);
					}
#endif
				} // TODO: DUPLICATED CODE--------------INI
			auto m2 = entity->getMapper();
			if (!m2.empty())
				for (auto &v : m2) { // TODO: DUPLICATED CODE

#ifdef INTERNAL_FIELD_SEARCH
					auto F=fieldsTupla.find(v.first);
					if (F!=fieldsTupla.end()){
						v.second(PQgetvalue(stmt, p1, F->second));
					}
#else
					fieldId = PQfnumber(stmt, v.first.c_str());
					if (fieldId != -1) {
						v.second(PQgetvalue(stmt, p1, fieldId));
					}
#endif
				} // TODO: DUPLICATED CODE--------------INI
			if(entity->isAutoClean())
				entity->__MAPPER_RESET();
		}
	}

	//---------------------------------------------------
	// ATTENZIONE I PUNTATIRI POSSONO PERDERE IL VALORE
	//---------------------------------------------------
	template<int VAL>
	void getResult(int p1, std::array<char *, VAL> &a1) {
		int nf = PQnfields(stmt);
		int nFields = (VAL > nf ? nf : VAL);
		for (int j = 0; j < nFields; j++)
			a1[j] = (PQgetvalue(stmt, p1, j));
	}

	void getResult(int p1, std::vector<char *> &v) {
		int nFields = PQnfields(stmt);
		char *L{nullptr};
		for (int j = 0; j < nFields; j++) {
			L = PQgetvalue(stmt, p1, j);
			v.push_back((L == nullptr ? (char *) "" : L));
		}
	}
	//---------------------------------------------------
	
	template<int VAL>
	void getResult(int p1, std::array<std::string, VAL> &a1) {
		int nf = PQnfields(stmt);
		int nFields = (VAL > nf ? nf : VAL);
		char *L{nullptr};
		for (int j = 0; j < nFields; j++){
			L = PQgetvalue(stmt, p1, j);
			a1[j].assign((L == nullptr ? (char *) "" : L));
		}
	}

	void getResult(int p1, std::vector<std::string> &v) {
		int nFields = PQnfields(stmt);
		char *L{nullptr};
		for (int j = 0; j < nFields; j++) {
			L = PQgetvalue(stmt, p1, j);
			v.push_back((L == nullptr ? (char *) "" : L));
		}
	}

	// void getResult(int p1, int &Q) {
	// 	if (tupla == -1) {
	// 		cursor = 0;
	// 		tupla = 0;
	// 	}
	// 	auto L = PQgetvalue(stmt, tupla, cursor++);
	// 	if(L == nullptr){
	// 		return;
	// 	}
	// 	std::istringstream iss (L);
	// 	iss >> Q;
	// 	if (!iss.good ()) {
	// 		std::runtime_error("INT CONVERSION FAILED");
	// 	}
	// }

	template<typename TT>
	void getResult(int p1, TT &Q) {
		if (tupla == -1) {
			cursor = 0;
			tupla = 0;
		}
		auto L = PQgetvalue(stmt, tupla, cursor++);
		if(L == nullptr){
			return;
		}
		std::istringstream iss (L);
		iss >> Q;
		if (!iss.good ()) {
			std::runtime_error("CONVERSION FAILED");
		}
	}

	void getResult(int p1, std::string &Q) {
		if (tupla == -1) {
			cursor = 0;
			tupla = 0;
		}

		auto L = PQgetvalue(stmt, tupla, cursor++);
		Q = (L == nullptr ? "" : L);
	}

	template<typename T>
	void getResults(int p1, T &&t) {
		if (tupla != p1) {
			cursor = 0;
			tupla = p1;
		}
		getResult(p1, t);
	}

	template<typename T, typename... Ts>
	void getResults(int p1, T &&t, Ts &&... ts) {
		if (tupla != p1) {
			cursor = 0;
			tupla = p1;
		}
		getResults(p1, SQL3_FWD(t));
		getResults(p1, SQL3_FWD(ts)...);
	}

	STMT()
			: stmt{nullptr},
			  query{""} {

	};

	explicit STMT(std::string_view query) : stmt{nullptr}, query{query} {
	}

	void setQuery(std::string _q) {
		query = std::move(_q);
	}

private:
	PGresult *stmt{nullptr};

	std::string_view query{""};
	std::vector<char *> params{};
	std::string messagError{""};
	int tupla{-1}, cursor{0};

private:
	void dispose() {
		if (stmt != nullptr) {
			PQclear(stmt);
		}
		// query.clear();

		if (!params.empty())
			for (auto &p : params)
				delete[] p;

		params.clear();
	}

public:
	explicit operator const std::string_view &() const {
		return query;
	}

	friend std::ostream &operator<<(std::ostream &os, const STMT &type) {
		os << type.query;
		return os;
	}

	bool isParamsEmpty() {
		return params.empty();
	}

	int getParamSize() {
		return static_cast<int>(params.size());
	}

	std::vector<char *> &getParams() {
		return params;
	}

	template<typename... Ts>
	void Bind(Ts &&... ts) {
		set_option(SQL3_FWD(ts)...);
	}

	template<typename Ts>
	STMT &operator<<(Ts & ts) {//NOT THREAD SAFE!!!
		set_option(SQL3_FWD(ts));
		return *this;
	}

	template<typename T>
	void set_option(T &&t) {
		setBind(t);
	}

	template<typename T, typename... Ts>
	void set_option(T &&t, Ts &&... ts) {
		set_option(SQL3_FWD(t));
		set_option(SQL3_FWD(ts)...);
	}

	void copyAndPush(std::string val) {
		if (!val.empty()) {
			int size = static_cast<int>(val.size()) + 1;
			auto myParam = new char[size];
			memset(myParam, '\0', size);
			strcpy(myParam, val.c_str());

			params.push_back(myParam);
		}
	}

	// void setBind(const std::vector<std::string>& _params) {
	// 	// copyAndPush(std::move(std::to_string(Q)));
	// 	params = _params;
	// }

	template<typename T>
	void setBind(T& Q) {
		copyAndPush(std::move(std::to_string(Q)));
	}


	// void setBind(long Q) {
	// 	copyAndPush(std::move(std::to_string(Q)));
	// }

	// void setBind(double Q) {
	// 	copyAndPush(std::move(std::to_string(Q)));
	// }

	// void setBind(int Q) {
	// 	copyAndPush(std::move(std::to_string(Q)));
	// }

	// void setBind(unsigned int Q) {
	// 	copyAndPush(std::move(std::to_string(Q)));
	// }

        void setBind(const std::string& Q) {
          copyAndPush(Q);
          // copyAndPush(std::move((Q)));
        }

	void setBind(std::string& Q) {
		copyAndPush(Q);
		// copyAndPush(std::move((Q)));
	}
};

class POSTGRESQL {
public:
	template<typename T>
	void operator()(std::vector<std::unique_ptr<T>> &_v, STMT &stml) {
		resetError();
		*this << &stml;
		messagError = (stml.getError());
		if (messagError.empty() && stml.getNTuple() > 0) {
			for (int i = 0; i < stml.getNTuple(); i++) {
				T *c = new T();
				c->__MAPPER_RUN();
				stml.getResult(i, c);
				// c->__MAPPER_RESET();
				c->__MAPPER_RESET();
				_v.push_back(std::unique_ptr<T>(c));
			}
		}
	}

	template<typename T>
	void operator()(std::vector<std::unique_ptr<T>> &_v, QueryMask &_q) {
		resetError();
		STMT stml(_q.getQuery());
		for (auto &t : _q.getParams())
			stml << t;
		*this << &stml;
		messagError = (stml.getError());

		if (messagError.empty() && stml.getNTuple() > 0) {
			for (int i = 0; i < stml.getNTuple(); i++) {
				T *c = new T();
				c->__MAPPER_RUN();
				stml.getResult(i, c);
				c->__MAPPER_RESET();
				_v.push_back(std::unique_ptr<T>(c));
			}
		}
	}

	void operator()(PersistenceEntity *_v, STMT &stml) {
		resetError();

		if (_v == nullptr) {

			if (throwException)
				throw std::runtime_error("Entity is nullptr");
			setError("Entity is nullptr");
		} else {
			*this << &stml;
			messagError = (stml.getError());
			if (messagError.empty() && stml.getNTuple() > 0) {

				auto rs = stml.getNTuple();
				if (rs == 1) {
					_v->__MAPPER_RUN();
					stml.getResult(0, _v);
					_v->__MAPPER_RESET();
				} else {
					if (rs > 0) {
						if (throwException)
							throw std::runtime_error("The result returns more tuples");
						setError("The result returns more tuples");
					}
				}
			}
		}
	}

	//*********************************************************
	// DO NOT REMOVE THIS COMMETs
	//*********************************************************
//	void operator()(PersistenceEntity *_e) {
//		if (_e) {
//			resetError();
//			_e->__MAPPER_RUN();
//			std::string query{"select * from "};
//			query.append(_e->__GET_TABLE());
//			query.append(" where id=$1;");
//
//			STMT stml(query);
//			stml << _e->getId();
//			*this << &stml;
//			messagError = (stml.getError());
//			if (messagError.empty() && stml.getNTuple() > 0) {
//				// _e->__MAPPER_RUN();
//				stml.getResult(0, _e);
//			}
//			_e->__MAPPER_RESET();
//		} else {
//			if (throwException)
//				throw std::runtime_error("Entity is nullptr");
//			setError("Entity is nullptr");
//		}
//	}
	//*********************************************************
	// DO NOT REMOVE THIS COMMETs
	//*********************************************************

public:
	POSTGRESQL(std::string conninfo, bool throwException = false) : throwException(throwException) {
		conn = PQconnectdb(conninfo.c_str());
		if (PQstatus(conn) != CONNECTION_OK) {
			setError("Connection to database failed");
			dispose();
		}
	}

	virtual ~POSTGRESQL() {
		if (conn) {
			dispose();
		}
	}

	PGconn *operator()() {
		return conn;
	}

	POSTGRESQL &operator<<(STMT *ts) {
		if (ts != nullptr) {
			ts->resetError();
			resetError();
			if (conn != nullptr) {
				PGresult *res{nullptr};
				if (ts->isParamsEmpty()) {
					res = PQexec(conn, (static_cast<std::string_view>((*ts))).data());
				} else {
					auto V = ts->getParams();
					const char *paramValues[V.size()];
					for (int i = 0; i < V.size(); i++) {
						paramValues[i] = V[i];
					}
					res = PQexecParams(conn, (static_cast<std::string_view>((*ts))).data(), V.size(), NULL, paramValues,
									   NULL, NULL, 0);
				}

				auto pRes = PQresultStatus(res);
				if (pRes == PGRES_COMMAND_OK || pRes == PGRES_TUPLES_OK || pRes == PGRES_SINGLE_TUPLE) {
					(*ts) << res;
				} else {
					PQclear(res);
					std::string error{PQerrorMessage(conn)};
					ts->setError(error);
					setError(error);

					if (throwException)
						throw std::runtime_error(error);
				}
			}
		}

		return *this;
	}

	void setThrowException(bool val) {
		throwException = val;
	}

	bool getThrowException() {
		return throwException;
	}

	const std::string &getMessageError() const {
		return messagError;
	}

private:
	void resetError() {
		messagError.clear();
	}

	void setError() {
		if (conn != nullptr) {
			messagError.assign(PQerrorMessage(conn));
		} else
			messagError.assign("Not connected");
	}

	void setError(std::string error) {
		if (conn != nullptr) {
			messagError.assign(error);
			messagError.append(": ");
			messagError.append(PQerrorMessage(conn));
		} else
			messagError.assign("Not connected");
	}

	void dispose() {

//		if(PQstatus(conn) != CONNECTION_OK){
		PQfinish(conn);
//		}
		conn = nullptr;
	}

	std::string messagError{""};
	std::string conninfo{""};
	PGconn *conn{nullptr};
	bool isConnected{false};
	bool throwException{false};
};

#endif // POSTGRESQL_CODELITHIC_H

// apt-get install -y libpq-dev postgresql-client mlocate
// g++ -std=c++11 -I/usr/include/postgresql/ -lpq t3.cpp
