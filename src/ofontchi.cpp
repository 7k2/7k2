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

//Filename    : OFONTCHI.CPP
//Description : Font Object


#if(defined(CHINESE))
// if in tranditional Chinese, TRADITIONAL is defined in the project setting

#include <ofont.h>
#include <all.h>
#include <imgfun.h>
#include <ovga.h>
#include <vga_util.h>
#include <omodeid.h>
#include <ostr.h>
#include <osys.h>
#include <ores.h>
#include <oinfo.h>
#include <ogame.h>
#include <omouse.h>
#include <otransl.h>
#include <onation.h>
#include <ohelp.h>
#include <oblob.h>
#include <resource.h>
#include <englishtochinese.h>
#include <string.h>
#include <stdio.h>
//
struct RGBColor
{
	unsigned char R;
	unsigned char G;
	unsigned char B;
};
//

//Fix up by SXM
//#undef DEFINE_GUID
//#define DEFINE_GUID(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
//        EXTERN_C const GUID name \
//                = { l, w1, w2, { b1, b2,  b3,  b4,  b5,  b6,  b7,  b8 } }

//DEFINE_GUID(IID_IDirectPlay4A,0xab1c531, 0x4745, 0x11d1, 0xa7, 0xa1, 0x0, 0x0, 0xf8, 0x3, 0xab, 0xfc);
//DEFINE_GUID(IID_IDirectPlayLobby3A, 0x2db72491, 0x652c, 0x11d1, 0xa7, 0xa8, 0x0, 0x0, 0xf8, 0x3, 0xab, 0xfc);
//DEFINE_GUID( IID_IDirectDraw4,                  0x9c59509a,0x39bd,0x11d1,0x8c,0x4a,0x00,0xc0,0x4f,0xd9,0x30,0xc5 );

//MACRO DEFINED BY SXM

#define		HZ_SHADOW(x)			(m_ChineseShadowColor)
#define		HZ_FOREGROUND(x)		(m_ChineseForegroundColor)
#define		HZ_BACKGROUND		(m_ChineseBackgroundColor)

//MACRO DEFINED BY SXM
//SXM: Debug functions
char g_VeryLongBuffer[10][10240];
char g_NowBuffer=0;
void Dump(char* s);
void MakeSafeCopy(char** s)
{
	if(*s==NULL)
		g_VeryLongBuffer[g_NowBuffer][0]=0;
	else
		strcpy(g_VeryLongBuffer[g_NowBuffer],*s);
	*s=g_VeryLongBuffer[g_NowBuffer];
	g_NowBuffer++;
	if(g_NowBuffer==10)
		g_NowBuffer=0;
}

void Font::BreakOnString(char* String)
{
//	static CEnglishToChinese* pETC=NULL;
#ifdef __DEBUG
	OutputDebugString(FontName);
	OutputDebugString(" : ");
	OutputDebugString(String);
	OutputDebugString("\n");
#endif //__DEBUG
#if(defined(TRADITIONAL))
	if(strstr(String,"¦w©~¼Ö"))
		GetTickCount();
#endif
	Dump(FontName);
	Dump(" : ");
	Dump(String);
	Dump("\n");
//	if(pETC==NULL)
//	{
//		pETC=new CEnglishToChinese;
//		pETC->Load("D:\\a.dat");
//	}
//	const char* p=pETC->GetChineseByEnglish(String);
//	if(p!=NULL)
//	{
//		strcpy(String,p);//A real risk
//	}
}
//SXM: Debug functions
//SXM: Dump Functions
#define DEFAULT_DUMP_FILE "D:\\7K.txt"
void DumpBin(char* FileName,char* s,int Length)
{
#ifdef __DEBUG
	if(s==NULL)
		return;
	HANDLE hFile=CreateFile(FileName,GENERIC_READ|GENERIC_WRITE,0,NULL,OPEN_ALWAYS,
		0,NULL);
	if(hFile==INVALID_HANDLE_VALUE)
	{
		return;
	}
	SetFilePointer(hFile,0,NULL,SEEK_END);
	DWORD Dummy;
	WriteFile(hFile,s,Length,&Dummy,NULL);
	CloseHandle(hFile);
#endif //__DEBUG
}

void Dump(char* FileName,char* s)
{
#ifdef __DEBUG
	DumpBin(FileName,s,strlen(s));
#endif //__DEBUG
}

void Dump(char* FileName,int i)
{
	char s[20];
	sprintf(s,"%d",i);
	Dump(FileName,s);
}

void DumpBin(char* s,int Length)
{
	DumpBin(DEFAULT_DUMP_FILE,s,Length);
}

void Dump(char* s)
{
	Dump(DEFAULT_DUMP_FILE,s);
}

void Dump(int i)
{
	Dump(DEFAULT_DUMP_FILE,i);
}
//SXM

//SXM: Chinese Font definition
TChineseFont* g_pChineseFont13=NULL,* g_pChineseFont16=NULL,* g_pChineseFont24=NULL;
static BOOL g_bIsChineseFontLoaded=FALSE;
static BYTE g_bChineseFontBitmap[25*25+sizeof(int)*2];

#if(defined(TRADITIONAL))

#pragma warning(push)
// disable constant int to char warning
#pragma warning( once : 4305 )
#pragma warning( once : 4309 )

