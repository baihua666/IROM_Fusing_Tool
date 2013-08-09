// IROM_Fusing_ToolDlg.cpp : implementation file
//

#include "stdafx.h"
#include "IROM_Fusing_Tool.h"
#include "IROM_Fusing_ToolDlg.h"
#include <WinIoCtl.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define EFUSE_RESEVED_SECTOR (2)
//#define SDHC_SHIFT_SECTOR (1024) // it's for SDHC card
#define SDHC_SHIFT_SECTOR (0) // it's for some SD/MMC cards that don't need any sector shift.
#define VOLUMEDISKSIZE (sizeof(VOLUME_DISK_EXTENTS))

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CIROM_Fusing_ToolDlg dialog




CIROM_Fusing_ToolDlg::CIROM_Fusing_ToolDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CIROM_Fusing_ToolDlg::IDD, pParent)
	, m_strCombo(_T(""))
	, m_dwDriveNumber(0)
	, m_strDriveSize(_T(""))
	, m_strFilePath(_T(""))
	, m_dwTotalSector(0)
	, m_pbWriteBuffer(NULL)
	, m_dwStartWriteSector(0)
	, m_strFromSector(_T(""))
	, m_strToSector(_T(""))
	, m_strTargetDrive(_T(""))
	, m_nPhysicalNumber(0)
	, m_nStartSector(1)
	,m_bIsWIN7(FALSE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CIROM_Fusing_ToolDlg::DoDataExchange(CDataExchange* pDX)
{
	DDX_CBString(pDX, IDC_COMBO2, m_strCombo);
	DDX_Text(pDX, IDC_EDIT_SIZE, m_strDriveSize);
	DDX_Text(pDX, IDC_EDIT1, m_strFilePath);
	DDX_Text(pDX, IDC_EDIT3, m_strFromSector);
	DDX_Text(pDX, IDC_EDIT4, m_strToSector);
	DDX_Text(pDX, IDC_EDIT5, m_strTargetDrive);
	DDX_Text(pDX, IDC_EDIT_STARTSECTOR, m_nStartSector);
}

BEGIN_MESSAGE_MAP(CIROM_Fusing_ToolDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_EN_CHANGE(IDC_EDIT3, &CIROM_Fusing_ToolDlg::OnEnChangeEdit3)
	ON_BN_CLICKED(ID_BUTTON_START, &CIROM_Fusing_ToolDlg::OnBnClickedButtonStart)
	ON_EN_CHANGE(IDC_EDIT_SIZE, &CIROM_Fusing_ToolDlg::OnEnChangeEditSize)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE, &CIROM_Fusing_ToolDlg::OnBnClickedButtonBrowse)
	ON_EN_CHANGE(IDC_EDIT5, &CIROM_Fusing_ToolDlg::OnEnChangeEdit5)
	ON_CBN_SELCHANGE(IDC_COMBO2, &CIROM_Fusing_ToolDlg::OnCbnSelchangeCombo2)

	ON_WM_DEVICECHANGE()
END_MESSAGE_MAP()


// CIROM_Fusing_ToolDlg message handlers

BOOL CIROM_Fusing_ToolDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	SetWindowText(L"九鼎创展SD卡烧写工具");
	GetMySystemVersion();

	// TODO: Add extra initialization here
	int length = 0;
	Fresource_driveinfo( length );
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CIROM_Fusing_ToolDlg::GetMySystemVersion()
{
	OSVERSIONINFO osvi;
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx (&osvi);
	if( (osvi.dwMajorVersion == 5)  && (osvi.dwMinorVersion == 1))
	{
		m_bIsWIN7 = FALSE;
	}
	if( (osvi.dwMajorVersion == 6) && (osvi.dwMinorVersion == 1))
	{
		m_bIsWIN7 = TRUE;
	}
	
}

void CIROM_Fusing_ToolDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CIROM_Fusing_ToolDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CIROM_Fusing_ToolDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CIROM_Fusing_ToolDlg::OnEnChangeEdit3()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}

void CIROM_Fusing_ToolDlg::OnCbnSelchangeComboDriveName()
{

}

