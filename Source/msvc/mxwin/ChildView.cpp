
// ChildView.cpp : CChildView ���ʵ��
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



// CChildView ��Ϣ�������

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
		TRACE0("��ͼ��Ϣע��ʧ��\n");
		return FALSE;
	}

	img_buf = NULL;

	return TRUE;
}

void CChildView::OnPaint() 
{
	CPaintDC dc(this); // ���ڻ��Ƶ��豸������

	// ��ȡ��ͼ����
	CRect rect;
	GetClientRect(&rect);
	Gdiplus::Graphics graphics(dc.m_hDC); // ��ȡgdi��ͼ����

	// TODO: �ڴ˴������Ϣ����������

	// ��ҪΪ������Ϣ������ CWnd::OnPaint()
	int width = rect.Width();
	int height = rect.Height();

	// �����ͼ����
	void *map_ptr = load_map_file("mapconfig.xml",width,height);

	// ������ͼ������
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
		//����PNGͼƬ
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
	
	// �ͷŵ�ͼ����
	map_destroy(map_ptr);
	CWnd::OnPaint();
	
	/*CDC dcMem;

	dc.CreateCompatibleDC(&dcMem);
	HBITMAP cross = (HBITMAP)LoadImage(NULL, _T("c:\\captureqwsx.bmp") ,IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
	dc.SelectObject(cross);
	dc.BitBlt(10,10,200,200,&dcMem,0,0,SRCCOPY);
	dc.ReleaseOutputDC(); */
}

