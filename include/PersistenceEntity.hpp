#ifndef BEEBLEBROX_ENTITIES_Entity_HPP
#define BEEBLEBROX_ENTITIES_Entity_HPP

#include "codelithic/delegate/Delegate.hpp"
// #include <iostream>
#include <iostream>
#include <map>
#include <ostream>
#include <string>

#define __ENT_CAST(D) dynamic_cast<PersistenceEntity*>(D)

#define __PTR_PERSISTENCE_CLASS_PROPERTY_VALUE void*
#define __PTR_PERSISTENCE_PROPERTY_VALUE const char*
#define __PERSISTENCE_DELEGATE_TYPE codelithic::delegate<void(__PTR_PERSISTENCE_PROPERTY_VALUE)>

#define __MAPPER_RESET unMapper
#define __MAPPER_RUN mapper
#define __SET_TABLE(X) setTable(X);
#define __GET_TABLE getTable
#define __MAPPER_INI_MASTER virtual void __MAPPER_RUN() {
#define __MAPPER_INI void clientMapper() override {
#define __MAPPER_FUNCT(__NAME, __CLASS, __FUNCTION) (mComunication[__NAME] = __PERSISTENCE_DELEGATE_TYPE::create<__CLASS, &__CLASS::__FUNCTION>(this));
#define __MAPPER_VAR(__NAME, __POINTER) (mComunicationString[__NAME] = __POINTER);
#define __MAPPER_LAMBDA(__NAME, X) mComunication[__NAME] = __PERSISTENCE_DELEGATE_TYPE::create(X);
#define __MAPPER_END }

#define __FIELD_TYPE std::string
#define __FIELD Field<__FIELD_TYPE>

class myfiedl {
public:
};

template <typename T>
class Field : protected myfiedl {
public:
	Field(const Field& other) : value{ other.value }, isM{ other.isM } {
	}
	Field(Field&& other) noexcept : value{ std::move(other.value) }, isM{ other.isM }  {
	}

	Field& operator=(const Field& other) {
		if (this == &other)
			return *this;
		this->value = other.value;
		this->isM = other.isM;
		return *this;
	}
	Field& operator=(Field&& other) noexcept   {
		if (this == &other)
			return *this;
		this->value = std::move(other.value);
		this->isM = other.isM;
		return *this;
	}

	Field& operator<<(const char* str) {
		this->value = str;
		// os << type.value;
		return *this;
	}

	friend std::ostream& operator<<(std::ostream& os, const Field& type) {
		os << type.value;
		return os;
	}
	inline bool operator==(const T& rhs) {
		return this->value == rhs;
	}
	inline bool operator!=(const T& rhs) {
		return !(this->value == rhs);
	}

public:
	Field() {
		isM = false;
	}
	explicit Field(const T& ssvalue) : value{ ssvalue } {
		isM = false;
	}
	operator T() const {
		return value;
	}
	T operator=(const char* value) {
		isM = true;
		return this->value = value;
	}
	T operator=(const T& value) {
		if (value != this->value) {
			isM = true;
		}
		return this->value = value;
	}
	void reset() {
		isM = false;
	}
	bool isModified() {
		return isM;
	}

private:
	T value{};
	bool isM{ false };
};

class PersistenceEntity {
public:
//	enum class Operation { none, get, insert, update, remove };

	void operator()(const std::string& val, __PTR_PERSISTENCE_PROPERTY_VALUE mt) {
		auto D = this->mComunication[val];
		if (D != nullptr) {
			D(mt);
		}
	}

	virtual void operator()(__PTR_PERSISTENCE_PROPERTY_VALUE val, __PTR_PERSISTENCE_CLASS_PROPERTY_VALUE sVal, const std::string& name);
	// virtual void operator()(__PTR_PERSISTENCE_PROPERTY_VALUE val, __PTR_PERSISTENCE_CLASS_PROPERTY_VALUE sVal, const std::string& name) {
	// 	if (val && sVal) {
	// 		(*(static_cast<__FIELD*>(sVal))) = (val);
	// 	}

	// 	// (static_cast<std::string*>(sVal))->assign(val);
	// }

	__PERSISTENCE_DELEGATE_TYPE operator()(std::string& val) {
		return mComunication[val];
	}

	__MAPPER_INI_MASTER
	clientMapper();
	__MAPPER_END

	virtual void clientMapper() {
	}

	virtual bool isModified() {
		return false;
	}

	 virtual const __FIELD& getId() const {
	 	return emptyField;
	 }

//	virtual const __FIELD& getId() const =0;


	std::map<std::string, __PTR_PERSISTENCE_CLASS_PROPERTY_VALUE>& getMapperVar() {
		return mComunicationString;
	}

	std::map<std::string, __PERSISTENCE_DELEGATE_TYPE>& getMapper() {
		return mComunication;
	}

public:
//	void setOperation(Operation op) {
//		this->mOperation = op;
//	}
//
//	const Operation& getOperation() const {
//		return this->mOperation;
//	}

	const std::string& getTable() const {
		return mTable;
	}

	void __MAPPER_RESET() {
		this->mComunication.clear();
		this->mComunicationString.clear();
	}

	unsigned long SIZE() {
		return this->mComunication.size();
	}

	std::string getMapperFields(){
		if (this->mComunication.empty()){
			return "*";
		}
		
		std::string _v{};
		for(auto& [a,b]:this->mComunication){			
			_v.append( " " + a );
		}

		return _v;
	}


	bool isAutoClean() const {
		return autoClean;
	}

	void setAutoClean(bool autoClean) {
		PersistenceEntity::autoClean = autoClean;
	}


protected:
	virtual void convert(__PTR_PERSISTENCE_PROPERTY_VALUE val, std::string& sVal) {
		if (val) {
			sVal.assign(val);
		} else {
			sVal.clear();
		}
	}

	void setTable(std::string table) {
		this->mTable =  std::move(table);
	}

	// TODO: should be a vector of pair | a class.
	std::map<std::string, __PERSISTENCE_DELEGATE_TYPE> mComunication{};
	std::map<std::string, __PTR_PERSISTENCE_CLASS_PROPERTY_VALUE> mComunicationString{};

private:
	std::string mTable{ "" };
//	Operation mOperation{ Operation::none };
	__FIELD emptyField{ "" };

	bool autoClean{true};

public:
	virtual void dump(std::ostream& os) {
	}

	PersistenceEntity()= default;
	virtual ~PersistenceEntity()= default;

	PersistenceEntity(const PersistenceEntity& other) : mTable{ other.mTable } {
		// if (other.mComunication.size() > 0) {
		// 	this->__MAPPER_RUN();
		// }
	}

	PersistenceEntity(PersistenceEntity&& other) noexcept(false) : mTable{ std::move(other.mTable) } {
		// if (other.mComunication.size() > 0) {
		// 	this->__MAPPER_RUN();
		// 	other.__MAPPER_RESET();
		// }
	}

	PersistenceEntity& operator=(const PersistenceEntity& other) {
		if (this == &other)
			return *this;

		this->mTable = other.mTable;

		// if (other.mComunication.size() > 0) {
		// 	this->__MAPPER_RUN();
		// }
		return *this;
	}
	PersistenceEntity& operator=(PersistenceEntity&& other) noexcept(false) {
		if (this == &other)
			return *this;

		this->mTable = std::move(other.mTable);

		// if (other.mComunication.size() > 0) {
		// 	this->__MAPPER_RUN();
		// 	other.__MAPPER_RESET();
		// }
		return *this;
	}
};

#endif