void CIROM_Fusing_ToolDlg::OnBnClickedButtonStart()
{
	UpdateData(TRUE);

	if (m_nStartSector < 1 || m_nStartSector > m_dwTotalSector)
	{
		MessageBox(TEXT("StartSector ERROR"),TEXT("ERROR"),NULL);
		return ;
	}
	char *pMBRBuffer = NULL;
	wchar_t temp[6];
	//wchar_t temp[20];
	wchar_t sectors[9];

	CString strMBR;

	if (!ChangeStringDriveNameToDriveNumber())
	{
		MessageBox(TEXT("Choose SDMMC card drvie"),TEXT("ERROR"),NULL);
		return ;
	}

	if( (m_nPhysicalNumber = GetPhysicalDriveNumber( char((char)(m_dwDriveNumber)+97) )) == -1)
	{
		
		MessageBox(TEXT("Can't Read Physic Drive Name"),TEXT("ERROR"),NULL);
		return ;
	}

	pMBRBuffer = ReadSector(0,1); // read MBR sector.
	if (pMBRBuffer == NULL )
	{
		MessageBox(TEXT("Can't Read BR sector from Drvie ")+m_strCombo,TEXT("ERROR"),NULL);
		return ;
	}
	else
	{
		temp[0] = pMBRBuffer[0x52];
		temp[1] = pMBRBuffer[0x53];
		temp[2] = pMBRBuffer[0x54];
		temp[3] = pMBRBuffer[0x55];
		temp[4] = pMBRBuffer[0x56];
		temp[5] = '\0';

		strMBR = temp;

		if ( strMBR != "FAT32")
		{
			MessageBox(TEXT("This Drive is not formatted as FAT32")+strMBR,TEXT("ERROR"),NULL);
			exit(-1);
		}

	}

	if (!GetTotalSector())
	{
		MessageBox(TEXT("Can not Get Whole Sector size"),TEXT("ERROR"),NULL);
		return ;
	}

	if (!m_fileImageFile.Open(m_strFilePath,CFile::modeRead))
	{
		MessageBox(TEXT("Can not Open the ")+m_strFilePath,TEXT("ERROR"),NULL);
		return ;
	}

	//m_dwStartWriteSector = m_dwTotalSector - EFUSE_RESEVED_SECTOR - SDHC_SHIFT_SECTOR - ((DWORD)m_fileImageFile.GetLength() / 512);
	m_dwStartWriteSector = m_nStartSector ;
	m_pbWriteBuffer = (char *)malloc((UINT)m_fileImageFile.GetLength());

	if (m_pbWriteBuffer == NULL)
	{
		MessageBox(TEXT("Can not allocate write buffer"),TEXT("ERROR"),NULL);
		return ;
	}

	if (!m_fileImageFile.Read(m_pbWriteBuffer,(UINT)m_fileImageFile.GetLength()))
	{
		MessageBox(TEXT("Can not Read the file"),TEXT("ERROR"),NULL);
		return ;
	}

	swprintf(sectors,9,L"%d",m_dwStartWriteSector);	
	m_strFromSector = sectors;
	swprintf(sectors,9,L"%d",m_dwStartWriteSector+((DWORD)m_fileImageFile.GetLength() / 512));	
	//swprintf(sectors,9,L"%d",((DWORD)m_fileImageFile.GetLength() / 512));	
	m_strToSector = sectors;
	m_strTargetDrive = m_strCombo;
	UpdateData(FALSE);
	
	if (!WriteSector(m_dwStartWriteSector, (UINT)m_fileImageFile.GetLength()/512,m_pbWriteBuffer))
	{
		MessageBox(TEXT("Can not Write image file to SD card"),TEXT("ERROR"),NULL);
		//exit(1) ;
	}
	m_fileImageFile.Close();

	MessageBox(TEXT("Fusing image done"),TEXT("NOTICE"),NULL);

	//exit(-1);


}


