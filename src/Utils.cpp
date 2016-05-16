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

void PRINTF(const char * format, ...)
{
	static char buffer[2048];
	va_list args;
	va_start(args, format);
	const DWORD len = FormatMessageA(FORMAT_MESSAGE_FROM_STRING, format, 0, 0, buffer, 2048, &args);
	if (len > 0)
	{
		DWORD bytesWritten = 0;
		WriteFile(GetStdHandle(STD_ERROR_HANDLE), buffer, len, &bytesWritten, NULL);
	}
	va_end(args);
}

BYTE GETCHAR(void)
{
	BYTE buffer[1];
	DWORD bytesRead = 0;
	if (ReadFile(GetStdHandle(STD_INPUT_HANDLE), buffer, 1, &bytesRead, NULL))
	{
		return buffer[0];
	}
	return 0x00;
}