/*
 * cb2util/src/mytypes.h
 *
 * Common used typedef declarations for data types
 *
 * Copyright (C) 2003-2007 misfire
 * All rights reserved.
 *
 * This file is part of cb2util, the CodeBreaker PS2 File Utility.
 *
 * cb2util is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * cb2util is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef _MYTYPES_H_
#define _MYTYPES_H_

typedef signed char s8;
typedef signed short s16;
typedef signed int s32;

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

/* Support for 64-bit integers */
#ifdef __GNUC__
	/* ISO C99 */
	typedef signed long long s64;
	typedef unsigned long long u64;
#else
	/* MS */
	typedef signed __int64 s64;
	typedef unsigned __int64 u64;
#endif

/* Boolean values */
#ifndef TRUE
	#define TRUE	1
#endif

#ifndef FALSE
	#define FALSE	0
#endif

#endif /*_MYTYPES_H_*/
