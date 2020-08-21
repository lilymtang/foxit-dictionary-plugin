/*********************************************************************

 Copyright (C) 2010 Foxit Corporation
 All rights reserved.

 NOTICE: Foxit permits you to use, modify, and distribute this file
 in accordance with the terms of the Foxit license agreement
 accompanying it. If you have received this file from a source other
 than Foxit, then your use, modification, or distribution of it
 requires the prior written permission of Foxit.

 ---------------------------------------------------------------------

RibbonElements.cpp

 - Defines the entry point for the DLL application, the entry point is PlugInMain.

*********************************************************************/

#include "stdafx.h"
#include "RibbonElements.h"
#include "keys.h"

#include <cpprest/http_client.h>
#include <cpprest/filestream.h>

using namespace web::http;                  // Common HTTP functionality
using namespace web::http::client;          // HTTP client features

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
//TODO: If this DLL is dynamically linked against the MFC DLLs,
//		any functions exported from this DLL which call into
//		MFC must have the AFX_MANAGE_STATE macro added at the
//		very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

// CDictionaryApp

BEGIN_MESSAGE_MAP(CDictionaryApp, CWinApp)
END_MESSAGE_MAP()


// CDictionaryApp construction

CDictionaryApp::CDictionaryApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

// The one and only CDictionaryApp object

CDictionaryApp theApp;

//////////////////////////////////////////////////////////////////////////
/* PIExportHFTs
** ------------------------------------------------------
**/
/** 
** Create and Add the extension HFT's.
**
** @return true to continue loading plug-in,
** false to cause plug-in loading to stop.
*/
FS_BOOL PIExportHFTs(void)
{
	return TRUE;
}

/** 
The application calls this function to allow it to

Import HFTs supplied by other plug-ins.
Replace functions in the HFTs you're using (where allowed).
Register to receive notification events.

*/
FS_BOOL PIImportReplaceAndRegister(void)
{
	return TRUE;
}

/* 
* Plug-ins can use their Initialization procedures to hook into Foxit Reader's 
* user interface by adding menu items, toolbar buttons, windows, and so on.
* It is also acceptable to modify Foxit Reader's user interface later when the plug-in is running.
*/	
void PILoadMenuBarUI(void* pParentWnd)
{

}

void PIReleaseMenuBarUI(void* pParentWnd)
{

}

void PILoadToolBarUI(void* pParentWnd)
{

}

void PIReleaseToolBarUI(void* pParentWnd)
{

}

void PILoadRibbonUI(void* pParentWnd)
{

}

void PILoadStatusBarUI(void* pParentWnd)
{

}

/* PIInit
** ------------------------------------------------------
**/
/** 
	The main initialization routine.
	
	@return true to continue loading the plug-in, 
	false to cause plug-in loading to stop.
*/
FS_BOOL PIInit(void)
{
	return TRUE;
}

pplx::task<int> requestTask(std::wstring searchWord, std::wstring* def)
{
	// Replace spaces with %20
	std::wstring wide;
	
	for (char ch : searchWord) {
		if (ch == ' ') {
			wide.append(U("%20"));
		}
		else {
			wide.push_back(ch);
		}
	}

	// Build URI
	http_client client(U("https://www.dictionaryapi.com/"));
	uri_builder builder;
	builder.append_path(U("api"));
	builder.append_path(U("v3"));
	builder.append_path(U("references"));
	builder.append_path(U("collegiate"));
	builder.append_path(U("json"));
	builder.append_path(wide);
	builder.append_query(U("key"), WEBSTER_KEY);
	utility::string_t request_uri = builder.to_string();
	return client.request(methods::GET, request_uri)

	.then([=](http_response response)
	{
		std::wstringstream buffer;

		// Main response processing 
		if (response.status_code() == status_codes::OK) {
			web::json::value body = response.extract_json().get();

			// Check if search word is in dictionary by checking if body is an array of objects
			if (body.at(0).is_object()) {

				// Use head word as the displayed word going forward
				// Strip head word of potential trailing numbers ie. jump:1
				std::wstring headWord = body.at(0).at(U("meta")).at(U("id")).as_string();
				headWord.erase(remove(headWord.begin(), headWord.end(), ':'), headWord.end());
				headWord.erase(std::remove_if(headWord.begin(), headWord.end(), &isdigit), headWord.end());

				// Print each entry in word
				for (int i = 0; i < body.as_array().size(); i++) {
					web::json::value def = body.at(i);

					// Strip entry word
					std::wstring entryWord = def.at(U("meta")).at(U("id")).as_string();
					entryWord.erase(remove(entryWord.begin(), entryWord.end(), ':'), entryWord.end());
					entryWord.erase(std::remove_if(entryWord.begin(), entryWord.end(), &isdigit), entryWord.end());

					int entry_num = i + 1;

					// Print entry if entry head word matches head word
					if (entryWord == headWord) {

						// Write entry # to buffer
						if (body.as_array().size() > 1) {
							buffer << U("Entry") << ' ' << entry_num << U(": ");
						}
						buffer << headWord << ' '
							<< U("(") << def.at(U("fl")).as_string() << U(")")
							<< std::endl;

						// Print each definition in entry
						for (int k = 0; k < def.at(U("shortdef")).as_array().size(); k++) {
							int def_num = k + 1;
							auto shortdef = def.at(U("shortdef")).as_array().at(k);
							buffer << def_num << U(": ") << shortdef.as_string() << std::endl;
						}
					}

					// Skip rest of entries if entry head word doesn't match head word
					else {
						break;
					}
				}
				*def = buffer.str();
				return TRUE;
			}
		}

		// If main response processing fails, return error message
		buffer << "No results found." << std::endl;
		*def = buffer.str();
		return FALSE;
	});
}

