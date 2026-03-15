/*
	Copyright 2025-2026, Philip Rose, GM3ZZA
	
    This file is part of ZZALOG. Amateur Radio Logging Software.

    ZZALOG is free software: you can redistribute it and/or modify it under the
	terms of the Lesser GNU General Public License as published by the Free Software
	Foundation, either version 3 of the License, or (at your option) any later version.

    ZZALOG is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
	without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
	PURPOSE. See the GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along with ZZALOG. 
	If not, see <https://www.gnu.org/licenses/>. 

*/
#include "cty_tree.h"

#include "book.h"
#include "cty_data.h"
#include "cty_element.h"
#include "fields.h"
#include "objects.h"
#include "spec_data.h"
#include "view.h"

#include "zc_callback.h"
#include "zc_settings.h"
#include "zc_status.h"
#include "zc_utils.h"

#include <FL/Enumerations.H>
#include <FL/Fl_Tree.H>
#include <FL/Fl_Tree_Item.H>
#include <FL/Fl_Tree_Prefs.H>

#include <cstdint>
#include <cstdio>
#include <list>
#include <map>
#include <string>

extern cty_data* cty_data_;

// Constructor
cty_tree::cty_tree(int X, int Y, int W, int H, const char* L, field_app_t app) :
	Fl_Tree(X, Y, W, H, L),
	view()
{
	// Read standard tree settings
	zc_settings top_settings;
	zc_settings view_settings(&top_settings, "Views");
	zc_settings tree_settings(&view_settings, "Tree Views");
	tree_settings.get("Font Name", font_, 0);
	tree_settings.get("Font Size", fontsize_, FL_NORMAL_SIZE);

	// Set tree properties
	sortorder(FL_TREE_SORT_ASCENDING);
	item_labelfont(font_);
	item_labelsize(fontsize_);

	// Call back standard tree callback
	callback(zc::cb_tree);

    end();
}

// Destructor
cty_tree::~cty_tree() {
	clear();
}

void cty_tree::update(hint_t hint, qso_num_t record_num_1, qso_num_t record_num_2) {
	if (hint == hint_t::HT_NEW_CTY) {
		clear();
		hang_all();
	}
}


// Hang all the data
void cty_tree::hang_all() {

	clear();
	Fl_Tree_Item* root_item = new Fl_Tree_Item(this);
	root(root_item);
	root_item->labelfont(item_labelfont() | FL_BOLD);
	root_label("Country data");
	if (cty_data_) {
		status_->misc_status(ST_NOTE, "CTY_DATA: Displaying country data");
		uint32_t count = cty_data_->data()->entities.size() +
			cty_data_->data()->prefixes.size() +
			cty_data_->data()->exceptions.size();
		status_->progress(count, OT_PREFIX, "Building country data view", "records");
		hang_count_ = 0;
		hang_point_dxcc_ = add("Entities by DXCC Number");
		hang_point_dxcc_->labelfont(FL_BOLD | FL_ITALIC);
		hang_point_nick_ = add("Entities by nickname");
		hang_point_nick_->labelfont(FL_BOLD | FL_ITALIC);
		hang_point_pfx_ = add("Prefixes");
		hang_point_pfx_->labelfont(FL_BOLD | FL_ITALIC);
		hang_point_exc_ = add("Exceptions");
		hang_point_exc_->labelfont(FL_BOLD | FL_ITALIC);
		for (auto& ent : cty_data_->data()->entities) {
			hang_entity(ent.second);
		}
		for (auto& pfx : cty_data_->data()->prefixes) {
			hang_prefix(pfx.first, pfx.second);
		}
		for (auto& exc : cty_data_->data()->exceptions) {
			hang_exception(exc.first, exc.second);
		}
		status_->misc_status(ST_OK, "CTY_DATA: Country data displayed");
	}
}

std::map<cty_filter::filter_t, const char*> FILTER_MAP = {
	{ cty_filter::FT_GEOGRAPHY, " " },
	{ cty_filter::FT_USAGE, " "}
};

// Hang an entity
void cty_tree::hang_entity(const cty_entity* ent) {
	static char text[1024];
	// Generate DXCC string
	snprintf(text, sizeof(text), "%03d(%s):- %s (%s)",
		ent->dxcc_id_,
		ent->nickname_.c_str(),
		ent->name_.c_str(),
		ent->continent_.c_str()
	);
	Fl_Tree_Item* hp_dxcc = hang_point_dxcc_->add(prefs(), text);
	if (ent->deleted_ || ent->time_validity_.finish != "*") {
		hp_dxcc->labelcolor(FL_RED);
	};
	hang_info(ent, hp_dxcc);
	hang_filters(ent->filters_, hp_dxcc);
	snprintf(text, sizeof(text), "%s(%d):- %s (%s)",
		ent->nickname_.c_str(),
		ent->dxcc_id_,
		ent->name_.c_str(),
		ent->continent_.c_str()
	);
	Fl_Tree_Item* hp_nick = hang_point_nick_->add(prefs(), text);
	if (ent->deleted_ || ent->time_validity_.finish != "*") {
		hp_nick->labelcolor(FL_RED);
	};
	hang_info(ent, hp_nick);
	hang_filters(ent->filters_, hp_nick);
	status_->progress(++hang_count_, OT_PREFIX);
}

