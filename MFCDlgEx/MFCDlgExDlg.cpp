#include "pch.h"
#include "framework.h"
#include "MFCDlgEx.h"
#include "MFCDlgExDlg.h"
#include "afxdialogex.h"
#include "CTypeDB.h"
#include "resource.h"
#include <atlimage.h> 
#include <vtkLight.h>
#include <vtkSTLReader.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRendererCollection.h>
#include <vtkProperty.h>
#include <vtkNamedColors.h>
#include <atlconv.h> 
#include <set>
#include <vtkCamera.h> 

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

class CAboutDlg : public CDialogEx
{
public:
    CAboutDlg();

#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_ABOUTBOX };
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX);

protected:
    DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

CMFCDlgExDlg::CMFCDlgExDlg(CWnd* pParent /*=nullptr*/)
    : CDialogEx(IDD_MFCDLGEX_DIALOG, pParent)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

    m_curSheet = 1;
    m_curType = 1;
    m_firstCharIndex = 0;
    m_strBasePath = _T("");
    m_bRotate = FALSE;
}

void CMFCDlgExDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Check(pDX, IDC_CHECK_ROTATE, m_bRotate);
}

BEGIN_MESSAGE_MAP(CMFCDlgExDlg, CDialogEx)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(IDC_BUTTON_OPENCSV, &CMFCDlgExDlg::OnBnClickedButtonOpencsv)
    ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_SHEET, &CMFCDlgExDlg::OnDeltaposSpinSheet)
    ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_TYPE, &CMFCDlgExDlg::OnDeltaposSpinType)
    ON_BN_CLICKED(IDC_CHECK_ROTATE, &CMFCDlgExDlg::OnBnClickedCheckRotate)
    ON_WM_TIMER()
    ON_WM_LBUTTONUP()
    ON_STN_CLICKED(IDC_STATIC_SHEETCHARS, &CMFCDlgExDlg::OnStnClickedStaticSheetchars)
END_MESSAGE_MAP()

BOOL CMFCDlgExDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
    ASSERT(IDM_ABOUTBOX < 0xF000);

    CMenu* pSysMenu = GetSystemMenu(FALSE);
    if (pSysMenu != nullptr)
    {
        BOOL bNameValid;
        CString strAboutMenu;
        bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
        ASSERT(bNameValid);
        if (!strAboutMenu.IsEmpty())
        {
            pSysMenu->AppendMenu(MF_SEPARATOR);
            pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
        }
    }

    SetIcon(m_hIcon, TRUE);
    SetIcon(m_hIcon, FALSE);

    if (this->GetDlgItem(IDC_STATIC_3D))
    {
        this->InitVtkWindow(this->GetDlgItem(IDC_STATIC_3D)->GetSafeHwnd());
        this->ResizeVtkWindow();
    }

    m_curSheet = 1;
    m_curType = 1;
    m_firstCharIndex = 0;
    m_strBasePath = _T("");

    SetDlgItemInt(IDC_EDIT_SHEET, m_curSheet, FALSE);
    SetDlgItemText(IDC_STATIC_SHEETCHARS, _T("현재 장 글자 수: 0"));

    CSpinButtonCtrl* pSpinSheet = (CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_SHEET);
    if (pSpinSheet)
    {
        pSpinSheet->SetRange(1, 1);
        pSpinSheet->SetPos(m_curSheet);
    }

    CSpinButtonCtrl* pSpinType = (CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_TYPE);
    if (pSpinType)
    {
        pSpinType->SetRange(1, 1);
        pSpinType->SetPos(1);
    }
    SetDlgItemText(IDC_CHECK_ROTATE, _T("회전"));
    return TRUE;
}

void CMFCDlgExDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
    if ((nID & 0xFFF0) == IDM_ABOUTBOX)
    {
        CAboutDlg dlgAbout;
        dlgAbout.DoModal();
    }
    else
    {
        CDialogEx::OnSysCommand(nID, lParam);
    }
}

void CMFCDlgExDlg::OnPaint()
{
    if (IsIconic())
    {
        CPaintDC dc(this);

        SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        int x = (rect.Width() - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;

        dc.DrawIcon(x, y, m_hIcon);
    }
    else
    {
        CDialogEx::OnPaint();

        if (m_db.m_nChar > 0)
        {
            DrawCurrentSheetBoxes();
            DrawCharImage();
        }
    }
}

HCURSOR CMFCDlgExDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}

