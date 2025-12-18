#pragma once

#include "resource.h"
#include "CTypeDB.h"
#include <vtkSmartPointer.h>
#include <vtkCamera.h>
class vtkRenderWindow;

class CMFCDlgExDlg : public CDialogEx
{
public:
    BOOL m_bRotate;
    CMFCDlgExDlg(CWnd* pParent = nullptr);

    CString m_strBasePath;
    CImage  m_imgCurrent;

    vtkSmartPointer<vtkRenderWindow> m_vtkWindow;
    void InitVtkWindow(void* hWnd);
    void ResizeVtkWindow();
    CImage m_imgChar;
    void DrawCharImage();
    void DrawCharImageToCtrl(int nCtrlID);

#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_MFCDLGEX_DIALOG };
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX);

    HICON m_hIcon;

    CTypeDB m_db;
    int     m_curSheet;
    int     m_curType;

    CArray<int, int> m_curSheetIndices;
    int m_firstCharIndex;

    virtual BOOL OnInitDialog();
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();

    afx_msg void OnBnClickedCheckRotate();
    afx_msg void OnTimer(UINT_PTR nIDEvent);

    afx_msg void OnBnClickedButtonOpencsv();

    afx_msg void OnDeltaposSpinSheet(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnDeltaposSpinType(NMHDR* pNMHDR, LRESULT* pResult);

    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);

    void UpdateSheetInfo();
    void UpdateCurrentSheetChars();
    void DrawCurrentSheetBoxes();

    void UpdateBookSummary();
    void UpdateSheetSummary(int nSheet);


    DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnStnClickedStaticSheetchars();
};