void cty_tree::hang_info(const cty_element* element, Fl_Tree_Item* item) {
	// Create static to avoid memory managemet overhead each time method is called
	static char text[1024];
	if (element->time_validity_.start == "*")
		if (element->time_validity_.finish == "*")
			strcpy(text, "Valid throughout");
		else
			snprintf(text, sizeof(text), "Valid until %s",
				format_datetime(element->time_validity_.finish, false).c_str());
	else
		if (element->time_validity_.finish == "*")
			snprintf(text, sizeof(text), "Valid from %s",
				format_datetime(element->time_validity_.start, true).c_str());
		else
			snprintf(text, sizeof(text), "Valid between %s and %s",
				format_datetime(element->time_validity_.start, true).c_str(),
				format_datetime(element->time_validity_.finish, false).c_str());
	Fl_Tree_Item* iv = item->add(prefs(), text);
	iv->labelfont(item->labelfont());
	iv->labelcolor(item->labelcolor());

	std::string location = "";
	if (element->cq_zone_ >= 0) location += ": CQ Zone " + std::to_string(element->cq_zone_);
	if (element->itu_zone_ >= 0) location += ": ITU Zone " + std::to_string(element->itu_zone_);
	if (!element->coordinates_.is_nan()) location += ": " +
		zc::degrees_to_dms(element->coordinates_.latitude, true) + " " +
		zc::degrees_to_dms(element->coordinates_.longitude, false);
	if (location.length()) {
		snprintf(text, sizeof(text), "Location%s", location.c_str());
		Fl_Tree_Item* il = item->add(prefs(), text);
		il->labelfont(item->labelfont());
		il->labelcolor(item->labelcolor());
	}
	if (element->pattern_.length()) {
		snprintf(text, sizeof(text), "Pattern: %s", element->pattern_.c_str());
		Fl_Tree_Item* ipd = item->add(prefs(), text);
		ipd->labelfont(item->labelfont());
		ipd->labelcolor(item->labelcolor());
	}
}

void cty_tree::hang_filter(const cty_filter* filter, Fl_Tree_Item* item) {
	static char text[1024];
	snprintf(text, sizeof(text), "%s %s%s",
		filter->nickname_.c_str(),
		FILTER_MAP.at(filter->filter_type_),
		filter->name_.c_str()
	);
	Fl_Tree_Item* idxcc = item->add(prefs(), text);
	idxcc->labelcolor(item->labelcolor());

	hang_info(filter, idxcc);
	
	// Add next level filter
	hang_filters(filter->filters_, idxcc);

}

void cty_tree::hang_filters(const std::list<cty_filter*> filters, Fl_Tree_Item* item) {
	int num_geos = 0;
	int num_uses = 0;
	for (auto& f : filters) {
		switch (f->filter_type_) {
		case cty_filter::FT_GEOGRAPHY:
			num_geos++;
			break;
		case cty_filter::FT_USAGE:
			num_uses++;
			break;
		default:
			break;
		}
	}
	Fl_Tree_Item* igeo = nullptr;
	Fl_Tree_Item* iuse = nullptr;
	if (num_geos) {
		igeo = item->add(prefs(), "Geographic subdivisions");
	}
	if (num_uses) {
		iuse = item->add(prefs(), "License class or special use");
	}
	for (auto& f : filters) {
		switch (f->filter_type_) {
		case cty_filter::FT_GEOGRAPHY:
			hang_filter(f, igeo);
			break;
		case cty_filter::FT_USAGE:
			hang_filter(f, iuse);
			break;
		default:
			break;
		}
	}
}