bool requestTaskWrap(const wchar_t* searchWord, std::wstring* def) {
	try
	{
		return requestTask(std::wstring(searchWord), def).get();
	}
	catch (const std::exception &e)
	{
		*def = U("No results found.");
		return FALSE;
	}
}

/* 
* Function: AddHighlightAnnot(FR_PageView frPageView, FS_FloatRect rect, FS_LPCWSTR displayText) {
* Description: Creates a highlight location at the specified rect and add the text to the highlight annotation
* Parameter: rect - represents the location of where the highlight annotation will be
*            displayText - represent the text
* Return: void
*/
void AddHighlightAnnot(FR_PageView frPageView, FS_FloatRect rect, FS_LPCWSTR displayText) {

	FPD_Object fpdObject = FPDDictionaryNew();
	FS_ByteString strtype = FSByteStringNew3("Annot", -1);
	FPD_Object fpdfstringAnnot = FPDStringNew(strtype, 0);
	FPDDictionaryAddValue(fpdObject, "Type", fpdfstringAnnot);
	FS_ByteString strSubtype = FSByteStringNew3("Highlight", -1);
	FPD_Object fpdfstringSubtype = FPDStringNew(strSubtype, 0);
	FPDDictionaryAddValue(fpdObject, "Subtype", fpdfstringSubtype);
	FPDDictionaryAddValue(fpdObject, "Subj", FPDStringNew(FSByteStringNew3("Highlight", -1), 0));

	FPD_Object quad = FPDArrayNew(); //{x1, y1, x2, y2, x3, y3, x4, y5}
	FPDArrayAddNumber(quad, rect.left);
	FPDArrayAddNumber(quad, rect.top); 
	FPDArrayAddNumber(quad, rect.right);
	FPDArrayAddNumber(quad, rect.top);
	FPDArrayAddNumber(quad, rect.left); 
	FPDArrayAddNumber(quad, rect.bottom);
	FPDArrayAddNumber(quad, rect.right); 
	FPDArrayAddNumber(quad, rect.bottom);
	FPDDictionaryAddValue(fpdObject, "QuadPoints", quad); 

	FPD_Object quadColor = FPDArrayNew();
	FPDArrayAddNumber(quadColor, 1); //1 means RGB Color
	FPDArrayAddNumber(quadColor, 0.929412); //Defines the color
	FPDArrayAddNumber(quadColor, 0); //Defines the color
	FPDDictionaryAddValue(fpdObject, "C", quadColor);

	FPD_Object fpdfstringPopout = FPDStringNewW(displayText); // Create string object from wide string
	FPDDictionaryAddValue(fpdObject, "Contents", fpdfstringPopout);
	FPDDictionarySetAtRect(fpdObject, "Rect", rect);
	FR_Annot frAnnot = FRPageViewAddAnnot(frPageView, fpdObject, 0); //Adds the highlight annotation
}

// This function triggers when user selects the context menu item "Dictionary"
void DictionaryExecuteProc(void* clientData) {
	FR_Document frDocument = FRAppGetActiveDocOfPDDoc(); 
	FR_DocView frDocView = FRDocGetCurrentDocView(frDocument);
	FR_PageView frPageView = FRDocViewGetCurrentPageView(frDocView);

	// Store selected text
	FR_TextSelectTool textSelectTool = FRDocGetTextSelectTool(frDocument);
	FS_WideString outText = FSWideStringNew();
	FRTextSelectToolGetSelectedText(textSelectTool, &outText);
	FSWideStringRemove(outText, '\r');
	FSWideStringReplace(outText, FSWideStringNew2('\n'), FSWideStringNew2(' '));
	
	// Execute dictionary function and save returned text
	std::wstring def;
	bool success = requestTaskWrap((const wchar_t*)FSWideStringCastToLPCWSTR(outText), &def);
	FS_LPCWSTR displayText = (FS_LPCWSTR)def.c_str();

	// Show option to add annotation if requestTask returns success
	if (success) {
		 FS_INT32 msgBoxId = FRSysShowMessageBox(displayText, MB_OKCANCEL | MB_ICONINFORMATION, (FS_LPCWSTR)L"Merriam-Webster's Collegiate® Dictionary", NULL, FRAppGetMainFrameWnd());

		 switch (msgBoxId) {
		 case IDOK:
			 // Add highlight annotation and highlight text
			 FS_FloatRect* fsFloatRect = new FS_FloatRect();
			 FS_INT32 nCount = 0;
			 FRTextSelectToolEnumTextObjectRect(textSelectTool, FRPageViewGetPageIndex(frPageView), NULL, &nCount);
			 FRTextSelectToolEnumTextObjectRect(textSelectTool, FRPageViewGetPageIndex(frPageView), &fsFloatRect, &nCount);
			 AddHighlightAnnot(frPageView, *fsFloatRect, displayText);
			 break;
		 }
	}
	else {
		 FRSysShowMessageBox(displayText, MB_OK | MB_ICONWARNING, (FS_LPCWSTR)L"Merriam-Webster's Collegiate® Dictionary", NULL, FRAppGetMainFrameWnd());
	}
}

