// 2DwaveDoc.h : interface of the CMy2DwaveDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_2DWAVEDOC_H__B4E4A6D2_3892_4CE9_9C67_DD8CD284BFFC__INCLUDED_)
#define AFX_2DWAVEDOC_H__B4E4A6D2_3892_4CE9_9C67_DD8CD284BFFC__INCLUDED_

#include "2DModel.h"  //Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CMy2DwaveDoc : public CDocument
{
protected: // create from serialization only
	CMy2DwaveDoc();
	DECLARE_DYNCREATE(CMy2DwaveDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMy2DwaveDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	C2DModel m_2DModel;
	virtual ~CMy2DwaveDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CMy2DwaveDoc)
	afx_msg void OnWaveRecordmat();
	afx_msg void OnWaveinversion();
	afx_msg void OnGridvelocitymat();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_2DWAVEDOC_H__B4E4A6D2_3892_4CE9_9C67_DD8CD284BFFC__INCLUDED_)