int CIROM_Fusing_ToolDlg::ChangeStringDriveNameToDriveNumber(void)
{
	DWORD dwReturnValue = TRUE;

	UpdateData(TRUE);	

	if ( m_strCombo == "D")
		m_dwDriveNumber = 3; // driver number [0=A, 1=B, 2=C, 3=D ....] 
	else if ( m_strCombo == "E")
		m_dwDriveNumber = 4; // driver number [0=A, 1=B, 2=C, 3=D ....] 
	else if ( m_strCombo == "F")
		m_dwDriveNumber = 5; // driver number [0=A, 1=B, 2=C, 3=D ....] 
	else if ( m_strCombo == "G")
		m_dwDriveNumber = 6; // driver number [0=A, 1=B, 2=C, 3=D ....] 
	else if ( m_strCombo == "H")
		m_dwDriveNumber = 7; // driver number [0=A, 1=B, 2=C, 3=D ....] 
	else if ( m_strCombo == "I")
		m_dwDriveNumber = 8; // driver number [0=A, 1=B, 2=C, 3=D ....] 
	else if ( m_strCombo == "J")
		m_dwDriveNumber = 9; // driver number [0=A, 1=B, 2=C, 3=D ....] 
	else if ( m_strCombo == "K")
		m_dwDriveNumber = 10; // driver number [0=A, 1=B, 2=C, 3=D ....] 
	else if ( m_strCombo == "L")
		m_dwDriveNumber = 11; // driver number [0=A, 1=B, 2=C, 3=D ....] 
	else if ( m_strCombo == "M")
		m_dwDriveNumber = 12; // driver number [0=A, 1=B, 2=C, 3=D ....] 
	else if ( m_strCombo == "N")
		m_dwDriveNumber = 13; // driver number [0=A, 1=B, 2=C, 3=D ....] 
	else if ( m_strCombo == "O")
		m_dwDriveNumber = 14; // driver number [0=A, 1=B, 2=C, 3=D ....] 
	else if ( m_strCombo == "P")
		m_dwDriveNumber = 15; // driver number [0=A, 1=B, 2=C, 3=D ....] 
	else if ( m_strCombo == "Q")
		m_dwDriveNumber = 16; // driver number [0=A, 1=B, 2=C, 3=D ....] 
	else if ( m_strCombo == "R")
		m_dwDriveNumber = 17; // driver number [0=A, 1=B, 2=C, 3=D ....] 
	else if ( m_strCombo == "S")
		m_dwDriveNumber = 18; // driver number [0=A, 1=B, 2=C, 3=D ....] 
	else if ( m_strCombo == "T")
		m_dwDriveNumber = 19; // driver number [0=A, 1=B, 2=C, 3=D ....] 
	else if ( m_strCombo == "U")
		m_dwDriveNumber = 20; // driver number [0=A, 1=B, 2=C, 3=D ....] 
	else if ( m_strCombo == "V")
		m_dwDriveNumber = 21; // driver number [0=A, 1=B, 2=C, 3=D ....] 
	else if ( m_strCombo == "W")
		m_dwDriveNumber = 22; // driver number [0=A, 1=B, 2=C, 3=D ....] 
	else if ( m_strCombo == "X")
		m_dwDriveNumber = 23; // driver number [0=A, 1=B, 2=C, 3=D ....] 
	else if ( m_strCombo == "Y")
		m_dwDriveNumber = 24; // driver number [0=A, 1=B, 2=C, 3=D ....] 
	else if ( m_strCombo == "Z")
		m_dwDriveNumber = 25; // driver number [0=A, 1=B, 2=C, 3=D ....] 
	else
		dwReturnValue = FALSE;

	return dwReturnValue;
}

char * CIROM_Fusing_ToolDlg::ReadSector(int startsector, int numberofsectors)
{
	// All msdos data structures must be packed on a 1 byte boundary
	#pragma pack (1)
	struct
	{
		DWORD StartingSector ;
		WORD NumberOfSectors ;
		DWORD pBuffer;
	}ControlBlock;
	#pragma pack ()

	#pragma pack (1)
	typedef struct _DIOC_REGISTERS
	{
		DWORD reg_EBX;
		DWORD reg_EDX;
		DWORD reg_ECX;
		DWORD reg_EAX;
		DWORD reg_EDI;
		DWORD reg_ESI;
		DWORD reg_Flags;
	} DIOC_REGISTERS ;
	#pragma pack ()

	char* buffer = (char*)malloc (512*numberofsectors);
	HANDLE hDevice ;
	DIOC_REGISTERS reg ;
	BOOL  fResult ;
	DWORD cb ;

	// Creating handle to vwin32.vxd (win 9x)
	hDevice = CreateFile ( TEXT("\\\\.\\vwin32"),
			0,
			0,
			NULL,
			0,
			FILE_FLAG_DELETE_ON_CLOSE,
			NULL );

	if ( hDevice == INVALID_HANDLE_VALUE )
	{
		// win NT/2K/XP code
		HANDLE hDevice;
		DWORD bytesread;

		wchar_t _devicename[] = TEXT("\\\\.\\A:");
		_devicename[4] += m_dwDriveNumber;

		// Creating a handle to disk drive using CreateFile () function ..
		hDevice = CreateFile(_devicename,
				GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
				NULL, OPEN_EXISTING, 0, NULL);

		if (hDevice == INVALID_HANDLE_VALUE)
		{
			free(buffer);
			buffer = NULL;
			goto finish;
		}

		// Setting the pointer to point to the start of the sector we want to read ..
		SetFilePointer (hDevice, (startsector*512), NULL, FILE_BEGIN);

		if (!ReadFile (hDevice, buffer, 512*numberofsectors, &bytesread, NULL) )
		{
			free(buffer);
			buffer = NULL;
			goto finish;
		}

	}
	else
	{
		// code for win 95/98
		ControlBlock.StartingSector = (DWORD)startsector;
		ControlBlock.NumberOfSectors = (WORD)numberofsectors ;
		ControlBlock.pBuffer = (DWORD)buffer ;

		//-----------------------------------------------------------
		// SI contains read/write mode flags
		// SI=0h for read and SI=1h for write
		// CX must be equal to ffffh for
		// int 21h's 7305h extention
		// DS:BX -> base addr of the
		// control block structure
		// DL must contain the drive number
		// (01h=A:, 02h=B: etc)
		//-----------------------------------------------------------

		reg.reg_ESI = 0x00 ;
		reg.reg_ECX = -1 ;
		reg.reg_EBX = (DWORD)(&ControlBlock);
		reg.reg_EDX = m_dwDriveNumber+1;
		reg.reg_EAX = 0x7305 ;

		//  6 == VWIN32_DIOC_DOS_DRIVEINFO
		fResult = DeviceIoControl ( hDevice,
		6,
		&(reg),
		sizeof (reg),
		&(reg),
		sizeof (reg),
		&cb,
		0);

		if (!fResult || (reg.reg_Flags & 0x0001)) return NULL;
	}
finish:
	CloseHandle(hDevice);
	return buffer;
}



