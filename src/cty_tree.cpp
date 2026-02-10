#include "cty_tree.h"

#include "book.h"
#include "cty_data.h"
#include "cty_element.h"
#include "fields.h"
#include "main.h"
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
	// sortorder(FL_TREE_SORT_ASCENDING);
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

	Fl_Tree_Item* root_item = new Fl_Tree_Item(this);
	root(root_item);
	root_item->labelfont(item_labelfont() | FL_BOLD);
	root_label("Country data");
	if (cty_data_) {
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
	snprintf(text, sizeof(text), "%03d: %s - %s (%s)",
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
	snprintf(text, sizeof(text), "%s %d - %s (%s)",
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
	snprintf(text, sizeof(text), "- Valid from %s to %s",
		element->time_validity_.start.c_str(),
		element->time_validity_.finish.c_str()
	);
	Fl_Tree_Item* iv = item->add(prefs(), text);
	iv->labelfont(item->labelfont());
	iv->labelcolor(item->labelcolor());

	snprintf(text, sizeof(text), "- CQ Zone %d: ITU Zone %d; %s %s",
		element->cq_zone_,
		element->itu_zone_,
		zc::degrees_to_dms(element->coordinates_.latitude, true).c_str(),
		zc::degrees_to_dms(element->coordinates_.longitude, false).c_str()
	);
	Fl_Tree_Item* il = item->add(prefs(), text);
	il->labelfont(item->labelfont());
	il->labelcolor(item->labelcolor());
}

void cty_tree::hang_filter(const cty_filter* filter, Fl_Tree_Item* item) {
	static char text[1024];
	snprintf(text, sizeof(text), "%s %s%s",
		filter->nickname_.c_str(),
		FILTER_MAP.at(filter->filter_type_),
		filter->name_.c_str()
	);
	Fl_Tree_Item* idxcc = item->add(prefs(), text);
	snprintf(text, sizeof(text), "  %s", filter->pattern_.c_str());
	Fl_Tree_Item* ipd = idxcc->add(prefs(), text);
	idxcc->labelcolor(item->labelcolor());
	ipd->labelcolor(item->labelcolor());
	ipd->labelfont(FL_ITALIC);

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
	Fl_Tree_Item* hp = hang_point_pfx_->add(prefs(), pfx.c_str());
	for (auto p : prefixes) {
		// GM - Scotland (EU): Valid=*-*: CQZ 14: ITUZ 27: 55�N 3�W: 
		snprintf(text, sizeof(text), "%s(%d) - %s (%s)",
			cty_data_->data()->entities.at(p->dxcc_id_)->nickname_.c_str(),
			p->dxcc_id_,
			p->name_.c_str(),
			p->continent_.c_str()
		);
		hp->add(prefs(), text);
		hang_info(p, hp);
	}
	status_->progress(++hang_count_, OT_PREFIX);
}

void cty_tree::hang_exception(const std::string& exc, const std::list<cty_exception*>& exceptions) {
	static char text[1024];
	Fl_Tree_Item* hp = hang_point_exc_->add(prefs(), exc.c_str());
	for (auto& e : exceptions) {
		switch (e->exc_type_) {
		case cty_exception::EXC_INVALID:
			snprintf(text, sizeof(text), "Invalid operation");
			hp->add(prefs(), text);
			hang_info(e, hp);
			break;
		case cty_exception::EXC_OVERRIDE:
			snprintf(text, sizeof(text), "DXCC/Zone overridden DXCC=%s(%d)",
				cty_data_->data()->entities.at(e->dxcc_id_)->nickname_.c_str(),
				e->dxcc_id_
			);
			hp->add(prefs(), text);
			hang_info(e, hp);
			break;
		}
	}
	status_->progress(++hang_count_, OT_PREFIX);
}