void CMFCDlgExDlg::InitVtkWindow(void* hWnd)
{
    if (m_vtkWindow == NULL)
    {
        vtkSmartPointer<vtkRenderWindowInteractor> interactor =
            vtkSmartPointer<vtkRenderWindowInteractor>::New();

        interactor->SetInteractorStyle(
            vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New());

        vtkSmartPointer<vtkRenderer> renderer =
            vtkSmartPointer<vtkRenderer>::New();
        renderer->SetBackground(0.0, 0.0, 0.0);

        m_vtkWindow = vtkSmartPointer<vtkRenderWindow>::New();
        m_vtkWindow->SetParentId(hWnd);
        m_vtkWindow->SetInteractor(interactor);
        m_vtkWindow->AddRenderer(renderer);
        m_vtkWindow->Render();
    }
}

void CMFCDlgExDlg::ResizeVtkWindow()
{
    CRect rc;
    CWnd* pWnd = GetDlgItem(IDC_STATIC_3D);
    if (pWnd)
    {
        pWnd->GetClientRect(rc);
        m_vtkWindow->SetSize(rc.Width(), rc.Height());
    }
}

void CMFCDlgExDlg::OnBnClickedButtonOpencsv()
{
    CFileDialog dlg(TRUE, _T("csv"), NULL,
        OFN_FILEMUSTEXIST | OFN_HIDEREADONLY,
        _T("CSV Files (*.csv)|*.csv|All Files (*.*)|*.*||"),
        this);

    if (dlg.DoModal() != IDOK)
        return;

    CString path = dlg.GetPathName();

    int pos = path.ReverseFind('\\');
    if (pos > 0)
        m_strBasePath = path.Left(pos);
    else
        m_strBasePath = _T("");

    CString bookName;
    if (!m_strBasePath.IsEmpty())
    {
        int pos2 = m_strBasePath.ReverseFind('\\');
        if (pos2 >= 0)
            bookName = m_strBasePath.Mid(pos2 + 1);
        else
            bookName = m_strBasePath;
    }

    int p = bookName.Find(_T('('));
    if (p > 0)
    {
        bookName = bookName.Left(p);
        bookName.TrimRight();
    }

    SetDlgItemText(IDC_STATIC_BOOKNAME, bookName);

    if (!m_db.ReadCSVFile(path))
    {
        AfxMessageBox(_T("CSV 파일을 읽지 못했습니다."));
        return;
    }

    UpdateBookSummary();

    if (m_db.m_chars.GetSize() > 0)
    {
        int firstSheet = m_db.m_chars[0].m_sheet;
        UpdateSheetSummary(firstSheet);
    }

    CString msg;
    msg.Format(_T("장 수: %d\n글자 수: %d"), m_db.m_nSheet, m_db.m_nChar);
    AfxMessageBox(msg);

    m_curSheet = 1;

    CSpinButtonCtrl* pSpin = (CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_SHEET);
    if (pSpin)
    {
        pSpin->SetRange(1, m_db.m_nSheet);
        pSpin->SetPos(m_curSheet);
    }

    UpdateSheetInfo();
}

void CMFCDlgExDlg::UpdateSheetInfo()
{
    CWaitCursor wait;
    SetDlgItemInt(IDC_EDIT_SHEET, m_curSheet, FALSE);

    m_curSheetIndices.RemoveAll();
    for (int i = 0; i < m_db.m_nChar; ++i)
    {
        if (m_db.m_chars[i].m_sheet == m_curSheet)
            m_curSheetIndices.Add(i);
    }

    CString text;
    text.Format(_T("현재 장 글자 수: %d"), (int)m_curSheetIndices.GetSize());
    SetDlgItemText(IDC_STATIC_SHEETCHARS, text);

    SetDlgItemInt(IDC_EDIT_TYPE, m_curSheet, FALSE);

    m_firstCharIndex = 0;

    CSpinButtonCtrl* pSpinType = (CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_TYPE);
    if (pSpinType)
    {
        pSpinType->SetRange(1, m_db.m_nSheet);
        pSpinType->SetPos(m_curSheet);
    }

    if (!m_strBasePath.IsEmpty())
    {
        CString imgFullPath;
        imgFullPath.Format(_T("%s\\01_scan\\%03d.jpg"), m_strBasePath, m_curSheet);

        if (!m_imgCurrent.IsNull()) m_imgCurrent.Destroy();

        HRESULT hr = m_imgCurrent.Load(imgFullPath);
    }

    UpdateCurrentSheetChars();
    UpdateSheetSummary(m_curSheet);

    CWnd* pFrame = GetDlgItem(IDC_STATIC_FRAME);
    if (pFrame) {
        pFrame->Invalidate(FALSE);
        pFrame->UpdateWindow();
    }
    DrawCurrentSheetBoxes();
}

