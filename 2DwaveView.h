// 2DwaveView.h : interface of the CMy2DwaveView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_2DWAVEVIEW_H__DCB60798_0D59_4543_8754_293959A3ED9C__INCLUDED_)
#define AFX_2DWAVEVIEW_H__DCB60798_0D59_4543_8754_293959A3ED9C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CMy2DwaveView : public CView
{
protected: // create from serialization only
	CMy2DwaveView();
	DECLARE_DYNCREATE(CMy2DwaveView)

// Attributes
public:
	CMy2DwaveDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMy2DwaveView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMy2DwaveView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CMy2DwaveView)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in 2DwaveView.cpp
inline CMy2DwaveDoc* CMy2DwaveView::GetDocument()
   { return (CMy2DwaveDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_2DWAVEVIEW_H__DCB60798_0D59_4543_8754_293959A3ED9C__INCLUDED_)
