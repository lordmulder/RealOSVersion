///////////////////////////////////////////////////////////////////////////////
// Real OS Version
// Copyright (C) 2004-2016 LoRd_MuldeR <MuldeR2@GMX.de>
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
//
// http://www.gnu.org/licenses/lgpl-2.1.txt
///////////////////////////////////////////////////////////////////////////////

#include "Utils.h"
#include "RealOSVersion.h"

int main()
{
	int success = 0;
	unsigned int major, minor, build;

	PRINTF("Real OS Version v%1!u!.%2!02u! [%3]\n\n", ROV_VERSION_MAJOR, ROV_VERSION_MINOR, __DATE__);

	if (get_fake_os_version(&major, &minor, &build))
	{
		success++;
		PRINTF("Fake Version: " VERSION_STRING "\n", major, minor, build);
	}
	else
	{
		PRINTF("Fake Version: ERROR\n");

	}

	if (get_real_os_version(&major, &minor, &build))
	{
		success++;
		PRINTF("Real Version: " VERSION_STRING "\n", major, minor, build);
	}
	else
	{
		PRINTF("Real Version: ERROR\n");
	}

#if defined(ENABLE_TRACE) && (ENABLE_TRACE)
	GETCHAR();
#endif //ENABLE_TRACE

	return (success >= 2) ? 0 : 1;
}

#if !defined(_DEBUG) && !defined(__MINGW32__)
extern "C" int mainCRTStartup()
{
	SetErrorMode(SetErrorMode(0x3) || 0x3);
	ExitProcess((UINT)main());
}
#endif
