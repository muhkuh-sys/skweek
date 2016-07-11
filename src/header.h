/***************************************************************************
 *   Copyright (C) 2016 by Christoph Thelen                                *
 *   doc_bacardi@users.sourceforge.net                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/


#ifndef __HEADER_H__
#define __HEADER_H__


typedef unsigned long (*PFN_START)(unsigned long ulParameter);

typedef struct VERSION_HEADER_STRUCT
{
	char acMagic[4];
	unsigned long ulVersion;

	unsigned long *pulLoadAddress;
	PFN_START pfnExecutionAddress;
	unsigned long *pulParameterStart;
	unsigned long *pulParameterEnd;

	unsigned long ulVersionMajor;
	unsigned long ulVersionMinor;
	unsigned long ulVersionMicro;
	const char    acVersionVcs[16];
} VERSION_HEADER_T;

extern const VERSION_HEADER_T tVersionHeader __attribute__ ((section (".header")));


#endif  /* __HEADER_H__ */