void CIROM_Fusing_ToolDlg::OnEnChangeEditSize()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}

void CIROM_Fusing_ToolDlg::OnBnClickedButtonBrowse()
{

	CFileDialog dlgFileOpen(TRUE,TEXT("bin"),NULL,OFN_FILEMUSTEXIST,
		TEXT("Image(*.bin)|*.bin|All(*.*)|*.*||"),NULL);

	if (dlgFileOpen.DoModal() == IDOK)
	{
		m_strFilePath = dlgFileOpen.GetPathName();
		UpdateData(FALSE);
	}
}

int CIROM_Fusing_ToolDlg::GetTotalSector(void)
{
	struct _DISK_GEOMETRY_EX 
	{	DISK_GEOMETRY  Geometry;  
		LARGE_INTEGER  DiskSize;  
		UCHAR  Data[1];
	} DiskEX;

	wchar_t temp2[20];
	HANDLE hDevice;
	DWORD bytesReturned;
	DWORD totalSector = 0;
	DWORD bRet;

	wchar_t _devicename[] = TEXT("\\\\.\\A:");
	_devicename[4] += m_dwDriveNumber;

	// Creating a handle to disk drive using CreateFile () function ..
	hDevice = CreateFile(_devicename,
			GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL, OPEN_EXISTING, 0, NULL);

	if (hDevice == INVALID_HANDLE_VALUE)
	{
		m_strDriveSize = "";
		m_dwTotalSector = 0;
		UpdateData(FALSE);
		return FALSE;
	}
	bRet = DeviceIoControl(
		  hDevice,              // handle to device
		  IOCTL_DISK_GET_DRIVE_GEOMETRY_EX,    // dwIoControlCode
		  NULL,                          // lpInBuffer
		  0,                             // nInBufferSize
		  (LPVOID) &DiskEX,          // output buffer
		  sizeof(DiskEX),        // size of output buffer
		  (LPDWORD) &bytesReturned ,     // number of bytes returned
		  NULL    // OVERLAPPED structure
	);

	if (bRet == 0)
	{	
		return FALSE;
	}

	if ( DiskEX.DiskSize.HighPart != 0 )
	{
		totalSector = ((DiskEX.DiskSize.HighPart) << 23);
	}
	totalSector |= (DiskEX.DiskSize.LowPart >> 9);


	swprintf(temp2,20,L"%d sectors",totalSector);	
	m_strDriveSize = temp2;
	UpdateData(FALSE);
	m_dwTotalSector = totalSector;

	return TRUE;
}

