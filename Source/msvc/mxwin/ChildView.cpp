
// ChildView.cpp : CChildView 类的实现
//

#include "stdafx.h"
#include "mxwin.h"
#include "ChildView.h"

#include "mapnix.h"
#include <GdiPlus.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CChildView

CChildView::CChildView()
{
	ULONG_PTR gdiplusToken;
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	Gdiplus::GdiplusStartup(&gdiplusToken,&gdiplusStartupInput,NULL);
}

CChildView::~CChildView()
{
}


BEGIN_MESSAGE_MAP(CChildView, CWnd)
	ON_WM_PAINT()
END_MESSAGE_MAP()



// CChildView 消息处理程序

BOOL CChildView::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		::LoadCursor(NULL, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW+1), NULL);

	int rc = register_info("E:\\ProSource\\MyProject\\mapnikview\\Source\\lib\\");
	if(rc != 0){
		TRACE0("地图信息注册失败\n");
		return FALSE;
	}

	img_buf = NULL;

	return TRUE;
}

void CChildView::OnPaint() 
{
	CPaintDC dc(this); // 用于绘制的设备上下文

	// 获取绘图区域
	CRect rect;
	GetClientRect(&rect);
	Gdiplus::Graphics graphics(dc.m_hDC); // 获取gdi绘图对象

	// TODO: 在此处添加消息处理程序代码

	// 不要为绘制消息而调用 CWnd::OnPaint()
	int width = rect.Width();
	int height = rect.Height();

	// 构造地图对象
	void *map_ptr = load_map_file("mapconfig.xml",width,height);

	// 构建绘图缓冲区
	if(img_buf){
		delete img_buf;
	}
	int imgl = sizeof(unsigned);
	int len = width*height*imgl; 
	img_buf = new unsigned char[len];
	

	if(map_ptr){
		render_agg(map_ptr,*img_buf);
	}

	if(img_buf){
		//加载PNG图片
		//Gdiplus::Bitmap background(L"demo256.png");
		BITMAPINFO bmi;
		memset(&bmi, 0, sizeof(bmi));
		bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmi.bmiHeader.biWidth = width;
		bmi.bmiHeader.biHeight = height;
		bmi.bmiHeader.biPlanes = 1;
		bmi.bmiHeader.biCompression = BI_RGB;
		bmi.bmiHeader.biBitCount = 32;

		Gdiplus::Bitmap background(&bmi,img_buf);

		Gdiplus::Point points[] = { Gdiplus::Point(0, height), Gdiplus::Point(width, height),
			Gdiplus::Point(0, 0)};
		graphics.DrawImage(&background,points,3);
		//graphics.DrawImage(&background,0,0,800,600);
	}
	
	// 释放地图对象
	map_destroy(map_ptr);
	CWnd::OnPaint();
	
	/*CDC dcMem;

	dc.CreateCompatibleDC(&dcMem);
	HBITMAP cross = (HBITMAP)LoadImage(NULL, _T("c:\\captureqwsx.bmp") ,IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
	dc.SelectObject(cross);
	dc.BitBlt(10,10,200,200,&dcMem,0,0,SRCCOPY);
	dc.ReleaseOutputDC(); */
}