static char g_bTraditionalChineseFontMap[]={0xa4,0x43,0xa4,0x6a,0xa4,0xfd,0xb4,0xc2,0xa6,0x77,0xb8,0xcb,0xb5,0x7b,0xa6,0xa1,0xb5,0xb2,0xa7,0xf4,
0xa7,0x40,0xb7,0x7e,0xb0,0xf5,0xa6,0xe6,0xb2,0xbe,0xb0,0xa3,0xba,0xf4,0xaf,0xb8,0xb7,0x73,0xc2,0x49,
0xa8,0xfa,0xae,0xf8,0xa7,0x52,0xa9,0xe8,0xa7,0xdc,0xab,0xd7,0xba,0xa1,0xc3,0x42,0xbc,0xc8,0xb0,0xb1,
0xb9,0x43,0xc0,0xb8,0xc4,0x7e,0xc4,0xf2,0xad,0x50,0xa9,0x52,0xaa,0x5a,0xbe,0xb9,0xa9,0xdc,0xad,0xb7,
0xa9,0x4d,0xa4,0x50,0xad,0xba,0xbf,0xef,0xb3,0xe6,0xac,0x4f,0xa6,0x57,0xa4,0x48,0xb0,0xf3,0xac,0x44,
0xb0,0x5f,0xac,0xf0,0xb5,0x6f,0xa8,0xc6,0xa5,0xf3,0xc5,0xd1,0xa7,0xde,0xb3,0x4e,0xb3,0x74,0xbe,0xf0,
0xbe,0xd4,0xa7,0xd0,0xb3,0xf5,0xb4,0xba,0xa4,0xa3,0xaf,0xe0,0xb2,0x4b,0xa5,0x5b,0xa4,0x77,0xa5,0x58,
0xb2,0x7b,0xb9,0x4c,0xaa,0xba,0xad,0x5e,0xb6,0xaf,0xb3,0x6e,0xc5,0xe9,0xb3,0x5c,0xa5,0x69,0xc3,0xd2,
0xa8,0xf3,0xc4,0xb3,0xbd,0xd0,0xbe,0x5c,0xc5,0xaa,0xa4,0x55,0xad,0xb1,0xa1,0x43,0xab,0xf6,0xc1,0xe4,
0xa8,0xe4,0xa5,0xa6,0xb3,0xa1,0xa4,0xc0,0xb1,0x7a,0xa7,0x5f,0xb1,0xb5,0xa8,0xfc,0xab,0x65,0xa4,0xa4,
0xa9,0xd2,0xa6,0xb3,0xb1,0xf8,0xb4,0xda,0xa1,0x48,0xa6,0x70,0xaa,0x47,0xbe,0xdc,0xa1,0xa7,0xa1,0xa8,
0xa1,0x41,0xb1,0x4e,0xad,0x6e,0xa5,0xb2,0xb6,0xb7,0xa6,0xb9,0xa7,0xb9,0xa5,0xfe,0xbb,0xdd,0xb5,0x77,
0xba,0xd0,0xb6,0xa1,0xa7,0x41,0xb3,0xd1,0xbe,0x6c,0xaa,0xc5,0xac,0xb0,0xa5,0xfc,0xa5,0x74,0xa4,0x40,
0xa5,0x78,0xb8,0x67,0xa6,0x62,0xb9,0x71,0xb8,0xa3,0xa4,0x57,0xa4,0x46,0xa8,0xcf,0xaa,0xcc,0xb1,0x61,
0xa8,0xd3,0xb3,0xcc,0xb0,0xaa,0xa4,0xb8,0xbb,0xbc,0xa5,0xe6,0xa7,0xeb,0xad,0xb0,0xae,0xd1,0xc5,0xb3,
0xa4,0x5f,0xa7,0xda,0xb0,0xea,0xa4,0x70,0xb7,0xed,0xb9,0x44,0xb7,0xf9,0xb2,0xb3,0xab,0x71,0xbf,0xcb,
0xc2,0xf7,0xb1,0xcd,0xaf,0x53,0xb2,0x76,0xb4,0xdd,0xb1,0xfd,0xa6,0x56,0xb6,0x51,0xb8,0xdb,0xa5,0x48,
0xa4,0x4d,0xa7,0x4c,0xa4,0xa7,0xa8,0x61,0xac,0x40,0xb1,0xcf,0xa6,0xca,0xa9,0x6d,0xa9,0xf3,0xa4,0xf4,
0xa4,0xf5,0xa6,0x73,0xa4,0x60,0xc3,0xf6,0xab,0x59,0xa4,0xd1,0xa6,0x4d,0xa6,0x55,0xbd,0xd1,0xab,0x4a,
0xb6,0xd5,0xa4,0x4f,0xb0,0x74,0xa7,0xf3,0xa8,0xec,0xaa,0xa7,0xc5,0x51,0xad,0xab,0xb1,0xe6,0xa4,0x54,
0xab,0xe4,0xb0,0xc8,0xa6,0xac,0xb1,0x60,0xbb,0x44,0xa7,0x67,0xa5,0x44,0xa9,0xfa,0xbb,0x5c,0xa5,0x40,
0xac,0x7d,0xb1,0x78,0xa5,0xbb,0xa8,0x7d,0xad,0x57,0xa5,0xce,0xa4,0xdf,0xa6,0xad,0xa8,0x4d,0xa9,0x77,
0xa9,0xbe,0xb9,0xea,0xb9,0xb2,0xa6,0x6e,0xa7,0x61,0xac,0x4a,0xb5,0x4d,0xbb,0xa1,0xb1,0x6f,0xb3,0x6f,
0xbc,0xcb,0xbc,0xa6,0xa4,0xbc,0xad,0xcc,0xb4,0x4e,0xad,0xd3,0xa1,0x49,0xb7,0x7c,0xb5,0xb9,0xa8,0xc7,
0xc3,0xf8,0xb0,0xd9,0xb0,0xa9,0xc0,0x59,0xa5,0xf4,0xa6,0xd2,0xc5,0xe7,0xb7,0x4e,0xae,0xc9,0xad,0xd4,
0xa7,0x4f,0xa5,0x73,0xb3,0x73,0xb0,0xda,0xa9,0x41,0xa4,0x7e,0xb0,0xdf,0xba,0xd7,0xae,0xda,0xb3,0xba,
0xc1,0xd9,0xc1,0x79,0xb0,0x66,0xa6,0xba,0xb6,0x64,0xa8,0xc4,0xb5,0xa5,0xb5,0xdb,0xa6,0xa8,0xad,0x78,
0xb0,0xad,0xab,0x48,0xa8,0xbe,0xa6,0xda,0xb0,0xa1,0xa1,0x4a,0xb6,0xa4,0xaf,0xc7,0xa4,0x4a,0xbd,0x73,
0xa8,0xee,0xb9,0xdc,0xa8,0x74,0xa6,0xd3,0xb8,0xd3,0xa7,0xe5,0xc5,0x5d,0xb1,0xda,0xa4,0x68,0xb4,0xa3,
0xb0,0xab,0xbb,0x50,0xa6,0x68,0xa6,0x7e,0xab,0xe8,0xac,0xc6,0xb1,0xfe,0xa4,0xf7,0xb3,0xe0,0xa4,0x6c,
0xa6,0x40,0xc0,0xb9,0xa4,0xb3,0xb6,0x54,0xb5,0x41,0xb4,0xb2,0xa5,0xc1,0xa7,0x5e,0xc1,0x6f,0xb5,0xb4,
0xb3,0xbb,0xb8,0xd1,0xb4,0xe5,0xa4,0x62,0xc4,0x40,0xa8,0xf5,0xc2,0xbe,0xa4,0x44,0xbc,0x7b,0xa5,0xa3,
0xae,0xd3,0xb1,0xc2,0xc5,0x76,0xa6,0xbc,0xb3,0x42,0xb2,0x7a,0xa9,0x79,0xab,0xb5,0xbf,0xc5,0xa4,0xe9,
0xc4,0x5e,0xb2,0x60,0xa5,0xdf,0xa5,0x5c,0xc5,0xab,0xb8,0x6f,0xa5,0xe7,0xc4,0x59,0xba,0xde,0xb1,0xd0,
0xa6,0x41,0xa7,0xd4,0xb5,0x68,0xb3,0xce,0xb7,0x52,0xb1,0xf3,0xa9,0xdb,0xae,0xa3,0xb7,0x53,0xab,0xe3,
0xaf,0xab,0xad,0x49,0xb7,0x6c,0xc0,0x73,0xab,0xc2,0xac,0xd2,0xbc,0xf6,0xa6,0xe5,0xa8,0x6b,0xa8,0xe0,
0xac,0x47,0xa6,0x50,0xbc,0xc4,0xb7,0x5a,0xba,0xeb,0xb3,0xf8,0xad,0xd5,0xae,0x65,0xb2,0xa7,0xa9,0xce,
0xc2,0x5a,0xb6,0xc3,0xb8,0xb7,0xb6,0xb6,0xc0,0xb3,0xb2,0xce,0xc1,0x7c,0xa5,0xd8,0xa7,0x59,0xa4,0x53,
0xa6,0xf3,0xa7,0x55,0xb0,0x7d,0xc1,0xf6,0xb8,0xb9,0xba,0xd9,0xae,0x76,0xa9,0x7c,0xb5,0x4c,0xad,0x59,
0xad,0xc9,0xaa,0x46,0xa4,0xe8,0xa4,0xb4,0xab,0xe1,0xb8,0xa8,0xb8,0xdc,0xac,0x60,0xa5,0x7c,0xa6,0xdc,
0xb8,0xf4,0xb8,0x73,0xa7,0xf0,0xa4,0xde,0xaf,0x54,0xab,0xc7,0xa4,0x51,0xb8,0x55,0xab,0xe6,0xa4,0xe0,
0xac,0xdb,0xb1,0xa4,0xa8,0xcc,0xad,0x70,0xab,0x4b,0xc1,0xc1,0xa8,0xa5,0xac,0x6f,0xb0,0x5a,0xa7,0x76,
0xa9,0xf1,0xab,0xd2,0xa6,0xe8,0xc6,0x5a,0xa6,0x69,0xa5,0xfd,0xa7,0xe2,0xac,0xdd,0xb2,0x4d,0xb7,0xa1,
0xb7,0x51,0xc5,0x6f,0xb6,0xdb,0xc3,0x43,0xb1,0x41,0xbd,0xa5,0xb7,0xa5,0xba,0xc9,0xbd,0xd4,0xb4,0x41,
0xba,0x41,0xba,0xb8,0xc1,0x75,0xa9,0x7e,0xc0,0x49,0xb4,0x63,0xa5,0x4f,0xb9,0xc3,0xaf,0x51,0xa6,0x58,
0xb9,0xf0,0xb1,0xd1,0xaf,0xa5,0xa6,0xc7,0xb0,0xd3,0xba,0x65,0xa7,0x45,0xa6,0x61,0xaf,0x75,0xa4,0x7a,
0xac,0xd9,0xa5,0xf7,0xa8,0xc3,0xaa,0xa9,0xb9,0xcf,0xc3,0xe3,0xa8,0xaf,0xb3,0xd2,0xbc,0x6f,0xad,0xd1,
0xbf,0xb3,0xac,0x46,0xb3,0x71,0xaa,0x4f,0xa8,0xd1,0xa5,0xe9,0xae,0xc4,0xac,0xa3,0xbb,0xba,0xb1,0x6a,
0xc5,0xe3,0xb4,0xad,0xb3,0xd3,0xb5,0xaa,0xb4,0x5f,0xae,0xa4,0xa9,0x5e,0xb3,0xad,0xa5,0x7e,0xa8,0xad,
0xb1,0x77,0xa8,0x75,0xa8,0xa3,0xa5,0xd6,0xbd,0xa7,0xaf,0x66,0xa8,0x44,0xc2,0xe5,0xa7,0xa1,0xa7,0x69,
0xaa,0xbe,0xbf,0x6e,0xb2,0xd6,0xaa,0x76,0xc0,0xf8,0xc5,0xfd,0xb3,0x79,0xba,0xd6,0xc1,0xc2,0xb4,0xa9,
0xa4,0xe2,0xa5,0x42,0xa4,0x6f,0xa4,0x67,0xaa,0xf7,0xae,0xa2,0xb7,0x5c,0xb9,0xef,0xa6,0x43,0xaf,0xaa,
0xa9,0x76,0xc3,0xfe,0xa9,0xde,0xb5,0xd1,0xaf,0x64,0xaf,0xc5,0xb6,0xea,0xb9,0xda,0xc2,0x5f,0xae,0x61,
0xb3,0xb1,0xbf,0xd1,0xbf,0x79,0xa6,0xec,0xb7,0x49,0xbd,0x6c,0xac,0xf9,0xb6,0x58,0xa5,0xbc,0xc3,0xad,
0xbb,0x7d,0xb0,0x51,0xa5,0xef,0xa5,0xad,0xa7,0x4b,0xa5,0xcd,0xc6,0x46,0xb6,0xee,0xac,0xb4,0xb8,0xe9,
0xb8,0xdd,0xa5,0x47,0xb5,0xbd,0xb6,0xfc,0xbc,0x57,0xbc,0xd6,0xa4,0xf1,0xb7,0x4c,0xa8,0xac,0xb6,0x7d,
0xa9,0xbc,0xb8,0x7b,0xa6,0xd5,0xa6,0xd8,0xc4,0x5f,0xbc,0x43,0xae,0xb6,0xc0,0x54,0xc5,0x44,0xb8,0xd5,
0xa8,0x7e,0xa8,0xeb,0xc0,0x52,0xa6,0xdb,0xa4,0x5a,0xb4,0xed,0xb4,0xb1,0xc4,0x6d,0xbb,0xf2,0xb6,0xf4,
0xaf,0xf3,0xb3,0xa3,0xb6,0xcc,0xa5,0xca,0xbd,0x6b,0xa7,0xcb,0xb6,0xdc,0xc4,0xea,0xa8,0xc9,0xb8,0xea,
0xb7,0xbd,0xaa,0xf9,0xa8,0x53,0xa4,0x5d,0xab,0xc8,0xae,0xf0,0xa7,0xef,0xc5,0xdc,0xbb,0xee,0xbb,0xb4,
0xa4,0x6b,0xa9,0xca,0xb1,0xa1,0xb5,0xa3,0xc1,0xf5,0xc4,0x71,0xa7,0x7c,0xa4,0x75,0xbc,0x74,0xb8,0xcc,
0xb7,0x46,0xac,0xa1,0xa4,0xb0,0xb0,0x65,0xb0,0xa8,0xc1,0x7b,0xab,0xb0,0xb8,0x71,0xa6,0x5d,0xb3,0xf4,
0xa5,0x4c,0xbc,0xc9,0xb2,0xd7,0xb4,0xa6,0xac,0xf1,0xab,0xc5,0xa5,0xac,0xb0,0xed,0xa5,0xbf,0xaa,0xed,
0xa5,0xdc,0xbe,0xd7,0xa4,0xcf,0xbc,0xb3,0xbc,0x79,0xaf,0xac,0xb5,0x66,0xab,0xdd,0xa6,0x5e,0xc2,0x6b,
0xad,0x4d,0xb7,0xb3,0xad,0xe8,0xb1,0x4b,0xb1,0xb4,0xa5,0xb4,0xc5,0xa5,0xbf,0x57,0xa5,0xd1,0xa8,0xba,
0xaa,0x70,0xab,0x44,0xb2,0x56,0xa5,0x75,0xae,0xa7,0xa4,0xc1,0xaa,0x60,0xb5,0xf8,0xa4,0xc6,0xc0,0x48,
0xa4,0x76,0xab,0xd8,0xa5,0x52,0xbe,0x64,0xa9,0x54,0xa6,0xfd,0xb4,0xbf,0xa9,0x6c,0xab,0x4f,0xab,0xf9,
0xaa,0xd6,0xac,0xec,0xbe,0xc7,0xc2,0xd4,0xb7,0x56,0xa5,0xb9,0xae,0x69,0xc1,0x63,0xb4,0xde,0xa4,0x66,
0xb6,0xd7,0xc1,0x60,0xb6,0x56,0xc3,0xe4,0xac,0xc9,0xab,0x49,0xa6,0xfb,0xb0,0xcf,0xbb,0xe2,0xc4,0xdd,
0xa7,0x51,0xc0,0x75,0xbc,0xe7,0xa5,0xf1,0xbf,0xd2,0xc5,0xe5,0xb0,0xca,0xbe,0xfa,0xa4,0x45,0xb9,0xae,
0xad,0xc6,0xb0,0x6b,0xbe,0xf7,0xbc,0xf4,0xc0,0xbb,0xb1,0xb9,0xa4,0xce,0xb5,0x79,0xb3,0xdd,0xbe,0x44,
0xb3,0xd0,0xa7,0xbd,0xab,0xdc,0xae,0xd4,0xa4,0xcd,0xa8,0x49,0xc3,0x61,0xac,0x50,0xb6,0x48,0xa9,0x78,
0xac,0x51,0xa9,0x5d,0xbd,0xf7,0xa6,0x4e,0xb7,0xd3,0xbc,0x71,0xa4,0xba,0xb9,0xac,0xab,0x51,0xb6,0xc7,
0xb6,0x6d,0xa4,0xd6,0xb9,0x4a,0xc2,0xf9,0xa4,0x4b,0xbb,0x4c,0xa4,0x64,0xc3,0x7e,0xb2,0xb1,0xb2,0xbb,
0xb7,0xe7,0xba,0xd8,0xb8,0xf1,0xab,0x43,0xb7,0xfd,0xb9,0x7c,0xbe,0xe3,0xa6,0xbf,0xaa,0x65,0xa8,0xce,
0xa5,0xa2,0xad,0xa6,0xc1,0x6e,0xa9,0xb3,0xa7,0xd6,0xa5,0x68,0xb7,0xc7,0xb3,0xc6,0xa4,0xb5,0xb1,0x46,
0xb1,0xc8,0xae,0xf6,0xba,0x71,0xbb,0x52,0xa4,0xc9,0xb0,0xdb,0xac,0xd3,0xb3,0x43,0xac,0xb1,0xbc,0x77,
0xbe,0xd6,0xbb,0x61,0xb2,0xb4,0xb0,0x4a,0xb6,0x50,0xa5,0xc3,0xbb,0xb7,0xc5,0x58,0xb5,0xa6,0xa7,0x6a,
0xae,0xb5,0xbb,0x71,0xc1,0xf4,0xbf,0x40,0xaf,0x50,0xc5,0x4b,0xbf,0xe1,0xbd,0xee,0xbd,0xf1,0xa4,0xf9,
0xad,0xec,0xc6,0x60,0xa9,0x7d,0xaa,0x41,0xbd,0xe2,0xc2,0xdf,0xaa,0x6b,0xae,0xbe,0xab,0x6f,0xb9,0x77,
0xae,0xc6,0xa8,0x67,0xf6,0x74,0xa5,0x76,0xa8,0xae,0xbd,0xfc,0xaf,0xeb,0xaa,0xfd,0xb7,0xc5,0xc2,0xea,
0xc3,0xec,0xba,0xf2,0xa9,0x5f,0xc0,0xb6,0xa4,0xd2,0xa9,0xf9,0xa7,0x43,0xab,0xab,0xae,0x4d,0xa4,0x73,
0xa5,0x5d,0xac,0x41,0xc2,0xb2,0xaa,0xbd,0xad,0xc8,0xc0,0x7e,0xaa,0xfe,0xb7,0x57,0xc1,0xab,0xa5,0xd5,
0xb2,0x73,0xb8,0x76,0xad,0x68,0xc1,0x78,0xb6,0x71,0xc3,0xea,0xb9,0xfa,0xb0,0xb6,0xa4,0xd3,0xb2,0xa4,
0xb7,0x74,0xa9,0xaf,0xa4,0xe4,0xab,0x69,0xab,0xa5,0xab,0xa2,0xa7,0xdd,0xa4,0xfb,0xa8,0xd8,0xa4,0xad,
0xb9,0xc4,0xa7,0xe4,0xaf,0xc8,0xa8,0xe8,0xaf,0xe1,0xa9,0x44,0xa7,0x75,0xbf,0xf4,0xa7,0xd1,0xc2,0xd0,
0xaa,0xd1,0xb0,0x52,0xb7,0xb4,0xaa,0xf8,0xa9,0xb9,0xaa,0xd3,0xa5,0x53,0xa7,0xcc,0xaf,0x45,0xbf,0xba,
0xa5,0xfa,0xc0,0xf4,0xc5,0xa2,0xb8,0x6e,0xbd,0xfa,0xab,0xcd,0xbe,0xee,0xb9,0x4d,0xb3,0xa5,0xa5,0x4e,
0xbb,0xf9,0xa8,0xe3,0xb5,0xf5,0xb3,0x53,0xbe,0xc9,0xba,0x58,0xb9,0xaa,0xb1,0xb2,0xba,0xc3,0xad,0xb5,
0xc0,0xb0,0xaa,0xf1,0xab,0xfc,0xb4,0xa7,0xa6,0xb8,0xbd,0xe7,0xb6,0xcb,0xc0,0xeb,0xa7,0xc4,0xaa,0xaf,
0xaf,0xba,0xb9,0xb3,0xc3,0xc7,0xa9,0x62,0xa8,0xab,0xc2,0xe0,0xa7,0xe9,0xc1,0x70,0xbe,0xd3,0xab,0xe5,
0xa7,0xc6,0xb0,0xf7,0xb6,0x69,0xa9,0x60,0xb9,0xe8,0xb2,0x5f,0xb3,0xb4,0xbf,0xe4,0xc6,0x6e,0xbd,0xc3,
0xac,0x79,0xba,0x77,0xc2,0x41,0xc4,0x61,0xae,0xed,0xbd,0x75,0xb0,0x7b,0xc5,0xa7,0xb3,0x51,0xc1,0xdf,
0xc6,0x43,0xa4,0x5b,0xbb,0x45,0xb6,0xb0,0xb6,0x57,0xae,0x74,0xb1,0x4f,0xa5,0xc7,0xbe,0x76,0xbe,0xe1,
0xad,0x74,0xb3,0x64,0xb8,0x6d,0xbc,0xec,0xbf,0x6d,0xa9,0x53,0xbe,0xcc,0xb8,0xfc,0xbe,0x5f,0xa4,0xcc,
0xb9,0x42,0xae,0x60,0xaf,0x71,0xbf,0x50,0xb3,0xb6,0xb2,0x6a,0xc2,0x45,0xc3,0x5b,0xa7,0xd3,0xab,0xe7,
0xa6,0x6b,0xb5,0xfb,0xbd,0xd7,0xba,0xee,0xa4,0xe6,0xc0,0xb5,0xa7,0xa7,0xb9,0xc5,0xa9,0xba,0xb7,0x76,
0xc5,0x55,0xa7,0xae,0xbc,0xbe,0xb2,0xbd,0xab,0xf4,0xb0,0xdd,0xbb,0xca,0xb0,0x4f,0xbb,0xbb,0xae,0xe7,
0xb0,0xd1,0xbc,0xae,0xbc,0xbd,0xa5,0xf5,0xb8,0x60,0xb0,0x5e,0xaa,0xab,0xab,0x7e,0xbc,0xc6,0xa8,0xe2,
0xbc,0xfb,0xb4,0xbc,0xae,0x7a,0xac,0x59,0xb4,0xc1,0xb1,0x69,0xa9,0x49,0xb9,0xd0,0xc4,0xdb,0xbb,0x7b,
0xb2,0xe6,0xb4,0xab,0xa4,0xbf,0xaa,0xf6,0xc2,0xa7,0xa8,0xe7,0xbf,0xed,0xb1,0x71,0xbf,0xec,0xab,0x68,
0xbd,0x61,0xbb,0xf7,0xc4,0x5b,0xb3,0x68,0xa7,0x78,0xbc,0x73,0xc2,0xd7,0xb1,0xbe,0xbe,0xa6,0xab,0xca,
0xb7,0x71,0xb7,0xf6,0xb9,0x70,0xa6,0xc0,0xa5,0x62,0xa8,0x42,0xa6,0xd1,0xaa,0xec,0xad,0x54,0xc3,0x66,
0xaa,0xef,0xae,0xc8,0xa5,0xd2,0xb6,0x7e,0xa4,0xa1,0xa8,0xb3,0xb1,0xbb,0xb7,0xc0,0xb5,0x4b,0xb7,0x4d,
0xad,0x56,0xc3,0x67,0xa4,0x4e,0xaf,0xee,0xb9,0x41,0xbd,0x7d,0xbe,0x69,0xa9,0xc5,0xab,0xd3,0xc4,0xe0,
0xb2,0x5d,0xc4,0xb5,0xbc,0x46,0xaf,0xb0,0xaa,0xd7,0xc1,0x7e,0xb9,0xc1,0xb6,0xb5,0xa6,0xd7,0xa4,0x72,
0xbd,0xd6,0xba,0x5d,0xaa,0x59,0xba,0x61,0xba,0xcd,0xb0,0x49,0xbe,0x5e,0xb6,0x76,0xac,0x4b,0xc0,0x5c,
0xb6,0xbc,0xb6,0x5b,0xaf,0x41,0xb3,0x7e,0xbc,0x52,0xb0,0xd7,0xb6,0xda,0xb4,0xc0,0xac,0x78,0xc4,0xc1,
0xbc,0x7a,0xbc,0x7e,0xa9,0xc0,0xbb,0x57,0xb7,0x50,0xc1,0x49,0xaa,0x4c,0xa6,0x74,0xa9,0x7a,0xb6,0xf8,
0xb9,0x79,0xae,0xa9,0xb3,0xd5,0xbe,0xc4,0xc3,0x6b,0xb5,0x6e,0xb9,0xd2,0xa6,0xb4,0xb2,0x40,0xbe,0xda,
0xb4,0x58,0xbe,0x55,0xa5,0x49,0xaf,0x62,0xb8,0xdf,0xba,0x42,0xb4,0x6e,0xc4,0xf8,0xc3,0xc4,0xa4,0xfa,
0xc0,0xd9,0xb7,0x68,0xc2,0xb3,0xbd,0xfd,0xbd,0xd5,0xaa,0xea,0xc4,0x55,0xb6,0xcf,0xa5,0xd9,0xc5,0x42,
0xb7,0x55,0xba,0x74,0xc0,0xf2,0xa7,0x74,0xa7,0xf1,0xbb,0xed,0xba,0x4e,0xb3,0xbd,0xa9,0xf6,0xaf,0x7d,
0xa5,0xf0,0xac,0xbd,0xbf,0xd7,0xaf,0xe4,0xac,0x4c,0xa4,0xa9,0xb8,0x42,0xb0,0xb5,0xb3,0xdf,0xc5,0x77,
0xa9,0xda,0xad,0xcd,0xb5,0x75,0xbd,0x77,0xb0,0xbd,0xc3,0x69,0xbc,0x78,0xb9,0x53,0xaa,0xce,0xa8,0x55,
0xbd,0x4d,0xbf,0x69,0xf3,0x4f,0xb9,0xfd,0xc3,0x50,0xb1,0xa7,0xbc,0xd0,0xae,0xbc,0xae,0xb3,0xa6,0x59,
0xa7,0xec,0xa8,0xdf,0xd8,0xe6,0xa5,0x6a,0xab,0xeb,0xb2,0x5c,0xbc,0xe4,0xa5,0xc4,0xa8,0x7c,0xad,0x5f,
0xaa,0xe1,0xa6,0xb7,0xc5,0xd6,0xb2,0xd3,0xaa,0xcf,0xb1,0xbd,0xb5,0xc4,0xb7,0xc6,0xbf,0xb0,0xaf,0xd7,
0xaa,0xd5,0xc0,0xbf,0xab,0x47,0xf9,0xd7,0xb4,0xb3,0xba,0x6a,0xa9,0x64,0xa6,0x6f,0xa9,0x74,0xaa,0x45,
0xaf,0x76,0xb8,0xd6,0xb5,0xa7,0xbc,0x72,0xb9,0x46,0xb6,0xc8,0xbf,0x76,0xa6,0x7a,0xad,0xcb,0xad,0xd7,
0xdf,0xc2,0xbb,0xb0,0xb6,0xe9,0xa6,0xed,0xba,0xfb,0xa8,0xca,0xa5,0xeb,0xc4,0xcc,0xbe,0x60,0xbb,0x58,
0xb5,0xc3,0xbe,0xc2,0xbf,0xf1,0xc1,0xef,0xb4,0x49,0xba,0x52,0xa6,0x4c,0xb0,0x67,0xa8,0xb8,0xb1,0xbc,
0xc2,0xed,0xb4,0x4d,0xb5,0x53,0xb1,0xec,0xc4,0x41,0xb6,0x4f,0xa8,0xf2,0xc1,0x6c,0xc5,0x41,0xa7,0xd9,
0xa5,0xee,0xad,0xad,0xb4,0x78,0xb4,0xa4,0xaf,0xb5,0xbe,0x78,0xae,0x79,0xa4,0xac,0xbd,0x54,0xa5,0xcb,
0xb4,0xa1,0xc1,0xa1,0xb2,0x72,0xb3,0x5d,0xb9,0xba,0xac,0x71,0xbe,0xa8,0xb1,0xc0,0xc2,0xbd,0xbb,0x47,
0xb9,0xf1,0xaf,0xca,0xa5,0x46,0xab,0x64,0xb4,0xee,0xb5,0xf4,0xb0,0x68,0xa7,0xe1,0xb0,0xae,0xa9,0x5b,
0xc5,0x53,0xbd,0x64,0xb3,0xf2,0xb4,0xfd,0xae,0xc0,0xad,0xfe,0xa8,0xfd,0xab,0xf7,0xa9,0x50,0xb1,0xdb,
0xa4,0xe5,0xa5,0xa8,0xd0,0x60,0xb5,0xae,0xa6,0x7d,0xb2,0x62,0xc9,0xf3,0xc9,0x4f,0xbe,0x46,0xa6,0x75,
0xaa,0x51,0xa5,0x65,0xa8,0xbd,0xcc,0xb0,0xad,0xfa,0xb6,0x44,0xb4,0x64,0xbc,0x68,0xba,0xbb,0xad,0x40,
0xa4,0xbd,0xa4,0xf6,0xb0,0x64,0xb6,0x5e,0xc0,0xc6,0xa1,0x54,0xd3,0xe3,0xac,0xde,0xb5,0x50,0xbe,0x62,
0xce,0x60,0xb3,0xcd,0xd6,0xc3,0xb3,0xb7,0xbb,0xf4,0xbe,0x61,0xa8,0x43,0xb1,0x4d,0xac,0x55,0xae,0xba,
0xc0,0xb4,0xad,0xe3,0xa6,0x5a,0xae,0xbd,0xb1,0x7b,0xb9,0xce,0xc3,0xfb,0xc9,0x45,0xbf,0xd5,0xb1,0x73,
0xc4,0xe8,0xbe,0xde,0xa9,0xc7,0xc1,0x40,0xbf,0xf9,0xc4,0xb9,0xa7,0x6c,0xb0,0x56,0xa4,0x5c,0xba,0x40,
0xb7,0xa7,0xaa,0x42,0xd2,0x5c,0xb9,0x5f,0xa8,0xa4,0xbd,0xcc,0xa8,0xf7,0xac,0xc3,0xa8,0xf4,0xa7,0xf7,
0xaa,0xb1,0xc3,0xaa,0xb8,0x7d,0xc1,0x7d,0xaa,0x4d,0xb1,0xd9,0xb1,0x5f,0xa5,0xde,0xbd,0x4c,0xbd,0xf5,
0xc0,0xa6,0xb0,0xf2,0xcc,0xe5,0xaf,0xd9,0xc5,0x40,0xb3,0xbe,0xad,0xb8,0xa4,0xe3,0xc0,0xe7,0xbf,0xe9,
0xc9,0x61,0xbb,0xfe,0xb1,0xdf,0xac,0xfc,0xb4,0xfe,0xb1,0x6d,0xac,0xe3,0xbb,0x73,0xcf,0xfa,0xab,0x52,
0xb4,0xf9,0xaf,0xed,0xc3,0x44,0xb2,0xc2,0xb3,0x55,0xbf,0x55,0xb2,0xc4,0xa7,0xc0,0xb3,0xe8,0xc2,0xdd,
0xbe,0x41,0xc4,0xeb,0xac,0xb9,0xc0,0xfc,0xa7,0x62,0xb9,0x78,0xa5,0xf8,0xb2,0xa3,0xbc,0x76,0xc5,0x54,
0xa7,0x5d,0xb6,0xc6,0xba,0x43,0xb7,0xbe,0xb2,0xee,0xc5,0xba,0xae,0x78,0xbb,0x79,0xa1,0x52,0xb2,0x79,
0xae,0x5d,0xc3,0xb1,0xad,0x71,0xb2,0x6f,0xaa,0xf0,0xbf,0xdd,0xa9,0xdf,0xb4,0xb6,0xaf,0x44,0xae,0xc7,
0xc6,0x5b,0xbc,0xab,0xb3,0xc3,0xa1,0x53,0xa5,0xe1,0xbf,0xe0,0xbd,0xe0,0xad,0xdb,0xa7,0x54,0xaa,0xd9,
0xad,0x47,0xb6,0xf1,0xb1,0xc6,0xb6,0x73,0xac,0x64,0xa6,0x72,0xac,0x57,0xbd,0x7c,0xa4,0xee,0xb1,0xb0,
0xbd,0x43,0xbf,0xbe,0xad,0x65,0xbd,0xc4,0xaf,0xc9,0xa6,0x6c,0xb8,0xde,0xa8,0x46,0xb7,0xc8,0xa5,0xcc,
0xa8,0x63,0xae,0xae,0xb5,0xce,0xba,0x5a,0xbb,0xf3,0xa4,0xef,0xb7,0xdd,0xa4,0xf2,0xbd,0x6d,0xaf,0xc0,
0xc0,0x76,0xab,0x70,0xa6,0xe2,0xb4,0xce,0xad,0xbc,0xb0,0x6c,0xb7,0x6f,0xb6,0xc0,0xa9,0xd9,0xc2,0xfb,
0xe4,0xea,0xb5,0x55,0xc3,0xd1,0xbd,0xec,0xa7,0xab,0xba,0x47,0xa9,0xf7,0xac,0x42,0xbd,0xcd,0xb1,0xc4,
0xbb,0xc0,0xb1,0x52,0xb2,0xcc,0xa5,0xdb,0xc1,0x5a,0xb6,0xa8,0xa5,0xbd,0xb1,0xb6,0xaf,0x56,0xb0,0x76,
0xb1,0x59,0xaa,0xda,0xb8,0xfb,0xc4,0xe2,0xbb,0xd9,0xa4,0xc4,0xb7,0x47,0xb2,0xeb,0xc0,0x57,0xad,0xbf,
0xaf,0xd3,0xab,0xec,0xb5,0xc2,0xb3,0x57,0xa6,0xf4,0xc2,0x58,0xbc,0x40,0xa9,0xd4,0xb8,0xc9,0xa9,0xc8,
0xb0,0xf6,0xb3,0xc7,0xbf,0xf2,0xbe,0xd1,0xc0,0x71,0xb1,0xe7,0xb6,0x4b,0xae,0x5f,0xb1,0x65,0xb6,0xb3,
0xae,0x4a,0xb6,0x62,0xb5,0x7d,0xb9,0xbc,0xa6,0xf6,0xb8,0xce,0xa6,0xfc,0xb0,0x5d,0xbf,0xfa,0xc2,0xc3,
0xb2,0xd5,0xc2,0xb4,0xad,0xa2,0xbd,0xe8,0xc2,0xa6,0xc3,0xdd,0xa5,0x4d,0xa9,0xf4,0xae,0x49,0xc1,0xf9,
0xb0,0x60,0xb7,0xf1,0xa4,0x42,0xaf,0xd5,0xa9,0xd3,0xb3,0x66,0xc3,0xd9,0xb5,0xfd,0xb7,0x64,0xac,0x7e,
0xa8,0xea,0xbb,0x5d,0xb0,0xd5,0xb7,0xd7,0xa5,0xab,0xad,0x4a,0xa5,0x6c,0xcf,0xe0,0xbd,0xde,0xb5,0xd8,
0xa4,0x5e,0xae,0xfc,0xa7,0xea,0xba,0xdb,0xc5,0x6e,0xa6,0xe7,0xbd,0xaa,0xad,0xb9,0xb8,0xa1,0xb3,0xdb,
0xbc,0xf5,0xac,0xdf,0xba,0x7d,0xbb,0xe1,0xbc,0xd2,0xac,0x72,0xc3,0x68,0xae,0xfb,0xb3,0x7a,0xbd,0x50,
0xb7,0xd0,0xbc,0xe5,0xc4,0xe9,0xa5,0xd0,0xac,0x49,0xb8,0xf2,0xa7,0xd2,0xbf,0x4e,0xa7,0xa5,0xa9,0x4f,
0xb6,0xfb,0xae,0xab,0xb7,0x6e,0xbc,0x59,0xc3,0x65,0xc2,0x5c,0xa4,0xdb,0xc4,0xa3,0xac,0xe8,0xc3,0xab,
0xb3,0xf9,0xc2,0x53,0xb9,0xeb,0xa4,0xab,0xb0,0x73,0xc0,0x5d,0xb6,0xf0,0xb0,0x7c,0xb8,0xc1,0xc3,0xc6,
0xb7,0xd2,0xbe,0x75,0xc5,0x5c,0xb6,0xeb,0xa6,0x42,0xb0,0xd2,0xc3,0x4d,0xc2,0x59,0xa9,0xf2,0xc3,0xb9,
0xc3,0xbe,0xad,0x7b,0xaa,0x69,0xb4,0xb5,0xa9,0xb8,0xbc,0x51,0xbc,0x75,0xbf,0xf7,0xb7,0xe0,0xb3,0x44,
0xc6,0x4e,0xb6,0xa7,0xc4,0x51,0xc5,0xef,0xb3,0xea,0xba,0xf1,0xa7,0xaf,0xa6,0xe4,0xbb,0x68,0xac,0x77,
0xb0,0x5c,0xb5,0x56,0xb5,0x52,0xa6,0x78,0xc2,0xce,0xa8,0xc8,0xc1,0xc9,0xaa,0xfc,0xa5,0xec,0xa7,0xba,
0xa6,0xcc,0xbc,0xbb,0xa7,0xc8,0xaa,0x4e,0xa5,0xa7,0xae,0x77,0xaf,0x49,0xba,0x7e,0xad,0xf0,0xa6,0xd0,
0xad,0xdc,0xa4,0x74,0xb1,0x64,0xa4,0xa5,0xa8,0x71,0xa5,0xc8,0xa9,0xbf,0xa4,0xda,0xae,0xe6,0xa7,0x4a,
0xa7,0x42,0xbe,0x7c,0xa6,0xab,0xad,0x7d,0xac,0xa5,0xb1,0xf6,0xa4,0x47,0xc1,0xa7,0xa9,0xa3,0xc4,0xcb,
0xa4,0xd5,0xa8,0xa9,0xb4,0xcb,0xae,0xa6,0xa5,0xb1,0xbc,0xda,0xab,0xb6,0xa5,0x64,0xa9,0xac,0xa6,0xb6,
0xbe,0x48,0xad,0xe2,0xb5,0xdc,0xac,0xf5,0xbe,0xa5,0xc4,0xf5,0xbb,0xab,0xb2,0xf6,0xb3,0xc1,0xb6,0xa9,
0xab,0x6e,0xa4,0xa6,0xaa,0xe2,0xb8,0xa6,0xa4,0xd7,0xa9,0x5a,0xb3,0xec,0xa9,0x69,0xa6,0xeb,0xbb,0xae,
0xa5,0xc5,0xc0,0x4e,0xb4,0xf5,0xb6,0xae,0xb8,0xca,0xa7,0xf5,0xbc,0x42,0xb0,0xc7,0xae,0xf5,0xa7,0x56,
0xad,0x43,0xa5,0xaf,0xa5,0xa9,0xc2,0xc4,0xb7,0xe6,0xa7,0xf9,0xaf,0xf7,0xbb,0xa8,0xbc,0xef,0xa8,0xb9,
0xac,0x5f,0xab,0x6b,0xaf,0xc1,0xc3,0xa3,0xb8,0xeb,0xbc,0xf0,0xbf,0x41,0xb5,0xe1,0xbf,0x70,0xc4,0xac,
0xb2,0xd0,0xbf,0x63,0xa4,0xc7,0xa6,0xf5,0xae,0xd0,0xb3,0xaf,0xb8,0xb3,0xad,0x53,0xb3,0xc5,0xc1,0xfa,
0xa9,0xf8,0xc9,0x72,0xc0,0x6a,0xb4,0x5e,0xaf,0xb3,0xa8,0x48,0xa7,0x64,0xbf,0xbd,0xb1,0x4a,0xb7,0xa8,
0xab,0xc0,0xb8,0xad,0xc3,0x51,0xbb,0xaf,0xbe,0x47,0xbf,0xf0,0xc4,0x75,0xb6,0xbe,0xb3,0xa2,0xae,0x70,
0xa5,0x43,0xf9,0xda,0xae,0x4c,0xe6,0xf9,0xab,0x61,0xab,0xb8,0xf1,0x4b,0xb4,0x50,0xa4,0xaf,0xad,0xd9,
0xae,0xfa,0xaa,0xf2,0xb9,0xd8,0xbb,0xf1,0xbb,0xc8,0xc4,0xaf,0xa5,0xb6,0xb7,0x4f,0xb2,0xdf,0xae,0x7d,
0xad,0x44,0xa9,0xa8,0xae,0xe3,0xa8,0xaa,0xa4,0xec,0xac,0xee,0xc3,0xc3,0xa6,0xe0,0xbe,0xf4,0xc0,0xd8,
0xd4,0xb3,0xbd,0x5f,0xb6,0xc2,0xa9,0xa5,0xb1,0x54,0xac,0x66,0xb5,0xe2,0xa7,0xf8,0xa4,0x49,0xa5,0xc9,
0xba,0xb5,0xa4,0x59,0xae,0x63,0xc3,0x75,0xa8,0xa6,0xa8,0x52,0xa6,0xcb,0xc2,0x4e,0xa4,0xe1,0xdf,0x4e,
0xae,0x71,0xa7,0xfc,0xf2,0x5d,0xa5,0x71,0xad,0xe0,0xb4,0xe7,0xac,0x68,0xb4,0xf6,0xad,0xf5,0xb1,0xde,
0xc1,0x46,0xb2,0x45,0xab,0xdb,0xc1,0xbe,0xb5,0x61,0xa8,0xe5,0xa7,0xb5,0xad,0x5a,0xa4,0xbb,0xad,0x73,
0xab,0x54,0xc9,0xdc,0xb4,0xef,0xa5,0xda,0xac,0x76,0xa5,0x6b,0xa4,0x7d,0xa7,0xe8,0xaf,0x5a,0xa6,0xf9,
0xb5,0x5e,0xa5,0xd3,0xae,0xe1,0xa8,0x76,0xc2,0x48,0xbb,0xc9,0xa7,0xa2,0xba,0xb4,0xb8,0xec,0xb0,0xb0,
0xb6,0x5a,0xae,0x67,0xbd,0x62,0xad,0xe1,0xb4,0xb0,0xac,0x7a,0xc9,0x6f,0xaf,0xc3,0xa7,0xca,0xab,0x77,
0xa5,0x5f,0xab,0xd5,0xa6,0x7b,0xa8,0xb6,0xbe,0x49,0xa8,0x57,0xcd,0xd5,0xe8,0x58,0xaf,0xf0,0xb3,0xae,
0xb9,0x73,0xac,0x75,0xc3,0x73,0xa5,0x50,0xb7,0x45,0xb7,0xec,0xa8,0xa7,0xf4,0xbf,0xb9,0x61,0xad,0xb6,
0xb4,0xdc,0xa6,0xbd,0xb6,0x6b,0xa4,0x4c,0xa9,0x73,0xb4,0xb4,0xa6,0xf7,0xb3,0xac,0xc0,0x78,0xb1,0xb1,
0xad,0xfb,0xaa,0xac,0xa1,0x42,0xb5,0x7c,0xb7,0x6d,0xa7,0xce,0xbb,0xaa,0xc4,0x79,0xb9,0xab,0xb8,0xef,
0xbe,0x6e,0xbc,0xd3,0xb6,0x53,0xb6,0xb1,0xa6,0xfe,0xc1,0xca,0xb6,0x52,0xb0,0xa6,0xa9,0xa4,0xba,0x4d,
0xa7,0xa4,0xa8,0x73,0xb0,0xec,0xb9,0xfc,0xc1,0x61,0xb2,0x50,0xab,0xea,0xb5,0x65,0xa5,0xa1,0xb2,0xa6,
0xbd,0x58,0xa9,0xee,0xbd,0xe6,0xae,0xd8,0xbe,0x50,0xb0,0xe2,0xa4,0xeb,0xa4,0xd8,0xa5,0xaa,0xbe,0x45,
0xc0,0xf0,0xae,0x7e,0xb6,0xd2,0xae,0xca,0xb3,0xc2,0xde,0xcd,0xbf,0x6a,0xac,0x4d,0xb0,0xb7,0xab,0xac,
0xb0,0xa7,0xb1,0xd2,0xa4,0xb9,0xbb,0xda,0xc3,0x7b,0xad,0xe7,0xb5,0xa1,0xba,0x75,0xbf,0xfd,0xba,0x49,
0xbc,0xfa,0xc0,0x79,0xae,0x7b,0xc1,0xae,0xbd,0xb8,0xc2,0xb6,0xc5,0xa6,0xc5,0xcd,0xa9,0xc6,0xbd,0xc0,
0xb0,0x77,0xbe,0x57,0xbe,0xc0,0xc4,0xb2,0xc3,0xef,0xbb,0xb6,0xae,0xe2,0xc2,0x4f,0xc1,0xd7,0xa7,0x6b,
0xa9,0xf5,0xc1,0x71,0xbc,0xe9,0xa9,0x65,0xbe,0xd0,0xb3,0xbc,0xc4,0xc6,0xaf,0x42,0xb5,0xd3,0xb8,0xb8,
0xac,0x5c,0xc3,0x77,0xab,0x42,0xbc,0x53,0xc0,0x7a,0xc4,0xdf,0xb0,0xbc,0xb2,0xa8,0xad,0xdd,0xad,0xc1,
0xbc,0x7d,0xc2,0x79,0xba,0xd1,0xb1,0x49,0xb9,0xe6,0xb6,0xd4,0xbc,0xed,0xc1,0x59,0xa9,0xb2,0xbb,0xc5,
0xc0,0xa3,0xb6,0x5d,0xb6,0x65,0xae,0xb7,0xb3,0xb3,0xbf,0xe8,0xca,0x49,0xa7,0x7d,0xc0,0xc9,0xb0,0xc9,
0xc3,0xfa,0xb5,0xb8,0xbf,0xeb,0xbb,0x7e,0xb4,0x79,0xad,0x7a,0xb2,0xc5,0xa6,0xf8,0xca,0x5e,0xa4,0xb6,
0xae,0xe0,0xcf,0xa1,0xb8,0xd4,0xa6,0xea,0xb8,0x56,0xb9,0x75,0xb4,0xd2,0xb4,0xfa,0xc3,0xc0,0xa7,0xfd,
0xae,0xd6,0xb0,0xbb,0xb9,0xee,0xb1,0x55,0xb0,0xc3,0xbd,0xf8,0xac,0x56,0xbd,0x45,0xac,0xcc,0xc3,0x7a,
0xac,0xb5,0xb6,0xf2,0xc3,0xb4,0xb4,0xd3,0xb4,0xf2,0xaa,0x79,0xb2,0xca,0xb8,0x54,0xda,0xcc,0xa8,0xd6,
0xc5,0x52,0xc6,0x45,0xb2,0xcf,0xc4,0xfd,0xa6,0x45,0xae,0xbb,0xc1,0xe8,0xbe,0xd5,0xb7,0x69,0xaa,0x52,
0xb3,0x75,0xac,0xb6,0xc9,0x44,0xc9,0xb2,0xc1,0xc8,0xba,0xc6,0xd3,0xec,0xb1,0x70,0xb8,0x79,0xb5,0xe6,
0xc5,0x75,0xb3,0x4a,0xbc,0xd1,0xc2,0xc5,0xb3,0x76,0xba,0xa5,0xbd,0xc6,0xb6,0xc4,0xb5,0xb5,0xd5,0x75,
0xbc,0x6d,0xb6,0xa5,0xb0,0xe9,0xab,0xf5,0xb1,0xc3,0xa6,0xf1,0xc0,0x64,0xbe,0x6d,0xc4,0xe6,0xa8,0xf8,
0xab,0x50,0xae,0xc1,0xba,0xe2,0xaf,0x68,0xad,0xc2,0xb0,0xb2,0xba,0xce,0xb9,0xbd,0xba,0xae,0xaf,0xce,
0xc5,0xf3,0xac,0x70,0xb9,0x6a};
/*	"¤C¤j¤ý´Â¦w¸Ëµ{¦¡µ²§ô§@·~°õ¦æ²¾°£ºô¯¸·sÂI¨ú®ø§R©è§Ü«×º¡ÃB¼È°±¹CÀ¸Ä~Äò­P©RªZ¾¹©Ü­·"
"©M¤P­º¿ï³æ¬O¦W¤H°ó¬D°_¬ðµo¨Æ¥óÅÑ§Þ³N³t¾ð¾Ô§Ð³õ´º¤£¯à²K¥[¤w¥X²{¹Lªº­^¶¯³nÅé³\¥iÃÒ"
"¨óÄ³½Ð¾\Åª¤U­±¡C«öÁä¨ä¥¦³¡¤À±z§_±µ¨ü«e¤¤©Ò¦³±ø´Ú¡H¦pªG¾Ü¡§¡¨¡A±N­n¥²¶·¦¹§¹¥þ»Ýµw"
"ºÐ¶¡§A³Ñ¾lªÅ¬°¥ü¥t¤@¥x¸g¦b¹q¸£¤W¤F¨ÏªÌ±a¨Ó³Ì°ª¤¸»¼¥æ§ë­°®ÑÅ³¤_§Ú°ê¤p·í¹D·ù²³«q¿Ë"
"Â÷±Í¯S²v´Ý±ý¦V¶Q¸Û¥H¤M§L¤§¨a¬@±Ï¦Ê©m©ó¤ô¤õ¦s¤`Ãö«Y¤Ñ¦M¦U½Ñ«J¶Õ¤O°t§ó¨ìª§ÅQ­«±æ¤T"
"«ä°È¦¬±`»D§g¥D©ú»\¥@¬}±x¥»¨}­W¥Î¤ß¦­¨M©w©¾¹ê¹²¦n§a¬JµM»¡±o³o¼Ë¼¦¤¼­Ì´N­Ó¡I·|µ¹¨Ç"
"Ãø°Ù°©ÀY¥ô¦ÒÅç·N®É­Ô§O¥s³s°Ú©A¤~°ßº×®Ú³ºÁÙÁy°f¦º¶d¨Äµ¥µÛ¦¨­x°­«H¨¾¦Ú°¡¡J¶¤¯Ç¤J½s"
"¨î¹Ü¨t¦Ó¸Ó§åÅ]±Ú¤h´£°«»P¦h¦~«è¬Æ±þ¤÷³à¤l¦@À¹¤³¶TµA´²¥Á§^Áoµ´³»¸Ñ´å¤bÄ@¨õÂ¾¤D¼{¥£"
"®Ó±ÂÅv¦¼³B²z©y«µ¿Å¤éÄ^²`¥ß¥\Å«¸o¥çÄYºÞ±Ð¦A§Ôµh³Î·R±ó©Û®£·S«ã¯«­I·lÀs«Â¬Ò¼ö¦å¨k¨à"
"¬G¦P¼Ä·Zºë³ø­Õ®e²§©ÎÂZ¶Ã¸·¶¶À³²ÎÁ|¥Ø§Y¤S¦ó§U°}Áö¸¹ºÙ®v©|µL­Y­ÉªF¤è¤´«á¸¨¸Ü¬`¥|¦Ü"
"¸ô¸s§ð¤Þ¯T«Ç¤Q¸U«æ¤à¬Û±¤¨Ì­p«KÁÁ¨¥¬o°Z§v©ñ«Ò¦èÆZ¦i¥ý§â¬Ý²M·¡·QÅo¶ÛÃC±A½¥·¥ºÉ½Ô´A"
"ºAº¸Áu©~ÀI´c¥O¹Ã¯Q¦X¹ð±Ñ¯¥¦Ç°Óºe§E¦a¯u¤z¬Ù¥÷¨Ãª©¹ÏÃã¨¯³Ò¼o­Ñ¿³¬F³qªO¨Ñ¥é®Ä¬£»º±j"
"Åã´­³Óµª´_®¤©^³­¥~¨­±w¨u¨£¥Ö½§¯f¨DÂå§¡§iª¾¿n²ÖªvÀøÅý³yºÖÁÂ´©¤â¥B¤o¤gª÷®¢·\¹ï¦C¯ª"
"©vÃþ©ÞµÑ¯d¯Å¶ê¹ÚÂ_®a³±¿Ñ¿y¦ì·I½l¬ù¶X¥¼Ã­»}°Q¥ï¥­§K¥ÍÆF¶î¬´¸é¸Ý¥Gµ½¶ü¼W¼Ö¤ñ·L¨¬¶}"
"©¼¸{¦Õ¦ØÄ_¼C®¶ÀTÅD¸Õ¨~¨ëÀR¦Û¤Z´í´±Äm»ò¶ô¯ó³£¶Ì¥Ê½k§Ë¶ÜÄê¨É¸ê·½ªù¨S¤]«È®ð§ïÅÜ»î»´"
"¤k©Ê±¡µ£ÁõÄq§|¤u¼t¸Ì·F¬¡¤°°e°¨Á{«°¸q¦]³ô¥L¼É²×´¦¬ñ«Å¥¬°í¥¿ªí¥Ü¾×¤Ï¼³¼y¯¬µf«Ý¦^Âk"
"­M·³­è±K±´¥´Å¥¿W¥Ñ¨ºªp«D²V¥u®§¤Áª`µø¤ÆÀH¤v«Ø¥R¾d©T¦ý´¿©l«O«ùªÖ¬ì¾ÇÂÔ·V¥¹®iÁc´Þ¤f"
"¶×Á`¶VÃä¬É«I¦û°Ï»âÄÝ§QÀu¼ç¥ñ¿ÒÅå°Ê¾ú¤E¹®­Æ°k¾÷¼ôÀ»±¹¤Îµy³Ý¾D³Ð§½«Ü®Ô¤Í¨IÃa¬P¶H©x"
"¬Q©]½÷¦N·Ó¼q¤º¹¬«Q¶Ç¶m¤Ö¹JÂù¤K»L¤dÃ~²±²»·çºØ¸ñ«C·ý¹|¾ã¦¿ªe¨Î¥¢­¦Án©³§Ö¥h·Ç³Æ¤µ±F"
"±È®öºq»R¤É°Û¬Ó³C¬±¼w¾Ö»a²´°J¶P¥Ã»·ÅXµ¦§j®µ»qÁô¿@¯PÅK¿á½î½ñ¤ù­ìÆ`©}ªA½âÂßªk®¾«o¹w"
"®Æ¨göt¥v¨®½ü¯ëªý·ÅÂêÃìºò©_À¶¤Ò©ù§C««®M¤s¥]¬AÂ²ª½­ÈÀ~ªþ·WÁ«¥Õ²s¸v­hÁx¶qÃê¹ú°¶¤Ó²¤"
"·t©¯¤ä«i«¥«¢§Ý¤û¨Ø¤­¹Ä§ä¯È¨è¯á©D§u¿ô§ÑÂÐªÑ°R·´ªø©¹ªÓ¥S§Ì¯E¿º¥úÀôÅ¢¸n½ú«Í¾î¹M³¥¥N"
"»ù¨ãµõ³S¾ÉºX¹ª±²ºÃ­µÀ°ªñ«ü´§¦¸½ç¶ËÀë§Äª¯¯º¹³ÃÇ©b¨«Âà§éÁp¾Ó«å§Æ°÷¶i©`¹è²_³´¿äÆn½Ã"
"¬yºwÂAÄa®í½u°{Å§³QÁßÆC¤[»E¶°¶W®t±O¥Ç¾v¾á­t³d¸m¼ì¿m©S¾Ì¸ü¾_¤Ì¹B®`¯q¿P³¶²jÂEÃ[§Ó«ç"
"¦kµû½×ºî¤æÀµ§§¹Å©º·vÅU§®¼¾²½«ô°Ý»Ê°O»»®ç°Ñ¼®¼½¥õ¸`°^ª««~¼Æ¨â¼û´¼®z¬Y´Á±i©I¹ÐÄÛ»{"
"²æ´«¤¿ªöÂ§¨ç¿í±q¿ì«h½a»÷Ä[³h§x¼sÂ×±¾¾¦«Ê·q·ö¹p¦À¥b¨B¦Ñªì­TÃfªï®È¥Ò¶~¤¡¨³±»·ÀµK·M"
"­VÃg¤N¯î¹A½}¾i©Å«ÓÄà²]Äµ¼F¯°ª×Á~¹Á¶µ¦×¤r½Öº]ªYºaºÍ°I¾^¶v¬KÀ\¶¼¶[¯A³~¼R°×¶Ú´À¬xÄÁ"
"¼z¼~©À»W·PÁIªL¦t©z¶ø¹y®©³Õ¾ÄÃkµn¹Ò¦´²@¾Ú´X¾U¥I¯b¸ßºB´nÄøÃÄ¤úÀÙ·hÂ³½ý½ÕªêÄU¶Ï¥ÙÅB"
"·UºtÀò§t§ñ»íºN³½©ö¯}¥ð¬½¿×¯ä¬L¤©¸B°µ³ßÅw©Ú­Íµu½w°½Ãi¼x¹SªÎ¨U½M¿ióO¹ýÃP±§¼Ð®¼®³¦Y"
"§ì¨ßØæ¥j«ë²\¼ä¥Ä¨|­_ªá¦·ÅÖ²ÓªÏ±½µÄ·Æ¿°¯×ªÕÀ¿«Gù×´³ºj©d¦o©tªE¯v¸Öµ§¼r¹F¶È¿v¦z­Ë­×"
"ßÂ»°¶é¦íºû¨Ê¥ëÄÌ¾`»XµÃ¾Â¿ñÁï´IºR¦L°g¨¸±¼Âí´MµS±ìÄA¶O¨òÁlÅA§Ù¥î­­´x´¤¯µ¾x®y¤¬½T¥Ë"
"´¡Á¡²r³]¹º¬q¾¨±ÀÂ½»G¹ñ¯Ê¥F«d´îµô°h§á°®©[ÅS½d³ò´ý®À­þ¨ý«÷©P±Û¤å¥¨Ð`µ®¦}²bÉóÉO¾F¦u"
"ªQ¥e¨½Ì°­ú¶D´d¼hº»­@¤½¤ö°d¶^ÀÆ¡TÓã¬ÞµP¾bÎ`³ÍÖÃ³·»ô¾a¨C±M¬U®ºÀ´­ã¦Z®½±{¹ÎÃûÉE¿Õ±s"
"Äè¾Þ©ÇÁ@¿ùÄ¹§l°V¤\º@·§ªBÒ\¹_¨¤½Ì¨÷¬Ã¨ô§÷ª±Ãª¸}Á}ªM±Ù±_¥Þ½L½õÀ¦°òÌå¯ÙÅ@³¾­¸¤ãÀç¿é"
"Éa»þ±ß¬ü´þ±m¬ã»sÏú«R´ù¯íÃD²Â³U¿U²Ä§À³èÂÝ¾AÄë¬¹Àü§b¹x¥ø²£¼vÅT§]¶ÆºC·¾²îÅº®x»y¡R²y"
"®]Ã±­q²oªð¿Ý©ß´¶¯D®ÇÆ[¼«³Ã¡S¥á¿à½à­Û§TªÙ­G¶ñ±Æ¶s¬d¦r¬W½|¤î±°½C¿¾­e½Ä¯É¦l¸Þ¨F·È¥Ì"
"¨c®®µÎºZ»ó¤ï·Ý¤ò½m¯ÀÀv«p¦â´Î­¼°l·o¶À©ÙÂûäêµUÃÑ½ì§«ºG©÷¬B½Í±Ä»À±R²Ì¥ÛÁZ¶¨¥½±¶¯V°v"
"±YªÚ¸ûÄâ»Ù¤Ä·G²ëÀW­¿¯Ó«ìµÂ³W¦ôÂX¼@©Ô¸É©È°ö³Ç¿ò¾ÑÀq±ç¶K®_±e¶³®J¶bµ}¹¼¦ö¸Î¦ü°]¿úÂÃ"
"²ÕÂ´­¢½èÂ¦ÃÝ¥M©ô®IÁù°`·ñ¤B¯Õ©Ó³fÃÙµý·d¬~¨ê»]°Õ·×¥«­J¥lÏà½ÞµØ¤^®ü§êºÛÅn¦ç½ª­¹¸¡³Û"
"¼õ¬ßº}»á¼Ò¬rÃh®û³z½P·Ð¼åÄé¥Ð¬I¸ò§Ò¿N§¥©O¶û®«·n¼YÃeÂ\¤ÛÄ£¬èÃ«³ùÂS¹ë¤«°sÀ]¶ð°|¸ÁÃÆ"
"·Ò¾uÅ\¶ë¦B°ÒÃMÂY©òÃ¹Ã¾­{ªi´µ©¸¼Q¼u¿÷·à³DÆN¶§ÄQÅï³êºñ§¯¦ä»h¬w°\µVµR¦xÂÎ¨ÈÁÉªü¥ì§º"
"¦Ì¼»§ÈªN¥§®w¯Iº~­ð¦Ð­Ü¤t±d¤¥¨q¥È©¿¤Ú®æ§J§B¾|¦«­}¬¥±ö¤GÁ§©£ÄË¤Õ¨©´Ë®¦¥±¼Ú«¶¥d©¬¦¶"
"¾H­âµÜ¬õ¾¥Äõ»«²ö³Á¶©«n¤¦ªâ¸¦¤×©Z³ì©i¦ë»®¥ÅÀN´õ¶®¸Ê§õ¼B°Ç®õ§V­C¥¯¥©ÂÄ·æ§ù¯÷»¨¼ï¨¹"
"¬_«k¯ÁÃ£¸ë¼ð¿Aµá¿pÄ¬²Ð¿c¤Ç¦õ®Ð³¯¸³­S³ÅÁú©øÉrÀj´^¯³¨H§d¿½±J·¨«À¸­ÃQ»¯¾G¿ðÄu¶¾³¢®p"
"¥CùÚ®Læù«a«¸ñK´P¤¯­Ù®úªò¹Ø»ñ»ÈÄ¯¥¶·O²ß®}­D©¨®ã¨ª¤ì¬îÃÃ¦à¾ôÀØÔ³½_¶Â©¥±T¬fµâ§ø¤I¥É"
"ºµ¤Y®cÃu¨¦¨R¦ËÂN¤áßN®q§üò]¥q­à´ç¬h´ö­õ±ÞÁF²E«ÛÁ¾µa¨å§µ­Z¤»­s«TÉÜ´ï¥Ú¬v¥k¤}§è¯Z¦ù"
"µ^¥Ó®á¨vÂH»É§¢º´¸ì°°¶Z®g½b­á´°¬zÉo¯Ã§Ê«w¥_«Õ¦{¨¶¾I¨WÍÕèX¯ð³®¹s¬uÃs¥P·E·ì¨§ô¿¹a­¶"
"´Ü¦½¶k¤L©s´´¦÷³¬Àx±±­ûª¬¡Bµ|·m§Î»ªÄy¹«¸ï¾n¼Ó¶S¶±¦þÁÊ¶R°¦©¤ºM§¤¨s°ì¹üÁa²P«êµe¥¡²¦"
"½X©î½æ®Ø¾P°â¤ë¤Ø¥ª¾EÀð®~¶Ò®Ê³ÂÞÍ¿j¬M°·«¬°§±Ò¤¹»ÚÃ{­çµ¡ºu¿ýºI¼úÀy®{Á®½¸Â¶Å¦ÅÍ©Æ½À"
"°w¾W¾ÀÄ²Ãï»¶®âÂOÁ×§k©õÁq¼é©e¾Ð³¼ÄÆ¯BµÓ¸¸¬\Ãw«B¼SÀzÄß°¼²¨­Ý­Á¼}ÂyºÑ±I¹æ¶Ô¼íÁY©²»Å"
"À£¶]¶e®·³³¿èÊI§}ÀÉ°ÉÃúµ¸¿ë»~´y­z²Å¦øÊ^¤¶®àÏ¡¸Ô¦ê¸V¹u´Ò´úÃÀ§ý®Ö°»¹î±U°Ã½ø¬V½E¬ÌÃz"
"¬µ¶òÃ´´Ó´òªy²Ê¸TÚÌ¨ÖÅRÆE²ÏÄý¦E®»Áè¾Õ·iªR³u¬¶ÉDÉ²ÁÈºÆÓì±p¸yµæÅu³J¼ÑÂÅ³vº¥½Æ¶ÄµµÕu"
"¼m¶¥°é«õ±Ã¦ñÀd¾mÄæ¨ø«P®Áºâ¯h­Â°²ºÎ¹½º®¯ÎÅó¬p¹j";*/