int CIROM_Fusing_ToolDlg::WriteSector(int startsector, int numberofsectors, char *buffer)
{
	wchar_t temp2[20];
	LARGE_INTEGER startbytes;
	CString StringTemp;

	// All msdos data structures must be packed on a 1 byte boundary
	#pragma pack (1)
	struct
	{
		DWORD StartingSector ;
		WORD NumberOfSectors ;
		DWORD pBuffer;
	}ControlBlock;
	#pragma pack ()

	#pragma pack (1)
	typedef struct _DIOC_REGISTERS
	{
		DWORD reg_EBX;
		DWORD reg_EDX;
		DWORD reg_ECX;
		DWORD reg_EAX;
		DWORD reg_EDI;
		DWORD reg_ESI;
		DWORD reg_Flags;
	} DIOC_REGISTERS ;
	#pragma pack ()

	HANDLE hDevice ;
	DIOC_REGISTERS reg ;
	BOOL  fResult ;
	DWORD cb ;

	// Creating handle to vwin32.vxd (win 9x)
	hDevice = CreateFile ( TEXT("\\\\.\\vwin32"),
			0,
			0,
			NULL,
			0,
			FILE_FLAG_DELETE_ON_CLOSE,
			NULL );

	if ( hDevice == INVALID_HANDLE_VALUE )
	{
		// win NT/2K/XP code
		HANDLE hDevice;
		DWORD byteswrite;

		wchar_t _devicename[30];

		if (m_nPhysicalNumber == 0 )
			return FALSE;

		swprintf(_devicename,30,TEXT("\\\\.\\PhysicalDrive%d"), m_nPhysicalNumber);

		// Creating a handle to disk drive using CreateFile () function ..
		hDevice = CreateFile(_devicename,
				GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
				NULL, OPEN_EXISTING, 0 , NULL);

		if (hDevice == INVALID_HANDLE_VALUE)
			return FALSE;

		startbytes.LowPart = ((startsector&0x7fffff) << 9);
		if (startsector&0xff800000)
			startbytes.HighPart = ((startsector&0xff800000)>>23);
		else
			startbytes.HighPart = 0;

		// Setting the pointer to point to the start of the sector we want to read ..
		SetFilePointerEx (hDevice, startbytes, NULL, FILE_BEGIN);
		//SetFilePointer (hDevice, ((startsector&0x7fffff) << 9), NULL, FILE_BEGIN);
		if(!m_bIsWIN7)
		{
			if (!WriteFile (hDevice, buffer, 512*numberofsectors, &byteswrite, NULL) )
			{	
				//int err = GetLastError();
				CloseHandle(hDevice);
				return FALSE;
			}
		}
		else//win7
		{
			UnMountVolume(m_strCombo.GetAt(0));

			if (!WriteFile (hDevice, buffer, 512*numberofsectors, &byteswrite, NULL) )
			{	
				int err = GetLastError();
				CloseHandle(hDevice);
				return FALSE;
			}
			MountVolume(m_strCombo.GetAt(0));
		}

		
		
		//if (!WriteFileEx(hDevice, buffer, 512*numberofsectors, &byteswrite, NULL) )
		//	return FALSE;

		if (byteswrite != (512*numberofsectors))
		{
			swprintf(temp2,20,L"%d sectors",byteswrite);
			StringTemp = temp2;
			MessageBox((TEXT("Only copy ")+StringTemp),TEXT("ERROR"),NULL);
			return FALSE;
		}
	}
	else
	{
		// code for win 95/98
		ControlBlock.StartingSector = (DWORD)startsector;
		ControlBlock.NumberOfSectors = (WORD)numberofsectors ;
		ControlBlock.pBuffer = (DWORD)buffer ;

		//-----------------------------------------------------------
		// SI contains read/write mode flags
		// SI=0h for read and SI=1h for write
		// CX must be equal to ffffh for
		// int 21h's 7305h extention
		// DS:BX -> base addr of the
		// control block structure
		// DL must contain the drive number
		// (01h=A:, 02h=B: etc)
		//-----------------------------------------------------------

		reg.reg_ESI = 0x00 ;
		reg.reg_ECX = -1 ;
		reg.reg_EBX = (DWORD)(&ControlBlock);
		reg.reg_EDX = m_dwDriveNumber+1;
		reg.reg_EAX = 0x7305 ;

		//  6 == VWIN32_DIOC_DOS_DRIVEINFO
		fResult = DeviceIoControl ( hDevice,
		6,
		&(reg),
		sizeof (reg),
		&(reg),
		sizeof (reg),
		&cb,
		0);

		if (!fResult || (reg.reg_Flags & 0x0001)) return FALSE;
	}

	CloseHandle(hDevice);
	return TRUE;
}

