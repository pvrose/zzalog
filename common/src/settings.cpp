#include "settings.h"

#include "file_holder.h"

#include <nlohmann/json.hpp>

#include <fstream>
#include <istream>
#include <ostream>
#include <string>

using json = nlohmann::json;

extern std::string PROGRAM_ID;

//! Basic constructor
settings::settings() {
	// Ddfault filename ZZALOG.json
	std::string filename;
	std::ifstream i;
	parent_ = nullptr;
	if (file_holder_->get_file(FILE_SETTINGS, i, filename)) {
		// Load in file
		try {
			json j;
			i >> j;
			if (j.find(PROGRAM_ID) != j.end() &&
				!j.at(PROGRAM_ID).is_null()) {
				data_ = new json(j.at(PROGRAM_ID));
			}
			else {
				data_ = new json;
			}
			i.close();
		}
		catch (const json::exception& e) {
			printf("SETTINGS: Reading JSON failed %d (%s)\n",
				e.id, e.what());
			i.close();
			data_ = new json;
		}
	}
	else {
		data_ = new json;
	}
}

//! Construct a sub-group of settings
settings::settings(settings* parent, const std::string name) {
	parent_ = parent;
	name_ = name;
	if (parent->data_->find(name) == parent->data_->end()) {
		(*parent_->data_)[name] = json();
	}
	data_ = &parent->data_->at(name);
}

//! DEstructor - copy data back to file
settings::~settings() {
	if (parent_ == nullptr) {
		std::string filename;
		std::ofstream o;
		file_holder_->get_file(FILE_SETTINGS, o, filename);
		parent_ = nullptr;
		json j;
		j[PROGRAM_ID] = *data_;
		o << std::setw(2) << j << '\n';
		o.close();
	}
}

//! Clear the settings
void settings::clear() {
	// Do nothing for now
}