#pragma warning(pop)

int GetChineseFontOffset(char* pcChar,UINT uiWidth)
{
#ifdef DEBUG
	OutputDebugString(pcChar);
	OutputDebugString("  ");
#endif

	for(int i=0;i<sizeof(g_bTraditionalChineseFontMap)/
		sizeof(g_bTraditionalChineseFontMap[0]);i+=2)
	{
		if(g_bTraditionalChineseFontMap[i]==pcChar[1]&&
			g_bTraditionalChineseFontMap[i+1]==pcChar[0])
			break;
	}

#ifdef DEBUG
	char s[20];
	sprintf(s,"%d  %d\n",i,sizeof(g_bTraditionalChineseFontMap)/
		sizeof(g_bTraditionalChineseFontMap[0]));
	OutputDebugString(s);
#endif

	if(i==sizeof(g_bTraditionalChineseFontMap)/
		sizeof(g_bTraditionalChineseFontMap[0]))
		return 0;
	int FontOffset=i/2;//(*(unsigned char*)(pcChar+1)-161)*94+*(unsigned char*)pcChar-161;
	int FontBytes=(uiWidth+7);
	FontBytes=uiWidth*(FontBytes/8);
	FontOffset*=FontBytes;
	if(uiWidth==13)
	{
		FontOffset-=9688;
		if(FontOffset<0)
			FontOffset+=-9688+19984-520;
	}
	FontOffset+=-1;
	return FontOffset;
}
#endif