void CIROM_Fusing_ToolDlg::InitMemberVariables(void)
{
	m_dwDriveNumber = 0x100; 
	m_dwStartWriteSector = 0x0;
	m_dwTotalSector = 0;
	if (m_pbWriteBuffer != NULL)
	{
		free(m_pbWriteBuffer);
		m_pbWriteBuffer = NULL;
	}
	m_strCombo.Empty();
	m_strDriveSize.Empty();
	m_strFilePath.Empty();
	m_fileImageFile.Close();
}

void CIROM_Fusing_ToolDlg::OnEnChangeEdit5()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}


int CIROM_Fusing_ToolDlg::GetPhysicalDriveNumber( char cDriveName )
{
    HANDLE hDevice;
    DWORD dwOut;
    BOOL bRet;
    wchar_t vcDriveName[ 40 ];
    VOLUME_DISK_EXTENTS* pstVolumeData;
    int iDiskNumber; 

    // alloc memory
    pstVolumeData = ( VOLUME_DISK_EXTENTS * ) malloc( VOLUMEDISKSIZE );
    if( pstVolumeData == NULL )
    {
        return -1;
    } 
    // 
    swprintf( vcDriveName,7,TEXT("\\\\?\\%c:"), cDriveName );
    // Open Device
    hDevice = CreateFile( vcDriveName, GENERIC_READ, FILE_SHARE_READ |
        FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL );
    if( hDevice == INVALID_HANDLE_VALUE )
    {
        return -1;
    } 
    // Call Device Io Control
    bRet = DeviceIoControl( hDevice, IOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS,
            NULL, 0, pstVolumeData, VOLUMEDISKSIZE, &dwOut, NULL );
    if( bRet == FALSE )
    {
        free( pstVolumeData );
        return -1;
    }
    CloseHandle( hDevice ); 
    // Disk number is lower than 1 is failed.
    if( pstVolumeData->NumberOfDiskExtents < 1 )
    {
        free( pstVolumeData );
        return -1;
    } 
    iDiskNumber = pstVolumeData->Extents[ 0 ].DiskNumber;
    free( pstVolumeData ); 
    return iDiskNumber;
} 

int CIROM_Fusing_ToolDlg::GetSettingFromINIFile(void)
{
	HANDLE hFile;
	wchar_t buffer[1024];
//	wchar_t filename[868];
//	wchar_t drivename;
//	wchar_t temp2[868];
	DWORD bytesread;

	hFile = CreateFile( TEXT("irom.ini"), GENERIC_READ, FILE_SHARE_READ |
			FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL );
	
	if (hFile == INVALID_HANDLE_VALUE)
	{
		if (!CreateINIFile())
		{
			CloseHandle(hFile);
			return FALSE;
		}
		hFile = CreateFile( TEXT("irom.ini"), GENERIC_READ, FILE_SHARE_READ |
		        FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL );

		if (hFile == INVALID_HANDLE_VALUE)
		{
			CloseHandle(hFile);
			return FALSE;
		}
	}

	if(!ReadFile (hFile, buffer, 1024 * sizeof(wchar_t), &bytesread, NULL))
	{
		CloseHandle(hFile);
		return FALSE;
	}

	if ( buffer[0] != 0xaa || 
		buffer[1] != 0xbb || 
		buffer[2] != 0xcc || 
		buffer[3] != 0xdd || 
		buffer[4] != 0xee )
	{
		CloseHandle(hFile);
		return FALSE;
	}

	if ( buffer[6] == TEXT('\0'))
	{
		return FALSE;
	}

	return TRUE;
}

int CIROM_Fusing_ToolDlg::CreateINIFile(void)
{
	wchar_t buffer[1024] = {TEXT('\0'),};
	DWORD byteswrite;
	HANDLE hFile;

	buffer[0] = 0xaa;
	buffer[1] = 0xbb;
	buffer[2] = 0xcc;
	buffer[3] = 0xdd;
	buffer[4] = 0xff;
	
	hFile = CreateFile( TEXT("irom.ini"), GENERIC_WRITE, FILE_SHARE_READ |
        FILE_SHARE_WRITE, NULL, CREATE_NEW, 0, NULL );

	if (hFile == INVALID_HANDLE_VALUE)
	{
		CloseHandle(hFile);
		return FALSE;
	}

	if(!WriteFile (hFile, buffer, 1024 * sizeof(wchar_t), &byteswrite, NULL))
	{
		CloseHandle(hFile);
		return FALSE;
	}

	CloseHandle(hFile);

	return TRUE;
}