void CMFCDlgExDlg::OnDeltaposSpinSheet(NMHDR* pNMHDR, LRESULT* pResult)
{
    LPNMUPDOWN p = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
    m_curSheet += p->iDelta;

    if (m_curSheet < 1) m_curSheet = 1;
    if (m_db.m_nSheet > 0 && m_curSheet > m_db.m_nSheet) m_curSheet = m_db.m_nSheet;

    UpdateSheetInfo();
    *pResult = 0;
}

void CMFCDlgExDlg::DrawCurrentSheetBoxes()
{
    CWnd* pFrame = GetDlgItem(IDC_STATIC_FRAME);
    if (!pFrame) return;

    CRect rc;
    pFrame->GetClientRect(&rc);

    CClientDC dc(pFrame);

    if (m_imgCurrent.IsNull())
    {
        dc.FillSolidRect(rc, RGB(0, 0, 0));
        dc.SetTextColor(RGB(255, 255, 255));
        dc.SetBkMode(TRANSPARENT);
        dc.TextOutW(10, 10, _T("이미지가 로드되지 않았습니다."));
        return;
    }

    dc.SetStretchBltMode(HALFTONE);
    m_imgCurrent.Draw(dc, rc);

    int imgW = m_imgCurrent.GetWidth();
    int imgH = m_imgCurrent.GetHeight();

    if (imgW <= 0 || imgH <= 0) return;

    double scaleX = (double)rc.Width() / (double)imgW;
    double scaleY = (double)rc.Height() / (double)imgH;

    CPen penNormal(PS_SOLID, 1, RGB(0, 255, 0));
    CPen penSelected(PS_SOLID, 2, RGB(255, 0, 0));

    CPen* pOldPen = dc.SelectObject(&penNormal);
    CBrush* pOldBrush = (CBrush*)dc.SelectStockObject(NULL_BRUSH);

    for (int i = 0; i < m_curSheetIndices.GetSize(); ++i)
    {
        int idx = m_curSheetIndices[i];
        const SCharInfo& ch = m_db.m_chars[idx];

        int x1 = (int)(ch.m_sx * scaleX);
        int y1 = (int)(ch.m_sy * scaleY);
        int x2 = (int)((ch.m_sx + ch.m_width) * scaleX);
        int y2 = (int)((ch.m_sy + ch.m_height) * scaleY);

        if (i == m_firstCharIndex)
            dc.SelectObject(&penSelected);
        else
            dc.SelectObject(&penNormal);

        dc.Rectangle(x1, y1, x2, y2);
    }

    dc.SelectObject(pOldPen);
    dc.SelectObject(pOldBrush);
}

void CMFCDlgExDlg::OnDeltaposSpinType(NMHDR* pNMHDR, LRESULT* pResult)
{
    LPNMUPDOWN p = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
    m_curSheet += p->iDelta;

    if (m_curSheet < 1) m_curSheet = 1;
    if (m_db.m_nSheet > 0 && m_curSheet > m_db.m_nSheet) m_curSheet = m_db.m_nSheet;

    SetDlgItemInt(IDC_EDIT_TYPE, m_curSheet, FALSE);
    UpdateSheetInfo();
    *pResult = 0;
}

