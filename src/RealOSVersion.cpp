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

#include "RealOSVersion.h"
#include "Utils.h"

static inline DWORD SAFE_ADD(const DWORD &a, const DWORD &b, const DWORD &limit = MAXDWORD)
{
	return ((a >= limit) || (b >= limit) || ((limit - a) <= b)) ? limit : (a + b);
}

static inline void initialize_os_version(OSVERSIONINFOEXW *const osInfo)
{
	SecureZeroMemory(osInfo, sizeof(OSVERSIONINFOEXW));
	osInfo->dwOSVersionInfoSize = sizeof(OSVERSIONINFOEXW);
}

static inline DWORD initialize_step_size(const DWORD &limit)
{
	DWORD result = 1;
	while (result < limit)
	{
		result = SAFE_ADD(result, result);
	}
	return result;
}

static bool get_os_info(OSVERSIONINFOEXW *const osInfo)
{
	typedef LONG(__stdcall *RtlGetVersion)(LPOSVERSIONINFOEXW);
	if (const HMODULE ntdll = GetModuleHandleW(L"ntdll"))
	{
		if (const RtlGetVersion pRtlGetVersion = (RtlGetVersion)GetProcAddress(ntdll, "RtlGetVersion"))
		{
			initialize_os_version(osInfo);
			if (pRtlGetVersion(osInfo) == 0)
			{
				return true;
			}
		}
	}

	initialize_os_version(osInfo);
	return (GetVersionExW((LPOSVERSIONINFOW)osInfo) != FALSE);
}

static bool verify_os_info(OSVERSIONINFOEXW *const osInfo, const ULONG typeMask, const ULONGLONG condMask)
{
	typedef LONG(__stdcall *RtlVerifyVersionInfo)(LPOSVERSIONINFOEXW, ULONG, ULONGLONG);
	if (const HMODULE ntdll = GetModuleHandleW(L"ntdll"))
	{
		if (const RtlVerifyVersionInfo pRtlVerifyVersionInfo = (RtlVerifyVersionInfo)GetProcAddress(ntdll, "RtlVerifyVersionInfo"))
		{
			if (pRtlVerifyVersionInfo(osInfo, typeMask, condMask) == 0)
			{
				return true;
			}
		}
	}

	return (VerifyVersionInfoW(osInfo, typeMask, condMask) != FALSE);
}

static bool verify_os_version(const DWORD major, const DWORD minor)
{
	OSVERSIONINFOEXW osvi;
	DWORDLONG dwlConditionMask = 0;
	initialize_os_version(&osvi);

	osvi.dwMajorVersion = major;
	osvi.dwMinorVersion = minor;
	osvi.dwPlatformId = VER_PLATFORM_WIN32_NT;

	VER_SET_CONDITION(dwlConditionMask, VER_MAJORVERSION, VER_GREATER_EQUAL);
	VER_SET_CONDITION(dwlConditionMask, VER_MINORVERSION, VER_GREATER_EQUAL);
	VER_SET_CONDITION(dwlConditionMask, VER_PLATFORMID, VER_EQUAL);

	const BOOL ret = verify_os_info(&osvi, VER_MAJORVERSION | VER_MINORVERSION | VER_PLATFORMID, dwlConditionMask);
	return (ret != FALSE);
}

static bool verify_os_buildNo(const DWORD buildNo)
{
	OSVERSIONINFOEXW osvi;
	DWORDLONG dwlConditionMask = 0;
	initialize_os_version(&osvi);

	osvi.dwBuildNumber = buildNo;
	osvi.dwPlatformId = VER_PLATFORM_WIN32_NT;

	VER_SET_CONDITION(dwlConditionMask, VER_BUILDNUMBER, VER_GREATER_EQUAL);
	VER_SET_CONDITION(dwlConditionMask, VER_PLATFORMID, VER_EQUAL);

	const BOOL ret = verify_os_info(&osvi, VER_BUILDNUMBER, dwlConditionMask);
	return (ret != FALSE);
}

bool get_fake_os_version(unsigned int *const major, unsigned int *const minor, unsigned int *const build)
{
	OSVERSIONINFOEXW osInfo;
	initialize_os_version(&osInfo);

	if (GetVersionExW((LPOSVERSIONINFOW)&osInfo))
	{
		*major = osInfo.dwMajorVersion;
		*minor = osInfo.dwMinorVersion;
		*build = osInfo.dwBuildNumber;
		return true;
	}

	return false;
}

bool get_real_os_version(unsigned int *const major, unsigned int *const minor, unsigned int *const build)
{
	static const DWORD MAX_VERSION = 0xFFFF;
	static const DWORD MAX_BUILDNO = (((DWORD)~((DWORD)0)) >> 1);

	OSVERSIONINFOEXW osvi;

	if (get_os_info(&osvi) == FALSE)
	{
		return false;
	}

	if (osvi.dwPlatformId == VER_PLATFORM_WIN32_NT)
	{
		*major = osvi.dwMajorVersion;
		*minor = osvi.dwMinorVersion;
		*build = osvi.dwBuildNumber;
	}
	else
	{
		TRACE1("Not running on WIN_NT, says get_os_info()");
		if (verify_os_version(4, 0))
		{
			TRACE1("Win9x compat mode detected!");
			*major = 4;
			*build = 1381;
		}
		else
		{
			TRACE1("Stopping.");
			return false;
		}
	}

	TRACE2("Initial version: " VERSION_STRING, (*major), (*minor), (*build));

	//Major Version
	for (DWORD nextMajor = (*major) + 1; nextMajor <= MAX_VERSION; nextMajor++)
	{
		if (verify_os_version(nextMajor, 0))
		{
			*major = nextMajor;
			*minor = 0;
			TRACE2("--> Bump major version: " VERSION_STRING, (*major), (*minor), (*build));
			continue;
		}
		TRACE2("Major version unsupported: %1!u!", nextMajor);
		break;
	}

	//Minor Version
	for (DWORD nextMinor = (*minor) + 1; nextMinor <= MAX_VERSION; nextMinor++)
	{
		if (verify_os_version((*major), nextMinor))
		{
			*minor = nextMinor;
			TRACE2("--> Bump minor version: " VERSION_STRING, (*major), (*minor), (*build));
			continue;
		}
		TRACE2("Minor version unsupported: %1!u!", nextMinor);
		break;
	}

	//Build Version
	if (verify_os_buildNo(SAFE_ADD((*build), 1, MAX_BUILDNO)))
	{
		DWORD stepSize = initialize_step_size(MAX_BUILDNO);
		for (DWORD nextBuildNo = SAFE_ADD((*build), stepSize, MAX_BUILDNO); (*build) < MAXDWORD; nextBuildNo = SAFE_ADD((*build), stepSize, MAX_BUILDNO))
		{
			TRACE2("Current step size: %1!u!", stepSize);
			if (verify_os_buildNo(nextBuildNo))
			{
				*build = nextBuildNo;
				TRACE2("--> Bump build version: " VERSION_STRING, (*major), (*minor), (*build));
				continue;
			}
			TRACE2("Build version unsupported: %1!u!", nextBuildNo);
			if (stepSize > 1)
			{
				stepSize = stepSize / 2;
				continue;
			}
			break;
		}
	}
	else
	{
		TRACE2("Build version unsupported: %1!u!", SAFE_ADD((*build), 1, MAX_BUILDNO));
	}

	if ((*major >= MAX_VERSION) || (*minor >= MAX_VERSION) || (*build >= MAXDWORD))
	{
		TRACE1("Overflow was detected!");
		return false;
	}

	return true;
}
