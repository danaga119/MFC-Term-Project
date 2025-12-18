// MFCDlgExDlg.cpp: 구현 파일
//


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


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
    CAboutDlg();

    // 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_ABOUTBOX };
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

    // 구현입니다.
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


// CMFCDlgExDlg 대화 상자



CMFCDlgExDlg::CMFCDlgExDlg(CWnd* pParent /*=nullptr*/)
    : CDialogEx(IDD_MFCDLGEX_DIALOG, pParent)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

    // [수정] 멤버 변수 초기화 (오류 해결)
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



// CMFCDlgExDlg 메시지 처리기

BOOL CMFCDlgExDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // (시스템 메뉴, 아이콘 설정 코드 등은 그대로 두세요...)
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

    // ★ [수정] 주석을 풀고, 대상을 'IDC_STATIC_3D'로 변경!
    // 이제 3D 화면은 오른쪽 아래 작은 박스에만 나타납니다.
    if (this->GetDlgItem(IDC_STATIC_3D))
    {
        this->InitVtkWindow(this->GetDlgItem(IDC_STATIC_3D)->GetSafeHwnd());
        this->ResizeVtkWindow();
    }

    // === 변수 초기화 ===
    m_curSheet = 1;
    m_curType = 1;
    m_firstCharIndex = 0;
    m_strBasePath = _T("");

    // UI 초기값 설정
    SetDlgItemInt(IDC_EDIT_SHEET, m_curSheet, FALSE);
    SetDlgItemText(IDC_STATIC_SHEETCHARS, _T("현재 장 글자 수: 0"));

    CSpinButtonCtrl* pSpinSheet = (CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_SHEET);
    if (pSpinSheet)
    {
        pSpinSheet->SetRange(1, 1);
        pSpinSheet->SetPos(m_curSheet);
    }

    //SetDlgItemInt(IDC_EDIT_TYPE, 1, FALSE);
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

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 애플리케이션의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CMFCDlgExDlg::OnPaint()
{
    if (IsIconic())
    {
        // ... (기존 아이콘 코드 생략) ...
    }
    else
    {
        CDialogEx::OnPaint();

        // ★ [확인] 이 부분이 있어야 창을 흔들었을 때 그림이 복구됩니다.
        // 그리고 UpdateSheetInfo에서 Invalidate()를 호출했을 때 실행되는 곳이 여기입니다.
        if (m_db.m_nChar > 0)
        {
            DrawCurrentSheetBoxes();
            DrawCharImage();
        }
    }
}
// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
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
    // ★ [수정] IDC_STATIC_FRAME -> IDC_STATIC_3D 로 변경
    CWnd* pWnd = GetDlgItem(IDC_STATIC_3D);
    if (pWnd)
    {
        pWnd->GetClientRect(rc);
        // 3D 뷰어 크기를 작은 박스 크기에 맞춤
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

    // 1) CSV 파일이 있는 폴더 경로만 떼어내기
    int pos = path.ReverseFind('\\');
    if (pos > 0)
        m_strBasePath = path.Left(pos);
    else
        m_strBasePath = _T("");

    // 2) 폴더 이름을 책 제목으로 사용
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

    // 4) CSV 실제 로딩
    if (!m_db.ReadCSVFile(path))
    {
        AfxMessageBox(_T("CSV 파일을 읽지 못했습니다."));
        return;
    }

    // 🔹 여기서 통계 한 번 초기화
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

    UpdateSheetInfo(); // 첫 장 이미지 로드
}




