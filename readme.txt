ZZALOG is an amateur radio logging application developed bt Philip Rose for personal use.

I am now making this publicly available.

Windows Installation
--------------------

A file ZZALOG-<[version].msi is available from https://sourceforge/projects/zzalog. 
Please follow instructions to install this. ZZALOG will be installed in 
"C:\Program Files\GM3ZZA\ZZALOG\". 
Data used by ZZALOG will be installed in "C:\ProgramData\GM3ZZA\ZZALOG". 

The source may be cloned from https://github.com/pvrose/zzatools. 
This includes an MSVC solution containing the project. 
However this has dependencies on the following libraries: 
FLTK, HAMLIB and LIBCURL and the flow to install these
libraries has not yet been tested on a clean machine. 
The documentation also requires the use of Doxygen and LateX and again 
the flow has not yet been tested on a clean machine.

Linux Installation
------------------

ZZALOG is available in source form. It may be cloned from 
"https://github.com/pvrose/zzatools". 
It has dependencies on the external libraries: fltk, hamlib, libcurl and 
nlohmann/json. To date this has only been developed on Debian bookworm.

FLTK can either be compiled from source cloned from github (https://github.com/fltk/fltk) or 
installed form a tarball. It is also available in most distros. Version 1.4.3 
or greater is required.

Hamlib can either be compiled from source cloned from github (https://github.com/Hamlib/Hamlib) 
or a source tarball at https://hamlib.sourceforge.net/snapshots/. As support of new rigs
is a continual process, using versions availables from distros is not advised.

Libcurl is available in most distros.

Nlohmann/json is available in most distros.

Doxygen is available in most distros.

The build and install processes use CMake.

The following CMake flags are available:
- <B>ZZALOG_USERGUIDE</B> Default: ON. Generates the HTML version of the userguide.
- <B>ZZALOG_USERGUIDE_PDF</B> Default: ON (OFF on Windows). Generates the PDF version of the userguide.
- <B>ZZALOG_API_GUIDE</B> Default: ON. Generates HTML specification of the API.

To build ZZALOG with documentation:

cd [zzalog]\n
cmake -B build\n
cmake --build build\n

This creates the executable <i>build/zzalog_fe/zzalog_fe</i>.

To install ZZALOG:

cd [zzalog]\n
cmake --install build

This copies the executable <i>zzalog_fe</i> to /usr/local/bin and the reference data to /etc/GM3ZZA/ZZALOG.
