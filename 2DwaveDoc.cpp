// 2DwaveDoc.cpp : implementation of the CMy2DwaveDoc class
//

#include "stdafx.h"
#include "2Dwave.h"

#include "2DwaveDoc.h"
#include <cstring>
#include <ctime>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMy2DwaveDoc

IMPLEMENT_DYNCREATE(CMy2DwaveDoc, CDocument)

BEGIN_MESSAGE_MAP(CMy2DwaveDoc, CDocument)
	//{{AFX_MSG_MAP(CMy2DwaveDoc)
	ON_COMMAND(ID_WAVERECORDMAT, OnWaveRecordmat)
	ON_COMMAND(ID_WAVEINVERSION, OnWaveinversion)
	ON_COMMAND(ID_GRIDVELOCITYMAT, OnGridvelocitymat)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMy2DwaveDoc construction/destruction

CMy2DwaveDoc::CMy2DwaveDoc()
{
	// TODO: add one-time construction code here

}

CMy2DwaveDoc::~CMy2DwaveDoc()
{
}

BOOL CMy2DwaveDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CMy2DwaveDoc serialization

void CMy2DwaveDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CMy2DwaveDoc diagnostics

#ifdef _DEBUG
void CMy2DwaveDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CMy2DwaveDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMy2DwaveDoc commands

void CMy2DwaveDoc::OnWaveRecordmat() 
{
	// TODO: Add your command handler code here
	CTime start,end;//����ʱ��
	CTimeSpan use;//���ʱ��
	CString str;
	str="�����������ɵ����¼��mat�ļ��ɹ�......\n\n��ʼʱ�̣�";
	start=CTime::GetCurrentTime();//���㿪ʼʱ��
	str+=start.Format("%H:%M:%S,%B %d,%Y")+"\n\n";

	if(!m_2DModel.InitialMember())
		AfxMessageBox("Initial member fail......");
	m_2DModel.InitialMember();
	m_2DModel.InitialSource();

	m_2DModel.InitialVelocity();
	//m_2DModel.RecordInitial();
//	m_2DModel.IncreaseVelocity();
//	m_2DModel.ThreeLayer();
	
	m_2DModel.WaveRecord(250.0, 250.0,m_2DModel.m_vecGridVelocity,2,0);
	if(!m_2DModel.CreateRecordmat(0))
		AfxMessageBox("�����������ɵ����¼��mat�ļ�ʧ��......");

	if(!m_2DModel.InitialMember())
		AfxMessageBox("Initial member fail......");
	m_2DModel.InitialMember();
	m_2DModel.InitialSource();

	m_2DModel.InitialVelocity();
	m_2DModel.WaveRecord(250.0,250.0,m_2DModel.m_vecGridVelocity,2,1);

	end=CTime::GetCurrentTime();//�������ʱ��
	str+="����ʱ�̣�"+end.Format("%H:%M:%S,%B %d,%Y")+"\n\n";
	use=end-start;
	str+="������ʱ��"+use.Format("%H:%M:%S");
	if(!m_2DModel.CreateRecordmat(1))
		AfxMessageBox("�����������ɵ����¼��mat�ļ�ʧ��......");
	else
	{
		AfxMessageBox(str);
		return;
	}	
	
}

void CMy2DwaveDoc::OnWaveinversion() 
{
	// TODO: Add your command handler code here
	CTime start,end;
	CTimeSpan use;
	CString str;
	str="���η����������ȵ�mat�ļ��ɹ�......\n\n��ʼʱ��";
	start=CTime::GetCurrentTime();//���㿪ʼʱ��
	str+=start.Format("%H:%M:%S,%B %d,%Y")+"\n\n";

    m_2DModel.Test(4);
//	m_2DModel.Gradient(4);
	
	end=CTime::GetCurrentTime();//�������ʱ��
	str+="����ʱ��"+end.Format("%H:%M:%S,%B %d,%Y")+"\n\n";
	use=end-start;
	str+="������ʱ"+use.Format("%H:%M:%S");
	AfxMessageBox(str);
}

void CMy2DwaveDoc::OnGridvelocitymat() 
{
	// TODO: Add your command handler code here
	//if(!m_2DModel.InitialMember())
	//	AfxMessageBox("Initial member fail......");
	//m_2DModel.InitialVelocity();	
	//
	//if(!m_2DModel.Createvelocitymat(1001))
	//	AfxMessageBox("�����������ȵ�mat�ļ�ʧ��......");
	//else
	//{
	//	AfxMessageBox("�����������ȵ�mat�ļ��ɹ�......");
	//	return;
	//}
	CTime start,end;
	CTimeSpan use;
	CString str;
	str="���η����������ȵ�mat�ļ��ɹ�......\n\n��ʼʱ��";
	start=CTime::GetCurrentTime();//���㿪ʼʱ��
	str+=start.Format("%H:%M:%S,%B %d,%Y")+"\n\n";

    m_2DModel.Test(12);
//	m_2DModel.Gradient(4);
	//m_2DModel.CreateRecordmat();
	
	end=CTime::GetCurrentTime();//�������ʱ��
	str+="����ʱ��"+end.Format("%H:%M:%S,%B %d,%Y")+"\n\n";
	use=end-start;
	str+="������ʱ"+use.Format("%H:%M:%S");
	AfxMessageBox(str);
}