BOOL LoadChineseFont(void)
{
	if(!g_bIsChineseFontLoaded)
	{
		{
			HRSRC hrsrc=FindResource(NULL,"IDR_FNT13","FNT");
			if(hrsrc==NULL)
				ExitProcess(0);
			HGLOBAL hg=LoadResource(NULL,hrsrc);
			if(hg==NULL)
				ExitProcess(0);
			g_pChineseFont13=(TChineseFont*)LockResource(hg);
			if(g_pChineseFont13 == NULL)
				ExitProcess(0);
		}
		{
			HRSRC hrsrc=FindResource(NULL,"IDR_FNT16","FNT");
			if(hrsrc==NULL)
				ExitProcess(0);
			HGLOBAL hg=LoadResource(NULL,hrsrc);
			if(hg==NULL)
				ExitProcess(0);
			g_pChineseFont16=(TChineseFont*)LockResource(hg);
			if(g_pChineseFont16 == NULL)
				ExitProcess(0);
		}
		{
			HRSRC hrsrc=FindResource(NULL,"IDR_FNT24","FNT");
			if(hrsrc==NULL)
				ExitProcess(0);
			HGLOBAL hg=LoadResource(NULL,hrsrc);
			if(hg==NULL)
				ExitProcess(0);
			g_pChineseFont24=(TChineseFont*)LockResource(hg);
			if(g_pChineseFont24 == NULL)
				ExitProcess(0);
		}
		g_bIsChineseFontLoaded=TRUE;
	}
	return TRUE;
}

TChineseFont* GetChineseFont(Font* f)
{
	if(f->max_font_height<16)
		return g_pChineseFont13;
	if(f->max_font_height<24)
		return g_pChineseFont16;
	return g_pChineseFont24;
}
char* StrReplace(char* String,const char* ReplaceWith,const char* ReplaceTo)
{
	static char VeryLongBuffer[10240];
	UINT uiStringLength=strlen(String);
	UINT uiOffset=0;
	UINT uiReplaceWithLength=strlen(ReplaceWith);
	UINT uiReplaceToLength=strlen(ReplaceTo);
	for(UINT i=0;i<uiStringLength;i++)
	{
		if(strncmp(String+i,ReplaceWith,uiReplaceWithLength)==0)
		{
			strcpy(VeryLongBuffer+uiOffset,ReplaceTo);
			uiOffset+=uiReplaceToLength;
			i+=uiReplaceWithLength-1;
		}
		else
			VeryLongBuffer[uiOffset++]=String[i];
	}
	VeryLongBuffer[uiOffset]=0;
	strcpy(String,VeryLongBuffer);
	return String;
}
//SXM