short CIROM_Fusing_ToolDlg::Fresource_driveinfo( int& length )
{
	//Fdrive_info DriveInfo;
	//memset( &DriveInfo, 0, sizeof( Fdrive_info ) );

	CComboBox * pComboBox = (CComboBox *)GetDlgItem(IDC_COMBO2);
	pComboBox->ResetContent();
	
	DWORD dwBuffer=256;
	char szBuffer[256];
	memset( szBuffer, 0, dwBuffer );
	int nSize = GetLogicalDriveStringsA( dwBuffer, szBuffer );
	if( nSize == 0 )
	{
		return -1;
	}

	char szDrivers[32][5];
	memset( szDrivers, 0, 160 );
	char szTemp[4];
	memset( szTemp, 0, 4 );
	int j=0;
	int nLength=0; // 有几个盘符
	for( int i=0; i<nSize; i++ )
	{
		if( szBuffer[i] == '\0' )
		{
			szTemp[j]='\0';
			strcpy( szDrivers[nLength], szTemp );

			UINT nDriver = GetDriveTypeA( szDrivers[nLength] );
			// 得到驱动器的信息
			switch( nDriver )
			{
			case DRIVE_REMOVABLE: // 是否是软驱
				pComboBox->AddString(CString(szTemp[0]));
				if (pComboBox->GetCount() == 1)
				{
					pComboBox->SetCurSel(0);
					UpdateData(TRUE);
					ChangeStringDriveNameToDriveNumber();
					GetTotalSector();
				}
				break;
			case DRIVE_CDROM:
				break;
			case DRIVE_FIXED:
				//strcpy( DriveInfo.drive_type, "硬盘" );
				//strcat( DriveInfo.drive_type, "DRIVE_FIXED" );

				//GetVolumeInformation();
				//DriveInfo.Partitiontype = ;
				//strcpy( DriveInfo.drive_id, szDrivers[nLength] );
				//DriveInfo.drive_size = ;
				//GetDiskFreeSpace( szDrivers );
				break;

			case DRIVE_REMOTE:
				break;
			case DRIVE_RAMDISK:
				break;
			}
			nLength++;
			j=0;
			continue;
		}
		szTemp[j]=szBuffer[i];
		j++;
	}

	length = nLength;
	return 1;
}   

void CIROM_Fusing_ToolDlg::OnCbnSelchangeCombo2()
{
	// TODO: Add your control notification handler code here
	//CComboBox * pComboBox = (CComboBox *)GetDlgItem(IDC_COMBO2);
	//pComboBox->ResetContent();
	UpdateData(TRUE);
	ChangeStringDriveNameToDriveNumber();
	GetTotalSector();
}

//BOOL CIROM_Fusing_ToolDlg::GetDrvSpaceInfo(CString strDriver)//获得磁盘空间数据
//{
//	CString strAllInfo;
//
//	//获得磁盘空间信息
//	ULARGE_INTEGER FreeAv,TotalBytes,FreeBytes;
//	if (GetDiskFreeSpaceEx(strDriver,&FreeAv,&TotalBytes,&FreeBytes))
//	{
//		//格式化信息，并显示出来
//		CString strTotalBytes,strFreeBytes;
//		//strTotalBytes.Format("Total:%u bytes",TotalBytes.QuadPart);
//		//strFreeBytes.Format("Left:%u bytes",FreeBytes.QuadPart);
//
//		strTotalBytes.Format(L"Total:%luG",TotalBytes.QuadPart/(ULONGLONG)(1024*1024*1024));
//		strFreeBytes.Format(L"Left:%luG",FreeBytes.QuadPart/(ULONGLONG)(1024*1024*1024));
//
//		strAllInfo.Format(L"Info:[%s] %s %s",strDriver,strTotalBytes,strFreeBytes);
//		//SendMsgToControler(strAllInfo);
//
//		return TRUE;
//	}
//	return FALSE;
//}
#include "dbt.h"
BOOL CIROM_Fusing_ToolDlg::OnDeviceChange(UINT nEventType,DWORD dwData)
{ 
	int length = 0;
	switch(nEventType) 
	{ 
	case DBT_DEVICEARRIVAL:
		{
			printf("插入SD卡\n");
			Fresource_driveinfo(length);
			UpdateData(TRUE);
			Sleep(1500);
			ChangeStringDriveNameToDriveNumber();
			GetTotalSector();
		}
		break;
		//拔出SD卡
	case DBT_DEVICEREMOVECOMPLETE:
		{
			printf("拔出SD卡\n");
			Fresource_driveinfo(length);
			UpdateData(TRUE);
			ChangeStringDriveNameToDriveNumber();
			GetTotalSector();
		}
		break;
	default:
		break;
	} 
	return FALSE; 
}


