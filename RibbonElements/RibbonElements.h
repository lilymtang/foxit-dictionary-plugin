// RibbonElements.h : main header file for the RibbonElements DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols

// CRibbonElementsApp
// See RibbonElements.cpp for the implementation of this class
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
