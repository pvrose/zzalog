#pragma once

#include <cstdint>
#include <map>
#include <string>

//! This class provides access to the API keys for non-user specific data.
class keyring {

public:
	//! Constructor.
	//! Opens the keyring file "keys.json", craeting it if necessary and reading all the keys.
	keyring();
	//! Destructor
	~keyring();
	//! Load data
	bool load_data();
	//! Save data
	bool save_data();
	//! Return the key for specified \p item.
	const std::string key(const std::string& item);
	//! Set key \p item to \p value.
	void key(const std::string& item, const std::string& value);

protected:
	//! The bunch of keys
	std::map<std::string, std::string> keys_;
	//! Seed used
	uint32_t seed_ = 0;
};
