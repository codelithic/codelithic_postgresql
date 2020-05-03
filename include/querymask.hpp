#ifndef BEEBLEBROX_PERSISTENCE_QUERYMASK_ROUTE_HPP
#define BEEBLEBROX_PERSISTENCE_QUERYMASK_ROUTE_HPP

#include <string>
#include <vector>

class QueryMask {
public:
	QueryMask() {
	}

	~QueryMask() {
	}
	void setQuery(std::string _q) {
		this->mQuery = std::move(_q);
	}

	const std::string_view& getQuery() {
		return mQuery;
	}

	const std::vector<std::string_view>& getParams() const {
		return mParam;
	}

	QueryMask& operator<<(std::string_view& p) {
		mParam.push_back(std::move(p));
		return *this;
	}

	QueryMask& operator<<(std::string& p) {
		mParam.push_back(std::move(p));
          return *this;
	}

	QueryMask& operator<<(int p) {
		mParam.push_back(std::to_string(p));
          return *this;
	}

	QueryMask& operator<<(double p) {
		mParam.push_back(std::to_string(p));
          return *this;
	}

private:
	std::string_view mQuery{ "" };
	std::vector<std::string_view> mParam{};

	// QueryMask(const QueryMask& p);

public:
	QueryMask(const QueryMask& other) : mQuery{ other.mQuery }, mParam{ other.mParam } {
	}
	QueryMask(QueryMask&& other) noexcept(false) : mQuery{ std::move(other.mQuery) }, mParam{ std::move(other.mParam) } {
	}

	QueryMask& operator=(const QueryMask& other) {
		if (this == &other)
			return *this;

		this->mQuery = other.mQuery;
		this->mParam = other.mParam;

		return *this;
	}

	QueryMask& operator=(QueryMask&& other) noexcept(false) {
		if (this == &other)
			return *this;

		this->mQuery = std::move(other.mQuery);
		this->mParam = std::move(other.mParam);

		return *this;
	}
};

#endif // BEEBLEBROX_PERSISTENCE_querymask_ROUTE_HPP