//--------------------------------------------------------//
// A font resource file is composed of three parts :
//
// one FontHeader;
// one FontInfo for each character;
// Bitmap data.
//--------------------------------------------------------//

//-------- Define struct FontHeader -------//

struct FontHeader
{
	unsigned	short	max_width;
	unsigned short max_height;
   unsigned short std_height;
	unsigned short first_char;		// ascii code of the first character
	unsigned	short last_char;		// ascii code of the last character
};

//-------- Define struct FontInfo -------//

struct FontInfo	// info for each character
{
	char				offset_y;
	unsigned char	width;
	unsigned char	height;
	long				bitmap_offset;	// file offset relative to bitmap data
};

//--------- Define macro constant ------------//

#define HYPER_FIELD_COLOR 	V_LIGHT_BLUE
#define TEXT_END_CHAR		26					// Ctrl-Z

//------- Define static member variable --------//

short      Font::hyper_field_count;
HyperField Font::hyper_field_array[MAX_HYPER_FIELD];

static int text_line_count;     // used by text_width() & text_height() only

//----------- Begin of function Font Constructor -------//

Font::Font(char* fontName)
{
	memset( this, 0, sizeof(Font) );

	if( fontName )
		init(fontName);
}
//------------- End of function Font Constructor -------//


//----------- Begin of function Font Destructor -------//

Font::~Font()
{
	deinit();
}
//------------- End of function Font Destructor -------//


//-------- Begin of function Font::init -----------//
//
// <char*> fontName     = name of the font type.
//                        (e.g. "STD" will load "FNT_STD.RES" )
//
// [int] interCharSpace = inter-character space
//                        (default:1)
//
// [int] italicShift    = horzontal back shifting for italic font
//                        (default:0)
//
// Return : 1 - succeed;
//          0 - reading palette file error
//
void Font::init(char* fontName, int interCharSpace, int italicShift)
{
	strcpy(FontName,fontName);
	if( init_flag )
		deinit();
	//SXM
	LoadChineseFont();
	//SXM
	inter_char_space = interCharSpace;

	//---------- open the font file -----------//

	File fontFile;

	String str;

	str  = DIR_RES;
	str += "FNT_";
	str += fontName;
	str += ".RES";

	fontFile.file_open(str);

	//-------- read in the font header ---------//

	FontHeader fontHeader;

	fontFile.file_read( &fontHeader, sizeof(FontHeader) );

	max_font_width  = fontHeader.max_width;
	max_font_height = fontHeader.max_height;
	std_font_height = fontHeader.std_height;

	font_height		 = std_font_height;		// its default is std_font_height, but can be set to max_font_height 

	first_char  	 = fontHeader.first_char;
	last_char		 = fontHeader.last_char;

	//----------- read in font info ------------//

	int infoArraySize = sizeof(FontInfo) * (last_char-first_char+1);

	font_info_array = (FontInfo*) mem_add( infoArraySize );

	fontFile.file_read( font_info_array, infoArraySize );

	//------- process italic shift --------//

	if( italicShift )
	{
		for( int i=0 ; i<last_char-first_char+1 ; i++ )
			font_info_array[i].width -= italicShift;
	}

	//---------- read in bitmap data ----------//

	int bitmapBufSize = fontFile.file_size() - sizeof(FontHeader) - infoArraySize;

	font_bitmap_buf = mem_add( bitmapBufSize);

	fontFile.file_read( font_bitmap_buf, bitmapBufSize );

	//---- get the width of the space character ----//
	//
	// since in some font, the space char is too narrow,
	// we use the width of 't' as space instead
	//
	//----------------------------------------------//

	space_width = font_info_array['t'-first_char].width;

	//----------------------------------------------//

	fontFile.file_close();

	init_flag = 1;
	//Add by SXM to get the color of font
	FontInfo* fontInfo=font_info_array+'A'-first_char;
	DWORD bc=0xffffff,fc=0;
	unsigned char code[256];
	memset(code,0,256);
	for(int i=0;i<200;i++)
	{
		unsigned char* p=(unsigned char*)(font_bitmap_buf+fontInfo->bitmap_offset+i);
		code[*p]++;
//		RGBColor r,r1;
//		vga.decode_pixel(vga.translate_color(*p),&r);
//		char s[1024];
//		sprintf(s,"%u:  %u  %u  %u\n",*p,r.R,r.G,r.B);
//		Dump(s);
/*		vga.decode_pixel(vga.translate_color(m_ChineseBackgroundColor),&r1);
		if(r.R+r.G+r.B<r1.R+r1.G+r1.B)
			m_ChineseBackgroundColor=*p;
		vga.decode_pixel(vga.translate_color(m_ChineseBackgroundColor),&r1);
		if(r.R+r.G+r.B>r1.R+r1.G+r1.B)
			m_ChineseForegroundColor=*p;*/
/*		if(*p>m_ChineseForegroundColor)
			m_ChineseForegroundColor=*p;
		if(*p<m_ChineseBackgroundColor)
			m_ChineseBackgroundColor=*p;*/
	}
	bc=0;fc=0;

	// ##### begin Gilbert 11/4 #######//
//	for(i=0;i<256;i++)
//	{
//		unsigned char p=code[i],p1=bc,p2=fc;
//		bc=MAX(p,p1,p2);
//		fc=p1+p2+p-MAX(p,p1,p2)-MIN(p,p1,p2);
//	}
	bc = 255;		// always 255 for transparent
	// choose foreground color as the most frequent color
	for(i=0;i<256;i++)
	{
		if( i != 255 && code[i] > code[fc] )
			fc = i;
	}
	// ##### end Gilbert 11/4 #######//

	m_ChineseBackgroundColor=bc;
	m_ChineseForegroundColor=fc;
	m_ChineseShadowX=1;
	m_ChineseShadowY=1;

	m_ChineseShadowColor=0x10;
	m_ChineseBackgroundColor=255;
	if(!strcmp(FontName,"SMAL"))
	{
		m_ChineseShadowColor=154;
	}
	if(!strcmp(FontName,"CMPH"))
	{
		m_ChineseForegroundColor=0;
		m_ChineseShadowColor=255;
		m_ChineseShadowY=0;
		m_ChineseShadowX=0;
	}
	if(!strcmp(FontName,"NEWS"))
	{
		m_ChineseForegroundColor=16;
		m_ChineseShadowColor=16;
		m_ChineseShadowY=1;
		m_ChineseShadowX=0;
	}
	if(!strcmp(FontName,"SCRE"))
	{
		m_ChineseShadowColor=0;
		m_ChineseShadowY=0;
	}
	if(!strcmp(FontName,"VILL"))
	{
		m_ChineseShadowColor=183;
	}
	if(!strcmp(FontName,"SNDS"))
	{
		m_ChineseShadowColor=154;
	}
	if(!strcmp(FontName,"TBLK"))
	{
		m_ChineseForegroundColor=0x1;
		m_ChineseShadowColor=122;
	}
	if(!strcmp(FontName,"BRED"))
	{
		m_ChineseForegroundColor=0xa1;
	}
	if(!strcmp(FontName,"CMPO"))
	{
		m_ChineseForegroundColor=0x10;
	}
	if(!strcmp(FontName,"MID"))
	{
		m_ChineseForegroundColor=0x10;
	}
	if(!strcmp(FontName,"HALL"))
	{
		m_ChineseShadowColor=151;
	}
	if(!strcmp(FontName,"BBLK"))
	{
		m_ChineseShadowColor=154;
	}
	if(!strcmp(FontName,"CARA"))
	{
		m_ChineseShadowColor=154;
	}
	if(!strcmp(FontName,"BLD"))
	{
		m_ChineseShadowColor=154;
	}
/*	if(!strcmp(FontName,"CMPH"))
	{
		m_ChineseShadowColor=154;
	}*/
	if(!strcmp(FontName,"CMPA"))
	{
		m_ChineseShadowColor=154;
	}
	if(!strcmp(FontName,"VILB"))
	{
		m_ChineseShadowColor=183;
	}
	if(!strcmp(FontName,"MID"))
	{
		m_ChineseShadowColor=150;
	}
	if(!strcmp(FontName,"WHBL"))
	{
		m_ChineseForegroundColor=0x10;
		m_ChineseShadowColor=0xff;
	}
	if(!strcmp(FontName,"BRED"))
	{
		m_ChineseShadowColor=141;
	}
	//Add by SXM to get font info
/*	Dump("Font Name: ");
	Dump(fontName);
	Dump("\nHeight: ");
	Dump(font_height);
	Dump("\nStdHeight: ");
	Dump(std_font_height);
	Dump("\nMaxHeight: ");
	Dump(max_font_height);
	Dump("\nMaxWidth: ");
	Dump(max_font_width);
	Dump("\nSpaceWidth: ");
	Dump(space_width);
	Dump("\nInterSpace: ");
	Dump(inter_char_space);
	Dump("\nFirstChar: ");
	Dump(first_char);
	Dump("\nLastChar: ");
	Dump(last_char);
	Dump("\n");
	Dump("Start Of Font Data\n");*/
	fontInfo = font_info_array+'0'-first_char;
	char* p=font_bitmap_buf + fontInfo->bitmap_offset;
/*	DumpBin(p,p[0]*p[2]+2);
	Dump("---------------------------------------\n");*/
}
//---------- End of function Font::init ----------//


//----------- Start of function Font::deinit ---------//
//
void Font::deinit()
{
	if( font_info_array )
	{
		mem_del( font_info_array );
		font_info_array = NULL;
	}

	if( font_bitmap_buf )
	{
		mem_del( font_bitmap_buf );
		font_bitmap_buf = NULL;
	}

	init_flag = 0;
}
//------------- End of function Font::deinit ---------//


