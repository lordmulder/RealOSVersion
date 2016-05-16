# Real OS Version

A simple command-line tool (and library) for determining the **real** Windows version, in the post Windows 8 era.

## Getting Started

	#include "RealOSVersion.h"

	int main()
	{
		unsigned int major, minor, build;
		if (get_real_os_version(&major, &minor, &build))
		{
			printf("Real Version: %u.%02u.%05u\n", major, minor, build);
		}
	}

## Background

Starting with *Windows 8.1*, Microsoft has **broken** then `GetVersion()` and `GetVersionEx()` API's. Starting with *Windows 10*, Microsoft has **broken** then `VerifyVersionInfo()` API as well. So, be aware that these Win32 API functions, from now on, do **not** return the *actual* Windows version, if your code is running on Windows 8.1 or later! Instead, these functions now look at the program's *compatibility manifest* and return **at most** the Windows version that the program has been compiled for. If the program file is missing the required manifest entry, `GetVersionEx()` and friends will pretend that we are running on Windows 8.0, even when we are actually running on Windows 8.1, Windows 10, or whatever comes next! On other words: These function do **not** return the actual Windows version, but the Windows version that Microsoft believes your application *probably wants to hear*. Of course this is *nonsensical* and absolutely **not** what the programmer expects!

Also note that the "recommended" solution (by Microsoft) &ndash; which is adding distinct GUID's for every supported Windows version to the programs *compatibility manifest* &ndash; is **fundamentally flawed**. First of all, you can only add GUID's for those Windows versions that existed by the time your program or library was compiled. Future GUID's are not known yet. This means that you will **never** be able to get the *correct* result on future Windows versions. Secondly, and even more important: *Library* developers have exactly **zero** influence which GUID's are present in the *compatibility manifest* of the calling application! This means that any library code that needs to *relibaly* detect the Windows versions (even Windows versions that existed by the time the library was compiled) now has **no** chance to get the *correct* result. Bummer!

![Facepalm](http://i.imgur.com/MEtX6mb.jpg?1)

## The Solution

Workarounds to "reveal" the actual Windows version on Windows 8.1 and later exist. For example, one may try *guestimating* the real Windows version from certain Registry entries, or from the file versions of certain system libraries. But, while these methods may seem to work in most cases, they still are *dubious* and **not** reliable. It is also possible to obtain the real Windows version via WMI (Windows Management Instrumentation), but this is *complex* and requires a lot of COM hackery. Certainly **not** what you want for a task as simple (at least in theory) as determing the Windows version.

At the same time, the approach implemented in *Real OS Version* manages to reveal the **actual** Windows version strictly by using "official" Win32 API calls in a "creative" way. Also, this approach works regardless of the application's *compatibility manifest*, i.e. it will continue to work (to the best of our knowledge) correctly in *future* Windows versions! Finally, this approach still returns the correct Windows version, even when the program or library runs in "compatibility mode".

Simply put, *Real OS Version* does what any sane programmer would expect the `GetVersionEx()` API to do.

### Details

*Real OS Version* uses an *iterative* approach: It first calls `GetVersionEx()` in order to obtain the "fake" Windows version. Then it tries to *refine* the Windows version, step by step, using the `VerifyVersionInfo()` function &ndash; until the "real" version and build number have been revealed. Furthermore, because the `VerifyVersionInfo()` function has been broken too on Windows 10 ([d'oh!](http://vignette2.wikia.nocookie.net/simpsons/images/c/c3/D'oh.jpg/revision/latest?cb=20141226214635)), we now try to call the "native" version getter functions `RtlGetVersion()` and `RtlVerifyVersion()` directly from NTDLL.DLL, instead of using their "crippled" counterparts from KERNEL32.DLL. This has been tested to give *correct* expected result on all Windows versions from Windows XP up to and including Windows 10.

## License

	Real OS Version
	Copyright (C) 2004-2016 LoRd_MuldeR <MuldeR2@GMX.de>

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License along
	with this program; if not, write to the Free Software Foundation, Inc.,
	51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
