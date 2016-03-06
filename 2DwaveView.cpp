// 2DwaveView.cpp : implementation of the CMy2DwaveView class
//

#include "stdafx.h"
#include "2Dwave.h"

#include "2DwaveDoc.h"
#include "2DwaveView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMy2DwaveView

IMPLEMENT_DYNCREATE(CMy2DwaveView, CView)

BEGIN_MESSAGE_MAP(CMy2DwaveView, CView)
	//{{AFX_MSG_MAP(CMy2DwaveView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMy2DwaveView construction/destruction

CMy2DwaveView::CMy2DwaveView()
{
	// TODO: add construction code here

}

CMy2DwaveView::~CMy2DwaveView()
{
}

BOOL CMy2DwaveView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CMy2DwaveView drawing

void CMy2DwaveView::OnDraw(CDC* pDC)
{
	CMy2DwaveDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CMy2DwaveView printing

BOOL CMy2DwaveView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CMy2DwaveView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CMy2DwaveView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CMy2DwaveView diagnostics

#ifdef _DEBUG
void CMy2DwaveView::AssertValid() const
{
	CView::AssertValid();
}

void CMy2DwaveView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CMy2DwaveDoc* CMy2DwaveView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMy2DwaveDoc)));
	return (CMy2DwaveDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMy2DwaveView message handlers
