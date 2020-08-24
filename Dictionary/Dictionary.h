// Dictionary.h : main header file for the Dictionary DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols

// CDictionaryApp
// See Dictionary.cpp for the implementation of this class
//

class CDictionaryApp : public CWinApp
{
public:
	CDictionaryApp();

	DECLARE_MESSAGE_MAP()
public:
	void CreateElementsToNewCategory();
	void AddMenuToFilePage();

	static FS_DIBitmap GetBmpFromRes(UINT uID);
};
