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
#pragma once

#include <string>



class record;

//! This class generates an e-mail to send a QSL imgae to a QSO partner.
class qsl_emailer
{
public:
	//! Constructor.
	qsl_emailer();
	//! Destructor.
	~qsl_emailer();

	//! Generate the e-mail for \p qso.
	bool generate_email(record* qso);

	//! Send the generated e-mail.
	bool send_email();

protected:

	//! Read the default email settings
	void load_values();
	//! QSO record to be QSL'd by e-mail.
	record* qso_;

	//! e-mail server
	std::string email_url_;
	//! user account
	std::string email_user_;
	//! password
	std::string email_password_;
	//! To address
	std::string to_address_;
	//! Cc address
	std::string cc_address_;
	//! Subject
	std::string subject_;
	//! QSL card attachment - filename.
	std::string qsl_filename_;
	//! e-mail text body
	std::string text_body_;

};