void CMFCDlgExDlg::UpdateCurrentSheetChars()
{
    if (m_curSheetIndices.GetSize() <= 0)
        return;

    if (m_firstCharIndex < 0 || m_firstCharIndex >= m_curSheetIndices.GetSize())
        m_firstCharIndex = 0;

    int idx = m_curSheetIndices[m_firstCharIndex];
    const SCharInfo& ch = m_db.m_chars[idx];

    CString info;
    info.Format(_T("코드: %s\r\n장: %d장\r\n행: %d행\r\n번호: %d번"),
        (LPCTSTR)ch.m_char, ch.m_sheet, ch.m_line, ch.m_order);
    SetDlgItemText(IDC_STATIC_CHARINFO, info);

    if (!m_imgChar.IsNull())
        m_imgChar.Destroy();

    if (!m_strBasePath.IsEmpty())
    {
        CString dirPath;
        dirPath.Format(_T("%s\\03_type\\%s\\%d"),
            (LPCTSTR)m_strBasePath, (LPCTSTR)ch.m_char, ch.m_type);

        CString searchPattern = dirPath + _T("\\*.png");

        WIN32_FIND_DATA fd;
        HANDLE hFind = FindFirstFile(searchPattern, &fd);

        if (hFind != INVALID_HANDLE_VALUE)
        {
            CString imgPath = dirPath + _T("\\") + fd.cFileName;
            FindClose(hFind);
            m_imgChar.Load(imgPath);
        }
    }

    DrawCharImage();

    CListCtrl* pList = (CListCtrl*)GetDlgItem(IDC_LIST_CHARLIST);
    if (pList)
    {
        CHeaderCtrl* pHeader = pList->GetHeaderCtrl();
        if (pHeader && pHeader->GetItemCount() == 0)
        {
            pList->InsertColumn(0, _T("장"), LVCFMT_LEFT, 50);
            pList->InsertColumn(1, _T("행"), LVCFMT_LEFT, 50);
            pList->InsertColumn(2, _T("번"), LVCFMT_LEFT, 50);
        }

        pList->DeleteAllItems();

        CString targetCode = ch.m_char;

        int totalCount = 0;
        int curIndex = 0;
        int row = 0;

        for (int i = 0; i < m_db.m_chars.GetSize(); ++i)
        {
            const SCharInfo& item = m_db.m_chars[i];
            if (item.m_char == targetCode)
            {
                ++totalCount;

                CString sSheet, sLine, sOrder;
                sSheet.Format(_T("%d"), item.m_sheet);
                sLine.Format(_T("%d"), item.m_line);
                sOrder.Format(_T("%d"), item.m_order);

                int nItem = pList->InsertItem(row, sSheet);
                pList->SetItemText(nItem, 1, sLine);
                pList->SetItemText(nItem, 2, sOrder);
                ++row;

                if (i == idx)
                    curIndex = totalCount;
            }
        }

        if (curIndex <= 0) curIndex = 1;

        CString sIdx, sCnt;
        sIdx.Format(_T("%d"), curIndex);
        sCnt.Format(_T("/ %d개"), totalCount);

        SetDlgItemText(IDC_STATIC_CHARINDEX, sIdx);
        SetDlgItemText(IDC_STATIC_CHARCOUNT, sCnt);
    }

    if (m_vtkWindow)
    {
        m_vtkWindow->GetRenderers()->RemoveAllItems();
        vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
        m_vtkWindow->AddRenderer(renderer);

        CString stlPath;
        stlPath.Format(_T("%s\\04_3d\\%s_%d.stl"),
            m_strBasePath, (LPCTSTR)ch.m_char, ch.m_type);

        vtkSmartPointer<vtkSTLReader> reader = vtkSmartPointer<vtkSTLReader>::New();

        USES_CONVERSION;
        reader->SetFileName(CT2A(stlPath, CP_UTF8));
        reader->Update();

        if (reader->GetOutput()->GetNumberOfPoints() > 0)
        {
            vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
            mapper->SetInputConnection(reader->GetOutputPort());

            vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
            actor->SetMapper(mapper);
            actor->SetScale(-1, 1, 1);
            renderer->AddActor(actor);
            renderer->ResetCamera();
        }

        m_vtkWindow->Render();
    }
}

void CMFCDlgExDlg::DrawCharImageToCtrl(int nCtrlID)
{
    CWnd* pImgWnd = GetDlgItem(nCtrlID);
    if (!pImgWnd) return;

    CRect rc;
    pImgWnd->GetClientRect(&rc);
    CClientDC dc(pImgWnd);

    dc.FillSolidRect(rc, RGB(240, 240, 240));

    if (m_imgChar.IsNull())
        return;

    int imgW = m_imgChar.GetWidth();
    int imgH = m_imgChar.GetHeight();

    double scaleX = (double)rc.Width() / (double)imgW;
    double scaleY = (double)rc.Height() / (double)imgH;
    double scale = min(scaleX, scaleY);

    int drawW = (int)(imgW * scale);
    int drawH = (int)(imgH * scale);

    int offsetX = (rc.Width() - drawW) / 2;
    int offsetY = (rc.Height() - drawH) / 2;

    dc.SetStretchBltMode(HALFTONE);
    m_imgChar.Draw(dc, CRect(offsetX, offsetY,
        offsetX + drawW, offsetY + drawH));
}

void CMFCDlgExDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
    if (m_imgCurrent.IsNull() || m_curSheetIndices.GetSize() <= 0)
    {
        CDialogEx::OnLButtonUp(nFlags, point);
        return;
    }

    CWnd* pFrame = GetDlgItem(IDC_STATIC_FRAME);
    if (!pFrame) return;

    CRect rcFrame;
    pFrame->GetWindowRect(&rcFrame);
    ScreenToClient(&rcFrame);

    if (rcFrame.PtInRect(point))
    {
        int mouseX = point.x - rcFrame.left;
        int mouseY = point.y - rcFrame.top;

        int imgW = m_imgCurrent.GetWidth();
        int imgH = m_imgCurrent.GetHeight();
        if (imgW <= 0 || imgH <= 0) return;

        double scaleX = (double)rcFrame.Width() / (double)imgW;
        double scaleY = (double)rcFrame.Height() / (double)imgH;

        for (int i = 0; i < m_curSheetIndices.GetSize(); ++i)
        {
            int idx = m_curSheetIndices[i];
            const SCharInfo& ch = m_db.m_chars[idx];

            int x1 = (int)(ch.m_sx * scaleX);
            int y1 = (int)(ch.m_sy * scaleY);
            int x2 = (int)((ch.m_sx + ch.m_width) * scaleX);
            int y2 = (int)((ch.m_sy + ch.m_height) * scaleY);

            CRect rcChar(x1, y1, x2, y2);

            if (rcChar.PtInRect(CPoint(mouseX, mouseY)))
            {
                m_firstCharIndex = i;

                UpdateCurrentSheetChars();
                DrawCurrentSheetBoxes();

                break;
            }
        }
    }

    CDialogEx::OnLButtonUp(nFlags, point);
}

void CMFCDlgExDlg::DrawCharImage()
{
    DrawCharImageToCtrl(IDC_STATIC_CHARIMG);
    DrawCharImageToCtrl(IDC_STATIC_CHARIMG2);
}

void CMFCDlgExDlg::OnStnClickedStaticSheetchars()
{
}

struct CStringLess
{
    bool operator()(const CString& a, const CString& b) const
    {
        return a.Compare(b) < 0;
    }
};

void CMFCDlgExDlg::UpdateBookSummary()
{
    int totalCount = m_db.m_chars.GetSize();

    std::set<CString, CStringLess> uniqueChars;
    std::set<CString, CStringLess> uniqueTypes;

    for (int i = 0; i < totalCount; ++i)
    {
        const SCharInfo& ch = m_db.m_chars[i];

        uniqueChars.insert(ch.m_char);

        CString typeKey;
        typeKey.Format(_T("%s_%d"), (LPCTSTR)ch.m_char, ch.m_type);
        uniqueTypes.insert(typeKey);
    }

    int kindCount = (int)uniqueChars.size();
    int syllCount = (int)uniqueTypes.size();

    CString buf;
    buf.Format(_T("한글 글자수   %d 개"), totalCount);
    SetDlgItemText(IDC_STATIC_BOOK_CNT, buf);

    buf.Format(_T("한글 글자 종류 %d 종"), kindCount);
    SetDlgItemText(IDC_STATIC_BOOK_KIND, buf);

    buf.Format(_T("한글 절자수   %d 개"), syllCount);
    SetDlgItemText(IDC_STATIC_BOOK_SYLL, buf);
}

void CMFCDlgExDlg::UpdateSheetSummary(int nSheet)
{
    int totalCount = 0;
    std::set<CString, CStringLess> uniqueChars;
    std::set<CString, CStringLess> uniqueTypes;

    for (int i = 0; i < m_db.m_chars.GetSize(); ++i)
    {
        const SCharInfo& ch = m_db.m_chars[i];

        if (ch.m_sheet == nSheet)
        {
            ++totalCount;

            uniqueChars.insert(ch.m_char);

            CString typeKey;
            typeKey.Format(_T("%s_%d"), (LPCTSTR)ch.m_char, ch.m_type);
            uniqueTypes.insert(typeKey);
        }
    }

    int kindCount = (int)uniqueChars.size();
    int syllCount = (int)uniqueTypes.size();

    CString buf;
    buf.Format(_T("한글 글자수   %d 개"), totalCount);
    SetDlgItemText(IDC_STATIC_SHEET_CNT, buf);

    buf.Format(_T("한글 글자 종류 %d 종"), kindCount);
    SetDlgItemText(IDC_STATIC_SHEET_KIND, buf);

    buf.Format(_T("한글 절자수   %d 개"), syllCount);
    SetDlgItemText(IDC_STATIC_SHEET_SYLL, buf);
}

void CMFCDlgExDlg::OnBnClickedCheckRotate()
{
    if (IsDlgButtonChecked(IDC_CHECK_ROTATE))
    {
        SetTimer(1, 20, NULL);
    }
    else
    {
        KillTimer(1);
    }
}

void CMFCDlgExDlg::OnTimer(UINT_PTR nIDEvent)
{
    if (nIDEvent == 1 && m_vtkWindow)
    {
        vtkSmartPointer<vtkRenderer> renderer = m_vtkWindow->GetRenderers()->GetFirstRenderer();
        if (renderer)
        {
            renderer->GetActiveCamera()->Azimuth(1);
            m_vtkWindow->Render();
        }
    }

    CDialogEx::OnTimer(nIDEvent);
}