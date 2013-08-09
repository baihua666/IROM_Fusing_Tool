// IROM_Fusing_ToolDlg.h : header file
//

#pragma once
#include "afx.h"

typedef struct _PARTENTRY {
        BYTE            Part_BootInd;           // If 80h means this is boot partition
        BYTE            Part_FirstHead;         // Partition starting head based 0
        BYTE            Part_FirstSector;       // Partition starting sector based 1
        BYTE            Part_FirstTrack;        // Partition starting track based 0
        BYTE            Part_FileSystem;        // Partition type signature field
        BYTE            Part_LastHead;          // Partition ending head based 0
        BYTE            Part_LastSector;        // Partition ending sector based 1
        BYTE            Part_LastTrack;         // Partition ending track based 0
        DWORD           Part_StartSector;       // Logical starting sector based 0
        DWORD           Part_TotalSectors;      // Total logical sectors in partition
} PARTENTRY;

// CIROM_Fusing_ToolDlg dialog
class CIROM_Fusing_ToolDlg : public CDialog
{
// Construction
public:
	CIROM_Fusing_ToolDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_IROM_FUSING_TOOL_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnEnChangeEdit3();
	afx_msg void OnCbnSelchangeComboDriveName();
	afx_msg void OnBnClickedButtonStart();
	CString m_strCombo;
	int m_dwDriveNumber;
	int ChangeStringDriveNameToDriveNumber(void);
	char * ReadSector(int startsector, int numberofsectors);
	int GetPhysicalDriveNumber( char cDriveName );

	CString m_strDriveSize;
	afx_msg void OnEnChangeEditSize();
	afx_msg void OnBnClickedButtonBrowse();
	CString m_strFilePath;
	DWORD m_dwTotalSector;
	int GetTotalSector(void);
	int WriteSector(int startsector, int numberofsector, char *buffer);
	char *m_pbWriteBuffer;
	DWORD m_dwStartWriteSector;
	CFile m_fileImageFile;
	void InitMemberVariables(void);
	CString m_strFromSector;
	CString m_strToSector;
	afx_msg void OnEnChangeEdit5();
	CString m_strTargetDrive;
	int m_nPhysicalNumber;
	int GetSettingFromINIFile(void);
	int CreateINIFile(void);

	short Fresource_driveinfo( int& length );
	//BOOL GetDrvSpaceInfo(CString strDriver);
	afx_msg void OnCbnSelchangeCombo2();
	afx_msg BOOL OnDeviceChange(UINT nEventType,DWORD dwData);
	unsigned int m_nStartSector;
	BOOL m_bIsWIN7;
	void GetMySystemVersion();
	int UnMountVolume(TCHAR DriveLetter);
	int MountVolume(TCHAR DriveLetter);
};
