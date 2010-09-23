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
