#include "cty_tree.h"

#include "book.h"
#include "cty_data.h"
#include "cty_element.h"
#include "fields.h"
#include "view.h"

#include "zc_settings.h"
#include "zc_utils.h"

#include <FL/Enumerations.H>
#include <FL/Fl_Tree.H>
#include <FL/Fl_Tree_Item.H>

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
	{ cty_filter::FT_GEOGRAPHY, "Geo" },
	{ cty_filter::FT_USAGE, "Use"}
};

// Hang an entity
void cty_tree::hang_entity(const cty_entity* ent) {
	// Generate DXCC string
	char text[1024];
	// 278: GM - Scotland (EU): Valid=*-*: CQZ 14: ITUZ 27: 55°N 3°W: 
	snprintf(text, sizeof(text), "%03d: %s - %s (%s): Valid=%s-%s: CQZ %d: ITUZ %d: %s %s",
		ent->dxcc_id_,
		ent->nickname_.c_str(),
		ent->name_.c_str(),
		ent->continent_.c_str(),
		ent->time_validity_.start.c_str(),
		ent->time_validity_.finish.c_str(),
		ent->cq_zone_,
		ent->itu_zone_,
		zc::degrees_to_dms(ent->coordinates_.latitude, true).c_str(),
		zc::degrees_to_dms(ent->coordinates_.longitude, false).c_str()
	);
	Fl_Tree_Item* hp_dxcc = hang_point_dxcc_->add(prefs(), text);
	if (ent->deleted_ || ent->time_validity_.finish != "*")
		hp_dxcc->labelcolor(FL_RED);
	// GM - Scotland (EU): Valid=*-*: CQZ 14: ITUZ 27: 55°N 3°W: 
	snprintf(text, sizeof(text), "%s %d - %s (%s): Valid=%s-%s: CQZ %d: ITUZ %d: %s %s",
		ent->nickname_.c_str(),
		ent->dxcc_id_,
		ent->name_.c_str(),
		ent->continent_.c_str(),
		ent->time_validity_.start.c_str(),
		ent->time_validity_.finish.c_str(),
		ent->cq_zone_,
		ent->itu_zone_,
		zc::degrees_to_dms(ent->coordinates_.latitude, true).c_str(),
		zc::degrees_to_dms(ent->coordinates_.longitude, false).c_str()
	);
	Fl_Tree_Item* hp_nick = hang_point_nick_->add(prefs(), text);
	if (ent->deleted_ || ent->time_validity_.finish != "*")
		hp_nick->labelcolor(FL_RED);
	for (auto& filt : ent->filters_) {
		snprintf(text, sizeof(text), "%s %s %s (patt=%s)",
			FILTER_MAP.at(filt->filter_type_),
			filt->nickname_.c_str(),
			filt->name_.c_str(),
			filt->pattern_.c_str()
		);
		Fl_Tree_Item* idxcc = hp_dxcc->add(prefs(), text);
		if (ent->deleted_ || ent->time_validity_.finish != "*")
			idxcc->labelcolor(FL_RED);
		Fl_Tree_Item* inick = hp_nick->add(prefs(), text);
		if (ent->deleted_ || ent->time_validity_.finish != "*")
			inick->labelcolor(FL_RED);
	}
}

void cty_tree::hang_prefix(const std::string& pfx, const std::list<cty_prefix*>& prefixes) {
	Fl_Tree_Item* hp = hang_point_pfx_->add(prefs(), pfx.c_str());
	char text[1024];
	for (auto p : prefixes) {
		// GM - Scotland (EU): Valid=*-*: CQZ 14: ITUZ 27: 55°N 3°W: 
		snprintf(text, sizeof(text), "%s(%d) - %s (%s): Valid=%s-%s: CQZ %d: ITUZ %d: %s %s",
			cty_data_->data()->entities.at(p->dxcc_id_)->nickname_.c_str(),
			p->dxcc_id_,
			p->name_.c_str(),
			p->continent_.c_str(),
			p->time_validity_.start.c_str(),
			p->time_validity_.finish.c_str(),
			p->cq_zone_,
			p->itu_zone_,
			zc::degrees_to_dms(p->coordinates_.latitude, true).c_str(),
			zc::degrees_to_dms(p->coordinates_.longitude, false).c_str()
		);
		hp->add(prefs(), text);
	}
}

void cty_tree::hang_exception(const std::string& exc, const std::list<cty_exception*>& exceptions) {
	Fl_Tree_Item* hp = hang_point_exc_->add(prefs(), exc.c_str());
	char text[1024];
	for (auto& e : exceptions) {
		switch (e->exc_type_) {
		case cty_exception::EXC_INVALID:
			snprintf(text, sizeof(text), "Invalid operation %s-%s",
				e->time_validity_.start.c_str(),
				e->time_validity_.finish.c_str()
			);
			hp->add(prefs(), text);
			break;
		case cty_exception::EXC_OVERRIDE:
			snprintf(text, sizeof(text), "DXCC/Zone overridden DXCC=%s(%d): Valid=%s-%s: CQZ=%d: ITUZ=%d:",
				cty_data_->data()->entities.at(e->dxcc_id_)->nickname_.c_str(),
				e->dxcc_id_,
				e->time_validity_.start.c_str(),
				e->time_validity_.finish.c_str(),
				e->cq_zone_,
				e->itu_zone_
			);
			hp->add(prefs(), text);
			break;
		}
	}

}