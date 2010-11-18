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

// EnglishToChinese.cpp: implementation of the CEnglishToChinese class.
//
//////////////////////////////////////////////////////////////////////
#include "windows.h"
#include "tchar.h"
#include "EnglishToChinese.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEnglishToChinese::CEnglishToChinese()
{
	m_lStrings=0;
	m_ppchStringTable=NULL;
}

CEnglishToChinese::~CEnglishToChinese()
{
	Clear();
}

#ifdef GENERATE_WORD_TABLE
BOOL CEnglishToChinese::BuildList(LPCTSTR ChineseDir, LPCTSTR EnglishDir, LPCTSTR OutputFile)
{
	CArray<char*,char*> m_ChineseString;
	CArray<char*,char*> m_EnglishString;
	HANDLE hFind=INVALID_HANDLE_VALUE;
	WIN32_FIND_DATA wfd;
	BOOL bFirst=TRUE;
	LPTSTR ChineseRoot=NULL,EnglishRoot=NULL,ChineseTemp=NULL,EnglishTemp=NULL;
	UINT uiLength=_tcslen(ChineseDir);
	uiLength=uiLength<_tcslen(EnglishDir)?_tcslen(EnglishDir):uiLength;
	uiLength+=MAX_PATH+1;
	ChineseRoot=new TCHAR[uiLength];
	EnglishRoot=new TCHAR[uiLength];
	ChineseTemp=new TCHAR[uiLength];
	EnglishTemp=new TCHAR[uiLength];
	if(ChineseRoot==NULL||EnglishRoot==NULL||ChineseTemp==NULL||EnglishTemp==NULL)
		return FALSE;
	_tcscpy(ChineseRoot,ChineseDir);
	_tcscpy(EnglishRoot,EnglishDir);
	if(ChineseRoot[_tcslen(ChineseRoot)-1]!=_T('\\'))
		_tcscat(ChineseRoot,_T("\\"));
	if(EnglishRoot[_tcslen(EnglishRoot)-1]!=_T('\\'))
		_tcscat(EnglishRoot,_T("\\"));
	_tcscpy(ChineseTemp,ChineseRoot);
	_tcscat(ChineseTemp,_T("*.txt"));
	hFind=FindFirstFile(ChineseTemp,&wfd);
	if(hFind==INVALID_HANDLE_VALUE)
		return FALSE;
	while(bFirst||FindNextFile(hFind,&wfd))
	{
		bFirst=FALSE;
		CStdioFile fEng,fChn;
		_tcscpy(ChineseTemp,ChineseRoot);
		_tcscat(ChineseTemp,wfd.cFileName);
		_tcscpy(EnglishTemp,EnglishRoot);
		_tcscat(EnglishTemp,wfd.cFileName);
		if(!fChn.Open(ChineseTemp,CFile::modeRead)||
			!fEng.Open(EnglishTemp,CFile::modeRead))
		{
			printf("Error in open file %s\n",wfd.cFileName);
			continue;
		}
		printf("Processing file %s\n",wfd.cFileName);
		CString sChn,sEng;
		while(fChn.ReadString(sChn))
		{
			if(!fEng.ReadString(sEng))
			{
				printf("%s file has too more lines in Chinese\n",wfd.cFileName);
				ASSERT(0);
				break;
			}
			char* s=new char[sChn.GetLength()+1];
			strcpy(s,sChn);
			m_ChineseString.Add(s);
			s=new char[sEng.GetLength()+1];
			strcpy(s,sEng);
			m_EnglishString.Add(s);
		}
		if(fEng.ReadString(sEng))
		{
			printf("%s file has too more lines in English\n",wfd.cFileName);
			ASSERT(0);
		}
		printf("End processing file %s\n",wfd.cFileName);
	}
	FindClose(hFind);
	printf("%d\n",m_ChineseString.GetSize());
	CMemFile mfHeader,mfData;
	long Offset=0;
	for(int i=0;i<m_ChineseString.GetSize();i++)
	{
		mfHeader.Write(&Offset,sizeof(Offset));
		mfData.Write(m_ChineseString.GetAt(i),strlen(m_ChineseString.GetAt(i))+1);
		Offset+=strlen(m_ChineseString.GetAt(i))+1;
		mfHeader.Write(&Offset,sizeof(Offset));
		mfData.Write(m_EnglishString.GetAt(i),strlen(m_EnglishString.GetAt(i))+1);
		Offset+=strlen(m_EnglishString.GetAt(i))+1;
	}
	long* pl=(long*)mfHeader.Detach();
	for(i=0;i<m_ChineseString.GetSize()*2;i++)
		pl[i]+=m_ChineseString.GetSize()*2*sizeof(long);
	CFile f;
	f.Open(OutputFile,CFile::modeReadWrite|CFile::modeCreate);
	Offset=m_ChineseString.GetSize();
	f.Write(&Offset,sizeof(Offset));
	f.Write(pl,pl[0]);
	f.Write(mfData.Detach(),mfData.GetLength());
	return TRUE;
}
#endif

BOOL CEnglishToChinese::Load(const char *lpszFileName)
{
	Clear();
	HANDLE hFile=CreateFile(lpszFileName,GENERIC_READ,0,NULL,OPEN_EXISTING,
		0,NULL);
	if(hFile==INVALID_HANDLE_VALUE)
		return FALSE;
	char* p=new char[GetFileSize(hFile,NULL)];
	if(p==NULL)
		return FALSE;
	DWORD dwBytesRead;
	ReadFile(hFile,p,GetFileSize(hFile,NULL),&dwBytesRead,NULL);
	if(dwBytesRead!=GetFileSize(hFile,NULL))
	{
		delete p;
		return FALSE;
	}
	m_lStrings=*(long*)p;
	m_ppchStringTable=(char**)p;
	m_ppchStringTable++;
	for(int i=0;i<m_lStrings*2;i++)
	{
		m_ppchStringTable[i]+=(long)m_ppchStringTable;
	}
	return TRUE;
}

BOOL CEnglishToChinese::Clear()
{
	return TRUE;
}

const char* CEnglishToChinese::GetChineseByEnglish(const char *lpcszEnglish)
{
	for(int i=0;i<m_lStrings;i++)
	{
		if(stricmp(m_ppchStringTable[i*2+1],lpcszEnglish)==0)
		{
			return m_ppchStringTable[i*2];
		}
	}
	return NULL;
}
