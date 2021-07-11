// mdo.cpp
//

/*
*  MSVC++ 4.2を使用し、MFC AppWizard(dll)にてスケルトンを作成した後、
*  PLG_EnumerateCommands/PLG_ExecuteCommand/PLG_Executable/PLG_RequireFiles/
*  PLG_GetCommandIcon/PLG_Initialize
*  を必ず実装してください。
*  必要に応じて、その他のSPIを実装してください。
*  動作が不要な場合でも何もしない空関数を作成してください。
*  
*/
#define _CRT_SECURE_NO_WARNINGS 1
#include "stdafx.h"
#include "mado.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// MadoApp

BEGIN_MESSAGE_MAP(CPlgsmpl2App, CWinApp)
	//{{AFX_MSG_MAP(CPlgsmpl2App)
		// メモ - ClassWizard はこの位置にマッピング用のマクロを追加または削除します。
		//        この位置に生成されるコードを編集しないでください。
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlgsmpl2App の構築

CPlgsmpl2App::CPlgsmpl2App()
{
}

/////////////////////////////////////////////////////////////////////////////

CPlgsmpl2App theApp;
CAboutDlg *theDlg = nullptr;

/* 以降がプラグインのSPIの実装です */
#include <string.h>
#include "mpplugin.h"
#include "resource.h"
#include "xdw_api.h"

PLUGGEDIN_EXPORT( long ) PLG_ExecuteCommand(PLUGGEDIN_STRUCT *ps)
{
    /* このコマンド実行の途中、引数として渡されるDWファイルは開放されています。
       読み書き可能でアクセスできます。
    */
    /* MFCを使用する場合、AFX_MANAGE_STATE( AfxGetStaticModuleState() );を
       必ずコールしてください。これにより、このDLLのリソースが使われるようになります。
    */
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if (strcmp(ps->ps_pszFunction, "MADO"))
		return 0;

    char *filepath=NULL;
    char *tmp1=NULL;
    char *path=NULL;
    filepath=new char[32+(MAX_PATH)];
    tmp1=new char[MAX_PATH];
    path=new char[MAX_PATH];

    if (ps->ps_nFiles <= 0) {
        if (theDlg != nullptr) {
            theDlg->FillGray();
            theDlg->UpdateFileName(path);
        }
    }else{
        strcat(filepath , " ");
        strcpy(filepath, ps->ps_pszFiles[0]);
        _splitpath(filepath, tmp1, tmp1, path, tmp1);
        XDW_DOCUMENT_HANDLE xHandle;
        unsigned int err = XDW_OpenDocumentHandle(filepath, &xHandle, XDW_OPEN_READONLY);
        if (err != 0) {
            if (theDlg != nullptr) {
                theDlg->FillGray();
                theDlg->UpdateFileName(path);
            }
            return 1;
        }
        XDW_HGLOBAL imageHandle;
        XDW_IMAGE_OPTION imageOption;
        imageOption.nSize = sizeof(XDW_IMAGE_OPTION);
        imageOption.nDpi = BMPDPI;
        imageOption.nColor = XDW_IMAGE_MONO_HIGHQUALITY;
        err = XDW_ConvertPageToImageHandle(xHandle, 1, &imageHandle, &imageOption);
        XDW_CloseDocumentHandle(xHandle, NULL);
        if (err != 0) {
            if (theDlg != nullptr) {
                theDlg->FillGray();
                theDlg->UpdateFileName(path);
            }
            return 1;
        }
        else 
        {
            if (theDlg == nullptr) {
                theDlg = new CAboutDlg();
                theDlg->Create(IDD_DIALOG1, NULL);
                theDlg->ShowWindow(SW_SHOW);
            }
            theDlg->UpdateFileName(path);
            int gSize = GlobalSize(imageHandle);
            HGLOBAL bmpHandle = GlobalAlloc(GHND, gSize + BMPFILEHEADERSIZE);
            uint8_t* frombuf = (uint8_t*)GlobalLock(imageHandle);
            uint8_t* tobuf = (uint8_t*)GlobalLock(bmpHandle);

            uint8_t header_buffer[BMPFILEHEADERSIZE];
            MYBMPFILEHEADER* header = (MYBMPFILEHEADER*)header_buffer;
            header->bfType = BMPFILETYPE;
            header->bfSize = BMPFILEHEADERSIZE + gSize;
            header->bfReserved1 = 0;
            header->bfReserved2 = 0;
            header->bfOffbits = BMPDAFULTHEADERSIZE;

            memcpy(tobuf, header_buffer, BMPFILEHEADERSIZE);
            memcpy(tobuf + BMPFILEHEADERSIZE, frombuf, gSize);
            GlobalUnlock(imageHandle);
            GlobalFree(imageHandle);
            GlobalUnlock(bmpHandle);
            IStream* pStream = NULL;
            if (!SUCCEEDED(CreateStreamOnHGlobal(bmpHandle, FALSE, &pStream))) {
                GlobalFree(bmpHandle);
                delete header;
                if (theDlg != nullptr) {
                    theDlg->FillGray();
                }
                return 1;
            }
            if (theDlg->m_cimg != nullptr) {
                theDlg->m_cimg->Destroy();
            }
            theDlg->m_cimg = new CImage();
            if (!SUCCEEDED(theDlg->m_cimg->Load(pStream))) {
                GlobalFree(bmpHandle);
                pStream->Release();
                delete header;
                if (theDlg != nullptr) {
                    theDlg->FillGray();
                }
                return 1;
            }

            GlobalFree(bmpHandle);
            pStream->Release();
                
            CWnd* pControl = theDlg->GetDlgItem(IDC_STATIC_IMAGE);
            CDC* cdc = pControl->GetDC();
            int iWidth, iHeight;
            iWidth = theDlg->m_rect.right - theDlg->m_rect.left;
            iHeight = theDlg->m_rect.bottom - theDlg->m_rect.top;
                
            CDC* pBitmapDC = new CDC();
            CBitmap* cbmp;
            cbmp = CBitmap::FromHandle(*(theDlg->m_cimg));
            pBitmapDC->CreateCompatibleDC(cdc);
            CBitmap* oldbmp = pBitmapDC->SelectObject(cbmp);
                
            cdc->SetStretchBltMode(STRETCH_HALFTONE);
            cdc->SetBrushOrg(0, 0);
            cdc->StretchBlt(0, 0, iWidth, iHeight, pBitmapDC, WINDOW_LEFT, theDlg->m_cimg->GetHeight() - WINDOW_HEIGHT - 1, WINDOW_WIDTH, WINDOW_HEIGHT, SRCCOPY);
            
            if (theDlg->memCDC != nullptr) {
                theDlg->memCDC->SelectObject(theDlg->pOldcbmp);
                theDlg->memCDC->DeleteDC();
            }
            pBitmapDC->SelectObject(oldbmp);
            theDlg->pOldcbmp = oldbmp;
            theDlg->ReleaseDC(cdc);
            theDlg->memCDC = pBitmapDC;
        }
        
    }
    
    delete filepath;
    delete tmp1;
    delete path;
    return 1;
}