int CIROM_Fusing_ToolDlg::UnMountVolume(TCHAR DriveLetter)
{
/**********************************************************
基本流程如下：
1.打开该卷标设备
2.锁定当前卷
3.关闭所有该卷上打开的所有文件
4.卸载该卷
5.解除该卷的锁定
6.关闭设备
7.删除卷标盘符
说明：由于当该卷标磁盘上的文件被打开时，23步骤有是会失败，所以
为了达到强制关闭的目的，封掉了235步骤
**********************************************************/
    TCHAR    VolumeName[] = L"\\\\.\\ :";
    HANDLE  Device;
    DWORD   BytesReturned;

//初始化该卷标设备(类似"\\\\.\\F:")
    VolumeName[4] = DriveLetter;

//打开该卷标的设备，不存在则返回
    Device = CreateFile(
        VolumeName,
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        OPEN_EXISTING,
        FILE_FLAG_NO_BUFFERING,
        NULL
        );
    if (Device == INVALID_HANDLE_VALUE)
    {
        return 1;
    }

 //锁定需要卸载的卷
/*if (!DeviceIoControl(
        Device,
        FSCTL_LOCK_VOLUME,
        NULL,
        0,
        NULL,
        0,
        &BytesReturned,
        NULL
        ))
    {
        return 2;
    }
*/

/* //关闭该卷上打开的所有文件
    if (!DeviceIoControl(
        Device,
        IOCTL_FILE_DISK_CLOSE_FILE,
        NULL,
        0,
        NULL,
        0,
        &BytesReturned,
        NULL
        ))
    {
        return 3;
    }
*/
//卸载该卷
    if (!DeviceIoControl(
        Device,
        FSCTL_DISMOUNT_VOLUME,
        NULL,
        0,
        NULL,
        0,
        &BytesReturned,
        NULL
        ))
    {
        return 4;
    }
	/*
 //解除对该卷的锁定
    if (!DeviceIoControl(
        Device,
        FSCTL_UNLOCK_VOLUME,
        NULL,
        0,
        NULL,
        0,
        &BytesReturned,
        NULL
        ))
    {
        return 5;
    }
*/

//关闭设备HANDLE
    CloseHandle(Device);

//删除虚拟盘符，系统中看到的盘符应该消失
    if (!DefineDosDevice(
        DDD_REMOVE_DEFINITION,
        &VolumeName[4],
        NULL
        ))
    {
        return 6;
    }

    return 0;
} 

int CIROM_Fusing_ToolDlg::MountVolume(TCHAR DriveLetter)
{

	TCHAR    szDevice[256] = {0};        // 存储磁盘根路径

	TCHAR    szDrive[3] = {0};            // 存储单个磁盘根路径

	TCHAR    szName[MAX_PATH];            // 存储 MS-DOS 设备信息

	PTCHAR    p = szDevice;
	int BUFSIZE = 256;

	GetLogicalDriveStrings(BUFSIZE, szDevice);

	_tprintf (TEXT("获取的 MS-DOS 设备名为：\n"));
	do {
		CopyMemory (szDrive, p, 2*sizeof(TCHAR));        // 将驱动器根路径拷贝到缓冲区，要去掉反斜杠

		if ( !QueryDosDevice(szDrive, szName, BUFSIZE) ) {
			_tprintf (TEXT("QueryDosDevice 获取失败 : %d\n"), GetLastError());
			return (-1);
		}
		UINT uNameLen = _tcslen(szName);    // 设备名长度

		if (uNameLen < MAX_PATH) {

			_tprintf (TEXT("%s  %s\n"), szDrive, szName);
		}
		while (*p++);        // 空循环，到下一个 NULL (C:\NULLD:\NULLE:\)
	} while (*p);            // 还有其它盘符

	return 0;



	TCHAR    VolumeName[] = L"\\\\.\\ :";
    HANDLE  Device;
    DWORD   BytesReturned;

//初始化该卷标设备(类似"\\\\.\\F:")
    VolumeName[4] = DriveLetter;

	TCHAR device[1024] = {0};
	QueryDosDevice(VolumeName,device,1024); // (驱动器Z是用"subst"虚拟的)来查询映射的路径
	return 0;

//删除虚拟盘符，系统中看到的盘符应该消失
    if (!DefineDosDevice(
        DDD_RAW_TARGET_PATH,
        &VolumeName[4],
        NULL
        ))
    {
		int err = GetLastError();
        return 6;
    }

    return 0;
}