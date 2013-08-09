// IROM_Fusing_Tool.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CIROM_Fusing_ToolApp:
// See IROM_Fusing_Tool.cpp for the implementation of this class
//

class CIROM_Fusing_ToolApp : public CWinApp
{
public:
	CIROM_Fusing_ToolApp();

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CIROM_Fusing_ToolApp theApp;