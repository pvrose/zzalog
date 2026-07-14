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
#ifndef __RIG_IF__
#define __RIG_IF__

#include "zc_callback.h"
#include "zc_fltk.h"

// hamlib icludes
#include "hamlib/rig.h"

// C/C++ includes
#include <atomic>
#include <chrono>
#include <cstdint>
#include <map>
#include <mutex>
#include <string>
#include <thread>
#include <vector>


	//! Internal representation of the mode read from the rig.
	enum rig_mode_t {
		GM_INVALID = 0,   //!< Invalid or unsupported mode
		GM_LSB = '1',     //!< LSB
		GM_USB = '2',     //!< USB
		GM_CWU = '3',     //!< CW - VFO frequency below signal frequency
		GM_FM = '4',      //!< FM
		GM_AM = '5',      //!< AM
		GM_DIGL = '6',    //!< Digital mode: Modulated as LSB
		GM_CWL = '7',     //!< CW - VFO frequency above signal frequency
		GM_DIGU = '9',    //!< Digital mode: Modulated as USB
		GM_DSTAR = 'A'    //!< Digital voice: Icom DSTAR.
	};

	//! slow rig polling - 1s -> 10min (default 1 min)
	const int SLOW_RIG_TIMER = 60;
	//! fast rig polling - 20ms -> 2s (default 1s);
	const int FAST_RIG_TIMER = 1;

	//! Method by which power level read from rig is converted to a power reading.
	enum power_mode_t : uint8_t {
		NO_POWER,        //!< No power returnable
		RF_METER,        //!< Read the RF power out meter directly.
		DRIVE_LEVEL,     //!< Read the drive level meter and multiply by maximum poer.
		MAX_POWER        //!< Use the specified maximum power.
	};

	//! Method by which frequency is provided.
	enum freq_mode_t : uint8_t {
		NO_FREQ,         //!< No frequency available
		VFO,             //!< Read a VFO
		XTAL             //!< Fixed freqency
	};

	//! Used to add accessories.
	enum accessory_t : uint8_t {
		BAREBACK,        //!< No accessory
		AMPLIFIER = 1,   //!< Amplifer attached
		TRANSVERTER = 2, //!< Transverter attached
		BOTH = AMPLIFIER | TRANSVERTER  //!< Both amplifier and transverter attached.
	};

	//! Rig interface configuration data (including hamlib configuration):
	struct hamlib_data_t {
		//! Manufacturer as known by hamlib.
		std::string mfr = "";
		//! Model as known by hamlib.
		std::string model = "";
		//! Port name used by hamlib.
		std::string port_name = "";
		//! Baud rate used by hamlib.
		int baud_rate = 9600;
		//! Model ID - index into hamlib capabilities table.
		rig_model_t model_id = -1;
		//! Port type used by hamlib.
		rig_port_t port_type = RIG_PORT_NONE;
		//! Maximum number of timeouts allowed before ignore meter
		int max_to_count = 5;
		// additional features required by rig_if to return data
		//! Timeout value (not a hamlib item
		double timeout = 1.0;
		//! S-meter reading queue length
		int num_smeters = 5;
		//! Default power mode.
		power_mode_t power_mode = RF_METER;
		//! Maximum power level (in watts)
		double max_power = 0.0;
		//! Default frequency mode
		freq_mode_t freq_mode = VFO;
		//! Fixed frequency (in megahertz)
		double frequency = 0.0;
		//! Amplifer and transverter defaults
		accessory_t accessory = BAREBACK;
		//! Spcified amplifier gain (in decibels)
		int gain = 0;
		//! Specified transverter power (in watts)
		double tvtr_power = 0.0;
		//! Specified transverter local oscillator frequency (in megawatts) 
		double freq_offset = 0.0;
	};

	//! This class is the base class for rig handlers. 
	class rig_if
	{
	public:

		//! Constructor.
		
		//! \param name Name as known to the user.
		//! \param data interface configuration data.
		rig_if(const char* name, hamlib_data_t* data);
		//! Destructor.
		~rig_if();

		//! Rig state
		enum rig_state_t : uint8_t {
			NOT_DEFINED,                    //!< Rig is not defined
			NOT_CONNECTABLE,                //!< Rig cannot be connected
			DISCONNECTED,                   //!< Rig is not connected
			CONNECTING,                     //!< Rig is being connected.
			CONNECTED_OK,                   //!< Rig is connected and responding satisfactorily
			CONNECTED_SLOW,                 //!< Rig may be connected but not responding
            CONNECTED_ERROR,                //!< Hamlib has resturned an error response
			UNPOWERED,                      //!< Rig appears not be powered on
            TIMED_OUT,                      //!< Connection timed out.
		};

		//! Rig state string
		const std::map<rig_state_t, std::string> STATE_MAP = {
			{ NOT_DEFINED, "Not defined"},
			{ NOT_CONNECTABLE, "CAT not supported"},
			{ DISCONNECTED, "Disconnected"},
			{ CONNECTING, "Connecting"},
			{ CONNECTED_OK, "Connected OK"},
			{ CONNECTED_SLOW, "Connected, slow to respond"},
			{ CONNECTED_ERROR, "Connected, error response"},
			{ UNPOWERED, "Appears not to be powered"}
		};

		//! Values read from rig
		struct rig_values {
			std::atomic<double> tx_frequency;  //!< Transmit Frequency (in megahertz)
			std::atomic<double> rx_frequency;  //!< Receive Frequency (in megahertz)
			std::atomic<rig_mode_t> mode;      //!< Transmit mode
			std::atomic<double> drive ;        //!< Drive level (fraction)
			std::atomic<bool> split;           //!< Split mode.
			std::atomic<int> s_value;          //!< Smoothed S-meter reading (in decibels)
			std::atomic<int> s_meter;          //!< Immediate S-meter reading (in decibels)
			std::atomic<double> pwr_value;     //!< Smoothed RF power meter reading (in watts)
			std::atomic<double> pwr_meter;     //!< Immediate power meter reading (in watts)
			std::atomic<bool> ptt;             //!< If true indicates transmitting otherwise receiving.
			//! Constrctor.
			rig_values() {
				tx_frequency = 0.0;
				rx_frequency = 0.0;
				mode =GM_CWU;
				drive = 0.0;
				split = false;
				s_value = -54;
				s_meter = -54;
				pwr_value = 0.0;
				pwr_meter = 0.0;
				ptt = false;
			}
		};

		//! Opens the connection to the rig
		bool open();
		//! Returns rig name
		std::string& rig_name();
		//! Returns the most recent error message and adds \p func_name.
		std::string error_message(const char* func_name);

		//! close rig - may be null for some 
		//! \param powerdown If true, powers down the rig when closing.
		void close(bool powerdown);

		//! Receives \p mode and \p submode.  
		void get_string_mode(std::string& mode, std::string& submode);
		//! Returns frequency as string (in megahertz to 1 hertz resolution)
		std::string get_frequency(bool tx);
		//! Returns frequency as double (in megahertz)
		double get_dfrequency(bool tx);
		//! Returns power (in watts): \p max maximum value over the transmit period.
		std::string get_tx_power(bool max = true);
		//! Returns power (in watts): \p max maximum value over the transmit period.
		double get_dpower(bool max = true);
		//! Returns S-meter reading - \p max - maximum over receive perion, false = instatntaneous
		std::string get_smeter(bool max = true);
		//! Returns PTT value: true indicates transmit.
		bool get_ptt();
		//! Returns Split value
		bool get_split();
		//! Run in thread to get the data from the rig
		static void th_run_rig(rig_if* that);
		//! Return rig state
		rig_state_t state();
		//! Open rig in rig access thread.
		static void th_sopen_rig(rig_if* that);
		//! Callback from rig thread if an error is detected.
		static void cb_rig_error(void* v);
		//! Callback from rig thread if a warning is detected.
		static void cb_rig_warning(void* v);
		//! Set frequency (in megahertz)
		bool set_frequency(double f);

		//! Rig is in a connected state (not necessarily OK)
		bool connected();


		// Protected attributes
	protected:
		//! Runs in rig thread to poll values every 1 second.
		void th_read_values();
		//! Open rig - run in thread
		void th_open_rig(rig_if* that);
		//! Handle errors.
		
		//! \param code Error code
		//! \param meter Name of meter value being accessed.
		//! \param flag Pointer to a Boolean value that if set inhibits further attempts to acccess.
		//! \param to_count Number of accesses allowed befor further ones are inhibited.
		//! \return true indicates error prevents further access.
		bool error_handler(int code, const char* meter, bool* flag, int* to_count);
		//! Full rig name
		std::string full_rig_name_;

		//! Returns error message for error \p code.
		const char* error_text(rig_errcode_e code);

		//! Name to log as MY_RIG
		std::string my_rig_name_;
		//! Interface specific attributes
		hamlib_data_t* hamlib_data_;
		//! Hamlib rig interface
		RIG* rig_;
		//! Numeric error code
		int error_code_;
		//! Values polled from rig.
		rig_values rig_data_;
		//! Rig state
		std::atomic<rig_state_t> state_;
		//! Timer count down
		int count_down_;
		//! Thread in whcih to run rig access.
		std::thread* thread_;
		//! Keep rig thread running.
		std::atomic<bool> run_read_;

		//! The time of the last PTT off - to decide if it's a new transmission.
		std::chrono::system_clock::time_point last_ptt_off_;

		//! The most recent S-meter readings: used for smoothing the value read.
		std::vector<int> smeters_;
		//! Cumulated value of smeter readings
		int sum_smeters_;
		// Flags to avoid unsupported meters
		bool has_smeter_;     //!< S-meter appears to be supported.
		bool has_drive_;      //!< Drive meter appears to be supported.
		bool has_rf_meter_;   //!< RF Power meter appears to be supported.

		//! Timeout counts
		int toc_split_;
		//! Function being performed - for error debug mostly
		std::string read_item_;
		//! Warning message
		std::string warning_message_;

		//! Semaphore to only allow accesses when rig power state is known.
		std::atomic<bool> allow_access_;


};
#endif
