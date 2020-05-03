#include "PersistenceEntity.hpp"

template <>
__FIELD_TYPE __FIELD::operator=(const char* value) {
	isM = true;
	this->value = value;
	return  this->value;
}

void PersistenceEntity::operator()(__PTR_PERSISTENCE_PROPERTY_VALUE val, __PTR_PERSISTENCE_CLASS_PROPERTY_VALUE sVal, const std::string& name) {
	if (val && sVal) {
		(*(static_cast<__FIELD*>(sVal))) << (val);
	}
}