/* EnumerateCommands Entry */
PLUGGEDIN_EXPORT( long ) PLG_EnumerateCommands(char* pszEntries, long nBufSize, long* pnRealSize)
{
    *pnRealSize = strlen("MADO") + 1;

    if (nBufSize<*pnRealSize)
        return 0;

    /*
     0ターミネートの文字列のリストを返してください。DWDeskのプラグイン/設定の候補に出ます
    */
   // DoCopy Commands!
    strcpy(pszEntries, "MADO");
	return 1;
}

PLUGGEDIN_EXPORT( long ) PLG_Executable(const char *pszFunction, long nSel)
{
    if (nSel > 1) return 0;
	return 1;
}
PLUGGEDIN_EXPORT( long ) PLG_RequireFiles(const char *pszFunction)
{
	return 1;
}

PLUGGEDIN_EXPORT( HICON ) PLG_GetCommandIcon(const char *pszFunction, long bNormal)
{

	AFX_MANAGE_STATE( AfxGetStaticModuleState() );
	HICON hIcon=NULL;
	unsigned resID=0;
	if (!strcmp(pszFunction, "MADO"))
		resID= IDI_APPICON1;
	else return 0;

	HINSTANCE myInst = (HINSTANCE) AfxGetInstanceHandle();
	if (bNormal)
		hIcon=LoadIcon(myInst,MAKEINTRESOURCE(resID));
	else
		hIcon=(HICON)LoadImage(myInst,MAKEINTRESOURCE(resID),IMAGE_ICON,16,16,0);
	return hIcon;
}

/* Followin SPI s  are NO-op */

PLUGGEDIN_EXPORT( long ) PLG_Initialize(const char* cmd)
{
    /* 必ず 1を返してください. 0を返すとプラグインがロードされません*/
	return 1;
}

PLUGGEDIN_EXPORT( long ) PLG_Finalize(const char*  cmd)
{
	return 1;
}

PLUGGEDIN_EXPORT( long ) PLG_CanFinalize(const char*  cmd)
{
	return 1;
}


PLUGGEDIN_EXPORT( long ) PLG_IsParallel(const char *pszFunction)
{
	return 0;
}


PLUGGEDIN_EXPORT( long ) PLG_CanSetProfile(const char *pszFunction)
{
	return 1;
}