void cty_tree::hang_prefix(const std::string& pfx, const std::list<cty_prefix*>& prefixes) {
	static char text[1024];
	static char hp_label[128];
	snprintf(hp_label, sizeof(hp_label), "Prefixes %c...", pfx[0]);
	Fl_Tree_Item* hp1 = hang_point_pfx_->find_child_item(hp_label);
	Fl_Tree_Item* hp = nullptr;
	if (hp1 == nullptr) {
		hp1 = hang_point_pfx_->add(prefs(), hp_label);
	}
	if (pfx.length() > 1) {
		snprintf(hp_label, sizeof(hp_label), "Prefixes %c%c...", pfx[0], pfx[1]);
		Fl_Tree_Item* hp2 = hp1->find_child_item(hp_label);
		if (hp2 == nullptr) {
			hp2 = hp1->add(prefs(), hp_label);
		}
		hp = hp2->add(prefs(), pfx.c_str());
	}
	else {
		hp = hp1->add(prefs(), pfx.c_str());
	}
	int index = 1;
	for (auto p : prefixes) {
		Fl_Tree_Item* hpn = nullptr;
		if (prefixes.size() == 1) hpn = hp;
		else {
			snprintf(text, sizeof(text), "%s #%d", 
				pfx.c_str(),
				index++
			);
			hpn = hp->add(prefs(), text);
		}
		// GM - Scotland (EU): Valid=*-*: CQZ 14: ITUZ 27: 55�N 3�W: 
		snprintf(text, sizeof(text), "%s(%d) - %s (%s)",
			cty_data_->data()->entities.at(p->dxcc_id_)->nickname_.c_str(),
			p->dxcc_id_,
			p->name_.c_str(),
			p->continent_.c_str()
		);
		hpn->add(prefs(), text);
		hang_info(p, hpn);
	}
	status_->progress(++hang_count_, OT_PREFIX);
}

void cty_tree::hang_exception(const std::string& exc, const std::list<cty_exception*>& exceptions) {
	static char text[1024];
	static char hp_label[128];
	snprintf(hp_label, sizeof(hp_label), "Exceptions %c...", exc[0]);
	Fl_Tree_Item* hp1 = hang_point_exc_->find_child_item(hp_label);
	Fl_Tree_Item* hp = nullptr;
	if (hp1 == nullptr) {
		hp1 = hang_point_exc_->add(prefs(), hp_label);
	}
	if (exc.length() > 1) {
		snprintf(hp_label, sizeof(hp_label), "Exceptions %c%c...", exc[0], exc[1]);
		Fl_Tree_Item* hp2 = hp1->find_child_item(hp_label);
		if (hp2 == nullptr) {
			hp2 = hp1->add(prefs(), hp_label);
		}
		hp = hp2->add(prefs(), exc.c_str());
	}
	else {
		hp = hp1->add(prefs(), exc.c_str());
	}
	int def_dxcc = cty_data_->dxcc_id(exc, false);
	cty_entity* def_entity = cty_data_->data()->entities.at(def_dxcc);
	int index = 1;
	for (auto& e : exceptions) {
		cty_entity* entity = cty_data_->data()->entities.at(e->dxcc_id_);
		Fl_Tree_Item* hpn = nullptr;
		if (exceptions.size() == 1) hpn = hp;
		else {
			snprintf(text, sizeof(text), "Exception #%d", index++);
			hpn = hp->add(prefs(), text);
		}
		switch (e->exc_type_) {
		case cty_exception::EXC_INVALID:
			snprintf(text, sizeof(text), "Invalid operation");
			break;
		case cty_exception::EXC_DXCC_OVERRIDE: {
			int decode_id = cty_data_->dxcc_id(exc, false);
			snprintf(text, sizeof(text), "DXCC overridden: %s (%d: %s) instead of %s (%d: %s)",
				entity->nickname_.c_str(),
				e->dxcc_id_,
				entity->name_.c_str(),
				def_entity->nickname_.c_str(),
				decode_id,
				def_entity->name_.c_str()
			);
			break;
		}
		case cty_exception::EXC_CQZ_OVERRIDE:
			snprintf(text, sizeof(text), "CQ Zone overridden: %d instead if %d",
				e->cq_zone_,
				def_entity->cq_zone_
			);
			break;
		case cty_exception::EXC_ITUZ_OVERRIDE:
			snprintf(text, sizeof(text), "ITU Zone overridden: %d instead of %d",
				e->itu_zone_,
				def_entity->itu_zone_
			);
		}
		hpn->add(prefs(), text);
		hang_info(e, hpn);
	}
	status_->progress(++hang_count_, OT_PREFIX);
}

void cty_tree::hang_hierarchy(const std::string& id, const std::string& label, Fl_Tree_Item* tree, int depth) {
	std::string hp_label = id + label.substr(0, depth);
	Fl_Tree_Item* hp;
	if (tree->find_child(hp_label.c_str()) == -1) {
		hp = tree->add(prefs(), hp_label.c_str());
	}
	else {

	}


}

std::string cty_tree::format_datetime(const std::string& text, bool start) {
	std::string result = text.substr(0, 4) + "/" +
		text.substr(4, 2) + "/" +
		text.substr(6, 2);
	if (start && text.substr(8, 4) != "0000" ||
		!start && text.substr(8, 4) != "2359") {
		result += "T" +
			text.substr(8, 2) + ":" +
			text.substr(10, 2);
	}
	return result;
}