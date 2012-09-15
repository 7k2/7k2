/*
 * Seven Kingdoms: Ancient Adversaries
 *
 * Copyright 2010 Jesse Allen
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


#ifndef _WIN32_COMPAT_H
#define _WIN32_COMPAT_H


#ifdef NO_WINDOWS // !WINE && !WIN32

#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>

typedef uint8_t  BYTE;
typedef uint8_t  UCHAR;
typedef uint16_t WORD;
typedef int32_t  BOOL;
typedef int32_t  LONG;
typedef uint32_t UINT;
typedef uint32_t DWORD;
typedef uint32_t ULONG;
typedef uint32_t WPARAM;

#define TRUE 1
#define FALSE 0

#define MAX_PATH 260

typedef struct {
	DWORD dwLowDateTime;
	DWORD dwHighDateTime;
} FILETIME;

typedef struct {
	WORD wYear;
	WORD wMonth;
	WORD wDayOfWeek;
	WORD wDay;
	WORD wHour;
	WORD wMinute;
	WORD wSecond;
	WORD wMilliseconds;
} SYSTEMTIME;

inline char *itoa(int num, char *str, int radix)
{
	sprintf(str,"%d",num);
	return str;
}

inline char *ltoa(long num, char *str, int radix)
{
	sprintf(str,"%ld",num);
	return str;
}

#define strcmpi(s1,s2) strcasecmp(s1,s2)
#define stricmp(s1,s2) strcasecmp(s1,s2)
#define strnicmp(s1,s2,len) strncasecmp(s1,s2,len)

#define _rotr(value,shift) (value<<shift)|(value>>(32-shift))

inline char *strupr(char *str)
{
	while (*str)
	{
		*str = toupper(*str);
		str++;
	}
	return str;
}

inline char *strlwr(char *str)
{
	while (*str)
	{
		*str = tolower(*str);
		str++;
	}
	return str;
}

inline void Sleep(long milliseconds)
{
	struct timespec ts = {0, milliseconds * 1000000};
	nanosleep(&ts, NULL);
}

static const uint64_t TICKSPERSEC = 10000000;
static const uint64_t SECSPERDAY = 86400;
static const uint64_t SECS_1601_TO_1970 = (369 * 365 + 89) * SECSPERDAY;
static const uint64_t TICKS_1601_TO_1970 = SECS_1601_TO_1970 * TICKSPERSEC;

// Implementation taken from the Wine project.
inline void CoFileTimeNow(FILETIME *lpFileTime)
{
	// http://msdn.microsoft.com/en-us/library/windows/desktop/aa383713(v=vs.85).aspx
	union LARGE_INTEGER
	{
		struct
		{
			uint32_t LowPart;
			int32_t  HighPart;
		};
		struct
		{
			uint32_t LowPart;
			int32_t  HighPart;
		} u;
		int64_t QuadPart;
	};

	struct timeval now;
	gettimeofday(&now, NULL);

	LARGE_INTEGER t = {{0}};
	t.QuadPart  = now.tv_sec * TICKSPERSEC + TICKS_1601_TO_1970;
	t.QuadPart += now.tv_usec * 10;

	lpFileTime->dwLowDateTime = t.u.LowPart;
	lpFileTime->dwHighDateTime = t.u.HighPart;
}

inline void FileTimeToLocalFileTime(const FILETIME *lpFileTime, FILETIME *lpLocalFileTime)
{
	lpLocalFileTime->dwLowDateTime = lpFileTime->dwLowDateTime;
	lpLocalFileTime->dwHighDateTime = lpFileTime->dwHighDateTime;
}

inline void FileTimeToSystemTime(const FILETIME *lpFileTime, SYSTEMTIME *lpSystemTime)
{
        uint64_t t = (static_cast<uint64_t>(lpFileTime->dwHighDateTime) << 32) | lpFileTime->dwLowDateTime;
        t -= TICKS_1601_TO_1970;
        time_t sec = t / TICKSPERSEC;
        struct tm systime;
        localtime_r(&sec, &systime);
        lpSystemTime->wYear = systime.tm_year + 1900;
        lpSystemTime->wMonth = systime.tm_mon + 1;
        lpSystemTime->wDay = systime.tm_mday;
        lpSystemTime->wHour = systime.tm_hour;
        lpSystemTime->wMinute = systime.tm_min;
}

#else // WINE || WIN32

#include <windows.h>

#endif


#endif // _WIN32_COMPAT_H