// This function triggers when the context menu appears
void DictionaryProc(void *pClientData, FR_Menu popUpMenu)
{
	int nMenuItemCount = FRMenuGetMenuItemCount(popUpMenu);

	FR_Document frDocument = FRAppGetActiveDocOfPDDoc();
	FR_DocView frDocView = FRDocGetCurrentDocView(frDocument);
	FR_PageView frPageView = FRDocViewGetCurrentPageView(frDocView);

	// Store selected text
	FR_TextSelectTool textSelectTool = FRDocGetTextSelectTool(frDocument);
	FS_WideString outText = FSWideStringNew();

	// Strip selected text and construct menu text
	FS_WideString menuText1 = FSWideStringNew3((FS_LPCWSTR)L"Look Up \"", -1);
	FS_WideString menuText2;
	FRTextSelectToolGetSelectedText(textSelectTool, &outText);
	FSWideStringRemove(outText, '\r');
	FSWideStringReplace(outText, FSWideStringNew2('\n'), FSWideStringNew2(' '));
	FS_WideString outTextTrunc = FSWideStringNew();
	FSWideStringLeft(outText, sizeof(WCHAR) * 15, &outTextTrunc);
	FSWideStringConcat(menuText1, outTextTrunc);
	if (FSWideStringCompare2(outTextTrunc, outText) == 0) {
		menuText2 = FSWideStringNew2('\"');
	}
	else {
		menuText2 = FSWideStringNew3((FS_LPCWSTR)L"...\"", -1);
	}
	FSWideStringConcat(menuText1, menuText2);

	// Add menu item and set action
	FR_MenuItem menuItem = FRMenuItemNew((const char *)L"Look Up", FSWideStringCastToLPCWSTR(menuText1), NULL, false, NULL);
	FRMenuAddMenuItem(popUpMenu, menuItem, nMenuItemCount); 
	FRMenuItemSetExecuteProc(menuItem, &DictionaryExecuteProc); 
}


/** PIUnload
	------------------------------------------------------
	The unload routine.
	Called when your plug-in is being unloaded when the application quits.
	Use this routine to release any system resources you may have
	allocated.

	Returning false will cause an alert to display that unloading failed.
	@return true to indicate the plug-in unloaded.
*/
FS_BOOL PIUnload(void)
{	
	return TRUE;
}

/** PIHandshake function provides the initial interface between your plug-in and the application.
	This function provides the callback functions to the application that allow it to 
	register the plug-in with the application environment.

	Required Plug-in handshaking routine:
	
	@param handshakeVersion the version this plug-in works with. 
	@param handshakeData OUT the data structure used to provide the primary entry points for the plug-in. These
	entry points are used in registering the plug-in with the application and allowing the plug-in to register for 
	other plug-in services and offer its own.
	@return true to indicate success, false otherwise (the plug-in will not load).
*/
FS_BOOL PIHandshake(FS_INT32 handshakeVersion, void *handshakeData)
{
if(handshakeVersion != HANDSHAKE_V0100)
return FALSE;

PIHandshakeData_V0100* pData = (PIHandshakeData_V0100*)handshakeData;
pData->PIHDRegisterPlugin(pData, "RibbonElements", (FS_LPCWSTR)L"RibbonElements");
pData->PIHDSetExportHFTsCallback(pData, &PIExportHFTs);
pData->PIHDSetImportReplaceAndRegisterCallback(pData, &PIImportReplaceAndRegister);

// Plugin initialization 
pData->PIHDSetInitDataCallback(pData, &PIInit);
PIInitUIProcs initUIProcs;
INIT_CALLBACK_STRUCT(&initUIProcs, sizeof(PIInitUIProcs));
initUIProcs.lStructSize = sizeof(PIInitUIProcs);
initUIProcs.PILoadMenuBarUI = PILoadMenuBarUI;
initUIProcs.PIReleaseMenuBarUI = PIReleaseMenuBarUI;
initUIProcs.PILoadToolBarUI = PILoadToolBarUI;
initUIProcs.PIReleaseToolBarUI = PIReleaseToolBarUI;
initUIProcs.PILoadRibbonUI = PILoadRibbonUI;
initUIProcs.PILoadStatusBarUI = PILoadStatusBarUI;
pData->PIHDSetInitUICallbacks(pData, &initUIProcs);

//Here is were we add the Dictionary item to the TextSelection Context Menu
FRAppRegisterForContextMenuAddition("Select",&DictionaryProc,"clientData");

pData->PIHDSetUnloadCallback(pData, &PIUnload);

return TRUE;
}