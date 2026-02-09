#pragma once

#include "fields.h"
#include "view.h"

#include <FL/Enumerations.H>
#include <FL/Fl_Tree.H>

#include <list>
#include <string>

class cty_data;
class Fl_Tree_Item;
class cty_entity;
class cty_prefix;
class cty_exception;

//! This class presents a tree view of the cty_data. 
//! There are four main branches:
//! - Entities by DXCC number.
//! - Entities by nickname (common prefix)
//! - Prefixes - alphanumeric
//! - Exceptions - alphanumeric
class cty_tree :
	public Fl_Tree, public view {

public:

	//! Constructor. Standard FLTK widget.
	cty_tree(int X, int Y, int W, int H, const char* L = nullptr, field_app_t app = FO_LAST);

	//! Destructor
	~cty_tree();

	//! Inherited from view

//! \param hint Indication of whatthe change is.
//! \param record_num_1 Index of QSO record changed.
//! \param record_num_2 Index of asociated QSO record.
	virtual void update(hint_t hint, qso_num_t record_num_1, qso_num_t record_num_2 = 0);


protected:
	//! Hang the entities on the tree
	void hang_entity(const cty_entity* entity);
	//! Hang the prefixes on the tree
	void hang_prefix(const std::string& pfx, const std::list<cty_prefix*>& prefixes);
	//! Hang the exceptions
	void hang_exception(const std::string& exc, const std::list<cty_exception*>& exceptions);

	//! Hng all items
	void hang_all();

	// HAng points
	Fl_Tree_Item* hang_point_dxcc_ = nullptr;        //!< Hang point for entries by DXCC.
	Fl_Tree_Item* hang_point_nick_ = nullptr;        //!< Hang point for entries by nickname.
	Fl_Tree_Item* hang_point_pfx_ = nullptr;         //!< Hang point for prefixes.
	Fl_Tree_Item* hang_point_exc_ = nullptr;         //!< Hang point for exceptions.

	//! Display font
	Fl_Font font_;
	//! Display font size
	Fl_Fontsize fontsize_;

	//! Progress count
	uint32_t hang_count_;

};