PLUGGEDIN_EXPORT( long ) PLG_SetProfile(const char *pszFunction)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );
	
	return 1;
}


/* IsCloning Entry*/
PLUGGEDIN_EXPORT( long ) PLG_IsCloningCommand(const char *pszFunction)
{
    return 0;
}

/* GetNewClone Entry*/
PLUGGEDIN_EXPORT( long ) PLG_GetNewClone(char* pszNewClone, long bufsize)
{
    return 0;
}

/* ReleaseClone Entry*/
PLUGGEDIN_EXPORT( long ) PLG_ReleaseClone(const char* pszClone)
{
    return 0;
}


/////////////////////////////////////////////////////////////////////////////
// CAboutDlg ダイアログ


CAboutDlg::CAboutDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD, pParent)
{
    m_cimg = nullptr;
    memCDC = nullptr;
	//{{AFX_DATA_INIT(CAboutDlg)
	m_strStat = _T("");
	//}}AFX_DATA_INIT
	m_strStat.LoadString(IDS_STRING1);
}

CAboutDlg::~CAboutDlg()
{
    if (m_cimg != nullptr) {
        m_cimg->Destroy();
    }
    m_cimg = nullptr;
    memCDC->SelectObject(pOldcbmp);
    memCDC->DeleteDC();
}

void CAboutDlg::FillGray() {
    CWnd* pControl = GetDlgItem(IDC_STATIC_IMAGE);
    CDC* cdc = pControl->GetDC();
    CBrush cBrush;
    CBrush* pOldBrush;
    int iWidth, iHeight;
    iWidth = m_rect.right - m_rect.left;
    iHeight = m_rect.bottom - m_rect.top;
    CBitmap* cbmp = new CBitmap();
    cbmp->CreateCompatibleBitmap(cdc, iWidth, iHeight);
    CBitmap* pOldcbmp = memCDC->SelectObject(cbmp);
    cBrush.CreateSolidBrush(RGB(128, 128, 128));
    pOldBrush = memCDC->SelectObject(&cBrush);
    cdc->PatBlt(0, 0, iWidth, iHeight, PATCOPY);
    memCDC->SelectObject(pOldcbmp);
    memCDC->SelectObject(pOldBrush);
    pOldcbmp->DeleteObject();
    pOldBrush->DeleteObject();
    ReleaseDC(cdc);
    if (m_cimg != nullptr) {
        m_cimg->Destroy();
        m_cimg = nullptr;
    }
}

void CAboutDlg::UpdateFileName(char* path) {
    m_strStat = path;
    UpdateData(FALSE);
}

void CAboutDlg::Redraw()
{
    CWnd* pControl = GetDlgItem(IDC_STATIC_IMAGE);
    CDC* cdc = pControl->GetDC();
    int iWidth, iHeight;
    iWidth = m_rect.right - m_rect.left;
    iHeight = m_rect.bottom - m_rect.top;
    CBitmap* cbmp;
    cbmp = CBitmap::FromHandle(*m_cimg);
    CBitmap *oldcbmp = memCDC->SelectObject(cbmp);
    cdc->SetStretchBltMode(STRETCH_HALFTONE);
    cdc->SetBrushOrg(0, 0);
    cdc->StretchBlt(0, 0, iWidth, iHeight, memCDC, WINDOW_LEFT, m_cimg->GetHeight() - WINDOW_HEIGHT - 1, WINDOW_WIDTH, WINDOW_HEIGHT, SRCCOPY);
    memCDC->SelectObject(oldcbmp);
    oldcbmp->DeleteObject();
    theDlg->ReleaseDC(cdc);
    
}

void CAboutDlg::OnOK()
{
    if (theDlg != nullptr) {
        delete theDlg;
    }
    theDlg = nullptr;
}

LRESULT CAboutDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
    // クローズメッセージの場合
    if (message == WM_CLOSE)
    {
        if (theDlg != nullptr) {
            delete theDlg;
        }
        theDlg = nullptr;
    }
    else if (message == WM_PAINT) {
        if (theDlg != nullptr) {
            if (theDlg->m_cimg == nullptr) {
                FillGray();
            }
            else {
                Redraw();
            }
        }
    }

    return CDialog::WindowProc(message, wParam, lParam);
}

BOOL CAboutDlg::OnInitDialog()
{
    if (theDlg != nullptr) {
        CWnd* pControl = theDlg->GetDlgItem(IDC_STATIC_IMAGE);
        pControl->GetWindowRect(&m_rect);
    }
    return CDialog::OnInitDialog();
}


void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	DDX_Text(pDX, IDC_STAT_EDIT, m_strStat);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