void CMFCDlgExDlg::UpdateSheetInfo()
{
    CWaitCursor wait;
    // 1. UI 텍스트 및 스핀 컨트롤 갱신
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

    // 장 번호 에디트 박스 갱신
    SetDlgItemInt(IDC_EDIT_TYPE, m_curSheet, FALSE);

    m_firstCharIndex = 0;

    CSpinButtonCtrl* pSpinType = (CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_TYPE);
    if (pSpinType)
    {
        pSpinType->SetRange(1, m_db.m_nSheet);
        pSpinType->SetPos(m_curSheet);
    }

    // 2. 이미지 로드
    if (!m_strBasePath.IsEmpty())
    {
        CString imgFullPath;
        imgFullPath.Format(_T("%s\\01_scan\\%03d.jpg"), m_strBasePath, m_curSheet);

        if (!m_imgCurrent.IsNull()) m_imgCurrent.Destroy();

        HRESULT hr = m_imgCurrent.Load(imgFullPath);

        // 로드 실패 시 디버깅용 (필요 없으면 주석 처리)
        // if (FAILED(hr)) { ... }
    }

    // 3. 텍스트 정보 갱신
    UpdateCurrentSheetChars();

    // ---------------------------------------------------------
    // ★ [핵심 해결책] 이 부분이 빠져서 창을 흔들어야 했던 겁니다.
    // ---------------------------------------------------------
    UpdateSheetSummary(m_curSheet);

    // 화면 그리기 명령
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
    // -= 를 += 로 수정
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

    // [최적화 1] 이미지가 있을 땐 검은색 배경을 칠하지 않습니다. (깜빡임 원인 제거)
    // 이미지가 없어서 실패했을 때만 검은색으로 칠합니다.
    if (m_imgCurrent.IsNull())
    {
        dc.FillSolidRect(rc, RGB(0, 0, 0));
        dc.SetTextColor(RGB(255, 255, 255));
        dc.SetBkMode(TRANSPARENT);
        dc.TextOutW(10, 10, _T("이미지가 로드되지 않았습니다."));
        return;
    }

    // 2) 이미지 그리기 (기존 그림 위에 덮어씌움 -> 깜빡임 없음)
    dc.SetStretchBltMode(HALFTONE);
    m_imgCurrent.Draw(dc, rc);

    // 3) 박스 그리기 좌표 계산
    int imgW = m_imgCurrent.GetWidth();
    int imgH = m_imgCurrent.GetHeight();

    if (imgW <= 0 || imgH <= 0) return;

    double scaleX = (double)rc.Width() / (double)imgW;
    double scaleY = (double)rc.Height() / (double)imgH;

    CPen penNormal(PS_SOLID, 1, RGB(0, 255, 0));   // 초록
    CPen penSelected(PS_SOLID, 2, RGB(255, 0, 0)); // 빨강

    CPen* pOldPen = dc.SelectObject(&penNormal);
    CBrush* pOldBrush = (CBrush*)dc.SelectStockObject(NULL_BRUSH);

    // 4) 박스 그리기
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
    // -= 를 += 로 수정
    m_curSheet += p->iDelta;

    if (m_curSheet < 1) m_curSheet = 1;
    if (m_db.m_nSheet > 0 && m_curSheet > m_db.m_nSheet) m_curSheet = m_db.m_nSheet;

    SetDlgItemInt(IDC_EDIT_TYPE, m_curSheet, FALSE);
    UpdateSheetInfo();
    *pResult = 0;
}
void CMFCDlgExDlg::UpdateCurrentSheetChars()
{
    // 0. 현재 장에 글자가 없으면 종료
    if (m_curSheetIndices.GetSize() <= 0)
        return;

    if (m_firstCharIndex < 0 || m_firstCharIndex >= m_curSheetIndices.GetSize())
        m_firstCharIndex = 0;

    int idx = m_curSheetIndices[m_firstCharIndex];
    const SCharInfo& ch = m_db.m_chars[idx];

    // 1) 글자 정보 텍스트
    CString info;
    info.Format(_T("코드: %s\r\n장: %d장\r\n행: %d행\r\n번호: %d번"),
        (LPCTSTR)ch.m_char, ch.m_sheet, ch.m_line, ch.m_order);
    SetDlgItemText(IDC_STATIC_CHARINFO, info);

    // 2) 03_type 이미지 로드
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

    // 선택 글자/선택 글자(아래) 둘 다 그리기
    DrawCharImage();

    // 3) 구성 글자 리스트 + 활자정보 (1 / 2개)
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

        int totalCount = 0;  // 동일 코드 전체 개수
        int curIndex = 0;  // 그 중 현재 선택된 글자가 몇 번째인지
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
        sIdx.Format(_T("%d"), curIndex);      // 왼쪽: 1
        sCnt.Format(_T("/ %d개"), totalCount); // 오른쪽: / 2개

        SetDlgItemText(IDC_STATIC_CHARINDEX, sIdx);   // ← 너가 만든 왼쪽 static ID
        SetDlgItemText(IDC_STATIC_CHARCOUNT, sCnt);   // ← 너가 만든 오른쪽 static ID
    }

    // 4) 3D STL 표시 (기존 코드 그대로)
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

    // 배경
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
                // 인덱스만 변경
                m_firstCharIndex = i;

                // 오른쪽 텍스트 정보 갱신
                UpdateCurrentSheetChars();

                // ★ [수정] Invalidate(강제 새로고침) 삭제!
                // 대신 그리기 함수를 직접 호출하여 즉시 덧칠합니다.
                DrawCurrentSheetBoxes();

                break;
            }
        }
    }

    CDialogEx::OnLButtonUp(nFlags, point);
}
void CMFCDlgExDlg::DrawCharImage()
{
    // 위쪽 Picture Control
    DrawCharImageToCtrl(IDC_STATIC_CHARIMG);

    // 아래쪽 Picture Control (선택 글자)
    DrawCharImageToCtrl(IDC_STATIC_CHARIMG2);
}