//--------- Start of function Font::put ---------//
//
// write text with pre-inited fonts
//
// int x,y         = location of the font
// char* text      = the text to be put on screen
// [int] clearBack = clear background with back_color or not
//                   (default : 0)
// [int]  x2       = display font up to the right border x2 and also
//                   clear the area between the last character and right border
//	[int] cap		 = set all letter to Cap letter (default : 0)
//
// Return : <int> lastX, the x coordination of the last pixel of last font
//
int Font::put(int x,int y,char* textPtr, char clearBack, int x2, int cap )
{
	MakeSafeCopy(&textPtr);
	BreakOnString(textPtr);
	err_when( x<0 || y<0 );

	Dump(textPtr);
	Dump(" &&x==");
	Dump(x);
	Dump("&&y==");
	Dump(y);
	Dump("&&x2==");
	Dump(x2);
	Dump("\n");

	if(strcmp("VILB",FontName)==0)
	{
#if(defined(TRADITIONAL))
		if(strcmp("¹A¥Á",textPtr)==0
#else
		if(strcmp("Å©Ãñ",textPtr)==0
#endif
			&&(x==736&&y==385&&x2==793||
			x==960&&y==553&&x2==1017))
		{
			x+=8;
			y+=3;
		}
#if(defined(TRADITIONAL))
		else if(strcmp("¤u¤H",textPtr)==0
#else
		else if(strcmp("¹¤ÈË",textPtr)==0
#endif
			&&(x==664&&y==385&&x2==-1||
			x==888&&y==553&&x2==-1))
		{
			x+=10;
			y+=3;
		}
#if(defined(TRADITIONAL))
		else if(strcmp("¤H¤f",textPtr)==0
#else
		else if(strcmp("ÈË¿Ú",textPtr)==0
#endif
			&&(x==596&&y==352&&x2==-1||
			x==820&&y==520&&x2==-1))
		{
			x+=20;
			y+=3;
		}
#if(defined(TRADITIONAL))
		else if(strcmp("©¾¸Û«×",textPtr)==0
#else
		else if(strcmp("ÖÒ³Ï¶È",textPtr)==0
#endif
			&&(x==696&&y==427&&x2==-1||
			x==920&&y==595&&x2==-1))
		{
			x+=5;
			y+=4;
		}
#if(defined(TRADITIONAL))
		else if(strcmp("¤Ï§Ü«×",textPtr)==0
#else
		else if(strcmp("·´¿¹¶È",textPtr)==0
#endif
			&&(x==688&&y==427&&x2==-1||
			x==912&&y==595&&x2==-1))
		{
			x+=10;
			y+=4;
		}
	}
	if( !init_flag )
		return x;

	//-------- process translation ---------//
//SXM
//	short textChar;
	unsigned short textChar;
//SXM
	textPtr = translate.process(textPtr);

	//-------------------------------------//

	int textPtrLen = strlen(textPtr);

	if( x2 < 0 ) // default
		x2 = x+max_font_width*textPtrLen;

	x2 = MIN( x2, VGA_WIDTH-1 );

	if( !Vga::use_back_buf )
		mouse.hide_area( x, y, x2, y+font_height );

	int y2 = y+font_height-1;

	//-------------------------------------//

	FontInfo* fontInfo;

#ifdef DEBUG
	OutputDebugString("HEHE  ");
	OutputDebugString(textPtr);
	OutputDebugString("\n");
#endif
	for( int lenCount=1 ; *textPtr && lenCount<=textPtrLen ; textPtr++, lenCount++ )
	{
		textChar = *((unsigned char*)textPtr);         // textChar is <unsiged char>
		//SXM
		if(textChar>160)
		{
			textChar*=0x100;
			textPtr++;
			textChar+=*((unsigned char*)textPtr);
		}
		//SXM
		// ###### begin Ban 18/1 ######//
		else if( cap==1 )
		{
			textChar = upper_case(textChar);
		}
		// ###### end Ban 18/1 ######//
		
		//--------------- space character ------------------//

		if( textChar == ' ' )
		{
			if( x+space_width > x2 )
				break;

			if( clearBack && !Vga::use_back_buf )	// copy texture from the back buffer as the background color
				vga_util.blt_buf( x, y, x+space_width-1, y+font_height-1, 0 );

			x += space_width;
		}

		//####### patch begin Gilbert 28/2 ########//
		// --------- control char: FIRST_NATION_COLOR_CODE_IN_TEXT -----------//

		else if( textChar >= FIRST_NATION_COLOR_CODE_IN_TEXT && textChar <= LAST_NATION_COLOR_CODE_IN_TEXT ) 	// display nation color bar in text
		{
			if( x2 >= 0 && x+NATION_COLOR_BAR_WIDTH-1 > x2 )      // exceed right border x2
				break;

			char colorCode = game.color_remap_array[textChar-FIRST_NATION_COLOR_CODE_IN_TEXT].main_color;

			nation_array.disp_nation_color(x, y+2, colorCode);

			x += NATION_COLOR_BAR_WIDTH;
		}

		//####### end begin Gilbert 28/2 ########//

		//------------- normal character ----------------//

		else if( textChar >= first_char && textChar <= last_char )
		{
			fontInfo = font_info_array+textChar-first_char;

			if( x+fontInfo->width > x2 )
				break;

			if( fontInfo->width > 0 )
			{
				if( clearBack && !Vga::use_back_buf )
				{
					//SXM
					if(strcmp("MID",FontName)==0)
						fontInfo->offset_y-=4;
					if(strcmp("NEWS",FontName)==0)
						fontInfo->offset_y+=4;
					//SXM
					if( fontInfo->offset_y > 0 )			// clear the upper space areas of the character
						vga_front.blt_buf_fast( &vga_back, x, y, x+fontInfo->width-1, y+fontInfo->offset_y-1 );

					vga_front.join_trans( &vga_back, x, y+fontInfo->offset_y, font_bitmap_buf + fontInfo->bitmap_offset );

					int ty = y+fontInfo->offset_y+fontInfo->height-1;

					if( ty < y2 )			// clear the lower space areas of the character
						vga_front.blt_buf_fast( &vga_back, x, ty+1, x+fontInfo->width-1, y2 );
					//SXM
					if(strcmp("MID",FontName)==0)
						fontInfo->offset_y+=4;
					if(strcmp("NEWS",FontName)==0)
						fontInfo->offset_y-=4;
					//SXM
				}
				else
				{
					//SXM
					if(strcmp("MID",FontName)==0)
						Vga::active_buf->put_bitmap_trans_fast(x, y+fontInfo->offset_y-4, font_bitmap_buf + fontInfo->bitmap_offset);
					else if(strcmp("NEWS",FontName)==0)
						Vga::active_buf->put_bitmap_trans_fast(x, y+fontInfo->offset_y+4, font_bitmap_buf + fontInfo->bitmap_offset);
					else
						Vga::active_buf->put_bitmap_trans_fast(x, y+fontInfo->offset_y, font_bitmap_buf + fontInfo->bitmap_offset);
					//SXM
				}

				x += fontInfo->width;		// inter-character space
			}
		}
		//SXM
		else if( textChar >= 0x100 )
		{
			TChineseFont* pcf=GetChineseFont(this);

			if( x+pcf->m_Width > x2 )
				break;

			memset(g_bChineseFontBitmap,HZ_BACKGROUND,25*25+sizeof(int)*2);
			g_bChineseFontBitmap[0]=pcf->m_Height+1;
			g_bChineseFontBitmap[1]=g_bChineseFontBitmap[3]=0;
			g_bChineseFontBitmap[2]=pcf->m_Width+1;
#if(!defined(TRADITIONAL))
			int FontOffset=(textChar/0x100-161)*94+textChar%0x100-161;
			int FontBytes=(pcf->m_Width+7);
			FontBytes=pcf->m_Height*(FontBytes/8);
			FontOffset*=FontBytes;
			if(pcf->m_Width==13)
			{
				FontOffset-=9688;
				if(FontOffset<0)
					FontOffset+=-9688+19984-520;
			}
			FontOffset+=-1;
#else
	#ifdef DEBUG
			char s[3]="  ";
			s[0]=textChar/256;
			s[1]=textChar%256;
			OutputDebugString("SXM  ");
			OutputDebugString(s);
			OutputDebugString("\n");
	#endif
			int FontOffset=GetChineseFontOffset((char*)&textChar,pcf->m_Width);
#endif
			for(int i=0;i<pcf->m_Height;i++)
				for(int j=0;j<pcf->m_Width;j++)
				{
					if(j%8==0)
						FontOffset++;
					if(*(pcf->m_FontData+FontOffset)&(1<<(7-j%8)))
					{
						*(g_bChineseFontBitmap+4+i*(pcf->m_Width+1)+j)=HZ_FOREGROUND(Vga::active_buf);
						if(HZ_SHADOW(Vga::active_buf)!=255)
							*(g_bChineseFontBitmap+4+(i+m_ChineseShadowY)*(pcf->m_Width+1)
								+(j+m_ChineseShadowX))=HZ_SHADOW(Vga::active_buf);
					}
//					else
//					{
//						*(g_bChineseFontBitmap+4+i*pcf->m_Width+j)=HZ_BACKGROUND;
//					}
				}
			if( pcf->m_Width > 0 )
			{
				if( clearBack && !Vga::use_back_buf )
				{
//					if( fontInfo->offset_y > 0 )			// clear the upper space areas of the character
//						vga_front.blt_buf_fast( &vga_back, x, y, x+fontInfo->width-1, y+fontInfo->offset_y-1 );

					vga_front.join_trans( &vga_back, x, y, (char*)g_bChineseFontBitmap );

//					int ty = y+fontInfo->offset_y+fontInfo->height-1;

//					if( ty < y2 )			// clear the lower space areas of the character
//						vga_front.blt_buf_fast( &vga_back, x, ty+1, x+fontInfo->width-1, y2 );
				}
				else
				{
					Vga::active_buf->put_bitmap_trans_fast(x, y, (char*)g_bChineseFontBitmap);
				}

				x += pcf->m_Width;
			}
		}
		//SXM
		else
		{
			//------ tab or unknown character -------//

			if( textChar == '\t' )         // Tab
				x += space_width*8;          // one tab = 8 space chars
			else
				x += space_width;
		}

		//--------- inter-character space ---------//

		if( clearBack && !Vga::use_back_buf )	// copy texture from the back buffer as the background color
			vga_util.blt_buf( x, y, x+inter_char_space-1, y+font_height-1, 0 );
	
		x+=inter_char_space;
	}

	//------ clear remaining area -------//

	if( clearBack && !Vga::use_back_buf )	// copy texture from the back buffer as the background color
		vga_util.blt_buf( x, y, x2, y+font_height-1, 0 );

	if( !Vga::use_back_buf )
		mouse.show_area();

	return x-1;
}
//----------- End of function Font::put ---------//


//--------- Start of function Font::put_char ---------//
//
// <int>  x, y      = the position of the character
// <char> textChar  = the character
//
void Font::put_char(int x, int y, unsigned short textChar)
{
	//SXM:Never been used
	ExitProcess(0);
	if( textChar >= 0x100 )
	{
		TChineseFont* pcf=GetChineseFont(this);

		memset(g_bChineseFontBitmap,HZ_BACKGROUND,25*25+sizeof(int)*2);
		g_bChineseFontBitmap[0]=pcf->m_Height+1;
		g_bChineseFontBitmap[1]=g_bChineseFontBitmap[3]=0;
		g_bChineseFontBitmap[2]=pcf->m_Width+1;
#if(!defined(TRADITIONAL))
		int FontOffset=(textChar/0x100-161)*94+textChar%0x100-161;
		int FontBytes=(pcf->m_Width+7);
		FontBytes=pcf->m_Height*(FontBytes/8);
		FontOffset*=FontBytes;
		if(pcf->m_Width==13)
		{
			FontOffset-=9688;
			if(FontOffset<0)
				FontOffset+=-9688+19984-520;
		}
		FontOffset+=-1;
#else
		int FontOffset=GetChineseFontOffset((char*)&textChar,pcf->m_Width);
#endif
		for(int i=0;i<pcf->m_Height;i++)
			for(int j=0;j<pcf->m_Width;j++)
			{
				if(j%8==0)
					FontOffset++;
				if(*(pcf->m_FontData+FontOffset)&(1<<(7-j%8)))
				{
					*(g_bChineseFontBitmap+4+i*(pcf->m_Width+1)+j)=HZ_FOREGROUND(Vga::active_buf);
					if(HZ_SHADOW(Vga::active_buf)!=255)
						*(g_bChineseFontBitmap+4+(i+m_ChineseShadowY)*(pcf->m_Width+1)+
							(j+m_ChineseShadowX))=HZ_SHADOW(Vga::active_buf);
				}
//				else
//				{
//					*(g_bChineseFontBitmap+4+i*(pcf->m_Width+1)+j)=HZ_BACKGROUND;
//				}
			}
		Vga::active_buf->put_bitmap_trans( x, y, (char*)g_bChineseFontBitmap );
	}
	if( textChar >= first_char && textChar <= last_char )
	{
		FontInfo* fontInfo = font_info_array+textChar-first_char;

		Vga::active_buf->put_bitmap_trans( x, y + fontInfo->offset_y, font_bitmap_buf + fontInfo->bitmap_offset );
	}
}
//----------- End of function Font::put_char ---------//


//--------- Start of function Font::right_put ---------//
//
// Display a string in right-justified format.
//
// <int>   x, y    = the right end position of displaying the string
// <char*> textPtr = the character
// [int] clearBack = clear background with back_color or not
//                   (default : 0)
//	[int] cap		 = set all letter to Cap letter (default : 0)
//
void Font::right_put(int x, int y, char* textPtr, char clearBack, char cap)
{
	int textWidth = text_width(textPtr, -1, 0, cap);

	put( x-textWidth-1, y, textPtr, clearBack, x, cap );
}
//----------- End of function Font::right_put ---------//


//--------- Begin of function Font::text_width ----//
//
// Given the textPtr string, then calculate the width (unit:pixel).
//
// char* textPtr         = the textPtr to be put on screen
// [int] textPtrLen      = the length of the textPtr to be put
//                      (default : -1, until NULL character)
// [int] maxDispWidth = the maximum width can be displayed
//                      the textPtr width cannot > maxDispWidth
// [cap] set all letter to Cap letter (default : 0)
//
// Note : static var text_line_count is used to pass value to text_height()
//
// Return : <int> the screen width of the textPtr display using this font
//
int Font::text_width(char* textPtr, int textPtrLen, int maxDispWidth, int cap)
{
	int   charWidth, x=0, lenCount, maxLen=0, wordWidth=0;
	//SXM
//	short textChar;
	unsigned short textChar;
	//SXM

	if( !init_flag )
		return x;

	textPtr = translate.process(textPtr);

	if( textPtrLen < 0 )
		textPtrLen = strlen(textPtr);

	text_line_count=1;

	//-------------------------------------//

	for( lenCount=1 ; *textPtr && lenCount<=textPtrLen ; textPtr++, lenCount++, x+=inter_char_space )
	{
		textChar = *((unsigned char*)textPtr);
		//SXM
		if(textChar>160)
		{
			textChar*=0x100;
			textPtr++;
			textChar+=*((unsigned char*)textPtr);
			lenCount++;
		}
		//SXM

		// ###### begin Ban 18/1 ######//
		if( cap==1 )
		{
			textChar = upper_case(textChar);			
		}
		// ###### end Ban 18/1 ######//
		
		//-- if the line exceed the given max width, advance to next line --//

		if( maxDispWidth && x > maxDispWidth )
		{
			maxLen = maxDispWidth;
			x      = wordWidth;    // the last word of the prev line wraps to next line
			text_line_count++;
		}

		//--- if the textPtr has more than 1 line, get the longest line ---//

		if( textChar == '\n' )
		{
			if( x>maxLen )
				maxLen=x;

			x=0;
			wordWidth=0;
			text_line_count++;
			continue;              // next character
		}

		//-------- control char: FIRST_NATION_COLOR_CODE_IN_TEXT -----------//

		else if( textChar >= FIRST_NATION_COLOR_CODE_IN_TEXT && textChar <= LAST_NATION_COLOR_CODE_IN_TEXT  ) 	// display nation color bar in text
		{
			x += NATION_COLOR_BAR_WIDTH;
			wordWidth = 0;
		}

		//--- add the width of the character to the total line width ---//

		else if( textChar == ' ' )
		{
			x += space_width;
			wordWidth = 0;
		}

		else if( textChar >= first_char && textChar <= last_char )
		{
			charWidth = font_info_array[textChar-first_char].width;

			x         += charWidth;
			wordWidth += charWidth;
		}
		else if( textChar >= 0x100 )
		{
			TChineseFont* pcf=GetChineseFont(this);

			x += pcf->m_Width;
			wordWidth=0;
			//SXM: Unfinished
			//SXM: Important Note
			//we shouldn't use wordWidth in Chinese version,  
			//Since we haven't finished Chinese translate now,
			//we can only keep the English ones here.
		}
		else
		{
			x         += space_width;
			wordWidth += space_width;
		}

		if( maxDispWidth && wordWidth > maxDispWidth )
		{
			x         -= wordWidth - maxDispWidth;
			wordWidth  = maxDispWidth;
		}
	}

	//-------------------------------------------//

	if( maxDispWidth && x > maxDispWidth )
		text_line_count++;

	if( textPtr[-1] == '\n' )       // if last character is line feed, don't count double
		text_line_count--;

	return MAX(maxLen,x);
}
//----------- End of function Font::text_width ----//


//--------- Begin of function Font::text_height ----//
//
// text_width() must first be called, this function use the result
// from text_width() to calc the textPtr height.
//
// [int] lineSpace = space between lines
//                   ( default : DEFAULT_LINE_SPACE )
//
// Return : <int> the screen height of the textPtr display using this font
//
int Font::text_height(int lineSpace)
{
//SXM
	if(lineSpace<2)
		lineSpace=3;
	TChineseFont* pcf=GetChineseFont(this);
	int FontHeight=font_height>pcf->m_Height?font_height:pcf->m_Height;
//	return font_height * text_line_count +
//			 lineSpace   * (text_line_count-1);
	return FontHeight * text_line_count +
			 lineSpace   * (text_line_count-1);
//SXM
}
//----------- End of function Font::text_height ----//


//--------- Begin of function Font::put_paragraph ------//
//
// Put a paragraph of textPtr on screen
// Given the area of the paragraph
// Automatical word wrap is available
//
// If the background color of the font is set, the area of the paragraph
// will be filled with the background color first.
//
// int x1,y1,x2,y2    = area of the textPtr
//
// char* textPtr         = the textPtr to be put on screen
//
// [int lineSpace]    = no. of pixel space between lines
//                      ( default : 2 )
//
// [int] startLine    = start display at which line
//                      ( default : 1, first line)
//
// [float wordPause]  = time pause between words
//                      ( default : 0 )
//
// [char dispFlag]    = display the textPtr or not, if called by count_line(),
//                      this would be 0.
//                      ( default : 1 )
//
//-------------------------------------------------------//
//
// It will store resulting paramteres :
//
// <char*> next_text_ptr = point to the textPtr just after this paragraph
//                         which has been put to the screen.
//
// <int>   next_text_y   = the endding y position of the paragraph and which
//                         is the y position which next paragraph can be put
//
// <int>   line_count    = the no. of lines has been displayed in current
//                         put_paragraph() session.
//
// <HyperField> hyper_field_array[] = an array of hyper-textPtred field
//
//-------------------------------------------------------//

void Font::put_paragraph(int x1, int y1, int x2, int y2, char *textPtr,
								 int lineSpace, int startLine, char dispFlag)
{
	MakeSafeCopy(&textPtr);
	BreakOnString(textPtr);
	//SXM
	if(lineSpace<2)
		lineSpace=3;
	//SXM
	if( !init_flag || y1+font_height-1 > y2 )
		return;

	textPtr = translate.process(textPtr);

	//--------- define vars ---------------//

	int   x,y,wordX;
	int   newWord;
	//SXM
	BOOL bBreak=FALSE;
	unsigned short textChar;
//	short textChar;
	//SXM
	char *wordPtr;

   char  flag_under_line=0;     // attribute control flags
   char  flag_hyper_field=0;

   int   under_line_x1;         // parameters for drawing under line

   HyperField* hyper_field_ptr = hyper_field_array;
	FontInfo*   fontInfo;

	//--------- initialize vars ---------------//

	x       = x1;
	y       = y1;
	wordX   = x1;          // the x of the start of the word
	wordPtr = textPtr;
	newWord = 0;

   line_count=0;

	hyper_field_count = 0;

   //---------- prepare for display font ----------//

	if( dispFlag )
   {
      err_when( x1>x2 || y1>y2 || x1<0 || y1<0 || x2>=VGA_WIDTH || y2>=VGA_HEIGHT );

      if( !Vga::use_back_buf )     // if not say word by word, turn off mouse one time, otherwise turn off mouse one word at a time
			mouse.hide_area( x1,y1,x2,y2 );  // if the mouse cursor is in that area, hide it
	}

	//--------- loop for displaying textPtr ----------//

	while(1)
	{
		//------ space appear, process the previous word ------//

		if ( newWord )
		{
			//SXM
			textChar = *((unsigned char*)wordPtr); // textChar is <unsiged char>
			if(textChar>160)
			{
				textChar=textChar*0x100+*((unsigned char*)(wordPtr+1));
			}
			//SXM
/*			if( x-1 > x2 )  // it is still okay if x-1==x2 because x is the next pixel, not the last displayed pixel
			{
				if( line_count >= startLine-1 )    // startLine start from 1
				{
					y += font_height + lineSpace;
					if ( y + font_height - 1 > y2 )     // no further space for printing textPtr, end
						break;
				}

				x = x1;
				line_count++;
			}*/
//SXM
			TChineseFont* pcf=GetChineseFont(this);
			if( x-1 > x2 )//|| textChar >= 160 && x - 1 + pcf->m_Width > x2)  // it is still okay if x-1==x2 because x is the next pixel, not the last displayed pixel
			{
				if( line_count >= startLine-1 )    // startLine start from 1
				{
					y += font_height + lineSpace;
					if ( y + font_height - 1 > y2 )     // no further space for printing textPtr, end
						break;
				}

				x = x1;
				line_count++;
			}
//SXM
			else
			{
				x = wordX;
			}

			//--------- Process current word ----------//

			for( ; wordPtr < textPtr && *wordPtr && *wordPtr != TEXT_END_CHAR ; wordPtr++, x+=inter_char_space )  // wordPtr point to the current word which will be printed
			{
				textChar = *((unsigned char*)wordPtr); // textChar is <unsiged char>
				//SXM
				if(textChar>160)
				{
					wordPtr++;
					textChar=textChar*0x100+*((unsigned char*)wordPtr);
				}
				//SXM

				//---------- control char: '_' -------------//

				if( textChar == '_' )       // e.g. _Title_, a line will be drawn under the word "Title"
				{
					if( flag_under_line )    // current '_' is the closing one, the open '_' has appeared previously
					{
						if( dispFlag && line_count >= startLine-1 )    // startLine start from 1
							Vga::active_buf->bar_fast(under_line_x1, y+font_height, x, y+font_height, HYPER_FIELD_COLOR );
					}
					else
						under_line_x1 = x;

					flag_under_line = !flag_under_line;
					continue;
				}

				//-------- control char: '~' -----------//

				else if( textChar == '~' )       // e.g. ~Captial~, the word "Capital" is a hyper-textPtred field, pressing on it will link to the topic "Capital"
				{
					if( !flag_hyper_field )   // current '~' is the opening one
					{
						hyper_field_ptr->x1       = x;
						hyper_field_ptr->y1       = y;
						hyper_field_ptr->text_ptr = (wordPtr+1);  // skip current '~'
					}
					else                      // current '~' is the closing one
					{
						hyper_field_ptr->x2       = x;
						hyper_field_ptr->y2       = y+font_height;
						hyper_field_ptr->text_len = wordPtr - hyper_field_ptr->text_ptr;

						hyper_field_count++;
						hyper_field_ptr++;
						err_when( hyper_field_count >= MAX_HYPER_FIELD );
					}

					flag_hyper_field = !flag_hyper_field;
					continue;
				}

				//-------- control char: FIRST_NATION_COLOR_CODE_IN_TEXT -----------//

				else if( textChar >= FIRST_NATION_COLOR_CODE_IN_TEXT && textChar <= LAST_NATION_COLOR_CODE_IN_TEXT ) 	// display nation color bar in text
				{
					if( x2 >= 0 && x+NATION_COLOR_BAR_WIDTH-1 > x2 )      // exceed right border x2
						break;

					char colorCode = game.color_remap_array[textChar-FIRST_NATION_COLOR_CODE_IN_TEXT].main_color;

					nation_array.disp_nation_color(x, y+2, colorCode);

					x += NATION_COLOR_BAR_WIDTH;
				}

				//--------------- space character ------------------//

				else if( textChar == ' ' )
				{
					if( x+space_width > x2 )
						break;

					x += space_width;
				}

				//----------- display char ------------//

				else if( textChar >= first_char && textChar <= last_char )
				{
					fontInfo = font_info_array+textChar-first_char;

					// character width = offset of next character - current offset

					if( x2 >= 0 && x+fontInfo->width-1 > x2 )      // exceed right border x2
					{
						//SXM
						x+=fontInfo->width-1;
						textPtr-=1;
						//SXM
						break;
					}

					if( fontInfo->width > 0 )
					{
						if( dispFlag && line_count >= startLine-1 )    // startLine start from 1
						{
//							if( flag_hyper_field )
//								mainColor = HYPER_FIELD_COLOR;
							//SXM
							if(strcmp("MID",FontName)==0)
								Vga::active_buf->put_bitmap_trans_fast(x,	y+fontInfo->offset_y-4,
									font_bitmap_buf + fontInfo->bitmap_offset);
							else if(strcmp("CMPH",FontName)==0)
								Vga::active_buf->put_bitmap_trans_fast(x,	y+fontInfo->offset_y-6,
									font_bitmap_buf + fontInfo->bitmap_offset);
							else if(strcmp("NEWS",FontName)==0)
								Vga::active_buf->put_bitmap_trans_fast(x,	y+fontInfo->offset_y+4,
									font_bitmap_buf + fontInfo->bitmap_offset);
							else
								Vga::active_buf->put_bitmap_trans_fast(x,	y+fontInfo->offset_y,
									font_bitmap_buf + fontInfo->bitmap_offset);
							//SXM
						}

						x += fontInfo->width;
					}
				}
//SXM
				else if( textChar >= 0x100 )
				{
					TChineseFont* pcf=GetChineseFont(this);

					if( x2 >= 0 && x+pcf->m_Width-1 > x2 )      // exceed right border x2
					{
						x += pcf->m_Width;
						textPtr-=2;
						break;
					}

					if( dispFlag && line_count >= startLine-1 )    // startLine start from 1
					{
						memset(g_bChineseFontBitmap,HZ_BACKGROUND,25*25+sizeof(int)*2);
						g_bChineseFontBitmap[0]=pcf->m_Height+1;
						g_bChineseFontBitmap[1]=g_bChineseFontBitmap[3]=0;
						g_bChineseFontBitmap[2]=pcf->m_Width+1;
#if(!defined(TRADITIONAL))
						int FontOffset=(textChar/0x100-161)*94+textChar%0x100-161;
						int FontBytes=(pcf->m_Width+7);
						FontBytes=pcf->m_Height*(FontBytes/8);
						FontOffset*=FontBytes;
						if(pcf->m_Width==13)
						{
							FontOffset-=9688;
							if(FontOffset<0)
								FontOffset+=-9688+19984-520;
						}
						FontOffset+=-1;
#else
						int FontOffset=GetChineseFontOffset((char*)&textChar,pcf->m_Width);
#endif
						for(int i=0;i<pcf->m_Height;i++)
							for(int j=0;j<pcf->m_Width;j++)
							{
								if(j%8==0)
									FontOffset++;
								if(*(pcf->m_FontData+FontOffset)&(1<<(7-j%8)))
								{
									*(g_bChineseFontBitmap+4+i*(pcf->m_Width+1)+j)=HZ_FOREGROUND(Vga::active_buf);
									if(HZ_SHADOW(Vga::active_buf)!=255)
										*(g_bChineseFontBitmap+4+(i+m_ChineseShadowY)*(pcf->m_Width+1)+
											(j+m_ChineseShadowX))=HZ_SHADOW(Vga::active_buf);
								}
//								else
//								{
//									*(g_bChineseFontBitmap+4+i*(pcf->m_Width+1)+j)=HZ_BACKGROUND;
//								}
							}
						if( pcf->m_Width > 0 )
						{
							Vga::active_buf->put_bitmap_trans_fast(x, y, (char*)g_bChineseFontBitmap);
						}
					}
					x += pcf->m_Width;
				}
//SXM
			}

			//--------- next line ----------------------//

			if( *textPtr == '\n' )   // next line
			{
				if( line_count >= startLine-1 )    // startLine start from 1
				{
					y += font_height + lineSpace;
					if ( y + font_height - 1 > y2 )     // no further space for printing textPtr, end
						break;
				}

				x = x1;
				textPtr++;
				line_count++;
			}

			if( *textPtr == NULL || *textPtr == TEXT_END_CHAR )     // all paragraph has been printed, 26 = CTRL-Z - the ending character
				break;

			wordPtr = textPtr;
			wordX   = x;
			newWord = 0;
		}

		//------------ process spacing -------------//
//SXM
//		if( *textPtr == ' ' || *textPtr == '\n' || *textPtr == NULL || *textPtr == TEXT_END_CHAR )    // not space
//			newWord = 1;
		if(1// *textPtr == ' ' || *textPtr == '\n' || *textPtr == NULL || *textPtr == TEXT_END_CHAR 
			|| *(unsigned char*)textPtr >= 160 )    // not space
		{
			if(*(unsigned char*)textPtr >= 160)
				textPtr++;
			newWord = 1;
		}
//SXM
		else
		{
			textChar = *((unsigned char*)textPtr); // textChar is <unsiged char>

			if( textChar >= first_char && textChar <= last_char )
			{
				fontInfo = font_info_array+textChar-first_char;

				if( textChar >= FIRST_NATION_COLOR_CODE_IN_TEXT && textChar <= LAST_NATION_COLOR_CODE_IN_TEXT ) 	// display nation color bar in text
					x += NATION_COLOR_BAR_WIDTH;

				else if( textChar == ' ' )
					x+=space_width;

				else if( fontInfo->width > 0 )   // current font width
					x+=fontInfo->width;

				else
					x+=space_width;
			}
			else
			{
				x+=space_width;
			}

			x+=inter_char_space;
		}

		if( *textPtr && *textPtr != TEXT_END_CHAR && *textPtr != '\n' )    // when finished, remain as NULL for next cycle to detect
			textPtr++;
	}

	//------------ finish displaying ----------------//

	if( dispFlag )
	{
		if( !Vga::use_back_buf )
			mouse.show_area();
	}

//   while( *textPtr == '\n' || *textPtr == '\r' )
//      textPtr++;

	next_text_ptr = textPtr;    // the current pointer in the textPtr
	next_text_y   = y + font_height + lineSpace;
	line_count++;
}
//---------- End of function Font::put_paragraph ------//


//---------- Begin of function Font::center_put_paragraph ------//
//
// Display the textPtr on the center of the given area
//
// <int>   x1,y1   = the coordination of the panel
// <int>   x2,y2   =
// <char*> desStr  = the spinner description
// [char] clearBack = clear background with back_color or not
//                   (default : 0)
// [cap] set all letter to Cap letter (default : 1)
//
// Return : <int> lastX, the x coordination of the last pixel of last font
//
// but unlike the put_paragrapit has no out of bound checking

void Font::center_put_paragraph(int x1, int y1, int x2, int y2, char* desStr, 
										  int lineSpace, char clearBack, int cap)
{
	//SXM:Risk
	//Set Display Flag to Zero so We do not draw any characters.
	//SXM
	if(lineSpace<2)
		lineSpace=3;
	//SXM
	put_paragraph(x1,y1,x2,y2,desStr,lineSpace,1,0);
	y1+=((y2-y1)-line_count*(lineSpace+font_height))/2;
	if(line_count==1)
	{
		x1+=((x2-x1)-text_width(desStr))/2;
	}
	put_paragraph(x1,y1,x2,y2,desStr,lineSpace,1,1);
	return;
	//SXM:Risk
	int textPtrLen = strlen(desStr);
	// ###### begin Gilbert 7/11 ######//
	Blob tempStrBlob(strlen(desStr)+400);
	char *tempStr = tempStrBlob.p();
	// ###### end Gilbert 7/11 ######//
	//SXM
//	char textChar;
	unsigned short textChar;
	//SXM
	int rowCount = 0;
	int rowCount2 = 0;
	int isFirstWordOfRow = 1;
	int letterCount = 0;	
	int endMark = 0;
	int recordLenCount = 0;
	int y;

	MakeSafeCopy(&desStr);
	BreakOnString(desStr);

	//SXM
	{
	int LastOffset=0;
	int Row=0;
	char* LargeBuffer=new char[1024];
	for( int lenCount=0 ; lenCount <= textPtrLen ; lenCount++ )
	{
		if((unsigned char)desStr[lenCount]>=160)
			lenCount++;
		strcpy(LargeBuffer,desStr+LastOffset);
		LargeBuffer[lenCount-LastOffset+1]=0;
		if(text_width(LargeBuffer, -1, 0, cap) + x1 > x2)
		{
			center_put(x1, y1 + Row * (font_height +lineSpace), x2, y1 + (Row+1) * 
				(font_height +lineSpace), LargeBuffer, clearBack, cap);
			Row++;
			LastOffset=lenCount+1;
			continue;
		}
		if(lenCount==textPtrLen)
			center_put(x1, y1 + Row * (font_height +lineSpace), x2, y1 + (Row+1) * 
				(font_height +lineSpace), LargeBuffer, clearBack, cap);
	}
	delete LargeBuffer;
	}
	return;
	//SXM
	for( int lenCount=0 ; lenCount <= textPtrLen ; lenCount++ )
	{
		if (lenCount == textPtrLen)
			textChar = '\0';
		else
			textChar = (unsigned char)desStr[lenCount];

		//SXM
		if(textChar>=160)
		{
			lenCount++;
			textChar=textChar*256+(unsigned char)desStr[lenCount];
		}
		//SXM
		// check for each letter
		if (textChar == ' ' || textChar =='\0' || textChar >= 160)
		{
			tempStr[letterCount] = '\0';
			// end of one line
		//	if (text_width(tempStr, -1, 0, cap) + x1 > x2 && endMark != 0)
			if (text_width(tempStr, -1, 0, cap) + x1 > x2 && !isFirstWordOfRow)
			{
				// error for not enought space for displaying first word of the para
			//	if(endMark == 0)
			//	{
			//		err_here();
			//		desStr[99] = '\0';
			//		err.run( desStr );
			//	}

				tempStr[endMark] = '\0';
			//	copyStr[rowCount] = tempStr;
				rowCount ++;
				isFirstWordOfRow = 1;
				letterCount = -1;
				lenCount = recordLenCount;
			}
			else
			// end of para
			if ( (text_width(tempStr, -1, 0, cap) + x1 <= x2) && (lenCount == textPtrLen) ) 
			{
			//	copyStr[rowCount] = tempStr;
				rowCount ++;
				isFirstWordOfRow = 1;
			}
			else
			{
				recordLenCount = lenCount;
				endMark = letterCount;
				//SXM
//				tempStr[letterCount] = ' ';
				if(textChar<160)
					tempStr[letterCount] = ' ';
				else
				{
					tempStr[letterCount++] = textChar/256;
					tempStr[letterCount] = textChar%256;
				}
				//SXM
				isFirstWordOfRow = 0;
			}
		}
		else
		{
			tempStr[letterCount] = (char) textChar;
		}
		letterCount ++;
	}		 
	
	letterCount = 0;	
	endMark = 0;
	recordLenCount = 0;
	isFirstWordOfRow = 1;

	for( lenCount=0 ; lenCount <= textPtrLen ; lenCount++ )
	{
		if (lenCount == textPtrLen)
			textChar = '\0';
		else
			textChar = (unsigned char)desStr[lenCount];

		// check for each letter
		if (textChar == ' ' || textChar =='\0')
		{
			tempStr[letterCount] = '\0';
			// end of one line
		//	if (text_width(tempStr, -1, 0, cap) + x1 > x2 && endMark != 0) 
			if (text_width(tempStr, -1, 0, cap) + x1 > x2 && !isFirstWordOfRow) 
			{
				// error for not enought space for displaying first word of the para
			//	if(endMark == 0)
			//	{
			//		err_here();
			//		desStr[99] = '\0';
			//		err.run( desStr );
			//	}

				tempStr[endMark] = '\0';
			//	copyStr[rowCount] = tempStr;
				y = (y2 + y1) /2 - rowCount * (font_height +lineSpace) /2;
				center_put(x1, y + rowCount2 * (font_height +lineSpace), x2, y + (rowCount2+1) * (font_height +lineSpace), tempStr, clearBack, cap);
				rowCount2 ++;
				isFirstWordOfRow = 1;
				letterCount = -1;
				lenCount = recordLenCount;
			}
			else
			// end of para
			if ( (text_width(tempStr, -1, 0, cap) + x1 <= x2) && (lenCount == textPtrLen) ) 
			{
				y = (y2 + y1) /2 - rowCount * (font_height +lineSpace) /2;
				center_put(x1, y + rowCount2 * (font_height +lineSpace), x2, y + (rowCount2+1) * (font_height +lineSpace), tempStr, clearBack, cap);
			//	copyStr[rowCount] = tempStr;
				rowCount2 ++;
				isFirstWordOfRow = 1;
			}
			else
			{
				recordLenCount = lenCount;
				endMark = letterCount;
				tempStr[letterCount] = ' ';
				isFirstWordOfRow = 0;
			}
		}
		else
		{
			tempStr[letterCount] = (char)textChar;
		}
		letterCount ++;
	}		 

//	for (int i = 0; i <rowCount; i++)
//	{
//		int y = (y2 + y1) /2 - rowCount * (font_height +lineSpace) /2;
//		center_put(x1, y + i * (font_height +lineSpace), x2, y + (i+1) * (font_height +lineSpace), copyStr[i], clearBack, cap);
//	}

}
//---------- End of function Font::center_put_paragraph ------//


//--------- Begin of function Font::count_line ------//
//
// Count the no. of lines of textPtr can be displayed on the given area.
//
// int x1,y1,x2,y2 = area of the textPtr
// char* textPtr      = the textPtr to be put on screen
//
// <int lineSpace> = no. of pixel space between lines
//                   ( default : 0 )
//
// <int& dispLines>  = no. of lines can be displayed in the textPtr area
// <int& totalLines> = total no. of lines of the textPtr
//
void Font::count_line(int x1, int y1, int x2, int y2, char *textPtr,
							 int lineSpace , int& dispLines, int& totalLines)
{
	//SXM
	if(lineSpace<2)
		lineSpace=3;
	//SXM
	dispLines =0;
	totalLines=0;

	while( *textPtr && *textPtr != TEXT_END_CHAR )    // if *textPtr==NULL means EOF
	{
		put_paragraph(x1, y1, x2, y2, textPtr, lineSpace, 1, 0);   // last 0-means don't display the textPtr, only returning the parameters for our calculating of line counts

		if( !dispLines )                  // only set once at the first time
			dispLines = line_count;

		totalLines += line_count;
		textPtr     = next_text_ptr;
	}
}
//---------- End of function Font::count_line ------//


//-------- Begin of function Font::d3_put --------//
//
// Display field description and field value
//
// <int>   x1,y1      = the coordination of the spinner description
// <char*> desStr     = the spinner description
//
// Return : <int> lastX, the x coordination of the last pixel of last font
//
int Font::d3_put(int x1, int y1, const char* desStr )
{
	int marginSpace = font_height/5;

	int x2 = x1+text_width(desStr)+marginSpace*2-1;

	vga_util.d3_panel_up( x1, y1, x2, y1+font_height+marginSpace*2-1 );

   put( x1+marginSpace, y1+marginSpace, desStr);

   return x2;
}
//--------- End of function Font::d3_put ---------//


//-------- Begin of function Font::d3_put --------//
//
// Display field description and field value.
// It will display the description on the center of the panel
//
// <int>   x1,y1      = the coordination of the panel
// <int>   x2,y2      =
// <char*> desStr     = the spinner description
//
void Font::d3_put(int x1, int y1, int x2, int y2, const char* desStr)
{
	int tx = x1 + ((x2-x1+1) - text_width(desStr))/2;
   int ty = y1 + ((y2-y1+1) - font_height)/2+1;

	vga_util.d3_panel_up( x1, y1, x2, y2);

   if( tx<x1+4 )
      tx=x1+4;

   put( tx, ty, desStr, 0, x2-2 );
}
//--------- End of function Font::d3_put ---------//


//-------- Begin of function Font::center_put --------//
//
// Display the textPtr on the center of the given area
//
// <int>   x1,y1   = the coordination of the panel
// <int>   x2,y2   =
// <char*> desStr  = the spinner description
// [char] clearBack = clear background with back_color or not
//                   (default : 0)
// [cap] set all letter to Cap letter (default : 0)
//
// Return : <int> lastX, the x coordination of the last pixel of last font
//
int Font::center_put(int x1, int y1, int x2, int y2, char* desStr, char clearBack, int cap)
{
	int tx = x1 + ((x2-x1+1) - text_width(desStr, -1, 0, cap))/2;
	int ty = y1 + ((y2-y1+1) - font_height)/2;

	if( tx<0 )
		tx=0;

	if( clearBack && !Vga::use_back_buf && tx>x1 )	// copy texture from the back buffer as the background color
		vga_util.blt_buf( x1, y1, tx-1, y2, 0 );

/*	Dump(desStr);
	Dump(" &&x1==");
	Dump(x1);
	Dump("&&x2==");
	Dump(x2);
	Dump("&&y1==");
	Dump(y1);
	Dump("&&y2==");
	Dump(y2);
	Dump("\n");*/
	if(strcmp("BBLK",FontName)==0)
	{
#if(defined(TRADITIONAL))
		if(strcmp("¹q¸£±±¨î°ê®a",desStr)==0
#else
		if(strcmp("µçÄÔ¿ØÖÆ¹ú¼Ò",desStr)==0
#endif
			&&x1==297&&x2==698&&y1==205&&y2==223)
			tx-=6;
#if(defined(TRADITIONAL))
		else if(strcmp("Ãø«×µ¥¯Å",desStr)==0
#else
		else if(strcmp("ÄÑ¶ÈµÈ¼¶",desStr)==0
#endif
			&&x1==341&&x2==654&&y1==275&&y2==294)
			tx-=6;
#if(defined(TRADITIONAL))
		else if(strcmp("¤gÄ[³]¸m",desStr)==0
#else
		else if(strcmp("ÍÁÈÀÉèÖÃ",desStr)==0
#endif
			&&x1==370&&x2==633&&y1==320&&y2==333)
			tx-=11;
#if(defined(TRADITIONAL))
		else if(strcmp("«Ø¿vª«¤j¤p",desStr)==0
#else
		else if(strcmp("½¨ÖþÎï´óÐ¡",desStr)==0
#endif
			&&x1==341&&x2==660&&y1==365&&y2==384)
			tx-=10;
	}
#define COMPUTER_CONTROL_NATION_OFFSET 30
#define DIFFICULT_LEVEL -2
	if(strcmp("BBLK",FontName)==0||strcmp("BRED",FontName)==0)
	{
#if(defined(TRADITIONAL))
		if(strcmp("¤HÃþ",desStr)==0
#else
		if(strcmp("ÈËÀà",desStr)==0
#endif
			&&x1==227&&x2==285&&y1==103&&y2==119)
			tx+=COMPUTER_CONTROL_NATION_OFFSET;
#if(defined(TRADITIONAL))
		else if(strcmp("¦Û©w¸q",desStr)==0
#else
		else if(strcmp("×Ô¶¨Òå",desStr)==0
#endif
			&&x1==583&&x2==650&&y1==296&&y2==312)
			tx+=DIFFICULT_LEVEL;
	}
	if(strcmp("TBLK",FontName)==0||strcmp("BRED",FontName)==0)
	{
		if(strcmp("0",desStr)==0
			&&x1==381&&x2==401&&y1==250&&y2==265
			/*||
			strcmp("1",desStr)==0||
			strcmp("2",desStr)==0||
			strcmp("3",desStr)==0||
			strcmp("4",desStr)==0||
			strcmp("5",desStr)==0||
			strcmp("6",desStr)==0*/
			)
			tx+=COMPUTER_CONTROL_NATION_OFFSET;
		if(strcmp("I",desStr)==0||
			strcmp("II",desStr)==0||
			strcmp("III",desStr)==0||
			strcmp("IV",desStr)==0||
			strcmp("V",desStr)==0||
			strcmp("VI",desStr)==0||
			strcmp("VII",desStr)==0||
			strcmp("VIII",desStr)==0
			)
			if(y1==331)
				tx+=DIFFICULT_LEVEL;//+20;
			else
				tx+=DIFFICULT_LEVEL;
//		else if(strcmp("·ÊÎÖ",desStr)==0)
//			tx+=COMPUTER_CONTROL_NATION_OFFSET;
/*		else if(strcmp("Æ¶ñ¤",desStr)==0
			&&x1==465&&x2==513&&y1==341&&y2==356)
			tx+=58;*/
//		else if(strcmp("´ó",desStr)==0)
//			tx+=COMPUTER_CONTROL_NATION_OFFSET+10;
/*		else if(strcmp("Ð¡",desStr)==0
			&&(x1==464&&x2==493&&y1==386&&y2==402||
			x1==464&&x2==493&&y1==386&&y2==401))
			tx+=52;*/
	}
	return put( tx, ty, desStr, clearBack, x2+100, cap );
}
//--------- End of function Font::center_put ---------//


//-------- Begin of function Font::put_field --------//
//
// Display field description and field value
//
// format - "Field : 999999" e.g. "List price : $92,232"
//
// <int>   x1,y1  = the coordination of the spinner description
// <char*> desStr = the spinner description
// <int>   x2     = the coordination of the spinner variable
// <int>   value  = value
//
// [int]   format = 1 - 1,000,000 normal, with thousand separator
//                  2 - $1,000,000 add dollar sign, with thousand separator
//                  3 - 100% percentage
//                  (default : 1 )
//
void Font::put_field(int x1, int y1, char* desStr, int x2, int value, int format )
{
	vga_util.d3_panel_up( x1, y1, x2, y1+font_height+6 );

	put( x1+4, y1+2, desStr);
	put( x2+4, y1+2, m.format(value,format) );
}
//--------- End of function Font::put_field ---------//


//-------- Begin of function Font::update_field --------//
//
// Update the value of the field which is previously displayed by
// Font::put_field()
//
// <int>   x1,y1  = the coordination of the field variable
// <int>   value  = value
//
// <int>   format = 1 - 1,000,000 normal, with thousand separator
//                  2 - $1,000,000 add dollar sign, with thousand separator
//                  3 - 100% percentage
//                  (default : 1 )
//
// <int>   x2     = the x2 (right border) of the field
//
void Font::update_field(int x1, int y1, int value, int format, int x2)
{
	if( x2<0 )
      x2 = x1+80;

   put( x1+4, y1+2, m.format(value,format), 1, x2 );
}
//--------- End of function Font::update_field ---------//


//-------- Begin of function Font::field --------//
//
// Combine the function of put_field() and update_field()
// depending on the refreshFlag passed
//
// format - "Field : 999999" e.g. "List price : $92,232"
//
// <int>   x1,y1  = the coordination of the spinner description
// <char*> desStr = the spinner description
// <int>   x2     = the coordination of the spinner variable
// <int>   value  = value
//
// <int>   format = 1 - 1,000,000 normal, with thousand separator
//                  2 - $1,000,000 add dollar sign, with thousand separator
//                  3 - 100% percentage
//
// <int> x2          = the x2 (right border) of the field
// <int> refreshFlag = refresh flag, either INFO_REPAINT or INFO_UPDATE
// [char*] helpCode  = help code
//
void Font::field(int xDes, int y1, char* desStr, int xValue, int value,
					  int format, int xEnd, int refreshFlag, char* helpCode)
{
	int x2;

	if( refreshFlag == INFO_REPAINT )
	{
		vga_util.d3_panel_up( xDes, y1, xValue, y1+font_height+6 );

		// ####### begin Gilbert 10/2 ########//
		put( xDes+4  , y1+2, desStr, 0, xValue-4);
		// ####### end Gilbert 10/2 ########//
		x2 = put( xValue+4, y1+2, m.format(value,format) );
	}
	else
	{
		x2 = put( xValue+4, y1+2, m.format(value,format), 1, xEnd );
	}

	if( helpCode )
		help.set_help(xDes, y1, x2, y1+font_height+3, helpCode );
}
//--------- End of function Font::field ---------//


//-------- Begin of function Font::put_field --------//
//
// Display field description and field value
//
// format - "Field : 999999" e.g. "List price : $92,232"
//
// <int>    x1,y1  = the coordination of the spinner description
// <char*>  desStr = the spinner description
// <int>    x2     = the coordination of the spinner variable
// <double> value  = value
//
// [int]    format = 1 - 1,000,000 normal, with thousand separator
//                   2 - $1,000,000 add dollar sign, with thousand separator
//                   3 - 100% percentage
//                   (default : 1 )
//
void Font::put_field(int x1, int y1, char* desStr, int x2, double value, int format)
{
	vga_util.d3_panel_up( x1, y1, x2, y1+font_height+6 );

	// ####### begin Gilbert 10/2 ########//
	put( x1+4, y1+2, desStr, 0, x2-4 );
	// ####### begin Gilbert 10/2 ########//
	put( x2+4, y1+2, m.format(value,format) );
}
//--------- End of function Font::put_field ---------//


//-------- Begin of function Font::update_field --------//
//
// Update the value of the field which is previously displayed by
// Font::put_field()
//
// <int>   x1,y1  = the coordination of the field variable
// <double>  value  = value
//
// <int>   format = 1 - 1,000,000 normal, with thousand separator
//                  2 - $1,000,000 add dollar sign, with thousand separator
//                  3 - 100% percentage
//                  (default : 1 )
//
// <int>   x2     = the x2 (right border) of the field
//
void Font::update_field(int x1, int y1, double value, int format, int x2)
{
	if( x2<0 )
      x2 = x1+80;

   put( x1+4, y1+2, m.format(value,format), 1, x2 );
}
//--------- End of function Font::update_field ---------//


//-------- Begin of function Font::field --------//
//
// Combine the function of put_field() and update_field()
// depending on the refreshFlag passed
//
// format - "Field : 999999" e.g. "List price : $92,232"
//
// <int>   x1,y1  = the coordination of the spinner description
// <char*> desStr = the spinner description
// <int>   x2     = the coordination of the spinner variable
// <double>  value  = value
//
// <int>   format = 1 - 1,000,000 normal, with thousand separator
//                  2 - $1,000,000 add dollar sign, with thousand separator
//                  3 - 100% percentage
//
// <int> x2          = the x2 (right border) of the field
// <int> refreshFlag = refresh flag, either INFO_REPAINT or INFO_UPDATE
// [char*] helpCode  = help code
//
void Font::field(int xDes, int y1, char* desStr, int xValue, double value,
					  int format, int xEnd, int refreshFlag, char* helpCode )
{
	int x2;

   if( refreshFlag == INFO_REPAINT )
   {
		vga_util.d3_panel_up( xDes, y1, xValue, y1+font_height+6 );

		// ####### begin Gilbert 10/2 ########//
      put( xDes+4  , y1+2, desStr, 0, xValue-4);
		// ####### begin Gilbert 10/2 ########//
      x2 = put( xValue+4, y1+2, m.format(value,format) );
   }
   else
   {
		x2 = put( xValue+4, y1+2, m.format(value,format), 1, xEnd );
	}

	if( helpCode )
		help.set_help(xDes, y1, x2, y1+font_height+3, helpCode );
}
//--------- End of function Font::field ---------//


//-------- Begin of function Font::put_field --------//
//
// Display field description and field value
//
// format - "Field : 999999" e.g. "List price : $92,232"
//
// <int>   x1,y1  = the coordination of the spinner description
// <char*> desStr = the spinner description
// <int>   x2     = the coordination of the spinner variable
// <char*> value  = value
//
void Font::put_field(int x1, int y1, char* desStr, int x2, char* value)
{
	vga_util.d3_panel_up( x1, y1, x2, y1+font_height+6 );

	// ####### begin Gilbert 10/2 ########//
   put( x1+4, y1+2, desStr, 0, x2-4);
	// ####### end Gilbert 10/2 ########//
   put( x2+4, y1+2, value);
}
//--------- End of function Font::put_field ---------//


//-------- Begin of function Font::update_field --------//
//
// Update the value of the field which is previously displayed by
// Font::put_field()
//
// <int>   x1,y1  = the coordination of the field variable
// <char*> value  = value
// <int>   x2     = the x2 (right border) of the field
//
void Font::update_field(int x1, int y1, char* value, int x2)
{
	if( x2<0 )
      x2 = x1+80;

   put( x1+4, y1+2, value, 1, x2 );
}
//--------- End of function Font::update_field ---------//


//-------- Begin of function Font::field --------//
//
// Combine the function of put_field() and update_field()
// depending on the refreshFlag passed
//
// format - "Field : 999999" e.g. "List price : $92,232"
//
// <int>   x1,y1  = the coordination of the spinner description
// <char*> desStr = the spinner description
// <int>   x2     = the coordination of the spinner variable
// <char*> value  = value
//
// <int> x3          = the x3 (right border) of the field
// <int> refreshFlag = refresh flag, either INFO_REPAINT or INFO_UPDATE
// [char*] helpCode  = help code
//
void Font::field(int xDes, int y1, char* desStr, int xValue, char* value,
					  int xEnd, int refreshFlag, char* helpCode )
{
	int x2;

	if( refreshFlag == INFO_REPAINT )
	{
		vga_util.d3_panel_up( xDes, y1, xValue, y1+font_height+6 );

		// ####### begin Gilbert 10/2 ########//
		put( xDes+4  , y1+2, desStr, 0, xValue-4);
		// ####### begin Gilbert 10/2 ########//
		x2 = put( xValue+4, y1+2, value );
	}
	else
	{
		x2 = put( xValue+4, y1+2, value, 1, xEnd );
	}

	if( helpCode )
		help.set_help(xDes, y1, x2, y1+font_height+3, helpCode );
}
//--------- End of function Font::field ---------//


//-------- Begin of function Font::disp --------//
//
// Display a value and clear the background
//
// <int>   x1,y1  = the coordination of the field variable
// <int>   value  = value
//
// <int>   format = 1 - 1,000,000 normal, with thousand separator
//                  2 - $1,000,000 add dollar sign, with thousand separator
//                  3 - 100% percentage
//                  (default : 1 )
//
// <int>   x2     = the x2 (right border) of the field
//
// Return : <int> lastX, the x coordination of the last pixel of last font
//
int Font::disp(int x1, int y1, int value, int format, int x2)
{
	if( x2<0 )
      x2 = x1+80;

   int lastX = put( x1, y1, m.format(value,format), 1, x2 );

   return lastX;
}
//--------- End of function Font::disp ---------//


//-------- Begin of function Font::disp --------//
//
// Display a value and clear the background
//
// <int>    x1,y1  = the coordination of the field variable
// <double> value  = value
//
// <int>    format = 1 - 1,000,000 normal, with thousand separator
//                   2 - $1,000,000 add dollar sign, with thousand separator
//                   3 - 100% percentage
//                   (default : 1 )
//
// <int>   x2      = the x2 (right border) of the field
//
// Return : <int> lastX, the x coordination of the last pixel of last font
//
int Font::disp(int x1, int y1, double value, int format, int x2)
{
	if( x2<0 )
      x2 = x1+80;

   int lastX = put( x1, y1, m.format(value,format), 1, x2 );

   return lastX;
}
//--------- End of function Font::disp ---------//


//-------- Begin of function Font::disp --------//
//
// Display a value and clear the background
//
// <int>   x1,y1     = the coordination of the field variable
// <char*> textPtr   = pointer to the textPtr
// <int>   x2        = the x2 (right border) of the field
//
// Return : <int> lastX, the x coordination of the last pixel of last font
//
int Font::disp(int x1, int y1, char* textPtr, int x2, int cap)
{
	if( x2<0 )
      x2 = x1+80;

	int lastX = put( x1, y1, textPtr, 1, x2, cap );

   return lastX;
}
//--------- End of function Font::disp ---------//


//--------- Begin of function Font::put_char_to_buffer ---------//
//SXM: So it will no need to localize thsi function since it will be never called.
void Font::put_char_to_buffer(char* dest, int destPitch, int x, int y, unsigned short textChar)
{
	if( textChar >= first_char && textChar <= last_char )
	{
		FontInfo* fontInfo = font_info_array+textChar-first_char;
		char *fontBitmap = font_bitmap_buf + fontInfo->bitmap_offset;

		err_here();		// 8-8 bit not supported
		// IMGbltTrans( dest, destPitch, x, y + fontInfo->offset_y, fontBitmap );
	}
}
//--------- End of function Font::put_char_to_buffer ---------//


//--------- Begin of function Font::put_to_buffer ---------//
void Font::put_to_buffer(char* dest, int destPitch, int x1, int y1, char *text)
{
	int x2 = destPitch;			// width of buffer
	while( *text != '\0' && x1 < x2)
	{
		int charSize = sizeof(unsigned char);		// 1 for byte character, 2 for word character
		unsigned short textChar = *(unsigned char *)text;
	
		//if( // unicode or a word character )
		//{
		//	textChar = *(unsigned short *)buffer;			// unicode
		// textChar = *(unsigned char *)text << 8 + *(1+(unsigned char *)text);	// big-5...
		//	charSize = sizeof(unsigned short);
		//}
		//SXM
		if(textChar>160)
		{
			text++;
			textChar=textChar*0x100+*(unsigned char*)text;
		}
		//SXM
		// --------- advance to next character------------//

		//SXM
//		int charWidth = textChar == ' ' ? space_width :
//			(font_info_array+textChar-first_char)->width;
		int charWidth;
		if(textChar>0x100)
		{
			TChineseFont* pcf=GetChineseFont(this);
			charWidth=pcf->m_Width;
		}
		else
		{
			charWidth = textChar == ' ' ? space_width :
				(font_info_array+textChar-first_char)->width;
		}
		//SXM

		if( x1 + charWidth <= x2 )
			put_char_to_buffer( dest, destPitch, x1, y1, textChar);

		// ###### begin Gilbert 29/10 ######//
		x1 += charWidth + inter_char_space;
		// ###### end Gilbert 29/10 ######//
		text += charSize;
	}
}
//--------- End of function Font::put_to_buffer ---------//


//--------- Begin of function Font::center_put_to_buffer ---------//
void Font::center_put_to_buffer(char* dest, int destPitch, int x1, int y1, int x2, int y2, char *desStr)
{
	int tx = x1 + ((x2-x1) - text_width(desStr))/2;
   int ty = y1 + ((y2-y1) - font_height)/2;

   if( tx<0 )
		tx=0;

   put_to_buffer( dest, destPitch, tx, ty, desStr);
}
//--------- End of function Font::center_put_to_buffer ---------//


//--------- Begin of function Font::put_char_to_bufferW ---------//
void Font::put_char_to_bufferW(short* dest, int destTruePitch, int x, int y, unsigned short textChar)
{
	if( textChar >= 0x100 )
	{
		TChineseFont* pcf=GetChineseFont(this);

		memset(g_bChineseFontBitmap,HZ_BACKGROUND,25*25+sizeof(int)*2);
		g_bChineseFontBitmap[0]=pcf->m_Height+1;
		g_bChineseFontBitmap[1]=g_bChineseFontBitmap[3]=0;
		g_bChineseFontBitmap[2]=pcf->m_Width+1;
#if(!defined(TRADITIONAL))
		int FontOffset=(textChar/0x100-161)*94+textChar%0x100-161;
		int FontBytes=(pcf->m_Width+7);
		FontBytes=pcf->m_Height*(FontBytes/8);
		FontOffset*=FontBytes;
		if(pcf->m_Width==13)
		{
			FontOffset-=9688;
			if(FontOffset<0)
				FontOffset+=-9688+19984-520;
		}
		FontOffset+=-1;
#else
		int FontOffset=GetChineseFontOffset((char*)&textChar,pcf->m_Width);
#endif
		for(int i=0;i<pcf->m_Height;i++)
			for(int j=0;j<pcf->m_Width;j++)
			{
				if(j%8==0)
					FontOffset++;
				if(*(pcf->m_FontData+FontOffset)&(1<<(7-j%8)))
				{
					*(g_bChineseFontBitmap+4+i*(pcf->m_Width+1)+j)=HZ_FOREGROUND(Vga::active_buf);
					if(HZ_SHADOW(Vga::active_buf)!=255)
						*(g_bChineseFontBitmap+4+(i+m_ChineseShadowY)*(pcf->m_Width+1)+
							(j+m_ChineseShadowX))=HZ_SHADOW(Vga::active_buf);
				}
//				else
//				{
//					*(g_bChineseFontBitmap+4+i*(pcf->m_Width+1)+j)=HZ_BACKGROUND;
//				}
			}
		IMGbltTransRemap( dest, destTruePitch, x, y, (char*)g_bChineseFontBitmap, vga.default_remap_table );
	}
	if( textChar >= first_char && textChar <= last_char )
	{
		FontInfo* fontInfo = font_info_array+textChar-first_char;
		char *fontBitmap = font_bitmap_buf + fontInfo->bitmap_offset;

		IMGbltTransRemap( dest, destTruePitch, x, y + fontInfo->offset_y, fontBitmap, vga.default_remap_table );
	}
}
//--------- End of function Font::put_char_to_bufferW ---------//


//--------- Begin of function Font::put_to_bufferW ---------//
void Font::put_to_bufferW(short* dest, int destTruePitch, int x1, int y1, char *text)
{
	MakeSafeCopy(&text);
	BreakOnString(text);
	// ###### begin Gilbert 29/10 ######//
	int x2 = destTruePitch/2;			// width of buffer
	// ###### begin Gilbert 29/10 ######//
	while( *text != '\0' && x1 < x2)
	{
		int charSize = sizeof(unsigned char);		// 1 for byte character, 2 for word character
		unsigned short textChar = *(unsigned char *)text;
		//SXM
		if(textChar>160)
		{
			text++;
			textChar=textChar*0x100+*(unsigned char*)text;
		}
		//SXM
		// --------- advance to next character------------//

		//SXM
//		int charWidth = textChar == ' ' ? space_width :
//			(font_info_array+textChar-first_char)->width;
		int charWidth;
		if(textChar>0x100)
		{
			TChineseFont* pcf=GetChineseFont(this);
			charWidth=pcf->m_Width;
		}
		else
		{
			charWidth = textChar == ' ' ? space_width :
				(font_info_array+textChar-first_char)->width;
		}
		//SXM
		if( x1 + charWidth <= x2 )
			put_char_to_bufferW( dest, destTruePitch, x1, y1, textChar);

		// ###### begin Gilbert 29/10 ######//
		x1 += charWidth + inter_char_space;
		// ###### end Gilbert 29/10 ######//
		text += charSize;
	}
}
//--------- End of function Font::put_to_bufferW ---------//


//--------- Begin of function Font::center_put_to_bufferW ---------//
void Font::center_put_to_bufferW(short* dest, int destTruePitch, int x1, int y1, int x2, int y2, char *desStr)
{
	int tx = x1 + ((x2-x1) - text_width(desStr))/2;
   int ty = y1 + ((y2-y1) - font_height)/2;

   if( tx<0 )
		tx=0;

   put_to_bufferW( dest, destTruePitch, tx, ty, desStr);
}
//--------- End of function Font::center_put_to_bufferW ---------//

//--------- Begin of function Font::upper_case ---------//
short Font::upper_case(short textChar)
{
	//SXM: There are no upper case in Chinese
	if( textChar>0x100 )
		return textChar;
	//SXM
	if((textChar > 96) && (textChar < 123))
		return(toupper(textChar));
	else
	if((textChar > 223) && (textChar < 255) && (textChar != 247))
		return(textChar - 32);
	else
	if(textChar == 255)
		return 159;
	else
		return textChar;
}
//--------- End of function Font::upper_case ---------//		

#endif		// CHINESE
