#pragma once

#ifndef __AFXWIN_H__
#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols
#include "stdafx.h"
#include "Dictionary.h"
#include "keys.h"

#include <cpprest/http_client.h>
#include <cpprest/filestream.h>

std::wstring WEBSTER_KEY = L"[your key here]";