void CMFCDlgExDlg::OnStnClickedStaticSheetchars()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
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
    int totalCount = m_db.m_chars.GetSize(); // 전체 단순 글자 수 (352개)

    std::set<CString, CStringLess> uniqueChars; // 글자 종류 (183종)
    std::set<CString, CStringLess> uniqueTypes; // 활자수/절자수 (330개)

    for (int i = 0; i < totalCount; ++i)
    {
        const SCharInfo& ch = m_db.m_chars[i];

        // 1. 종류: '가', '나' 등 글자 자체만 따짐
        uniqueChars.insert(ch.m_char);

        // 2. 활자수(절자수): 같은 '가'라도 1번 모양, 2번 모양 다르면 따로 셈
        CString typeKey;
        typeKey.Format(_T("%s_%d"), (LPCTSTR)ch.m_char, ch.m_type);
        uniqueTypes.insert(typeKey);
    }

    int kindCount = (int)uniqueChars.size();
    int syllCount = (int)uniqueTypes.size();

    CString buf;
    // 책 전체 통계 UI 업데이트
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
    std::set<CString, CStringLess> uniqueChars; // 장내 종류
    std::set<CString, CStringLess> uniqueTypes; // 장내 활자수

    for (int i = 0; i < m_db.m_chars.GetSize(); ++i)
    {
        const SCharInfo& ch = m_db.m_chars[i];

        // 현재 보고 있는 장(Sheet)에 있는 글자만 계산
        if (ch.m_sheet == nSheet)
        {
            ++totalCount;

            // 종류 등록
            uniqueChars.insert(ch.m_char);

            // 활자수(절자수) 등록 (글자+타입 조합)
            CString typeKey;
            typeKey.Format(_T("%s_%d"), (LPCTSTR)ch.m_char, ch.m_type);
            uniqueTypes.insert(typeKey);
        }
    }

    int kindCount = (int)uniqueChars.size();
    int syllCount = (int)uniqueTypes.size();

    CString buf;
    // 장 내 통계 UI 업데이트
    buf.Format(_T("한글 글자수   %d 개"), totalCount);
    SetDlgItemText(IDC_STATIC_SHEET_CNT, buf);

    buf.Format(_T("한글 글자 종류 %d 종"), kindCount);
    SetDlgItemText(IDC_STATIC_SHEET_KIND, buf);

    buf.Format(_T("한글 절자수   %d 개"), syllCount);
    SetDlgItemText(IDC_STATIC_SHEET_SYLL, buf);
}
// [추가 1] 3D 회전 체크박스 클릭 시 실행되는 함수
void CMFCDlgExDlg::OnBnClickedCheckRotate()
{
    // 체크박스가 켜져 있으면 타이머 시작, 꺼져 있으면 중지
    if (IsDlgButtonChecked(IDC_CHECK_ROTATE))
    {
        SetTimer(1, 20, NULL); // 1번 타이머, 0.02초마다 실행
    }
    else
    {
        KillTimer(1); // 1번 타이머 중지
    }
}

// [추가 2] 타이머가 작동할 때마다 3D 모델을 돌려주는 함수
void CMFCDlgExDlg::OnTimer(UINT_PTR nIDEvent)
{
    // 1번 타이머이고, VTK 창이 켜져 있다면
    if (nIDEvent == 1 && m_vtkWindow)
    {
        // 현재 화면을 담당하는 렌더러 가져오기
        vtkSmartPointer<vtkRenderer> renderer = m_vtkWindow->GetRenderers()->GetFirstRenderer();
        if (renderer)
        {
            // 카메라를 왼쪽으로 1도씩 회전 (Azimuth)
            renderer->GetActiveCamera()->Azimuth(1);
            m_vtkWindow->Render(); // 화면 갱신
        }
    }

    CDialogEx::OnTimer(nIDEvent);
}