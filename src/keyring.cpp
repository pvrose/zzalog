#include "keyring.h"

#include "main.h"
#include "zc_file_holder.h"
#include "zc_status.h"
#include "zc_utils.h"

#include <fstream>
#include <iomanip>
#include <map>
#include <string>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include <FL/fl_ask.H>

//! Constructor
keyring::keyring() {
	load_data();
}

//! Destructor
keyring::~keyring() {
	save_data();
	if (DEVELOPMENT_MODE) {
		file_holder_->copy_working_to_source(FILE_KEYS);
		file_holder_->copy_source_to_git(FILE_KEYS);
	}
}

//! Load data
bool keyring::load_data() {
	std::ifstream is;
	std::string filename;
	if (file_holder_->get_file(FILE_KEYS, is, filename)) {
		status_->misc_status(ST_NOTE, "KEYS: Loading API keys");
		json jall;
		try {
			is >> jall;
			if (jall.find("Seed") == jall.end() || jall.find("Keys") == jall.end()) {
				status_->misc_status(ST_WARNING, "KEYS: No API keys have been initialised.");
				keys_.clear();
				return false;
			}
			jall.at("Seed").get_to(seed_);
			auto tags = jall.at("Keys").get<std::map<std::string, std::string> >();
			for (auto& tag : tags) {
				keys_[tag.first] = zc::xor_crypt(zc::hex_to_string(tag.second), seed_, 0);
			}
			status_->misc_status(ST_OK, "KEYS: API Keys loaded from %s", filename.c_str());
			return true;
		}
		catch (const json::exception& e) {
			status_->misc_status(ST_ERROR, "KEYS: Reading JSON failed %d (%s)", e.id, e.what());
			return false;
		}
	}
	status_->misc_status(ST_ERROR, "KEYS: Unable to open keys.json");
	return false;
}

//! SAve data
bool keyring::save_data() {
	std::ofstream os;
	std::string filename;
	// generate seed if it's zero
	if (seed_ == 0) {
		seed_ = std::chrono::system_clock::now().time_since_epoch().count();
		status_->misc_status(ST_NOTE, "KEYS: Generated seed %d", seed_);
	}
	// Open file
	if (file_holder_->get_file(FILE_KEYS, os, filename)) {
		json jall;
		jall["Seed"] = seed_;
		json jkeys;
		for (auto& key : keys_) {
			jkeys[key.first] = zc::string_to_hex(zc::xor_crypt(key.second, seed_, 0));
		}
		jall["Keys"] = jkeys;
		os << std::setw(2) << jall;
		return true;
	}
	else {
		status_->misc_status(ST_ERROR, "KEYS: Unable to create keyring file");
		return false;
	}
}

// Get key
const std::string keyring::key(const std::string& item) {
	if (keys_.find(item) == keys_.end()) {
		std::string key = fl_input("Please enter the key for %s", "", item.c_str());
		keys_[item] = key;
	}
	return keys_.at(item);
}

// Set key
void keyring::key(const std::string& item, const std::string& value) {
	keys_[item] = value;
}