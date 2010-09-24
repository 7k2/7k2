/*
 * Seven Kingdoms 2: The Fryhtan War
 *
 * Copyright 1999 Enlight Software Ltd.
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

// EnglishToChinese.h: interface for the CEnglishToChinese class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ENGLISHTOCHINESE_H__9B198261_961B_11D3_84DC_0090272228CC__INCLUDED_)
#define AFX_ENGLISHTOCHINESE_H__9B198261_961B_11D3_84DC_0090272228CC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CEnglishToChinese  
{
public:
	const char* GetChineseByEnglish(const char* lpcszEnglish);
	BOOL Clear(void);
	BOOL Load(const char* lpszFileName);
#ifdef GENERATE_WORD_TABLE
	static BOOL BuildList(LPCTSTR ChineseDir,LPCTSTR EnglishDir,LPCTSTR OutputFile);
#endif
	CEnglishToChinese();
	virtual ~CEnglishToChinese();

private:
	LONG m_lStrings;
	char** m_ppchStringTable;
};

#endif // !defined(AFX_ENGLISHTOCHINESE_H__9B198261_961B_11D3_84DC_0090272228CC__INCLUDED_)
