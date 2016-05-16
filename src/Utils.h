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

#pragma once

#include "Config.h"

#define WIN32_LEAN_AND_MEAN 1
#include <Windows.h>

#define VERSION_STRING "%1!2u!.%2!02u!.%3!06u!"

#if defined(ENABLE_TRACE) && (ENABLE_TRACE)
#define TRACE(X, ...) PRINTF("[TRACE] " X "\n", __VA_ARGS__)
#else
#define TRACE(X ,...) __noop();
#endif //ENABLE_TRACE

void PRINTF(const char * format, ...);
BYTE GETCHAR(void);
