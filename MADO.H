
#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// メイン シンボル
#include "atlimage.h"


#define BMPFILEHEADERSIZE 14
#define BMPINFOHEADERSIZE 40
#define BMPDAFULTHEADERSIZE (BMPFILEHEADERSIZE + BMPINFOHEADERSIZE)
#define BMPFILETYPE 0x4D42

#define WINDOW_WIDTH 500 //表示する範囲の幅(px)
#define WINDOW_HEIGHT 170 //表示する範囲の高さ(px)
#define WINDOW_LEFT 110 //表示する四角形の左からの位置(px)
#define WINDOW_BOTTOM 0 //表示する四角形の下からの位置(px)
#define BMPDPI 200 //Docuworks文書のを描画するDpi

/////////////////////////////////////////////////////////////////////////////
// CPlgsmpl2App
// このクラスの動作の定義に関しては plgsmpl2.cpp ファイルを参照してください。
//

class CPlgsmpl2App : public CWinApp
{
public:
	CPlgsmpl2App();

// オーバーライド
	// ClassWizard は仮想関数を生成しオーバーライドします。
	//{{AFX_VIRTUAL(CPlgsmpl2App)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CPlgsmpl2App)
		// メモ -  ClassWizard はこの位置にメンバ関数を追加または削除します。
		//         この位置に生成されるコードを編集しないでください。
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg ダイアログ

class CAboutDlg : public CDialog
{
// コンストラクション
public:
	CAboutDlg(CWnd* pParent = NULL);   // 標準のコンストラクタ
	~CAboutDlg();
// ダイアログ データ
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_DIALOG1 };
	RECT m_rect;
	CImage *m_cimg;
	CString	m_strStat;
	CDC* memCDC;
	CBitmap* pOldcbmp;



	//}}AFX_DATA
	LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	void Redraw();
	void FillGray();
	void UpdateFileName(char*);
	void OnOK();

	BOOL OnInitDialog();

// オーバーライド
	// ClassWizard は仮想関数を生成しオーバーライドします。
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	//}}AFX_VIRTUAL

// インプリメンテーション
protected:

	// 生成されたメッセージ マップ関数
	//{{AFX_MSG(CAboutDlg)
		// メモ: ClassWizard はこの位置にメンバ関数を追加します。
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// DIBのファイルヘッダ
#pragma pack(2)
typedef struct MYBMPFILEHEADER
{
	uint16_t bfType;
	uint32_t bfSize;
	uint16_t bfReserved1;
	uint16_t bfReserved2;
	uint32_t bfOffbits;
}MYBMPFILEHEADER;
#pragma pack()

