// 2DModel.cpp : implementation file
//

#include "stdafx.h"
#include "2Dwave.h"
#include "2DModel.h"
#include "2DwaveDoc.h"
#include "2DwaveView.h"
#include <cmath>
#include <cstring>
#include <umfpack.h>
//#include <omp.h>
#include "mat.h"
#include "matrix.h"
#include <complex>
#include <string>

#pragma comment(lib,"libumfpack.lib")
#pragma comment(lib, "libamd.lib")


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
#define pi 3.1415926

/////////////////////////////////////////////////////////////////////////////

// C2DModel
C2DModel::C2DModel()
{
	/*2D模型的基本参数*/
	m_GridcellX=10.0;
	m_GridcellZ=10.0;
	m_GridnumX=50;
	m_GridnumZ=50;
	m_tstep=0.001;
	m_tsearch=0.5;
	m_tnum=(int)(m_tsearch/m_tstep);
	PML=20;
	NS=1;

	m_ShotX = 0.0f;
	m_ShotZ = 0.0f;
	m_ShotGridX=(int)(m_ShotX/m_GridcellX);
	m_ShotGridZ=(int)(m_ShotZ/m_GridcellZ);
}

C2DModel::~C2DModel()
{
}

BOOL C2DModel::InitialMember()
{
	m_vecGridRecord.clear();
	m_vecGridRecord.assign((m_GridnumX+1)*(m_tnum),0.0);
	
	return TRUE;
}

BOOL C2DModel::InitialSource()
{
	m_Source.clear();
	m_Source.assign(m_tnum,0.0);
	
	return TRUE;
}


void C2DModel::InitialVelocity()
{
	m_ShotGridX=(int)(m_ShotX/m_GridcellX);
	m_ShotGridZ=(int)(m_ShotZ/m_GridcellZ);

	m_vecGridVelocity.clear();
	m_vecGridVelocity.assign((m_GridnumX+1)*(m_GridnumZ+1),0.0);

	/*if(!IncreaseVelocity())
		AfxMessageBox("均匀递增模型初始化慢度失败!");
	else
		return;*/
	//
/*	if(!ThreeLayer())
		AfxMessageBox("三层模型初始化慢度失败!");
	else
		return;
//*/
/*	if(!Faultage())
		AfxMessageBox("断层模型初始化慢度失败!");
	else
		return;
*/
	if(!ConstVelocity())
		AfxMessageBox("匀速模型初始化慢度失败!");
	else
		return;
/*	if(!Highspeed_model())
		AfxMessageBox("高速模型初始化慢度失败!");
	else
		return;//*/
}

//匀速模型
BOOL C2DModel::ConstVelocity()
{
	int i,j;
	double speed0;
	GRIDCOOR origin;
    speed0 = 2000.0f;
	for(i=0;i<m_GridnumX+1;i++)
	{
		origin.i=i;
		for(j=0;j<m_GridnumZ+1;j++)
		{
			origin.j=j;
			m_vecGridVelocity[GetVelocitystore(origin)]=speed0;
		}
	}
  return TRUE;

}

//匀速递增模型
BOOL C2DModel::IncreaseVelocity()
{
	int i,j;
	double speed0,speed;
	GRIDCOOR origin;
	speed0=2000.0f;

	for(j=0;j<=m_GridnumZ;j++)
	{
		origin.j=j;
		speed=j*20+speed0;
		for(i=0;i<=m_GridnumX;i++)
		{
			origin.i=i;
			m_vecGridVelocity[GetVelocitystore(origin)]=speed;
		}
	}
	return TRUE;
}

//三层模型
BOOL C2DModel::ThreeLayer()
{
	int i,j,z1,z2;
	double speed0,speed1,speed2;
	GRIDCOOR origin;
	speed0=2000.0f;
	speed1=2500.0f;
	speed2=3000.0f;
	z1=22;
	z2=44;
	/*
	for(j=0;j<z1;j++)
	{
		origin.j=j;
		for(i=1;i<=m_GridnumX-1;i++)
		{
			origin.i=i;
			m_vecGridVelocity[GetVelocitystore(origin)]=speed0;
		}
	}
	for(j=z1;j<z2;j++)
	{
		origin.j=j;
		for(i=1;i<=m_GridnumX-1;i++)
		{
			origin.i=i;
			m_vecGridVelocity[GetVelocitystore(origin)]=speed1;
		}
	}
	for(j=z2;j<=m_GridnumZ-1;j++)
	{
		origin.j=j;
		for(i=1;i<=m_GridnumX-1;i++)
		{
			origin.i=i;
			m_vecGridVelocity[GetVelocitystore(origin)]=speed2;
		}
	}*/
	for(i=0;i<=m_GridnumX;i++)
	{
		for(j=0;j<=m_GridnumZ;j++)
		{
			origin.i=i;
			origin.j=j;
			if(j<=z1)
				m_vecGridVelocity[GetVelocitystore(origin)]=speed0;
			else if(j<=z2)
				m_vecGridVelocity[GetVelocitystore(origin)]=speed1;
			else
				m_vecGridVelocity[GetVelocitystore(origin)]=speed2;
		}
	}
	return TRUE;
}

//断层faultage
BOOL C2DModel::Faultage()
{
	int i,j;
	int x1,z1,z2;
	double speed0,speed,highspeed;
	GRIDCOOR origin;

	x1=m_GridnumX/2;
	z1=15;
	z2=30;
	speed0=500.0f;
	highspeed=2000;

	for(j=0;j<z1;j++)
	{
		origin.j=j;
		speed=j*20.0+speed0;
		for(i=1;i<=m_GridnumX-1;i++)
		{
			origin.i=i;
			m_vecGridVelocity[GetVelocitystore(origin)]=speed;
		}
	}
	for(j=z1;j<z2;j++)
	{
		origin.j=j;
		speed=j*20.0+speed0;
		for(i=1;i<x1;i++)
		{
			origin.i=i;
			m_vecGridVelocity[GetVelocitystore(origin)]=speed;
		}
		for(i=x1;i<=m_GridnumX-1;i++)
		{
			origin.i=i;
			m_vecGridVelocity[GetVelocitystore(origin)]=highspeed;
		}
	}
	for(j=z2;j<=m_GridnumZ-1;j++)
	{
		origin.j=j;
		for(i=1;i<=m_GridnumX-1;i++)
		{
			origin.i=i;
			m_vecGridVelocity[GetVelocitystore(origin)]=highspeed;
		}
	}
	return TRUE;
}

BOOL C2DModel::Highspeed_model()
{
	int i,j,x1,x2,z1,z2;
	double speed0,speed1;
	GRIDCOOR origin;
	speed0=2000.0f;
	speed1=2800.0f;
	x1=m_GridnumX/3;
	x2=2*m_GridnumX/3;
	z1=10;
	z2=25;
	for(j=0;j<z1;j++)
	{
		origin.j=j;
		for(i=0;i<=m_GridnumX;i++)
		{
			origin.i=i;
			m_vecGridVelocity[GetVelocitystore(origin)]=speed0;
		}
	}
	for(j=z1;j<z2;j++)
	{
		origin.j=j;
		for(i=0;i<x1;i++)
		{
			origin.i=i;
			m_vecGridVelocity[GetVelocitystore(origin)]=speed0;
		}
		for(i=x1;i<x2;i++)
		{
			origin.i=i;
			m_vecGridVelocity[GetVelocitystore(origin)]=speed1;
		}
		for(i=x2;i<=m_GridnumX;i++)
		{
			origin.i=i;
			m_vecGridVelocity[GetVelocitystore(origin)]=speed0;
		}
	}
	for(j=z2;j<=m_GridnumZ;j++)
	{
		origin.j=j;
		for(i=0;i<=m_GridnumX;i++)
		{
			origin.i=i;
			m_vecGridVelocity[GetVelocitystore(origin)]=speed0;
		}
	}
	/*for(j=0;j<=m_GridnumZ-1;j++)
	{
		for(i=1;i<=m_GridnumX-1;i++)
		{
			origin.i=i;
			origin.j=j;
			if(i>=x1 && i<=x2 && j>=z1 && j<=z2)
				m_vecGridVelocity[GetVelocitystore(origin)]=speed1;
			else
				m_vecGridVelocity[GetVelocitystore(origin)]=speed0;
		}
	}*/
	return TRUE;
}

BOOL C2DModel::Highspeed_model_tmp()
{
	int i,j,x1,x2,z1,z2;
	double speed0,speed1;
	GRIDCOOR origin;
	speed0=2000.0f;
	speed1=2800.0f;
	x1=10;
	x2=20;
	z1=10;
	z2=25;
	for(j=0;j<z1;j++)
	{
		origin.j=j;
		for(i=1;i<=m_GridnumX-1;i++)
		{
			origin.i=i;
			m_tmpGridVelocity[GetVelocitystore(origin)]=speed0;
		}
	}
	for(j=z1;j<z2;j++)
	{
		origin.j=j;
		for(i=1;i<x1;i++)
		{
			origin.i=i;
			m_tmpGridVelocity[GetVelocitystore(origin)]=speed0;
		}
		for(i=x1;i<x2;i++)
		{
			origin.i=i;
			m_tmpGridVelocity[GetVelocitystore(origin)]=speed1;
		}
		for(i=x2;i<=m_GridnumX-1;i++)
		{
			origin.i=i;
			m_tmpGridVelocity[GetVelocitystore(origin)]=speed0;
		}
	}
	for(j=z2;j<=m_GridnumZ-1;j++)
	{
		origin.j=j;
		for(i=1;i<=m_GridnumX-1;i++)
		{
			origin.i=i;
			m_tmpGridVelocity[GetVelocitystore(origin)]=speed0;
		}
	}
	return TRUE;
}

BOOL C2DModel::Step_model()
{
	int i,j;
	double speed0 = 2000.0, speed1=2500.0;
	GRIDCOOR origin;	
	for(i=1;i<m_GridnumX/2;i++)
	{
		origin.i=i;
		for(j=0; j<2*m_GridnumZ/5;j++)
			{
				origin.j=j;
				m_vecGridVelocity[GetVelocitystore(origin)]=speed0;
			}
		for(j = 2* m_GridnumZ/5; j < m_GridnumZ;j++)
			{
				origin.j=j;
				m_vecGridVelocity[GetVelocitystore(origin)]=speed1;
			}
	}
	for(i=m_GridnumX/2; i<m_GridnumX; i++)
	{
		origin.i=i;
		for(j=0;j<m_GridnumZ/4;j++)
			{
				origin.j=j;
				m_vecGridVelocity[GetVelocitystore(origin)]=speed0;
			}
		for(j = m_GridnumZ/4; j < m_GridnumZ;j++)
			{
				origin.j=j;
				m_vecGridVelocity[GetVelocitystore(origin)]=speed1;
			}
	}
	return true;
}

BOOL C2DModel::Step_model_tmp()
{
	int i,j;
	double speed0 = 2000.0, speed1=2200.0;
	GRIDCOOR origin;	
	for(i=1;i<m_GridnumX/2;i++)
	{
		origin.i=i;
		for(j=0; j<2*m_GridnumZ/5;j++)
		{
			origin.j=j;
			m_tmpGridVelocity[GetVelocitystore(origin)]=speed0;
		}
		for(j = 2* m_GridnumZ/5; j < m_GridnumZ;j++)
		{
			origin.j=j;
			m_tmpGridVelocity[GetVelocitystore(origin)]=speed1;
		}
	}
	for(i=m_GridnumX/2; i<m_GridnumX; i++)
	{
		origin.i=i;
		for(j=0;j<m_GridnumZ/4;j++)
		{
			origin.j=j;
			m_tmpGridVelocity[GetVelocitystore(origin)]=speed0;
		}
		for(j = m_GridnumZ/4; j < m_GridnumZ;j++)
		{
			origin.j=j;
			m_tmpGridVelocity[GetVelocitystore(origin)]=speed1;
		}
	}
	return true;
}


inline int C2DModel::GetVelocitystore(GRIDCOOR origin)
{
	int locate;
	locate=(origin.i)*(m_GridnumZ+1)+origin.j;

	return locate;
}

inline int C2DModel::GetUstore(int i,int j)//
{
	int locate;
	locate=i*(m_GridnumZ+1)+j;

	return locate;
}


inline int C2DModel::Get2Ustore(int i,int j)
{
	int locate;
	locate=i*(m_GridnumZ/2+1)+j;

	return locate;
}


inline int C2DModel::GetRecordstore(int i,int t)
{
	int locate;
	locate=(t-1)*(m_GridnumX+1)+i;
   // locate=(i-1)*m_tnum+(t-1);
	//locate=t*(m_GridnumX+1)+i;
	return locate;
}

inline int C2DModel::GetURstore(int i,int j,int t)
{
    int locate;
    locate=(t-1)*(m_GridnumX+1)*(m_GridnumZ+1)+i*(m_GridnumZ+1)+j;
    return locate;
}

//生成网格慢度mat文件
BOOL C2DModel::Createvelocitymat(int iter)
{
	//变量定义及初始化
	MATFile *pmat;//为mat文件定义指针
	mxArray *pslow;
	
	char buff[10];
	string tmpfile = "iter";
    itoa(iter, buff, 10);
	tmpfile += buff;
	tmpfile += ".mat";
    const char *filename = tmpfile.c_str();

	mwSize ndims[2];
	double *pr; 

	ndims[0]=m_GridnumZ+1;
	ndims[1]=m_GridnumX+1;


	pslow=mxCreateNumericArray(2,ndims,mxDOUBLE_CLASS,mxREAL);

	pr=mxGetPr(pslow);

	vector<double>::iterator pvec; 
	pvec=m_vecGridVelocity.begin();
	memcpy(pr,&pvec[0],(m_GridnumZ+1)*(m_GridnumX+1)*sizeof(double));


	char str[]="No1.Begin creating to store grid traveltime file named GridSlow.mat";
	CMy2DwaveView* pView=(CMy2DwaveView*)((::AfxGetApp()->m_pMainWnd)->GetActiveWindow());

	CDC *pDC=pView->GetDC();
	pDC->TextOut(10,100,str);


	pmat=matOpen(filename,"w");
	if(pmat==NULL)
	{
		::AfxMessageBox("Error creating file GridVelocity.mat!");
		return FALSE;
	}


	matPutVariable(pmat,"GridVelocity",pslow);


	matClose(pmat);
	mxDestroyArray(pslow);

	return TRUE;
}


BOOL C2DModel::CreateRecordmat(int i)
{
	MATFile *pmat;
	mxArray *precord;
	char *filename;
	if(i==0)
		filename="WaveRecord_U.mat";
	else
		filename="WaveRecord_H.mat";

	mwSize ndims[2];
	double *pr; 

	ndims[0]=m_GridnumX+1;
	ndims[1]=m_tnum;


	precord=mxCreateNumericArray(2,ndims,mxDOUBLE_CLASS,mxREAL);

	pr=mxGetPr(precord);

	vector<double>::iterator pvec;  
	pvec=m_vecGridRecord.begin();
	memcpy(pr,&pvec[0],(m_GridnumX+1)*m_tnum*sizeof(double));


	char str[]="No2.Begin creating to store wave record file named WaveRecord.mat";

	CMy2DwaveView* pView=(CMy2DwaveView*)((::AfxGetApp()->m_pMainWnd)->GetActiveWindow());

	CDC *pDC=pView->GetDC();
	pDC->TextOut(10,50,str);


	pmat=matOpen(filename,"w");
	if(pmat==NULL)
	{
		::AfxMessageBox("Error creating file WaveRecord.mat!");
		return FALSE;
	}

	matPutVariable(pmat,"WaveRecord",precord);


	matClose(pmat);
	mxDestroyArray(precord);

	return TRUE;
}

BOOL C2DModel::Create_testmat(string s,int i)
{
	MATFile *pmat;
	mxArray *precord;
	char a[4];
	itoa(i,a,10);
	string s1="Wave_Test_",s2(a),s3=".mat";
	string s_=(s1+s+s2+s3);
	char filename[20];
	strcpy(filename,s_.c_str());

	mwSize ndims[2];
	double *pr; 

	ndims[0]=m_GridnumX+1;
	ndims[1]=m_GridnumZ+1;


	precord=mxCreateNumericArray(2,ndims,mxDOUBLE_CLASS,mxREAL);

	pr=mxGetPr(precord);

	vector<double>::iterator pvec;  
	pvec=m_vecGridUNext.begin();
	memcpy(pr,&pvec[0],(m_GridnumX+1)*(m_GridnumZ+1)*sizeof(double));


	//char str[]="No2.Begin creating to store wave record file named WaveRecord.mat";

	//CMy2DwaveView* pView=(CMy2DwaveView*)((::AfxGetApp()->m_pMainWnd)->GetActiveWindow());

	//CDC *pDC=pView->GetDC();
	//pDC->TextOut(10,50,str);


	pmat=matOpen(filename,"w");
	/*if(pmat==NULL)
	{
		::AfxMessageBox("Error creating file WaveRecord.mat!");
		return FALSE;
	}*/

	matPutVariable(pmat,"WaveRecord",precord);


	matClose(pmat);
	mxDestroyArray(precord);

	return TRUE;
}

BOOL C2DModel::SaveVelocity(CString str, int n, vector<double> vec, int width, int depth)
{
	int i,j;
	char buffer[10];
	char EXTFileName[] = {".bin"};
	CFile cf;
	itoa(n,buffer,10);
	str+=buffer;
	str+=EXTFileName;
	cf.Open(str,CFile::modeCreate|CFile::modeWrite);
	float tmp;
	for(i=0;i<=width;i++)  //点数与vec赋值的点数一致，否则会溢出。
	{
		for(j=0;j<=depth;j++)
		{
			tmp=vec[i*(depth+1)+j];
			cf.Write(&tmp,sizeof(tmp));
		}
	}
	cf.Close();
	
	return true;
}

void C2DModel::OpenVelocity(CString VelName)
{

	m_vecGridVelocity.clear();
	m_vecGridVelocity.assign((m_GridnumX+1)*(m_GridnumZ+1),0);

	CFile cf;
	cf.Open(VelName,CFile::modeRead);
	int length = (int)(cf.GetLength());
	float *vec = new float[length+1];
	vec[length] = 0;
	cf.Read(vec,length);	

	for(int i=0; i<m_GridnumX; i++)
		for(int j=0; j<m_GridnumZ; j++)//
		{
			m_vecGridVelocity[i*m_GridnumZ+j]=vec[i*m_GridnumZ+j];
		}
	cf.Close();
	delete []vec;		
}

void C2DModel::WaveRecord(double sx, double sy,vector<double> m_Vel,int s,int a)
{                            //炮点坐标
	m_ShotGridX=(int)(sx/m_GridcellX);//炮点网格下标
	m_ShotGridZ=(int)(sy/m_GridcellZ);
	RecordInitial();
	for(m_t=1;m_t<=m_tnum;m_t++)
    {
		m_Source[m_t-1]=SourceG(m_t);
	}
if(a==0)
    for(m_t=1;m_t<=m_tnum;m_t++)
	{
		UpdateNextU(m_Vel,2);
		if(!(m_t%5)||m_t<5)
			Create_testmat("U",m_t);

		ShiftUstore();     //加C的程序区别在于Unext有没有clear，不加的话，release正常，DEBUG报错？resize即正常
	}
else if(a==1)
{
	Sourcet=0;
	for(m_t=1;m_t<=m_tnum;m_t++)
	{
		UpdateNextUH(m_Vel,2);
		Sourcet+=SourceG(m_t)*m_tstep;
		if(!(m_t%5)||m_t<5)
			Create_testmat("H",m_t);                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      
		//ShiftUstore();     
	}

}
else 
//{
//	for(m_t=1;m_t<=m_tnum;m_t++)
//	{
//		UpdateNextU(m_Vel,2);
//		if(m_t%5)
//			Create_testmat("U",m_t);
//
//		ShiftUstore();     //加C的程序区别在于Unext有没有clear，不加的话，release正常，DEBUG报错？resize即正常
//	}
//
//	for(m_t=1;m_t<=m_tnum;m_t++)
//	{
//		UpdateNextUH(m_Vel,3);
//		if(m_t%5)
//			Create_testmat("H",m_t);
//		//ShiftUstore();     
//	}
//}
//	
;
		

}



void C2DModel::RecordInitial()
{
	m_vecGridUOrigin.clear();
	m_vecGridUOrigin.assign((m_GridnumX+1)*(m_GridnumZ+1),0.0);
	m_vecGridUNow.clear();
	m_vecGridUNow.assign((m_GridnumX+1)*(m_GridnumZ+1),0.0);
	
	m_vecGridUNext.clear();
	m_vecGridUNext.assign((m_GridnumX+1)*(m_GridnumZ+1),0.0);

	m_vecGridUA.clear();
	m_vecGridUA.assign((m_GridnumX)*(m_GridnumZ-1),0.0);//正常网格
	m_vecGridUB.clear();
	m_vecGridUB.assign((m_GridnumX-1)*(m_GridnumZ),0.0);

	m_vecALX.clear();
	m_vecALX.assign((PML+1)*(2*PML+m_GridnumZ-1),0.0);//最外侧pml层不取交错网格点
	m_vecALZ.clear();
	m_vecALZ.assign(PML*(2*PML+m_GridnumZ),0.0);
	m_vecARX.clear();
	m_vecARX.assign((PML+1)*(2*PML+m_GridnumZ-1),0.0);
	m_vecARZ.clear();
	m_vecARZ.assign(PML*(2*PML+m_GridnumZ),0.0);
	
	m_vecAUX.clear();
	m_vecAUX.assign((m_GridnumX-2)*PML,0.0);
	m_vecAUZ.clear();
	m_vecAUZ.assign((m_GridnumX-1)*(PML+1),0.0);
	m_vecADX.clear();
	m_vecADX.assign((m_GridnumX-2)*PML,0.0);
	m_vecADZ.clear();
	m_vecADZ.assign((m_GridnumX-1)*(PML+1),0.0);
	
	
	m_vecULX.clear();//正常网格 左边区域 x方向位移
	m_vecULX.assign((PML+1)*(2*PML+m_GridnumZ+1),0.0);
	m_vecULZ.clear();
	m_vecULZ.assign((PML+1)*(2*PML+m_GridnumZ+1),0.0);

	m_vecURX.clear();
	m_vecURX.assign((PML+1)*(2*PML+m_GridnumZ+1),0.0);
	m_vecURZ.clear();
	m_vecURZ.assign((PML+1)*(2*PML+m_GridnumZ+1),0.0);
	
	m_vecUUX.clear();
	m_vecUUX.assign((m_GridnumX-1)*(PML+1),0.0);
	m_vecUUZ.clear();
	m_vecUUZ.assign((m_GridnumX-1)*(PML+1),0.0);

	m_vecUDX.clear();
	m_vecUDX.assign((m_GridnumX-1)*(PML+1),0.0);
	m_vecUDZ.clear();
	m_vecUDZ.assign((m_GridnumX-1)*(PML+1),0.0);



}


// 混合网格
void C2DModel::UpdateNextU(vector<double> m_Vel,int s)
{
	//计算gridenext   边界没计算
	//m_vecGridUNow[GetUstore(25,10)]=1;

	int i,j,k;
	double velocity,a1,a2,x1,x2,x3,x4,z1,z2,z3,z4,p1,p2;
	double src,alphax,alphaxm,alphaz,alphazm;
	int L=1;
    
    src=0.0;
	//常规网格法计算下一个时刻的位移（m_vecGridUNext）

		for(i=1;i<=m_GridnumX-1;i++)//第一列和最后一列不计算，由后面的交错网格方法计算
		{
			for(j=1;j<=m_GridnumZ-1;j++)//第一行和最后一行不计算
			{
				Compoint.i=i;
				Compoint.j=j;
				velocity=m_Vel[GetVelocitystore(Compoint)];
				a1=m_vecGridUNow[GetUstore(i,j)];
				a2=m_vecGridUOrigin[GetUstore(i,j)];
				x1=m_vecGridUNow[GetUstore(i+1,j)];
				x2=m_vecGridUNow[GetUstore(i-1,j)];
				z1=m_vecGridUNow[GetUstore(i,j+1)];
				z2=m_vecGridUNow[GetUstore(i,j-1)];

				if (i==m_ShotGridX&&j==m_ShotGridZ)
				{
					if(s==0)
						src =  m_Source[m_t-1];
					else if(s==1)
					{

						/* src =    SourceGT(m_t);*/

						if(m_t==1)
							src =    (m_AvecArecord[GetURstore(i,j,m_t+1)] - m_AvecArecord[GetURstore(i,j,m_t)])
							/(m_tstep*m_tstep)*(2)/(velocity*velocity*velocity);
						else if(m_t>1&&m_t<m_tnum)
							src =    (m_AvecArecord[GetURstore(i,j,m_t+1)]+m_AvecArecord[GetURstore(i,j,m_t-1)]-2*m_AvecArecord[GetURstore(i,j,m_t)])
							/(m_tstep*m_tstep)*(2)/(velocity*velocity*velocity);
						else if (m_t==m_tnum)
							src =    (m_AvecArecord[GetURstore(i,j,m_t-1)] -m_AvecArecord[GetURstore(i,j,m_t)]) 
							/(m_tstep*m_tstep)*(2)/(velocity*velocity*velocity);


					}  
					else if (s==2)

						src = SourceG(m_t);

					else if(s==3&&m_t==1)
						src=1;

					else
						src = 0.0;
				}
				else
					src=0;

			

				m_vecGridUNext[GetUstore(i,j)]=U1(a1,a2,x1,x2,z1,z2,velocity)-src*velocity*velocity*m_tstep*m_tstep;
				//m_vecGridUNext[GetUstore(i,j)]=src;

			}

			m_vecGridRecord[GetRecordstore(i,m_t)]+=m_vecGridUNext[GetUstore(i,L)];

		}
/******************************PML边界********************************************/

 	alphaxm=alphazm=800;
/////////////////////左边界Lside/////////////////////////////////////////
	alphax=alphaxm;
	alphaz=alphazm;
	//ALX   交错网格点  左边区域 在x方向上取得 x方向的位移
	for (i=0; i<PML+1; i++)
	{
		for (j=0; j<2*PML+m_GridnumZ-1; j++)
		{
			alphax=alphaxm*(1-sin(i*3.14159/2/PML));
			if (i==0)
			{
				a1=m_vecALX[j];          //自己
				p1=m_vecULX[2*PML+m_GridnumZ+1+j+1]+m_vecULZ[2*PML+m_GridnumZ+1+j+1];
				p2=p1;
				m_vecALX[j]=UPMLabsorb(a1,p1,p2,alphax,m_GridcellX);
			}
			else if (i==PML)
			{
				a1=m_vecALX[i*(2*PML+m_GridnumZ-1)+j];
				if (j<PML)
				{
					p1=m_vecUUX[j+1]+m_vecUUZ[j+1];
				}
				else if (j<PML+m_GridnumZ-1)
				{
					p1=m_vecGridUNow[GetUstore(1,j-PML+1)];//
				}
				else
				{
					p1=m_vecUDX[2*PML+m_GridnumZ-1-j]+m_vecUDZ[2*PML+m_GridnumZ-1-j];
				}
				p2=m_vecULX[i*(2*PML+m_GridnumZ+1)+j+1]+m_vecULZ[i*(2*PML+m_GridnumZ+1)+j+1];
				m_vecALX[i*(2*PML+m_GridnumZ-1)+j]=UPMLabsorb(a1,p1,p2,alphax,m_GridcellX);
			} 
			else
			{
				a1=m_vecALX[i*(2*PML+m_GridnumZ-1)+j];// 自己
				p1=m_vecULX[(i+1)*(2*PML+m_GridnumZ+1)+j+1]+m_vecULZ[(i+1)*(2*PML+m_GridnumZ+1)+j+1];//自己右边的正常网格点
				p2=m_vecULX[i*(2*PML+m_GridnumZ+1)+j+1]+m_vecULZ[i*(2*PML+m_GridnumZ+1)+j+1];//自己左边的正常网格点
				m_vecALX[i*(2*PML+m_GridnumZ-1)+j]=UPMLabsorb(a1,p1,p2,alphax,m_GridcellX);
			}
		}
	}

	//ALZ
	for (i=0; i<PML; i++)
	{
		for (j=0; j<2*PML+m_GridnumZ; j++)
		{

			if (j<PML)
			{
				alphaz=alphazm*(1-sin(j*3.14159/2/PML));
			}
			else if (j<PML+m_GridnumZ-1)
			{
				alphaz=0;
			}
			else
			{
				alphaz=alphazm*(1-sin((2*PML+m_GridnumZ-1-j)*3.14159/2/PML));
			}


			a1=m_vecALZ[i*(2*PML+m_GridnumZ)+j];
			if (j==0)
			{
				p1=m_vecULX[(i+1)*(2*PML+m_GridnumZ+1)+1]+m_vecULZ[(i+1)*(2*PML+m_GridnumZ+1)+1];
				p2=p1;
			}
			else if (j==2*PML+m_GridnumZ-1)
			{
				p2=m_vecULX[(i+1)*(2*PML+m_GridnumZ+1)+j]+m_vecULZ[(i+1)*(2*PML+m_GridnumZ+1)+j];
				p1=p2;
			}
			else
			{
				p1=m_vecULX[(i+1)*(2*PML+m_GridnumZ+1)+j+1]+m_vecULZ[(i+1)*(2*PML+m_GridnumZ+1)+j+1];
				p2=m_vecULX[(i+1)*(2*PML+m_GridnumZ+1)+j]+m_vecULZ[(i+1)*(2*PML+m_GridnumZ+1)+j];
			}
			m_vecALZ[i*(2*PML+m_GridnumZ)+j]=UPMLabsorb(a1,p1,p2,alphaz,m_GridcellZ);
		}
	}

	//ULX,ULZ    正常网格点
    Compoint.i=0;
	for (i=1; i<PML+1; i++)
	{
		for (j=1; j<2*PML+m_GridnumZ; j++)
		{
			alphax=alphaxm*(1-sin(i*3.14159/2/PML));
			if (j<PML+1)
			{
				Compoint.j=0;
				alphaz=alphazm*(1-sin(j*3.14159/2/PML));
			}
			else if (j<PML+m_GridnumZ)
			{
				Compoint.j=j-PML;
				alphaz=0;
			}
			else
			{
				Compoint.j=m_GridnumZ;
				alphaz=alphazm*(1-sin((2*PML+m_GridnumZ-j)*3.14159/2/PML));
			}
			velocity=m_Vel[GetVelocitystore(Compoint)];

			a1=m_vecULX[i*(2*PML+m_GridnumZ+1)+j];//自己
			p1=m_vecALX[i*(2*PML+m_GridnumZ-1)+j-1]*velocity*velocity;//右边的交错网格点
			p2=m_vecALX[(i-1)*(2*PML+m_GridnumZ-1)+j-1]*velocity*velocity;//左边的交错网格点
			m_vecULX[i*(2*PML+m_GridnumZ+1)+j]=UPMLabsorb(a1,p1,p2,alphax,m_GridcellX);

			a1=m_vecULZ[i*(2*PML+m_GridnumZ+1)+j];
			p1=m_vecALZ[(i-1)*(2*PML+m_GridnumZ)+j]*velocity*velocity;
			p2=m_vecALZ[(i-1)*(2*PML+m_GridnumZ)+j-1]*velocity*velocity;
			m_vecULZ[i*(2*PML+m_GridnumZ+1)+j]=UPMLabsorb(a1,p1,p2,alphaz,m_GridcellZ);

		}
	}

	//用ulx和ulz来计算gridnext的左边界线上的网格点
	for (j=PML; j<PML+m_GridnumZ+1; j++)
	{
		m_vecGridUNext[GetUstore(0,j-PML)]=m_vecULX[PML*(2*PML+m_GridnumZ+1)+j]+m_vecULZ[PML*(2*PML+m_GridnumZ+1)+j];
		    
		Compoint.i=0;
		Compoint.j=j-PML;
		velocity=m_Vel[GetVelocitystore(Compoint)];

		if (0==m_ShotGridX&&(j-PML)==m_ShotGridZ)//如果此点是震源
		{
			if(s==0)
				for(k=1;k<=m_t;k++)
              {
		        src+= m_Source[k-1];
	          } 

			else if(s==1)
			{
				if(m_t==1)
					src =    (m_AvecArecord[GetURstore(0,j-PML,m_t+1)] - m_AvecArecord[GetURstore(0,j-PML,m_t)])
					/(m_tstep*m_tstep)*(2)/(velocity*velocity*velocity);

				else if(m_t>1&&m_t<m_tnum)
					src =    (m_AvecArecord[GetURstore(0,j-PML,m_t+1)]+m_AvecArecord[GetURstore(0,j-PML,m_t-1)]-2*m_AvecArecord[GetURstore(0,j-PML,m_t)])
					/(m_tstep*m_tstep)*(2)/(velocity*velocity*velocity);

				else if (m_t==m_tnum)
					src =    (m_AvecArecord[GetURstore(0,j-PML,m_t-1)] -m_AvecArecord[GetURstore(0,j-PML,m_t)])
					/(m_tstep*m_tstep)*(2)/(velocity*velocity*velocity);
			} 
			else if (s==2)
			
					src = SourceGT(m_t);
			else if(s==3&&m_t==1)
				src=1;
			else
				src = 0.0;
		}
		else
			src=0;
		
		m_vecGridUNext[GetUstore(0,j-PML)]  -=src*velocity*velocity*m_tstep*m_tstep;//震源处理
       
	}
	
	 m_vecGridRecord[GetRecordstore(0,m_t)]+=m_vecGridUNext[GetUstore(0,L)];
	

//////////////////////////////////右边界Rside////////////////////////////////////////
	alphax=alphaxm;
	alphaz=alphazm;
	//ARX
	for (i=0; i<PML+1; i++)
	{
		for (j=0; j<2*PML+m_GridnumZ-1; j++)
		{
			alphax=alphaxm*(1-sin(i*3.14159/2/PML));
			if (i==0)
			{
				a1=m_vecARX[j];
				p2=m_vecURX[2*PML+m_GridnumZ+1+j+1]+m_vecURZ[2*PML+m_GridnumZ+1+j+1];
				p1=p2;
				m_vecARX[j]=UPMLabsorb(a1,p1,p2,alphax,m_GridcellX);
			}
			else if (i==PML)
			{
				a1=m_vecARX[i*(2*PML+m_GridnumZ-1)+j];
				if (j<PML)
				{
					p2=m_vecUUX[(m_GridnumX-2)*(PML+1)+j+1]+m_vecUUZ[(m_GridnumX-2)*(PML+1)+j+1];
				}
				else if (j<PML+m_GridnumZ-1)
				{
					p2=m_vecGridUNow[GetUstore(m_GridnumX-1,j-PML+1)];
				}
				else
				{
					p2=m_vecUDX[(m_GridnumX-2)*(PML+1)+2*PML+m_GridnumZ-1-j]+m_vecUDZ[(m_GridnumX-2)*(PML+1)+2*PML+m_GridnumZ-1-j];
				}
				p1=m_vecURX[i*(2*PML+m_GridnumZ+1)+j+1]+m_vecURZ[i*(2*PML+m_GridnumZ+1)+j+1];
				m_vecARX[i*(2*PML+m_GridnumZ-1)+j]=UPMLabsorb(a1,p1,p2,alphax,m_GridcellX);
			} 
			else
			{
				a1=m_vecARX[i*(2*PML+m_GridnumZ-1)+j];
				p2=m_vecURX[(i+1)*(2*PML+m_GridnumZ+1)+j+1]+m_vecURZ[(i+1)*(2*PML+m_GridnumZ+1)+j+1];
				p1=m_vecURX[i*(2*PML+m_GridnumZ+1)+j+1]+m_vecURZ[i*(2*PML+m_GridnumZ+1)+j+1];
				m_vecARX[i*(2*PML+m_GridnumZ-1)+j]=UPMLabsorb(a1,p1,p2,alphax,m_GridcellX);
			}
		}
	}

	//ARZ
	for (i=0; i<PML; i++)
	{
		for (j=0; j<2*PML+m_GridnumZ; j++)
		{
			if (j<PML)
			{
				alphaz=alphazm*(1-sin(j*3.14159/2/PML));
			}
			else if (j<PML+m_GridnumZ-1)
			{
				alphaz=0;
			}
			else
			{
				alphaz=alphazm*(1-sin((2*PML+m_GridnumZ-1-j)*3.14159/2/PML));
			}
			a1=m_vecARZ[i*(2*PML+m_GridnumZ)+j];
			if (j==0)
			{
				p1=m_vecURX[(i+1)*(2*PML+m_GridnumZ+1)+1]+m_vecURZ[(i+1)*(2*PML+m_GridnumZ+1)+1];
				p2=p1;
			}
			else if (j==2*PML+m_GridnumZ-1)
			{
				p2=m_vecURX[(i+1)*(2*PML+m_GridnumZ+1)+j]+m_vecURZ[(i+1)*(2*PML+m_GridnumZ+1)+j];
				p1=p2;
			}
			else
			{
				p1=m_vecURX[(i+1)*(2*PML+m_GridnumZ+1)+j+1]+m_vecURZ[(i+1)*(2*PML+m_GridnumZ+1)+j+1];
				p2=m_vecURX[(i+1)*(2*PML+m_GridnumZ+1)+j]+m_vecURZ[(i+1)*(2*PML+m_GridnumZ+1)+j];
			}
			m_vecARZ[i*(2*PML+m_GridnumZ)+j]=UPMLabsorb(a1,p1,p2,alphaz,m_GridcellZ);
		}
	}

	//URX,URZ
	Compoint.i=m_GridnumX;
	for (i=1; i<PML+1; i++)
	{
		for (j=1; j<2*PML+m_GridnumZ; j++)
		{
			alphax=alphaxm*(1-sin(i*3.14159/2/PML));
			if (j<PML+1)
			{
				Compoint.j=0;
				alphaz=alphazm*(1-sin(j*3.14159/2/PML));
			}
			else if (j<PML+m_GridnumZ)
			{
				Compoint.j=j-PML;
				alphaz=0;
			}
			else
			{
				Compoint.j=m_GridnumZ;
				alphaz=alphazm*(1-sin((2*PML+m_GridnumZ-j)*3.14159/2/PML));
			}
			velocity=m_Vel[GetVelocitystore(Compoint)];

			a1=m_vecURX[i*(2*PML+m_GridnumZ+1)+j];
			p2=m_vecARX[i*(2*PML+m_GridnumZ-1)+j-1]*velocity*velocity;
			p1=m_vecARX[(i-1)*(2*PML+m_GridnumZ-1)+j-1]*velocity*velocity;
			m_vecURX[i*(2*PML+m_GridnumZ+1)+j]=UPMLabsorb(a1,p1,p2,alphax,m_GridcellX);

			a1=m_vecURZ[i*(2*PML+m_GridnumZ+1)+j];
			p1=m_vecARZ[(i-1)*(2*PML+m_GridnumZ)+j]*velocity*velocity;
			p2=m_vecARZ[(i-1)*(2*PML+m_GridnumZ)+j-1]*velocity*velocity;
			m_vecURZ[i*(2*PML+m_GridnumZ+1)+j]=UPMLabsorb(a1,p1,p2,alphaz,m_GridcellZ);

		}
	}

	for (j=PML; j<PML+m_GridnumZ+1; j++)
	{
		m_vecGridUNext[GetUstore(m_GridnumX,j-PML)]=m_vecURX[PML*(2*PML+m_GridnumZ+1)+j]+m_vecURZ[PML*(2*PML+m_GridnumZ+1)+j];

		Compoint.i=m_GridnumX;
		Compoint.j=j-PML;
		velocity=m_Vel[GetVelocitystore(Compoint)];

		if (m_GridnumX==m_ShotGridX&&(j-PML)==m_ShotGridZ)
		{
			if(s==0)
				src =  m_Source[m_t-1];
			else if(s==1)
			{
				if(m_t==1)
					src =    (m_AvecArecord[GetURstore(m_GridnumX,j-PML,m_t+1)] - m_AvecArecord[GetURstore(m_GridnumX,j-PML,m_t)])
					/(m_tstep*m_tstep)*(2)/(velocity*velocity*velocity);

				else if(m_t>1&&m_t<m_tnum)
					src =    (m_AvecArecord[GetURstore(m_GridnumX,j-PML,m_t+1)]+m_AvecArecord[GetURstore(m_GridnumX,j-PML,m_t-1)]-2*m_AvecArecord[GetURstore(m_GridnumX,j-PML,m_t)])
					/(m_tstep*m_tstep)*(2)/(velocity*velocity*velocity);

				else if (m_t==m_tnum)
					src =    (m_AvecArecord[GetURstore(m_GridnumX,j-PML,m_t-1)] -m_AvecArecord[GetURstore(m_GridnumX,j-PML,m_t)])
					/(m_tstep*m_tstep)*(2)/(velocity*velocity*velocity);
			} 
			else if (s==2)
			
					src = SourceGT(m_t);
			else if(s==3&&m_t==1)
				src=1;
			else
				src = 0.0;
		}
		else
			src=0;
		m_vecGridUNext[GetUstore(m_GridnumX,j-PML)]  -=src*velocity*velocity*m_tstep*m_tstep;
        
	}


 m_vecGridRecord[GetRecordstore(m_GridnumX,m_t)]+=m_vecGridUNext[GetUstore(m_GridnumX,L)];
	
/////////////////////////////上边界Upside/////////////////////////////////////////////
	alphax=0;
	alphaz=alphazm;

	//AUX
	for (i=0; i<m_GridnumX-2; i++)
	{
		for (j=0; j<PML; j++)
		{
			alphaz=alphazm*(1-sin((j+1)*3.14159/2/PML));
			a1=m_vecAUX[i*PML+j];
			p1=m_vecUUX[(i+1)*(PML+1)+j+1]+m_vecUUZ[(i+1)*(PML+1)+j+1];
			p2=m_vecUUX[i*(PML+1)+j+1]+m_vecUUZ[i*(PML+1)+j+1];
			m_vecAUX[i*PML+j]=UPMLabsorb(a1,p1,p2,alphax,m_GridcellX);
		}
	}

	//AUZ
	for (i=0; i<m_GridnumX-1; i++)
	{
		for (j=0; j<PML+1; j++)
		{
			alphaz=alphazm*(1-sin(j*3.14159/2/PML));
			a1=m_vecAUZ[i*(PML+1)+j];
			if (j==0)
			{
				p1=m_vecUUX[i*(PML+1)+j+1]+m_vecUUZ[i*(PML+1)+j+1];
				p2=p1;
			}
			else if (j==PML)
			{
				p1=m_vecGridUNow[GetUstore(i+1,1)];
                p2=m_vecUUX[i*(PML+1)+j]+m_vecUUZ[i*(PML+1)+j];
			}
			else
			{
				p1=m_vecUUX[i*(PML+1)+j+1]+m_vecUUZ[i*(PML+1)+j+1];
				p2=m_vecUUX[i*(PML+1)+j]+m_vecUUZ[i*(PML+1)+j];
			}
            m_vecAUZ[i*(PML+1)+j]=UPMLabsorb(a1,p1,p2,alphaz,m_GridcellZ);
		}
	}

	//UUX,UUZ
	for (i=0; i<m_GridnumX-1; i++)
	{
		for (j=1; j<PML+1; j++)
		{
			Compoint.j=0;
			Compoint.i=1+i;
			velocity=m_Vel[GetVelocitystore(Compoint)];
			alphaz=alphazm*(1-sin(j*3.14159/2/PML));
			a1=m_vecUUX[i*(PML+1)+j];
			if (i==0)
			{
				p1=m_vecAUX[j-1]*velocity*velocity;
				p2=m_vecALX[PML*(2*PML+m_GridnumZ-1)+j-1]*velocity*velocity;
			}
			else if (i==m_GridnumX-2)
			{
				p1=m_vecARX[PML*(2*PML+m_GridnumZ-1)+j-1]*velocity*velocity;
				p2=m_vecAUX[(m_GridnumX-3)*PML+j-1]*velocity*velocity;
			}
			else
			{
				p1=m_vecAUX[i*PML+j-1]*velocity*velocity;
				p2=m_vecAUX[(i-1)*PML+j-1]*velocity*velocity;
			}
			m_vecUUX[i*(PML+1)+j]=UPMLabsorb(a1,p1,p2,alphax,m_GridcellX);

			a1=m_vecUUZ[i*(PML+1)+j];
			p1=m_vecAUZ[i*(PML+1)+j]*velocity*velocity;
			p2=m_vecAUZ[i*(PML+1)+j-1]*velocity*velocity;
			m_vecUUZ[i*(PML+1)+j]=UPMLabsorb(a1,p1,p2,alphaz,m_GridcellZ);
		}
	}

	for (i=0; i<m_GridnumX-1; i++)
	{
		m_vecGridUNext[GetUstore(i+1,0)]=m_vecUUX[i*(PML+1)+PML]+m_vecUUZ[i*(PML+1)+PML];

		Compoint.i=i+1;
		Compoint.j=0;
		velocity=m_Vel[GetVelocitystore(Compoint)];

		if ((i+1)==m_ShotGridX&&0==m_ShotGridZ)
		{
			if(s==0)
				src =  m_Source[m_t-1];
			else if(s==1)
			{
				if(m_t==1)
					src =    (m_AvecArecord[GetURstore(i+1,0,m_t+1)] - m_AvecArecord[GetURstore(i+1,0,m_t)])
					/(m_tstep*m_tstep)*(2)/(velocity*velocity*velocity);

				else if(m_t>1&&m_t<m_tnum)
					src =    (m_AvecArecord[GetURstore(i+1,0,m_t+1)]+m_AvecArecord[GetURstore(i+1,0,m_t-1)]-2*m_AvecArecord[GetURstore(i+1,0,m_t)])
					/(m_tstep*m_tstep)*(2)/(velocity*velocity*velocity);

				else if (m_t==m_tnum)
					src =    (m_AvecArecord[GetURstore(i+1,0,m_t-1)] -m_AvecArecord[GetURstore(i+1,0,m_t)])
					/(m_tstep*m_tstep)*(2)/(velocity*velocity*velocity);
			} 
			else if (s==2)
			
					src = SourceG(m_t);
			else if(s==3&&m_t==1)
				src=1;
			else
				src = 0.0;
		}
		else
			src=0;
		m_vecGridUNext[GetUstore(i+1,0)]  -= src*velocity*velocity*m_tstep*m_tstep;
		//m_vecGridRecord[GetRecordstore(i+1,0)]=m_vecGridUNext[GetUstore(i+1,0)];
		//m_vecGridRecord[GetRecordstore(i+1,m_t)]+=m_vecGridUNext[GetUstore(i+1,0)];
	}

	

////////////////////////////////下边界Downside//////////////////////////////////////////
	alphax=0;
	alphaz=alphazm;

	//ADX
	for (i=0; i<m_GridnumX-2; i++)
	{
		for (j=0; j<PML; j++)
		{
			alphaz=alphazm*(1-sin((j+1)*3.14159/2/PML));
			a1=m_vecADX[i*PML+j];
			p1=m_vecUDX[(i+1)*(PML+1)+j+1]+m_vecUDZ[(i+1)*(PML+1)+j+1];
			p2=m_vecUDX[i*(PML+1)+j+1]+m_vecUDZ[i*(PML+1)+j+1];
			m_vecADX[i*PML+j]=UPMLabsorb(a1,p1,p2,alphax,m_GridcellX);
		}
	}

	//ADZ
	for (i=0; i<m_GridnumX-1; i++)
	{
		for (j=0; j<PML+1; j++)
		{
			alphaz=alphazm*(1-sin(j*3.14159/2/PML));
			a1=m_vecADZ[i*(PML+1)+j];
			if (j==0)
			{
				p2=m_vecUDX[i*(PML+1)+j+1]+m_vecUDZ[i*(PML+1)+j+1];
				p1=p2;
			}
			else if (j==PML)
			{
				p2=m_vecGridUNow[GetUstore(i+1,m_GridnumZ-1)];
				p1=m_vecUDX[i*(PML+1)+j]+m_vecUDZ[i*(PML+1)+j];
			}
			else
			{
				p2=m_vecUDX[i*(PML+1)+j+1]+m_vecUDZ[i*(PML+1)+j+1];
				p1=m_vecUDX[i*(PML+1)+j]+m_vecUDZ[i*(PML+1)+j];
			}
			m_vecADZ[i*(PML+1)+j]=UPMLabsorb(a1,p1,p2,alphaz,m_GridcellZ);
		}
	}

	//UDX,UDZ
	for (i=0; i<m_GridnumX-1; i++)
	{
		for (j=1; j<PML+1; j++)
		{
			Compoint.j=m_GridnumZ;
			Compoint.i=1+i;
			velocity=m_Vel[GetVelocitystore(Compoint)];
			alphaz=alphazm*(1-sin(j*3.14159/2/PML));
			a1=m_vecUDX[i*(PML+1)+j];
			if (i==0)
			{
				p1=m_vecADX[j-1]*velocity*velocity;
				p2=m_vecALX[PML*(2*PML+m_GridnumZ-1)+2*PML+m_GridnumZ-j-1]*velocity*velocity;
			}
			else if (i==m_GridnumX-2)
			{
				p1=m_vecARX[PML*(2*PML+m_GridnumZ-1)+2*PML+m_GridnumZ-j-1]*velocity*velocity;
				p2=m_vecADX[(m_GridnumX-3)*PML+j-1]*velocity*velocity;
			}
			else
			{
				p1=m_vecADX[i*PML+j-1]*velocity*velocity;
				p2=m_vecADX[(i-1)*PML+j-1]*velocity*velocity;
			}
			m_vecUDX[i*(PML+1)+j]=UPMLabsorb(a1,p1,p2,alphax,m_GridcellX);

			a1=m_vecUDZ[i*(PML+1)+j];
			p2=m_vecADZ[i*(PML+1)+j]*velocity*velocity;
			p1=m_vecADZ[i*(PML+1)+j-1]*velocity*velocity;
			m_vecUDZ[i*(PML+1)+j]=UPMLabsorb(a1,p1,p2,alphaz,m_GridcellZ);
		}
	}

	for (i=0; i<m_GridnumX-1; i++)
	{
		m_vecGridUNext[GetUstore(i+1,m_GridnumZ)]=m_vecUDX[i*(PML+1)+PML]+m_vecUDZ[i*(PML+1)+PML];

		Compoint.i=i+1;
		Compoint.j=m_GridnumZ;
		velocity=m_Vel[GetVelocitystore(Compoint)];

		if ((i+1)==m_ShotGridX&&m_GridnumZ==m_ShotGridZ)
		{
			if(s==0)
				src =  m_Source[m_t-1];
			else if(s==1)
			{
				if(m_t==1)
					src =    (m_AvecArecord[GetURstore(i+1,m_GridnumZ,m_t+1)] - m_AvecArecord[GetURstore(i+1,m_GridnumZ,m_t)])
					/(m_tstep*m_tstep)*(2)/(velocity*velocity*velocity);

				else if(m_t>1&&m_t<m_tnum)
					src =    (m_AvecArecord[GetURstore(i+1,m_GridnumZ,m_t+1)]+m_AvecArecord[GetURstore(i+1,m_GridnumZ,m_t-1)]-2*m_AvecArecord[GetURstore(i+1,m_GridnumZ,m_t)])
					/(m_tstep*m_tstep)*(2)/(velocity*velocity*velocity);

				else if (m_t==m_tnum)
					src =    (m_AvecArecord[GetURstore(i+1,m_GridnumZ,m_t-1)] -m_AvecArecord[GetURstore(i+1,m_GridnumZ,m_t)])
					/(m_tstep*m_tstep)*(2)/(velocity*velocity*velocity);
			} 
			else if (s==2)
			
					src = SourceG(m_t);
			else if(s==3&&m_t==1)
				src=1;
			else
				src = 0.0;
		}
		else
			src=0;
		m_vecGridUNext[GetUstore(i+1,m_GridnumZ)] -= src*velocity*velocity*m_tstep*m_tstep;
		
	}
}
//22位
//交错网格
void C2DModel::UpdateNextUH(vector<double> m_Vel,int s)
{
	//m_vecGridUNext[GetUstore(25,10)]=1;
	
	int i,j,k;
	double velocity,a1,a2,x1,x2,x3,x4,z1,z2,z3,z4,p1,p2;
	double src,alphax,alphaxm,alphaz,alphazm;
	int L=1;

	src=0.0;
	

	

   //m_vecGridUA,m_vecGridUB(中间区域的交错网格点，x方向和z方向)
		for(i=0;i<=m_GridnumX-1;i++)//m_vecGridUA  和ALX，ARX各有一列重叠
		{
			for(j=0;j<=m_GridnumZ-2;j++)
			{
				/*if (i==0||i==(m_GridnumX-1))
				m_vecGridUA[i*(m_GridnumZ-1)+j] = m_vecGridUA[i*(m_GridnumZ-1)+j] - m_tstep/m_GridcellX*(m_vecGridUNext[GetUstore(i+1,j+1)]-m_vecGridUNext[GetUstore(i,j+1)]);
				else
				m_vecGridUA[i*(m_GridnumZ-1)+j] = m_vecGridUA[i*(m_GridnumZ-1)+j] - m_tstep/m_GridcellX*(1.125*(m_vecGridUNext[GetUstore(i+1,j+1)]-m_vecGridUNext[GetUstore(i,j+1)])-1/24.0*(m_vecGridUNext[GetUstore(i+2,j+1)]-m_vecGridUNext[GetUstore(i-1,j+1)]));*/
				m_vecGridUA[i*(m_GridnumZ-1)+j] = m_vecGridUA[i*(m_GridnumZ-1)+j] + m_tstep/m_GridcellX*(m_vecGridUNext[GetUstore(i+1,j+1)]-m_vecGridUNext[GetUstore(i,j+1)]);

			}
		}

		for(i=0;i<=m_GridnumX-2;i++)//m_vecGridUB  和AUZ，ADZ各有一行重叠
		{
			for(j=0;j<=m_GridnumZ-1;j++)
			{
				/*if (j==0||j==(m_GridnumZ-1))
				    m_vecGridUB[i*m_GridnumZ+j] = m_vecGridUB[i*m_GridnumZ+j] - m_tstep/m_GridcellZ*(m_vecGridUNext[GetUstore(i+1,j+1)]-m_vecGridUNext[GetUstore(i+1,j)]);
				else
					m_vecGridUB[i*m_GridnumZ+j] = m_vecGridUB[i*m_GridnumZ+j] - m_tstep/m_GridcellZ*(1.125*(m_vecGridUNext[GetUstore(i+1,j+1)]-m_vecGridUNext[GetUstore(i+1,j)])-1/24.0*(m_vecGridUNext[GetUstore(i+1,j+2)]-m_vecGridUNext[GetUstore(i+1,j-1)]));*/
				m_vecGridUB[i*m_GridnumZ+j] = m_vecGridUB[i*m_GridnumZ+j] + m_tstep/m_GridcellZ*(m_vecGridUNext[GetUstore(i+1,j+1)]-m_vecGridUNext[GetUstore(i+1,j)]);
			}
		}


		
   //中间区域 m_vecGridUNext  上下各有一行，左右各有一列没有计算
		for(i=1;i<=m_GridnumX-1;i++)
		{
			for(j=1;j<=m_GridnumZ-1;j++)
			{

				Compoint.i=i;
				Compoint.j=j;
				velocity=m_Vel[GetVelocitystore(Compoint)];

					/*if (i==1||j==1||i==(m_GridnumX-1)||j==(m_GridnumZ-1))
				m_vecGridUNext[GetUstore(i,j)]=m_vecGridUNext[GetUstore(i,j)]+m_tstep*velocity*velocity*((m_vecGridUA[i*(m_GridnumZ-1)+j-1]
				-m_vecGridUA[(i-1)*(m_GridnumZ-1)+j-1])/m_GridcellX+(m_vecGridUB[(i-1)*m_GridnumZ+j]-m_vecGridUB[(i-1)*m_GridnumZ+j-1])/m_GridcellZ)
				-(SourceG(m_t)*m_tstep+Sourcet)*velocity*velocity*m_tstep;
				else
				{

				m_vecGridUNext[GetUstore(i,j)]=m_vecGridUNext[GetUstore(i,j)]+m_tstep*velocity*velocity*(1.125*(m_vecGridUA[i*(m_GridnumZ-1)+j-1]
				-m_vecGridUA[(i-1)*(m_GridnumZ-1)+j-1])/m_GridcellX
				-1/24.0*(m_vecGridUA[(i+1)*(m_GridnumZ-1)+j-1]-m_vecGridUA[(i-2)*(m_GridnumZ-1)+j-1])/m_GridcellX
				+1.125*(m_vecGridUB[(i-1)*m_GridnumZ+j]-m_vecGridUB[(i-1)*m_GridnumZ+j-1])/m_GridcellZ
				-1/24.0*(m_vecGridUB[(i-1)*m_GridnumZ+j+1]-m_vecGridUB[(i-1)*m_GridnumZ+j-2])/m_GridcellZ
				)
				-(SourceG(m_t)*m_tstep+Sourcet)*velocity*velocity*m_tstep;}*/

				m_vecGridUNext[GetUstore(i,j)]=m_vecGridUNext[GetUstore(i,j)]+m_tstep*velocity*velocity*((m_vecGridUA[i*(m_GridnumZ-1)+j-1]
				-m_vecGridUA[(i-1)*(m_GridnumZ-1)+j-1])/m_GridcellX+(m_vecGridUB[(i-1)*m_GridnumZ+j]-m_vecGridUB[(i-1)*m_GridnumZ+j-1])/m_GridcellZ);

				if (i==m_ShotGridX&&j==m_ShotGridZ)
				{
				m_vecGridUNext[GetUstore(i,j)]-=(SourceG(m_t)*m_tstep+Sourcet)*velocity*velocity*m_tstep;
				}
				
			}

		m_vecGridRecord[GetRecordstore(i,m_t)]+=m_vecGridUNext[GetUstore(i,L)];
	}


	/******************************PML边界********************************************/
    alphaxm=alphazm=800;

	/////////////////////左边界Lside/////////////////////////////////////////
	alphax=alphaxm;
	alphaz=alphazm;
	//ALX     计算方法同混合网格    重合部分使用grideUA
	for (i=0; i<PML+1; i++)
	{
		for (j=0; j<2*PML+m_GridnumZ-1; j++)
		{
			alphax=alphaxm*(1-sin(i*3.14159/2/PML));
			if (i==0)
			{
				a1=m_vecALX[j];
				p1=m_vecULX[2*PML+m_GridnumZ+1+j+1]+m_vecULZ[2*PML+m_GridnumZ+1+j+1];
				p2=p1;
				m_vecALX[j]=UPMLabsorb(a1,p1,p2,alphax,m_GridcellX);
			}
			else if (i==PML)
			{
				a1=m_vecALX[i*(2*PML+m_GridnumZ-1)+j];
				if (j<PML)
				{
					p1=m_vecUUX[j+1]+m_vecUUZ[j+1];
				}
				else if (j<PML+m_GridnumZ-1)
				{
					p1=m_vecGridUNext[GetUstore(1,j-PML+1)];
					//p1=m_vecGridUA[j-PML-1];
				}
				else
				{
					p1=m_vecUDX[2*PML+m_GridnumZ-1-j]+m_vecUDZ[2*PML+m_GridnumZ-1-j];
				}
				p2=m_vecULX[i*(2*PML+m_GridnumZ+1)+j+1]+m_vecULZ[i*(2*PML+m_GridnumZ+1)+j+1];
				m_vecALX[i*(2*PML+m_GridnumZ-1)+j]=UPMLabsorb(a1,p1,p2,alphax,m_GridcellX);
			} 
			else
			{
				a1=m_vecALX[i*(2*PML+m_GridnumZ-1)+j];
				p1=m_vecULX[(i+1)*(2*PML+m_GridnumZ+1)+j+1]+m_vecULZ[(i+1)*(2*PML+m_GridnumZ+1)+j+1];
				p2=m_vecULX[i*(2*PML+m_GridnumZ+1)+j+1]+m_vecULZ[i*(2*PML+m_GridnumZ+1)+j+1];
				m_vecALX[i*(2*PML+m_GridnumZ-1)+j]=UPMLabsorb(a1,p1,p2,alphax,m_GridcellX);
			}
		}
	}

	for (j=PML+1; j<PML+m_GridnumZ; j++)
	{
	     m_vecALX[(PML)*(2*PML+m_GridnumZ-1)+j]=m_vecGridUA[j-PML-1];
	}

	//ALZ     计算方法同混合网格    重合部分使用grideUB
	for (i=0; i<PML; i++)
	{
		for (j=0; j<2*PML+m_GridnumZ; j++)
		{
			if (j<PML)
			{
				alphaz=alphazm*(1-sin(j*3.14159/2/PML));
			}
			else if (j<PML+m_GridnumZ-1)
			{
				alphaz=0;
			}
			else
			{
				alphaz=alphazm*(1-sin((2*PML+m_GridnumZ-1-j)*3.14159/2/PML));
			}
			a1=m_vecALZ[i*(2*PML+m_GridnumZ)+j];
			if (j==0)
			{
				p1=m_vecULX[(i+1)*(2*PML+m_GridnumZ+1)+1]+m_vecULZ[(i+1)*(2*PML+m_GridnumZ+1)+1];
				p2=p1;
			}
			else if (j==2*PML+m_GridnumZ-1)
			{
				p2=m_vecULX[(i+1)*(2*PML+m_GridnumZ+1)+j]+m_vecULZ[(i+1)*(2*PML+m_GridnumZ+1)+j];
				p1=p2;
			}
			else
			{
				p1=m_vecULX[(i+1)*(2*PML+m_GridnumZ+1)+j+1]+m_vecULZ[(i+1)*(2*PML+m_GridnumZ+1)+j+1];
				p2=m_vecULX[(i+1)*(2*PML+m_GridnumZ+1)+j]+m_vecULZ[(i+1)*(2*PML+m_GridnumZ+1)+j];
			}
			m_vecALZ[i*(2*PML+m_GridnumZ)+j]=UPMLabsorb(a1,p1,p2,alphaz,m_GridcellZ);
		}
	}

	//ULX,ULZ
	Compoint.i=0;
	for (i=1; i<PML+1; i++)
	{
		for (j=1; j<2*PML+m_GridnumZ; j++)
		{
			alphax=alphaxm*(1-sin(i*3.14159/2/PML));
			if (j<PML+1)
			{
				Compoint.j=0;
				alphaz=alphazm*(1-sin(j*3.14159/2/PML));
			}
			else if (j<PML+m_GridnumZ)
			{
				Compoint.j=j-PML;
				alphaz=0;
			}
			else
			{
				Compoint.j=m_GridnumZ;
				alphaz=alphazm*(1-sin((2*PML+m_GridnumZ-j)*3.14159/2/PML));
			}
			velocity=m_Vel[GetVelocitystore(Compoint)];

			a1=m_vecULX[i*(2*PML+m_GridnumZ+1)+j];
			p1=m_vecALX[i*(2*PML+m_GridnumZ-1)+j-1]*velocity*velocity;
			p2=m_vecALX[(i-1)*(2*PML+m_GridnumZ-1)+j-1]*velocity*velocity;
			m_vecULX[i*(2*PML+m_GridnumZ+1)+j]=UPMLabsorb(a1,p1,p2,alphax,m_GridcellX);

			a1=m_vecULZ[i*(2*PML+m_GridnumZ+1)+j];
			p1=m_vecALZ[(i-1)*(2*PML+m_GridnumZ)+j]*velocity*velocity;
			p2=m_vecALZ[(i-1)*(2*PML+m_GridnumZ)+j-1]*velocity*velocity;
			m_vecULZ[i*(2*PML+m_GridnumZ+1)+j]=UPMLabsorb(a1,p1,p2,alphaz,m_GridcellZ);

		}
	}

	//m_vecGridUNext左边界赋值
	for (j=PML; j<PML+m_GridnumZ+1; j++)
	{
		m_vecGridUNext[GetUstore(0,j-PML)]=m_vecULX[PML*(2*PML+m_GridnumZ+1)+j]+m_vecULZ[PML*(2*PML+m_GridnumZ+1)+j];

		Compoint.i=0;
		Compoint.j=j-PML;
		velocity=m_Vel[GetVelocitystore(Compoint)];

		if (0==m_ShotGridX&&(j-PML)==m_ShotGridZ)//震源处理
		{
			/*if(s==0)
				for(k=1;k<=m_t;k++)
				{
					src+= m_Source[k-1];
				} 

			else if(s==1)
			{
				if(m_t==1)
					src =    (m_AvecArecord[GetURstore(0,j-PML,m_t+1)] - m_AvecArecord[GetURstore(0,j-PML,m_t)])
					/(m_tstep*m_tstep)*(2)/(velocity*velocity*velocity);

				else if(m_t>1&&m_t<m_tnum)
					src =    (m_AvecArecord[GetURstore(0,j-PML,m_t+1)]+m_AvecArecord[GetURstore(0,j-PML,m_t-1)]-2*m_AvecArecord[GetURstore(0,j-PML,m_t)])
					/(m_tstep*m_tstep)*(2)/(velocity*velocity*velocity);

				else if (m_t==m_tnum)
					src =    (m_AvecArecord[GetURstore(0,j-PML,m_t-1)] -m_AvecArecord[GetURstore(0,j-PML,m_t)])
					/(m_tstep*m_tstep)*(2)/(velocity*velocity*velocity);
			} 
			else if (s==2)

				src = SourceGT(m_t);
			else if(s==3&&m_t==1)
				src=1;
			else
				src = 0.0;
		
		else
			src=0;*/

		m_vecGridUNext[GetUstore(0,j-PML)]  -= (SourceG(m_t)*m_tstep+Sourcet)*velocity*velocity*m_tstep;
		}
	}

	
	m_vecGridRecord[GetRecordstore(0,m_t)]+=m_vecGridUNext[GetUstore(0,L)];//


	//////////////////////////////////右边界Rside////////////////////////////////////////
	alphax=alphaxm;
	alphaz=alphazm;
	//ARX
	for (i=0; i<PML+1; i++)
	{
		for (j=0; j<2*PML+m_GridnumZ-1; j++)
		{
			alphax=alphaxm*(1-sin(i*3.14159/2/PML));
			if (i==0)
			{
				a1=m_vecARX[j];
				p2=m_vecURX[2*PML+m_GridnumZ+1+j+1]+m_vecURZ[2*PML+m_GridnumZ+1+j+1];
				p1=p2;
				m_vecARX[j]=UPMLabsorb(a1,p1,p2,alphax,m_GridcellX);
			}
			else if (i==PML)
			{
				a1=m_vecARX[i*(2*PML+m_GridnumZ-1)+j];
				if (j<PML)
				{
					p2=m_vecUUX[(m_GridnumX-2)*(PML+1)+j+1]+m_vecUUZ[(m_GridnumX-2)*(PML+1)+j+1];
				}
				else if (j<PML+m_GridnumZ-1)
				{
					p2=m_vecGridUNext[GetUstore(m_GridnumX-1,j-PML+1)];
					//p2=m_vecGridUA[(m_GridnumX-1)*(m_GridnumZ-1)+j-PML-1];
				}
				else
				{
					p2=m_vecUDX[(m_GridnumX-2)*(PML+1)+2*PML+m_GridnumZ-1-j]+m_vecUDZ[(m_GridnumX-2)*(PML+1)+2*PML+m_GridnumZ-1-j];
				}
				p1=m_vecURX[i*(2*PML+m_GridnumZ+1)+j+1]+m_vecURZ[i*(2*PML+m_GridnumZ+1)+j+1];
				m_vecARX[i*(2*PML+m_GridnumZ-1)+j]=UPMLabsorb(a1,p1,p2,alphax,m_GridcellX);
			} 
			else
			{
				a1=m_vecARX[i*(2*PML+m_GridnumZ-1)+j];
				p2=m_vecURX[(i+1)*(2*PML+m_GridnumZ+1)+j+1]+m_vecURZ[(i+1)*(2*PML+m_GridnumZ+1)+j+1];
				p1=m_vecURX[i*(2*PML+m_GridnumZ+1)+j+1]+m_vecURZ[i*(2*PML+m_GridnumZ+1)+j+1];
				m_vecARX[i*(2*PML+m_GridnumZ-1)+j]=UPMLabsorb(a1,p1,p2,alphax,m_GridcellX);
			}
		}
	}

	for (j=PML+1; j<PML+m_GridnumZ; j++)
	{
		m_vecARX[(PML)*(2*PML+m_GridnumZ-1)+j]=m_vecGridUA[(m_GridnumX-1)*(m_GridnumZ-1)+j-PML-1];
	}

	//ARZ
	for (i=0; i<PML; i++)
	{
		for (j=0; j<2*PML+m_GridnumZ; j++)
		{
			if (j<PML)
			{
				alphaz=alphazm*(1-sin(j*3.14159/2/PML));
			}
			else if (j<PML+m_GridnumZ-1)
			{
				alphaz=0;
			}
			else
			{
				alphaz=alphazm*(1-sin((2*PML+m_GridnumZ-1-j)*3.14159/2/PML));
			}
			a1=m_vecARZ[i*(2*PML+m_GridnumZ)+j];
			if (j==0)
			{
				p1=m_vecURX[(i+1)*(2*PML+m_GridnumZ+1)+1]+m_vecURZ[(i+1)*(2*PML+m_GridnumZ+1)+1];
				p2=p1;
			}
			else if (j==2*PML+m_GridnumZ-1)
			{
				p2=m_vecURX[(i+1)*(2*PML+m_GridnumZ+1)+j]+m_vecURZ[(i+1)*(2*PML+m_GridnumZ+1)+j];
				p1=p2;
			}
			else
			{
				p1=m_vecURX[(i+1)*(2*PML+m_GridnumZ+1)+j+1]+m_vecURZ[(i+1)*(2*PML+m_GridnumZ+1)+j+1];
				p2=m_vecURX[(i+1)*(2*PML+m_GridnumZ+1)+j]+m_vecURZ[(i+1)*(2*PML+m_GridnumZ+1)+j];
			}
			m_vecARZ[i*(2*PML+m_GridnumZ)+j]=UPMLabsorb(a1,p1,p2,alphaz,m_GridcellZ);
		}
	}

	//URX,URZ
	Compoint.i=m_GridnumX;
	for (i=1; i<PML+1; i++)
	{
		for (j=1; j<2*PML+m_GridnumZ; j++)
		{
			alphax=alphaxm*(1-sin(i*3.14159/2/PML));
			if (j<PML+1)
			{
				Compoint.j=0;
				alphaz=alphazm*(1-sin(j*3.14159/2/PML));
			}
			else if (j<PML+m_GridnumZ)
			{
				Compoint.j=j-PML;
				alphaz=0;
			}
			else
			{
				Compoint.j=m_GridnumZ;
				alphaz=alphazm*(1-sin((2*PML+m_GridnumZ-j)*3.14159/2/PML));
			}
			velocity=m_Vel[GetVelocitystore(Compoint)];

			a1=m_vecURX[i*(2*PML+m_GridnumZ+1)+j];
			p2=m_vecARX[i*(2*PML+m_GridnumZ-1)+j-1]*velocity*velocity;
			p1=m_vecARX[(i-1)*(2*PML+m_GridnumZ-1)+j-1]*velocity*velocity;
			m_vecURX[i*(2*PML+m_GridnumZ+1)+j]=UPMLabsorb(a1,p1,p2,alphax,m_GridcellX);

			a1=m_vecURZ[i*(2*PML+m_GridnumZ+1)+j];
			p1=m_vecARZ[(i-1)*(2*PML+m_GridnumZ)+j]*velocity*velocity;
			p2=m_vecARZ[(i-1)*(2*PML+m_GridnumZ)+j-1]*velocity*velocity;
			m_vecURZ[i*(2*PML+m_GridnumZ+1)+j]=UPMLabsorb(a1,p1,p2,alphaz,m_GridcellZ);

		}
	}

	
	//m_vecGridUNext右边界赋值
	for (j=PML; j<PML+m_GridnumZ+1; j++)
	{
		m_vecGridUNext[GetUstore(m_GridnumX,j-PML)]=m_vecURX[PML*(2*PML+m_GridnumZ+1)+j]+m_vecURZ[PML*(2*PML+m_GridnumZ+1)+j];

		Compoint.i=m_GridnumX;
		Compoint.j=j-PML;
		velocity=m_Vel[GetVelocitystore(Compoint)];

		if (m_GridnumX==m_ShotGridX&&(j-PML)==m_ShotGridZ)
		{
	/*		if(s==0)
				src =  m_Source[m_t-1];
			else if(s==1)
			{
				if(m_t==1)
					src =    (m_AvecArecord[GetURstore(m_GridnumX,j-PML,m_t+1)] - m_AvecArecord[GetURstore(m_GridnumX,j-PML,m_t)])
					/(m_tstep*m_tstep)*(2)/(velocity*velocity*velocity);

				else if(m_t>1&&m_t<m_tnum)
					src =    (m_AvecArecord[GetURstore(m_GridnumX,j-PML,m_t+1)]+m_AvecArecord[GetURstore(m_GridnumX,j-PML,m_t-1)]-2*m_AvecArecord[GetURstore(m_GridnumX,j-PML,m_t)])
					/(m_tstep*m_tstep)*(2)/(velocity*velocity*velocity);

				else if (m_t==m_tnum)
					src =    (m_AvecArecord[GetURstore(m_GridnumX,j-PML,m_t-1)] -m_AvecArecord[GetURstore(m_GridnumX,j-PML,m_t)])
					/(m_tstep*m_tstep)*(2)/(velocity*velocity*velocity);
			} 
			else if (s==2)

				src = SourceGT(m_t);
			else if(s==3&&m_t==1)
				src=1;
			else
				src = 0.0;
		
		else
			src=0;*/
		m_vecGridUNext[GetUstore(m_GridnumX,j-PML)]  -= (SourceG(m_t)*m_tstep+Sourcet)*velocity*velocity*m_tstep;
		}
	}

	
	m_vecGridRecord[GetRecordstore(m_GridnumX,m_t)]+=m_vecGridUNext[GetUstore(m_GridnumX,L)];

	/////////////////////////////上边界Upside/////////////////////////////////////////////
	alphax=0;
	alphaz=alphazm;

	//AUX
	for (i=0; i<m_GridnumX-2; i++)
	{
		for (j=0; j<PML; j++)
		{
			alphaz=alphazm*(1-sin((j+1)*3.14159/2/PML));
			a1=m_vecAUX[i*PML+j];
			p1=m_vecUUX[(i+1)*(PML+1)+j+1]+m_vecUUZ[(i+1)*(PML+1)+j+1];
			p2=m_vecUUX[i*(PML+1)+j+1]+m_vecUUZ[i*(PML+1)+j+1];
			m_vecAUX[i*PML+j]=UPMLabsorb(a1,p1,p2,alphax,m_GridcellX);
		}
	}

	//AUZ
	for (i=0; i<m_GridnumX-1; i++)
	{
		for (j=0; j<PML+1; j++)
		{
			alphaz=alphazm*(1-sin(j*3.14159/2/PML));
			a1=m_vecAUZ[i*(PML+1)+j];
			if (j==0)
			{
				p1=m_vecUUX[i*(PML+1)+j+1]+m_vecUUZ[i*(PML+1)+j+1];
				p2=p1;
			}
			else if (j==PML)
			{
				p1=m_vecGridUNow[GetUstore(i+1,1)];
				p2=m_vecUUX[i*(PML+1)+j]+m_vecUUZ[i*(PML+1)+j];
			}
			else
			{
				p1=m_vecUUX[i*(PML+1)+j+1]+m_vecUUZ[i*(PML+1)+j+1];
				p2=m_vecUUX[i*(PML+1)+j]+m_vecUUZ[i*(PML+1)+j];
			}
			m_vecAUZ[i*(PML+1)+j]=UPMLabsorb(a1,p1,p2,alphaz,m_GridcellZ);
		}
	}

	for (i=0; i<m_GridnumX-1; i++)
	{
		m_vecAUZ[i*(PML+1)+PML]=m_vecGridUB[i*m_GridnumZ];
	}


	//UUX,UUZ
	for (i=0; i<m_GridnumX-1; i++)
	{
		for (j=1; j<PML+1; j++)
		{
			Compoint.j=0;
			Compoint.i=1+i;
			velocity=m_Vel[GetVelocitystore(Compoint)];
			alphaz=alphazm*(1-sin(j*3.14159/2/PML));
			a1=m_vecUUX[i*(PML+1)+j];
			if (i==0)
			{
				p1=m_vecAUX[j-1]*velocity*velocity;
				p2=m_vecALX[PML*(2*PML+m_GridnumZ-1)+j-1]*velocity*velocity;
			}
			else if (i==m_GridnumX-2)
			{
				p1=m_vecARX[PML*(2*PML+m_GridnumZ-1)+j-1]*velocity*velocity;
				p2=m_vecAUX[(m_GridnumX-3)*PML+j-1]*velocity*velocity;
			}
			else
			{
				p1=m_vecAUX[i*PML+j-1]*velocity*velocity;
				p2=m_vecAUX[(i-1)*PML+j-1]*velocity*velocity;
			}
			m_vecUUX[i*(PML+1)+j]=UPMLabsorb(a1,p1,p2,alphax,m_GridcellX);

			a1=m_vecUUZ[i*(PML+1)+j];
			p1=m_vecAUZ[i*(PML+1)+j]*velocity*velocity;
			p2=m_vecAUZ[i*(PML+1)+j-1]*velocity*velocity;
			m_vecUUZ[i*(PML+1)+j]=UPMLabsorb(a1,p1,p2,alphaz,m_GridcellZ);
		}
	}

	//m_vecGridUNext上边界计算
	for (i=0; i<m_GridnumX-1; i++)
	{
		m_vecGridUNext[GetUstore(i+1,0)]=m_vecUUX[i*(PML+1)+PML]+m_vecUUZ[i*(PML+1)+PML];

		Compoint.i=i+1;
		Compoint.j=0;
		velocity=m_Vel[GetVelocitystore(Compoint)];

		if ((i+1)==m_ShotGridX&&0==m_ShotGridZ)//震源处理
		{
		/*	if(s==0)
				src =  m_Source[m_t-1];
			else if(s==1)
			{
				if(m_t==1)
					src =    (m_AvecArecord[GetURstore(i+1,0,m_t+1)] - m_AvecArecord[GetURstore(i+1,0,m_t)])
					/(m_tstep*m_tstep)*(2)/(velocity*velocity*velocity);

				else if(m_t>1&&m_t<m_tnum)
					src =    (m_AvecArecord[GetURstore(i+1,0,m_t+1)]+m_AvecArecord[GetURstore(i+1,0,m_t-1)]-2*m_AvecArecord[GetURstore(i+1,0,m_t)])
					/(m_tstep*m_tstep)*(2)/(velocity*velocity*velocity);

				else if (m_t==m_tnum)
					src =    (m_AvecArecord[GetURstore(i+1,0,m_t-1)] -m_AvecArecord[GetURstore(i+1,0,m_t)])
					/(m_tstep*m_tstep)*(2)/(velocity*velocity*velocity);
			} 
			else if (s==2)

				src = SourceG(m_t);
			else if(s==3&&m_t==1)
				src=1;
			else
				src = 0.0;
		
		else
			src=0;*/
		m_vecGridUNext[GetUstore(i+1,0)]  -=(SourceG(m_t)*m_tstep+Sourcet)*velocity*velocity*m_tstep;
		}
		
	}

	



	////////////////////////////////下边界Downside//////////////////////////////////////////
	alphax=0;
	alphaz=alphazm;

	//ADX
	for (i=0; i<m_GridnumX-2; i++)
	{
		for (j=0; j<PML; j++)
		{
			alphaz=alphazm*(1-sin((j+1)*3.14159/2/PML));
			a1=m_vecADX[i*PML+j];
			p1=m_vecUDX[(i+1)*(PML+1)+j+1]+m_vecUDZ[(i+1)*(PML+1)+j+1];
			p2=m_vecUDX[i*(PML+1)+j+1]+m_vecUDZ[i*(PML+1)+j+1];
			m_vecADX[i*PML+j]=UPMLabsorb(a1,p1,p2,alphax,m_GridcellX);
		}
	}

	//ADZ
	for (i=0; i<m_GridnumX-1; i++)
	{
		for (j=0; j<PML+1; j++)
		{
			alphaz=alphazm*(1-sin(j*3.14159/2/PML));
			a1=m_vecADZ[i*(PML+1)+j];
			if (j==0)
			{
				p2=m_vecUDX[i*(PML+1)+j+1]+m_vecUDZ[i*(PML+1)+j+1];
				p1=p2;
			}
			else if (j==PML)
			{
				p2=m_vecGridUNext[GetUstore(i+1,m_GridnumZ-1)];
				p1=m_vecUDX[i*(PML+1)+j]+m_vecUDZ[i*(PML+1)+j];
			}
			else
			{
				p2=m_vecUDX[i*(PML+1)+j+1]+m_vecUDZ[i*(PML+1)+j+1];
				p1=m_vecUDX[i*(PML+1)+j]+m_vecUDZ[i*(PML+1)+j];
			}
			m_vecADZ[i*(PML+1)+j]=UPMLabsorb(a1,p1,p2,alphaz,m_GridcellZ);
		}
	}

	for (i=0; i<m_GridnumX-1; i++)
	{
		m_vecADZ[i*(PML+1)+PML]=m_vecGridUB[i*m_GridnumZ+m_GridnumZ-1];
	}



	//UDX,UDZ
	for (i=0; i<m_GridnumX-1; i++)
	{
		for (j=1; j<PML+1; j++)
		{
			Compoint.j=m_GridnumZ;
			Compoint.i=1+i;
			velocity=m_Vel[GetVelocitystore(Compoint)];
			alphaz=alphazm*(1-sin(j*3.14159/2/PML));
			a1=m_vecUDX[i*(PML+1)+j];
			if (i==0)
			{
				p1=m_vecADX[j-1]*velocity*velocity;
				p2=m_vecALX[PML*(2*PML+m_GridnumZ-1)+2*PML+m_GridnumZ-j-1]*velocity*velocity;
			}
			else if (i==m_GridnumX-2)
			{
				p1=m_vecARX[PML*(2*PML+m_GridnumZ-1)+2*PML+m_GridnumZ-j-1]*velocity*velocity;
				p2=m_vecADX[(m_GridnumX-3)*PML+j-1]*velocity*velocity;
			}
			else
			{
				p1=m_vecADX[i*PML+j-1]*velocity*velocity;
				p2=m_vecADX[(i-1)*PML+j-1]*velocity*velocity;
			}
			m_vecUDX[i*(PML+1)+j]=UPMLabsorb(a1,p1,p2,alphax,m_GridcellX);

			a1=m_vecUDZ[i*(PML+1)+j];
			p2=m_vecADZ[i*(PML+1)+j]*velocity*velocity;
			p1=m_vecADZ[i*(PML+1)+j-1]*velocity*velocity;
			m_vecUDZ[i*(PML+1)+j]=UPMLabsorb(a1,p1,p2,alphaz,m_GridcellZ);
		}
	}

	//m_vecGridUNext下边界计算
	for (i=0; i<m_GridnumX-1; i++)
	{
		m_vecGridUNext[GetUstore(i+1,m_GridnumZ)]=m_vecUDX[i*(PML+1)+PML]+m_vecUDZ[i*(PML+1)+PML];

		Compoint.i=i+1;
		Compoint.j=m_GridnumZ;
		velocity=m_Vel[GetVelocitystore(Compoint)];

		if ((i+1)==m_ShotGridX&&m_GridnumZ==m_ShotGridZ)
		{
		/*	if(s==0)
				src =  m_Source[m_t-1];
			else if(s==1)
			{
				if(m_t==1)
					src =    (m_AvecArecord[GetURstore(i+1,m_GridnumZ,m_t+1)] - m_AvecArecord[GetURstore(i+1,m_GridnumZ,m_t)])
					/(m_tstep*m_tstep)*(2)/(velocity*velocity*velocity);

				else if(m_t>1&&m_t<m_tnum)
					src =    (m_AvecArecord[GetURstore(i+1,m_GridnumZ,m_t+1)]+m_AvecArecord[GetURstore(i+1,m_GridnumZ,m_t-1)]-2*m_AvecArecord[GetURstore(i+1,m_GridnumZ,m_t)])
					/(m_tstep*m_tstep)*(2)/(velocity*velocity*velocity);

				else if (m_t==m_tnum)
					src =    (m_AvecArecord[GetURstore(i+1,m_GridnumZ,m_t-1)] -m_AvecArecord[GetURstore(i+1,m_GridnumZ,m_t)])
					/(m_tstep*m_tstep)*(2)/(velocity*velocity*velocity);
			} 
			else if (s==2)

				src = SourceG(m_t);
			else if(s==3&&m_t==1)
				src=1;
			else
				src = 0.0;
		
		else
			src=0;*/
		m_vecGridUNext[GetUstore(i+1,m_GridnumZ)]  -= (SourceG(m_t)*m_tstep+Sourcet)*velocity*velocity*m_tstep;
		}
		
	}


	

}


void C2DModel::ShiftUstore()
{
	int i,j;
	for(i=0;i<=m_GridnumX;i++)
	{
		for(j=0;j<=m_GridnumZ;j++)
		{
			m_vecGridUOrigin[GetUstore(i,j)]=m_vecGridUNow[GetUstore(i,j)];
			m_vecGridUNow[GetUstore(i,j)]=m_vecGridUNext[GetUstore(i,j)];
		}
	}
	m_vecGridUNext.assign((m_GridnumX+1)*(m_GridnumZ+1),0.0);
//	m_vecGridUNext.clear();
}

double C2DModel::SourceG(int t)
{
	double time=0,result=0;
	const double PI=3.1415926;
	double Av=0,Bp=0;
	double freq=20;
	//if(t<=200)
	//	result=0;
	//else{
	//time=(t-200)*m_tstep;
	time=t*m_tstep;
	Av=1-2*(PI*freq*(time-1/freq))*(PI*freq*(time-1/freq));
	Bp=-(PI*freq*(time-1/freq))*(PI*freq*(time-1/freq));
	result=Av*exp(Bp);//}

	return result;
}

double C2DModel::SourceGT(int t)
{
	double time=0,result=0;
	if(t==1)
	    result=1.0;
	else
		result=0.0;
 
	return result;
}

void C2DModel::Filter(vector<double> &S,double freq,int t)
{   
	
	int i,s_n,N;
	int r=0;
	float freq0,cq;
	complex<float> *p = new complex<float> [t];
	complex<float> *q = new complex<float> [t];
	for (i=0; i<=t-1; i++)
	{
		p[i] = S[i];
	}
	
	
	/*for(i=t;i<N;i++)
           p[i] = 0;*/
	
	for(i=1;i<t;i*=2)
		r++;
	FFT(p, q, r);

	

	freq0 = 1/0.768;
	s_n = freq/freq0;
	N = 1.52 * s_n;
	//cq = freq/3/freq0;

	for (i=N+1; i<=t-N; i++)
	{
		q[i] = 0;
	} 


	for (i=0;i<=N;i++)
	{
		q[i] *= 0.54-0.46*cos(pi*(i+N)/N);
	}

	for (i=t-N+1;i<t;i++)
	{
		q[i] *= 0.54-0.46*cos(pi*(i-t-1+N)/(N-2));
	}



	IFFT(q, p, r);
    for (i=0; i<=t-1; i++)
	{
		S[i] = p[i].real();
	}
//FILE *fp3;
//fp3 = fopen("lvbo.txt","a+"); 
//	for (m_t=0;m_t<t;m_t++)
//        {	
//			fprintf(fp3,"%f",S[m_t]);
//                 fprintf(fp3,"%s"," "); 
//				 
//
//         }
//	fprintf(fp3,"\n");
//				 fprintf(fp3,"\n");fprintf(fp3,"\n");fprintf(fp3,"\n");fprintf(fp3,"\n");fprintf(fp3,"\n");
//
//	fclose(fp3);

}


double C2DModel::U0(double a1, double a2, double x1, double x2, double z1, double src, double v)
{
	float y1,y2,f;
	y1=m_tstep*m_tstep*v*v*(x1+x2-2*a1)/(m_GridcellX*m_GridcellX);
	y2=m_tstep*m_tstep*v*v*(z1-a1)/(m_GridcellZ*m_GridcellZ);
	f=y1+y2+2*a1-a2-src*m_tstep*m_tstep*v*v;
	
	return f;	
}

double C2DModel::U1(double a1, double a2, double x1, double x2, double z1, double z2, double v)
{
	float y1,y2,f;
	y1=m_tstep*m_tstep*v*v*(x1+x2-2*a1)/(m_GridcellX*m_GridcellX);
	y2=m_tstep*m_tstep*v*v*(z1+z2-2*a1)/(m_GridcellZ*m_GridcellZ);
	f=y1+y2+2*a1-a2;

	return f;
}

double C2DModel::U2(double a1, double a2, double x1, double x2, double x3, double x4, 
					double z1, double z2, double z3, double z4, double v)
{
	double y1,y2,f;
	y1=m_tstep*m_tstep*v*v*(-x3+16*x1-30*a1+16*x2-x4)/(12*m_GridcellX*m_GridcellX);
	y2=m_tstep*m_tstep*v*v*(-z3+16*z1-30*a1+16*z2-z4)/(12*m_GridcellZ*m_GridcellZ);
	f=y1+y2+2*a1-a2;
	
	return f;
}

double C2DModel::UPMLabsorb(double a1, double p1, double p2, double alpha, double h)
{
	double y1,y2,f;
	y1=1-m_tstep*alpha;
	y2=m_tstep*(p1-p2)/h;
	f=y1*a1+y2;
	
	return f;
}

void C2DModel::umfpack(double *rhs,SeqList *myList, double *sol,int Lside,int Rside)
{
	int nne = (Rside-Lside+1)*(m_GridnumZ+1);
	int nnz = myList->size;	
	int i;
	int *Ti = new int[nnz];
	int *Tj = new int[nnz];
	double *Tx = new double[nnz];
	for (i=0;i<nnz;i++)
	{
		Ti[i] = myList->list[i].i;
		Tj[i] = myList->list[i].j;
		Tx[i] = myList->list[i].val;
	}

	int *Ap = new int[nne+1];
	int *Ai = new int[nnz];
	double *Ax = new double[nnz];
	umfpack_di_triplet_to_col(nne,nne,nnz,Ti,Tj,Tx,Ap, Ai, Ax, (int *) NULL);


	double *Xx = new double[nne];
	double *Bx = new double[nne];
	for (i=0;i<nne;i++)
	{		
		Bx[i] = rhs[i];

	}
	double *null = (double *) NULL ;
	void *Symbolic, *Numeric ;
	(void) umfpack_di_symbolic (nne, nne, Ap, Ai, Ax, &Symbolic, null, null) ;
	(void) umfpack_di_numeric (Ap, Ai, Ax, Symbolic, &Numeric, null, null) ;
	umfpack_di_free_symbolic (&Symbolic) ;
	(void) umfpack_di_solve (UMFPACK_A, Ap, Ai, Ax, Xx,Bx, Numeric, null, null) ;
	umfpack_di_free_numeric (&Numeric) ;

	for (i=0;i<nne;i++)
	{
		sol[i]=(double)(Xx[i]);	
	}

	delete []Ti;
	delete []Tj;
	delete []Tx;
	delete []Ap;
	delete []Ai;
	delete []Ax;	

}

BOOL C2DModel::Test(int iterations)
{
	int i,j,k,n;
	double velocity;
	double m_GridcellXtmp,m_GridcellZtmp,m_tsteptmp;
	int m_GridnumXtmp,m_GridnumZtmp,m_tnumtmp,NStmp;
	GRIDCOOR origin;

	m_GridcellXtmp=m_GridcellX;
	m_GridcellZtmp=m_GridcellZ;
	m_GridnumXtmp=m_GridnumX;
	m_GridnumZtmp=m_GridnumZ;
	m_tsteptmp=m_tstep;
	m_tnumtmp=m_tnum;


	InitialMember();
	InitialSource();
	
	m_vecGridVelocity.clear();
	m_vecGridVelocity.assign((m_GridnumX+1)*(m_GridnumZ+1),0.0);
	m_vecGridRecord_tmp.clear();
	m_vecGridRecord_tmp.assign((m_GridnumX+1)*(m_tnum),0.0);
	m_Source_t.clear();
	m_Source_t.assign(m_tnum,0.0);
	m_Source_tmp.clear();
	m_Source_tmp.assign(m_tnum,0.0);
	
	//IncreaseVelocity();
	//Highspeed_model();
	ThreeLayer();
	Createvelocitymat(1000);

	/**************************
	源数据 m_Source,m_Source_tmp;
	***************************/
	for(m_t=1;m_t<=m_tnum;m_t++)
    {
		m_Source[m_t-1]=SourceG(m_t);
		m_Source_tmp[m_t-1]=m_Source[m_t-1];
	}

    m_ShotZ=2*m_GridcellZ;
	for(m_ShotX=0; m_ShotX<(m_GridnumX+1)*m_GridcellX; m_ShotX+=2*m_GridcellX)
		WaveRecord(m_ShotX, m_ShotZ,m_vecGridVelocity,0,2);

	/**************************
	正演数据：m_vecGridRecord
	4倍正演数据：m_vecGridRecord_tmp
	***************************/
	for(i=0; i<(m_GridnumX+1); i++)
		for(m_t=1;m_t<=m_tnum;m_t++)
			m_vecGridRecord_tmp[GetRecordstore(i,m_t)]=m_vecGridRecord[GetRecordstore(i,m_t)];
	
	/**************************
	源滤波：得m_Source_t
	观察数据滤波：得m_vecGridRecord
	***************************/
    
	for(m_t=1;m_t<=m_tnum;m_t++)
		m_Source_t[m_t-1]=m_Source_tmp[m_t-1];
	     Filter(m_Source_t,15,m_tnum);
	m_vecGridRecord_t.clear();
	m_vecGridRecord_t.assign(m_tnum,0.0);
    for(i=0; i<(m_GridnumX+1); i++)
	{
		for(m_t=1;m_t<=m_tnum;m_t++)
			/*网格要变大，四倍观察数据后再滤波*/
			m_vecGridRecord_t[m_t-1]=4*m_vecGridRecord_tmp[GetRecordstore(i,m_t)];
        Filter(m_vecGridRecord_t,15,m_tnum);
		for(m_t=1;m_t<=m_tnum;m_t++)
			m_vecGridRecord[GetRecordstore(i,m_t)]=m_vecGridRecord_t[m_t-1];
	}



	/*****************************************
	上面是观察数据和源数据的 15HZ低通滤波 处理。
	得到滤波后的源数据m_Source_t；
	    观察数据滤波：得m_vecGridRecord；
	****************************************/

	
	/*************************************************
	将网格做大一倍，网格点数减少一倍。先用大网格反演基本模块
	**************************************************/
    m_GridcellX=20.0;
	m_GridcellZ=10.0;
	m_GridnumX=33;
	m_GridnumZ=33;
	m_tstep=0.0015;
	m_tnum=(int)(m_tsearch/m_tstep);
	NS=2;
	LSQRInitial();
	InitialSource();

	/***************************************************
	观察的地震数据m_vecFrecord：(网格点变大，从m_vecGridRecord抽取数据)
	大网格的源数据m_Source：(网格点变大，从m_Source_t抽取数据)
	****************************************************/
    for(i=0; i<(m_GridnumX+1); i++)
		for(m_t=1;m_t<=m_tnum;m_t++)
			m_vecFrecord[(m_t-1)*(m_GridnumX+1)+i]=
				m_vecGridRecord[NS*(m_t-1)*(NS*m_GridnumX+1)+NS*i];
	for(m_t=0;m_t<m_tnum;m_t++)
		m_Source[m_t]=m_Source_t[NS*m_t];

    m_vecGridVelocity.clear();
	m_vecGridVelocity.assign((m_GridnumX+1)*(m_GridnumZ+1),0.0);
    m_tmpGridVelocity.clear();
	m_tmpGridVelocity.assign((m_GridnumX+1)*(m_GridnumZ+1),0.0);

	/***************************************************
	写文件观察的地震数据m_vecFrecord
	****************************************************/
	FILE *fp3;
	fp3 = fopen("jieshoudian2.txt","a+");
	fprintf(fp3,"%s","只是数据滤波m_vecFrecord：");	
	for (m_t=1;m_t<=m_tnum;m_t++)
	{	
		fprintf(fp3,"%f",m_vecFrecord[(m_t-1)*(m_GridnumX+1)+1]);
		fprintf(fp3,"%s"," "); 		 
	}
	fprintf(fp3,"\n");
	fprintf(fp3,"\n");
	fprintf(fp3,"\n");
	fprintf(fp3,"\n");
	fprintf(fp3,"\n");
	fprintf(fp3,"\n");
	fclose(fp3);
	

	//ConstVelocity();
	IncreaseVelocity();
	Createvelocitymat(0);

	/***************************************************
	Step1：大网格反演。得到一个有大概的速度模型。
	****************************************************/
	LSQR(iterations);

	/***************************************************
	将Step1大网格反演得到的速度模型作为小网格反演的初始模型。
	初始速度模型：m_tmpGridVelocity
	****************************************************/
	for(i=0;i<m_GridnumX+1;i++)
		for(j=0;j<m_GridnumZ+1;j++)
			m_tmpGridVelocity[GetUstore(i,j)]=m_vecGridVelocity[GetUstore(i,j)];

    m_GridcellX=10.0;
	m_GridcellZ=5.0;
	m_GridnumX=66;
	m_GridnumZ=66;
	m_tstep=0.00075;
	m_tnum=(int)(m_tsearch/m_tstep);
	NS=1;
	LSQRInitial();
	InitialSource();

	for(m_t=1;m_t<=m_tnum;m_t++)
		m_Source[m_t-1]=m_Source_tmp[m_t-1];
	/*
	m_vecGridRecord_tmp：没有滤波的观察数据
	*/
	 for(i=0; i<(m_GridnumX+1); i++)
		for(m_t=1;m_t<=m_tnum;m_t++)
		   {
			   m_vecGridRecord[GetRecordstore(i,m_t)]=m_vecGridRecord_tmp[GetRecordstore(i,m_t)];
			   m_vecFrecord[GetRecordstore(i,m_t)]=m_vecGridRecord[GetRecordstore(i,m_t)];
	       }

    m_vecGridVelocity.clear();
	m_vecGridVelocity.assign((m_GridnumX+1)*(m_GridnumZ+1),0.0);

    /*从大网格的速度模型m_tmpGridVelocity插值得到小网格的速度模型m_vecGridVelocity*/
	for(i=0;i<m_GridnumX+1;i++,i++)
		for(j=0;j<m_GridnumZ+1;j++,j++)
			m_vecGridVelocity[GetUstore(i,j)]=m_tmpGridVelocity[Get2Ustore(i/2,j/2)];
	Createvelocitymat(2000);
	for(i=1;i<m_GridnumX;i++,i++)
		for(j=0;j<m_GridnumZ+1;j++,j++)
			m_vecGridVelocity[GetUstore(i,j)]=0.5*(m_tmpGridVelocity[Get2Ustore((i+1)/2,j/2)]+m_tmpGridVelocity[Get2Ustore((i-1)/2,j/2)]);
	Createvelocitymat(3000);
	for(i=0;i<m_GridnumX+1;i++,i++)
		for(j=1;j<m_GridnumZ;j++,j++)
			m_vecGridVelocity[GetUstore(i,j)]=0.5*(m_tmpGridVelocity[Get2Ustore(i/2,(j+1)/2)]+m_tmpGridVelocity[Get2Ustore(i/2,(j-1)/2)]);
	Createvelocitymat(4000);
	for(i=1;i<m_GridnumX;i++,i++)
		for(j=1;j<m_GridnumZ;j++,j++)
			m_vecGridVelocity[GetUstore(i,j)]=0.25*(m_tmpGridVelocity[Get2Ustore((i+1)/2,j/2)]+m_tmpGridVelocity[Get2Ustore((i-1)/2,j/2)]+m_tmpGridVelocity[Get2Ustore(i/2,(j+1)/2)]+m_tmpGridVelocity[Get2Ustore(i/2,(j-1)/2)]);
    Createvelocitymat(5000);

	LSQR(iterations);

	return TRUE;
}

BOOL C2DModel::LSQR(int iterations)
{
	
	int i,j,k,n;
	double alpha1,alpha2,cost0,velocity;
	alpha1=0.001;
	alpha2=0.001;
	for(int iter = 0; iter < iterations; iter++)
	{
		cost0=0;
		InitialMember();
        m_AvecArecord.clear();
        m_AvecArecord.assign((m_GridnumX+1)*(m_GridnumZ+1)*m_tnum,0.0);
		m_AvecArecord1.clear();
        m_AvecArecord1.assign((m_GridnumX+1)*(m_GridnumZ+1)*m_tnum,0.0);
		m_AvecArecord0.clear();
        m_AvecArecord0.assign((m_GridnumX+1)*(m_GridnumZ+1)*m_tnum,0.0);
		m_AvecArecordY.clear();
        m_AvecArecordY.assign((m_GridnumX+1)*(m_GridnumZ+1)*m_tnum,0.0);
     

		for(m_ShotX=0; m_ShotX<(m_GridnumX+1)*m_GridcellX; m_ShotX+=2/NS*m_GridcellX)
		{
			m_ShotZ=2/NS*m_GridcellZ;
			/*m_ShotX=15*m_GridcellX;*/
            m_ShotGridX=(int)(m_ShotX/m_GridcellX);
            m_ShotGridZ=(int)(m_ShotZ/m_GridcellZ);

            RecordInitial();
            for(m_t=1;m_t<=m_tnum;m_t++)
            {
                UpdateNextU(m_vecGridVelocity,0);
                ShiftUstore();
                for(i=0;i<m_GridnumX+1;i++)
                    for(j=0;j<m_GridnumZ+1;j++)
                    {
                        m_AvecArecord[GetURstore(i,j,m_t)] += m_vecGridUNow[GetUstore(i,j)];
                       // m_DAvecArecord[(m_ShotX/m_GridcellX-1)*(m_GridnumX-1)*m_GridnumZ*m_tnum+GetURstore(i,j,m_t)]= m_vecGridUNow[GetUstore(i,j)];
                    }                                                                       
            }		
		}


		/*A(V)向量赋值*/
		/*for(i=0; i<(m_GridnumX-1)*(m_tnum/2); i++)
			m_vecArecord[i]=m_vecGridRecord[i+(i/(m_GridnumX-1)*2)*(m_GridnumX-1)];*/

		for(i=0; i<(m_GridnumX+1); i++)
		for(m_t=1;m_t<=m_tnum;m_t++)
		m_vecArecord[(m_t-1)*(m_GridnumX+1)+i]=m_vecGridRecord[GetRecordstore(i,m_t)];

		FILE *fp2;
		fp2 = fopen("jieshoudian2.txt","a+");
        fprintf(fp2,"%s","滤波前m_vecArecord：");	
	    for (m_t=1;m_t<=m_tnum;m_t++)
		{	
			fprintf(fp2,"%f",m_vecArecord[(m_t-1)*(m_GridnumX+1)+1]);
            fprintf(fp2,"%s"," "); 
         }
		fprintf(fp2,"\n");
		fprintf(fp2,"\n");
		fprintf(fp2,"\n");
		fprintf(fp2,"\n");
		fprintf(fp2,"\n");
		fprintf(fp2,"\n");
		fclose(fp2);

		/*A(V)-F*/
		for( i=0; i<(m_GridnumX+1)*(m_tnum); i++)
			m_vecdeta_record[i]=m_vecArecord[i]-m_vecFrecord[i];

		/*目标函数值，即泛函值cost0,每一个点的值都相加而得*/
		for (int t=0;t<(m_tnum);t++)
		{
			for (i=0;i<m_GridnumX+1;i++)
			{
				cost0+=m_vecdeta_record[t*(m_GridnumX+1)+i]*m_vecdeta_record[t*(m_GridnumX+1)+i];
			}
		}

		InitialMember();
		/*A(V)的导数*/
		 for(int Rnum=0;Rnum<m_GridnumX+1;Rnum+=2/NS)
		 {
			m_ShotX=Rnum*m_GridcellX ;
            m_ShotZ=2/NS*m_GridcellZ ;
			m_ShotGridX=(int)(m_ShotX/m_GridcellX);
            m_ShotGridZ=(int)(m_ShotZ/m_GridcellZ);

			m_AvecArecord1.clear();
			m_AvecArecord1.assign((m_GridnumX+1)*(m_GridnumZ+1)*m_tnum,0.0);
			m_AvecArecord0.clear();
			m_AvecArecord0.assign((m_GridnumX+1)*(m_GridnumZ+1)*m_tnum,0.0);
			m_AvecArecordY.clear();
			m_AvecArecordY.assign((m_GridnumX+1)*(m_GridnumZ+1)*m_tnum,0.0);
			
            RecordInitial();
            for(m_t=1;m_t<=m_tnum;m_t++)
            {
                UpdateNextU(m_vecGridVelocity,2);
                ShiftUstore();
                for(i=0;i<m_GridnumX+1;i++)
                    for(j=0;j<m_GridnumZ+1;j++)
                    {
                        m_AvecArecord1[GetURstore(i,j,m_t)] += m_vecGridUNow[GetUstore(i,j)];
                       // m_DAvecArecord[(m_ShotX/m_GridcellX-1)*(m_GridnumX-1)*m_GridnumZ*m_tnum+GetURstore(i,j,m_t)]= m_vecGridUNow[GetUstore(i,j)];
                    }                                                                       
            } 
		//	CreateRecordmat();

			for(i=0;i<m_GridnumX+1;i++)
                    for(j=0;j<m_GridnumZ+1;j++)
						for(m_t=1;m_t<=m_tnum;m_t++)
						{
							Compoint.i=i;
                            Compoint.j=j;
                            velocity=m_vecGridVelocity[GetVelocitystore(Compoint)];
							if(m_t==1)
								 m_AvecArecord0[GetURstore(i,j,m_t)] =    (m_AvecArecord[GetURstore(i,j,m_t+1)] - m_AvecArecord[GetURstore(i,j,m_t)])
										 /(m_tstep*m_tstep)*(2)/(velocity*velocity*velocity);
							else if(m_t>1&&m_t<m_tnum)
								 m_AvecArecord0[GetURstore(i,j,m_t)] =    (m_AvecArecord[GetURstore(i,j,m_t+1)]+m_AvecArecord[GetURstore(i,j,m_t-1)]-2*m_AvecArecord[GetURstore(i,j,m_t)])
										 /(m_tstep*m_tstep)*(2)/(velocity*velocity*velocity);
							else 
								 m_AvecArecord0[GetURstore(i,j,m_t)] =    (m_AvecArecord[GetURstore(i,j,m_t-1)] -m_AvecArecord[GetURstore(i,j,m_t)]) 
										 /(m_tstep*m_tstep)*(2)/(velocity*velocity*velocity);
						}


           for(i=0;i<m_GridnumX+1;i++)
                    for(j=0;j<m_GridnumZ+1;j++)
					{
						n=1;
						for(m_t=1;m_t<=m_tnum;m_t++)
						{
							n++;
							for(k=1; k<=m_t; k++)
                               m_AvecArecordY[GetURstore(i,j,m_t)] +=m_AvecArecord0[GetURstore(i,j,k)]*m_AvecArecord1[GetURstore(i,j,n-k)];
						}
					}

			

            for(i=0;i<m_GridnumX+1;i++)
                    for(j=0;j<m_GridnumZ+1;j++)
						for(m_t=1;m_t<=(m_tnum);m_t++)
                            ADerivative[GetRecordstore(Rnum,m_t)][i*(m_GridnumZ+1)+j] =m_AvecArecordY[GetURstore(i,j,m_t)];
			


		 }

		/*A(V)导数的转置*/                                           
		for(i=0; i<(m_GridnumX+1)*(m_GridnumZ+1); i++)
			for(j=0; j<(m_GridnumX+1)*(m_tnum); j++)
			{
				ADerivative_T[i][j]=ADerivative[j][i];
			}
		/*M1*/
		for(i=0; i<(m_GridnumX+1)*(m_GridnumZ+1); i++)
			for(j=0; j<(m_GridnumX+1)*(m_GridnumZ+1); j++)
			{
				M1[i][j]=0;
			}
		for(k=0; k<(m_GridnumZ+1); k++)
			for(i=k*(m_GridnumX+1)+1; i<(k+1)*(m_GridnumX+1)-1; i++)
			{
				j=(i-k*(m_GridnumX+1)-1)*(m_GridnumZ+1)+k;
				M1[i][j]=1;
				j=(i-k*(m_GridnumX+1))*(m_GridnumZ+1)+k;
				M1[i][j]=-2;
				j=(i-k*(m_GridnumX+1)+1)*(m_GridnumZ+1)+k;
				M1[i][j]=1;
			}
        /*M2*/
		for(i=0; i<(m_GridnumX+1)*(m_GridnumZ+1); i++)
			for(j=0; j<(m_GridnumX+1)*(m_GridnumZ+1); j++)
			{
				M2[i][j]=0;
			}
		for(k=0; k<m_GridnumX+1; k++)
			for(i=k*(m_GridnumZ+1)+1; i<(k+1)*(m_GridnumZ+1)-1; i++)
			{
				M2[i][i-1]=1;
				M2[i][i]=-2;
				M2[i][i+1]=1;
			}
		/*计算系数矩阵的三个元素*/
		Matric_multi_Matric(ADerivative,(m_GridnumX+1)*(m_GridnumZ+1),(m_GridnumX+1)*(m_tnum),C1,1);
		Matric_multi_Matric(M1,(m_GridnumX+1)*(m_GridnumZ+1),(m_GridnumX+1)*(m_GridnumZ+1),C2,alpha1);
		Matric_multi_Matric(M2,(m_GridnumX+1)*(m_GridnumZ+1),(m_GridnumX+1)*(m_GridnumZ+1),C3,alpha2);
		/*计算目标向量的三个元素*/
		Matric_multi_Vector(ADerivative_T,m_vecdeta_record,(m_GridnumX+1)*(m_GridnumZ+1),(m_GridnumX+1)*(m_tnum),B1);
		Matric_multi_Vector(C2,m_vecGridVelocity,(m_GridnumX+1)*(m_GridnumZ+1),(m_GridnumX+1)*(m_GridnumZ+1),B2);
		Matric_multi_Vector(C3,m_vecGridVelocity,(m_GridnumX+1)*(m_GridnumZ+1),(m_GridnumX+1)*(m_GridnumZ+1),B3);
		/*求矩阵方程的C和b*/
		for(i=0; i<(m_GridnumX+1)*(m_GridnumZ+1); i++)
			b[i]=B1[i]+B2[i]+B3[i];

		int max=(m_GridnumX+1)*(m_GridnumZ+1)*(m_GridnumX+1)*(m_GridnumZ+1);
		ListInitiate(&myList, max);
		int pos = 0;
		DataType elm;
		for(i=0; i<(m_GridnumX+1)*(m_GridnumZ+1); i++)
			for(j=0; j<(m_GridnumX+1)*(m_GridnumZ+1); j++)
			{
				elm.i=i;
				elm.j=j;
				elm.val=C1[i][j]+C2[i][j]+C3[i][j];
				ListInsert(&myList, pos, elm);
				pos++;
			}
//		lsqr_test(deta_V, &myList, b, (m_GridnumX-1)*m_GridnumZ, (m_GridnumX-1)*m_GridnumZ, 0.01, 100);
		umfpack(b,&myList, deta_V,0,m_GridnumX);

		FILE *fp1; 
		fp1 = fopen("直接法.txt","a+"); 
		fprintf(fp1,"\n");
		fprintf(fp1,"%s","第");	
		fprintf(fp1,"%d",iter);
		fprintf(fp1,"%s","次：");
		fprintf(fp1,"\n"); 
		fprintf(fp1,"%s","cost0:");	
		fprintf(fp1,"%f",cost0);
		fprintf(fp1,"\n"); 
		fprintf(fp1,"\n"); 
		fclose(fp1); 

		for(i=0; i<(m_GridnumX+1)*(m_GridnumZ+1); i++)
		{
			if(deta_V[i]!=0)
			{
				m_vecGridVelocity[i]-=deta_V[i];
				if(m_vecGridVelocity[i]<200)
					m_vecGridVelocity[i]=200;
				if(m_vecGridVelocity[i]>6000)
					m_vecGridVelocity[i]=6000;
			}
		}
		ListDelete(&myList);
		Createvelocitymat(iter+1);	
	}

	/*释放内存*/
	delete []deta_V;
	deta_V=NULL;
	delete []B1;
	B1=NULL;
	delete []B2;
	B2=NULL;
	delete []B3;
	B3=NULL;
	delete []b;
	b=NULL;
	
	for(i=0; i<(m_GridnumX+1)*(m_tnum); i++)
	{
		delete []ADerivative[i];
		ADerivative[i]=NULL;
	}
	delete []ADerivative;
	ADerivative=NULL;
	for(i=0; i<(m_GridnumX+1)*(m_GridnumZ+1); ++i)
	{
		delete []ADerivative_T[i];
		ADerivative_T[i]=NULL;
	}
	delete []ADerivative_T;
	ADerivative_T=NULL;


	return TRUE;
}

void C2DModel::LSQRInitial()
{
	/*初始化各向量*/
	m_vecFrecord.clear();
	m_vecFrecord.assign((m_GridnumX+1)*(m_tnum),0.0);
	m_vecArecord.clear();
	m_vecArecord.assign((m_GridnumX+1)*(m_tnum),0.0);
	m_vecdeta_record.clear();
	m_vecdeta_record.assign((m_GridnumX+1)*(m_tnum),0.0);

	int i,j;
	deta_V = new double[(m_GridnumX+1)*(m_GridnumZ+1)];
	for(i=0; i<(m_GridnumX+1)*(m_GridnumZ+1); ++i)
		deta_V[i]=0.0;

	B1 = new double[(m_GridnumX+1)*(m_GridnumZ+1)];
	for(i=0; i<(m_GridnumX+1)*(m_GridnumZ+1); ++i)
		B1[i]=1.0E+8;

	B2 = new double[(m_GridnumX+1)*(m_GridnumZ+1)];
	for(i=0; i<(m_GridnumX+1)*(m_GridnumZ+1); ++i)
		B2[i]=1.0E+8;

	B3 = new double[(m_GridnumX+1)*(m_GridnumZ+1)];
	for(i=0; i<(m_GridnumX+1)*(m_GridnumZ+1); ++i)
		B3[i]=1.0E+8;

	b = new double[(m_GridnumX+1)*(m_GridnumZ+1)];
	for(i=0; i<(m_GridnumX+1)*(m_GridnumZ+1); ++i)
		b[i]=1.0E+8;
	
	/*初始化各数组*/
	ADerivative = new double*[(m_GridnumX+1)*(m_tnum)];
	for( i=0; i<(m_GridnumX+1)*(m_tnum); ++i)
	{
		ADerivative[i]= new double[(m_GridnumX+1)*(m_GridnumZ+1)];
	}
	for( i=0; i<(m_GridnumX+1)*(m_tnum); ++i)
		for( j=0; j<(m_GridnumX+1)*(m_GridnumZ+1); ++j)
		{
			ADerivative[i][j]=0.0;
		}


	ADerivative_T = new double*[(m_GridnumX+1)*(m_GridnumZ+1)];
	for( i=0; i<(m_GridnumX+1)*(m_GridnumZ+1); ++i)
	{
		ADerivative_T[i]= new double[(m_GridnumX+1)*(m_tnum)];
	}
	for( i=0; i<(m_GridnumX+1)*(m_GridnumZ+1); ++i)
		for( j=0; j<(m_GridnumX+1)*(m_tnum); ++j)
		{
			ADerivative_T[i][j]=0.0;
		}
		
	M1=new int*[(m_GridnumX+1)*(m_GridnumZ+1)];
	for( i=0; i<(m_GridnumX+1)*(m_GridnumZ+1); ++i)
	{
		M1[i]= new int[(m_GridnumX+1)*(m_GridnumZ+1)];
	}
	for( i=0; i<(m_GridnumX+1)*(m_GridnumZ+1); ++i)
		for( j=0; j<(m_GridnumX+1)*(m_GridnumZ+1); ++j)
		{
			M1[i][j]=1.0E+8;
		}	

    M2=new int*[(m_GridnumX+1)*(m_GridnumZ+1)];
	for( i=0; i<(m_GridnumX+1)*(m_GridnumZ+1); ++i)
	{
		M2[i]= new int[(m_GridnumX+1)*(m_GridnumZ+1)];
	}
	for( i=0; i<(m_GridnumX+1)*(m_GridnumZ+1); ++i)
		for( j=0; j<(m_GridnumX+1)*(m_GridnumZ+1); ++j)
		{
			M2[i][j]=1.0E+8;
		}

    C1=new double*[(m_GridnumX+1)*(m_GridnumZ+1)];
	for( i=0; i<(m_GridnumX+1)*(m_GridnumZ+1); ++i)
	{
		C1[i]= new double[(m_GridnumX+1)*(m_GridnumZ+1)];
	}
	for( i=0; i<(m_GridnumX+1)*(m_GridnumZ+1); ++i)
		for( j=0; j<(m_GridnumX+1)*(m_GridnumZ+1); ++j)
		{
			C1[i][j]=1.0E+8;
		}	

	C2=new double*[(m_GridnumX+1)*(m_GridnumZ+1)];
	for( i=0; i<(m_GridnumX+1)*(m_GridnumZ+1); ++i)
	{
		C2[i]= new double[(m_GridnumX+1)*(m_GridnumZ+1)];
	}
	for( i=0; i<(m_GridnumX+1)*(m_GridnumZ+1); ++i)
		for( j=0; j<(m_GridnumX+1)*(m_GridnumZ+1); ++j)
		{
			C2[i][j]=1.0E+8;
		}	

    C3=new double*[(m_GridnumX+1)*(m_GridnumZ+1)];
	for( i=0; i<(m_GridnumX+1)*(m_GridnumZ+1); ++i)
	{
		C3[i]= new double[(m_GridnumX+1)*(m_GridnumZ+1)];
	}
	for( i=0; i<(m_GridnumX+1)*(m_GridnumZ+1); ++i)
		for( j=0; j<(m_GridnumX+1)*(m_GridnumZ+1); ++j)
		{
			C3[i][j]=1.0E+8;
		}	
}

void C2DModel::Matric_multi_Matric(double **matric,int size_A,int size_B,double **c,float alpha)
{
	int i,j,k;
	for(i=0; i<size_A; i++)
		for(j=0; j<size_A; j++)
		{
			c[i][j]=0;
			for(k=0; k<size_B; k++)
				c[i][j]+=alpha*matric[k][i]*matric[k][j];
		}
}

void C2DModel::Matric_multi_Matric(int **matric,int size_A,int size_B, double **c,float alpha)
{
	int i,j,k;
	for(i=0; i<size_A; i++)
		for(j=0; j<size_A; j++)
		{
			c[i][j]=0;
			for(k=0; k<size_B; k++)
				c[i][j]+=alpha*matric[k][i]*matric[k][j];
		}
}

void C2DModel::Matric_multi_Vector(double **matric,vector<double> vec,int size_A,int size_B,double *b)
{
	int i,j;
	for(i=0; i<size_A; i++)
	{
		b[i]=0;
		for(j=0; j<size_B; j++)
			b[i]+=matric[i][j]*vec[j];
	}
}

void C2DModel::ListInitiate(SeqList *L, int maxSize)
{
	L->size = 0;
	L->list = (DataType *)malloc(sizeof(DataType) * maxSize);
}

int C2DModel::ListInsert(SeqList *L, int i, DataType x)
{
	int j;
	if(i < 0 || i > L->size)
	{
		printf("index i illegle!\n");
		return 0;
	}
	else
	{
		for(j = L->size; j > i; j--)
			L->list[j] = L->list[j-1];
		L->list[i] = x;
		L->size++;
		return 1;
	}
}

int C2DModel::ListGet(SeqList *L, int i, DataType *x)
{
	*x = L->list[i];
	return 1;
}

void C2DModel::ListDelete(SeqList *L)
{
    //delete []L->list;
	free(L->list);
	L->list=NULL;
	
}

void C2DModel::lsqr_test(double *x, SeqList *myList,  double *b,
			        int size_A, int size_B, double tol, int maxit)
{
	double  n2b, 
		flag,
		tolb,
		beta,
		normr,	
		c,
		s,
		phibar,
		conda,
		alpha,
		normar,
	
		norma,
		sumnormd2,
		rho,
		thet,
		rhot,
		phi,
		tmp_val;
	
    n2b = 0;
	beta = 0;
	alpha = 0;
	
	long m, n; 
	int iter, i;
	int index;
    double tmp;
	//char buffer[10];
	m = size_A;
	n = size_B;
	//mn = size_A;
// 	itoa(mn, buffer, 10);
//     AfxMessageBox(buffer);
	//m = size_b;
// 	itoa(m, buffer, 10);
// 	AfxMessageBox(buffer);
	//n = mn / m;
// 	itoa(n, buffer, 10);
// 	AfxMessageBox(buffer);
	
	//CString str;
	//给z分配内存

	double *z = new double[n];

	///给u分配内存
	double *u = new double[m];
	
	/* n2b = norm(b); */

    n2b = vec_norm(b, m);
	flag = 1;                  //test
	
	tolb = tol * n2b;          ///test
	/*  Calculate  u = b - A * x; */

   	Matric_multi(1, myList, x, size_A, size_B, 1, 0, u); ///////////matric_multiple function

	//CFile  file;
	//file.Open("E:\\u.txt", CFile::modeCreate | CFile::modeWrite, NULL);
	//CStdioFile file;
	//sf.Open("E:\\u.txt", CFile::modeCreate|CFile::modeWrite);

	for(index = 0; index != m; index++)
	{
		u[index] = b[index] - u[index];
	} 
	/*for(index = 0; index != m; index++)
	{
		tmp = u[index];
		str.Format("%f  ", tmp);
		file.WriteString(str);
	}*/	
	/*   beta = norm(u);     */
	beta = vec_norm(u, m);	
	normr = beta;	
	if (beta != 0)
	{
		/* u = u / beta;   */
		for(index = 0; index != m; index++)
		{
			u[index] = u[index] / beta;
		}		
	}
	c = 1;    
	s = 0;	
	phibar = beta;
	/* d = zeros(n,1);  */
    double *d = new double[n];
	/* v = A' * u;  */
	double *v = new double[n];
	Matric_multi(0, myList, u, size_A, size_B, 1, 0, v);  
	for(index = 0; index != n; index++)
	{
		tmp = v[index];
	}
	/* alpha = norm(v);  */	
	alpha = vec_norm(v, n);	
    if (alpha != 0)
	{
        /* v = v / alpha;    */
		//double value_v;
		for(index = 0; index !=n; index++)
		{			
			v[index] = v[index] / alpha;
		}
	}
    normar = alpha * beta;	
	if(normar == 0)
	{		
		for(index = 0; index != n; index++)
			x[index] = 0;
		iter = 0;
        goto return_;		
	}	
    norma = 0;    
    sumnormd2 = 0;       
    for(i = 0; i < maxit; i++)
	{
		/* z = v;*/		
// 		itoa(i+1, buffer, 10);
// 		AfxMessageBox(buffer);
		for(index = 0; index != n; index++)
		{
			z[index] = v[index];
		}		   
        //u = A * z - alpha * u;		
		Matric_multi(1, myList, z, size_A, size_B, 1, alpha, u/*temp_u*/); ///     
		beta = vec_norm(u, m);	
	    for(index = 0; index !=m; index++)
		{
			u[index] = u[index] / beta;
		}		
		norma = norma * norma + alpha * alpha + beta * beta;
		norma = sqrt(norma);
        thet = - s * alpha;
        rhot = c * alpha;
        rho = sqrt(rhot * rhot + beta * beta);
		c = rhot / rho;
		s = -beta / rho;
		phi = c * phibar;
		phibar = s * phibar;
		/* d = (z - thet * d) / rho;  */
		double norm_d = 0;
		for(index = 0; index != n; index++)
			d[index] = (z[index] - thet * d[index]) / rho;
		for(index = 0; index != n; index++)
			norm_d += (d[index] * d[index]);
		sumnormd2 = sumnormd2 + norm_d;
		conda = norma * sqrt(sumnormd2);
		if (normar / (norma * normr) <= tol)
		{
			iter = i - 1;
			break;
		}
		if(normr <= tolb)
		{
			iter = i-1;
			break;
		}
		for(index = 0; index != n; index++)
			x[index] += (phi * d[index]);	
        normr = jueduizhi(s) * normr;
		//double *temp_v = new double[n];
		Matric_multi(0, myList, u, size_A, size_B, 1, beta, v/*temp_v*/);
        alpha = vec_norm(v, n);
		for(index = 0; index != n; index++)
		{
			v[index] = v[index] / alpha;
		}
		tmp_val = jueduizhi(s * phi);
		normar = alpha * tmp_val;
	}
	return_:
	if(v != NULL)
	{
		delete []v;
	    v = NULL;
	}
	if(u != NULL)
	{
		delete []u;
        u = NULL;
	}
	if(z != NULL)
	{
		delete []z;
	    z = NULL;
	}
    if(d != NULL)
	{
		delete []d;
		d = NULL;
	}
}

void C2DModel::Matric_multi(int mode, SeqList *myList,
				            double *b, 
							int size_A,
							int size_B,
							int c, 
							float num,
							double *rt_vec)
{	
    DataType elm;
	//mode == 1 表示计算的是A*b
	/******************************tested***********************/
	if(mode == 1)
	{
		int m = size_A;
	    //int m = size_A / size_b;
		int i, k;
		k = 0;
		float *tmp = new float[m];
		for(i = 0; i < m; i++)
			tmp[i] = 0;
		/*for(i = 0; i <  m; i++)
		{   
			//tmp = 0;
			for(j = 0; j < ListLen[m-i-1]; j++)
			{
				if(ListGet(myList, k, &elm) != 0)
				{
					k++;
					tmp[elm.i] += elm.val * b[elm.j];
				}
			}
			
            //rt_vec[index_rt] = c * tmp - num * rt_vec[index_rt];
		}*/
        for(i = 0; i < myList->size; i++)
		{
			if(ListGet(myList, k, &elm) != 0)
			{
				k++;
				tmp[elm.i] += elm.val * b[elm.j];
			}
			else
				break;
		}
		for(i = 0; i < m; i++)
			rt_vec[i] = c * tmp[i] - num * rt_vec[i];
		delete []tmp;
		tmp = NULL;
	}
	/*if(mode == 1)
	{ 
		
		int row_num = 0;
		int i = 0;
		for(int index_rt = 0; index_rt != k; index_rt++)
		{
			tmp = 0;
			for(int index_b = 0; index_b != size_b; index_b++)
			{
				tmp += A[i] * b[index_b];
				i++;
			}
			rt_vec[index_rt] = c * tmp - num * rt_vec[index_rt];
		}
		
	}*/
	/************************************************************/
	//mode == 0  计算A_t*b
	else
	{
		int i, j, k;
		int n = size_B;
		//int n = size_A / size_b;
		float *tmp = new float[n];
		for(i = 0; i < n; i++)
			tmp[i] = 0;
		
		
		k = 0;
		/*for(i = 0; i < m; i++)
		{   
			
			for(j = 0; j < ListLen[m-i-1]; j++)
			{
				if(ListGet(myList, k, &elm) != 0)
				{
					k++;
					tmp[elm.j] += elm.val * b[elm.i];
				}
			}
			
            //rt_vec[index_rt] = c * tmp - num * rt_vec[index_rt];
		}*/
	    for(i = 0; i < myList->size; i++)
		{
            if(ListGet(myList, k, &elm) != 0)
			{
				k++;
				tmp[elm.j] += elm.val * b[elm.i];
			}
			else
				break;
		}
		for(j = 0; j < n; j++)
		    rt_vec[j] = c * tmp[j] - num * rt_vec[j];
		delete []tmp;
		tmp = NULL;
  
	}
	/*else 
	{
		int col_num = 0;
		int i = 0;
		int jjj;
		int kkk = 0;
		for(int index_rt = 0; index_rt != k; index_rt++)
		{
			tmp = 0;
			for(int index_b = 0; index_b != m; index_b++)
			{
				jjj = i % m;
				tmp += A[kkk + k * jjj] * b[index_b];
				i++;
			}
			rt_vec[index_rt] = c * tmp - num * rt_vec[index_rt];
			kkk++;
		}
		
	}*/

}

double C2DModel::jueduizhi(double a)
{
	if(a > 0)
		a = a;
	else
		a = 0 - a;
	return a;
}

double C2DModel::vec_norm(double *m, int size_m)
{
	//int size_m = sizeof(m)/sizeof(m[0]);
	float norm = 0;
	for(int i = 0; i != size_m; i++)
	{
		norm += m[i] * m[i];
	}
	norm = sqrt(norm);
	return norm;
}

float PolyInt(double xa[],double ya[],int n,double x)
{
	double *c = new double[n];
	double *d = new double[n];
	int i,j,ns=0;
	double ho,hp,w,den,dy;

	double dif = fabs(x-xa[0]);	
	for (i=0;i<n;i++)
	{
		double dift = fabs(x-xa[i]);
		if (dift < dif)
		{
			dif  = dift ;
			ns = i;
		}
		c[i] = ya[i];
		d[i] = ya[i];
	}
	double y = ya[ns--];

	for (j=0;j<n-1;j++)
	{
		for (i=0;i<n-j-1;i++)
		{
			ho = xa[i] - x;
			hp = xa[i+j+1] - x;
			w = c[i+1] - d[i];
			den = ho - hp;
			if(den == 0) AfxMessageBox("Error in routine PolyInt");
			den = w/den;
			d[i] = hp*den;
			c[i] = ho*den;
		}
		y += (dy=(2*ns < (n-j-1) ? c[ns+1] : d[ns--]));
	}
	delete []c;
	delete []d;
	return y;
}

double C2DModel::Fcost(double alpha,double *grad,vector<double> v,vector<double> m_vecOFrecord)
{
		   
	double cost = 0;		        
	vector<double>   m_vecGridKVelocity((m_GridnumX-1)*m_GridnumZ);

			
	 for (int i=1;i<m_GridnumX;i++)
	 {		
			for (int j=0;j<m_GridnumZ;j++)
			{
			  m_vecGridKVelocity[(i-1)*m_GridnumZ+j]=v[(i-1)*m_GridnumZ+j]+alpha*grad[j*(m_GridnumX-1)+i-1];
			}
	 }

	for(m_ShotX=m_GridcellX; m_ShotX<m_GridnumX*m_GridcellX; m_ShotX+=m_GridcellX)
	{
		m_ShotZ=m_GridcellZ;
		
	    InitialMember();
		WaveRecord(m_ShotX, m_ShotZ,m_vecGridKVelocity,0,2);
	    for(int i=0; i<(m_GridnumX-1)*m_tnum; i++)
		{
          cost+=(m_vecOFrecord[(m_ShotX/m_GridcellX-1)*((m_GridnumX-1)*m_tnum)+i]-m_vecGridRecord[i])*(m_vecOFrecord[(m_ShotX/m_GridcellX-1)*((m_GridnumX-1)*m_tnum)+i]-m_vecGridRecord[i]);
		}
	}
			


	return cost;
}

double C2DModel::ParaFit(double x[], double y[], int n)
{
	float dc=(x[n-1]-x[0])/100; 
	float *xc = new float[100];
	float *yc = new float[100];
	double xcmin,ycmin=1e20;	 
	for (int i=0;i<100;i++) 
	{
		xc[i]=x[0]+i*dc;
		yc[i]=PolyInt(x,y,n,xc[i]); 
		if (yc[i] < ycmin) 
		{
			ycmin = yc[i];
			xcmin = xc[i];
		}
	}	
	delete []xc;
	delete []yc;
	return xcmin;
}

void C2DModel::SmoothVelocity(int len)
{
	int i,j,m,l,num;
	float sum;
	for(i=1; i<m_GridnumX; i++)
		for(j=0; j<m_GridnumZ; j++)
		{
			sum=0;
			num=0;
			
			for(m=i-len; (m<=i+len)&&(m<m_GridnumX); m++)
			{
				if(m<=0)
					continue;
				for (l=j-len; (l<=j+len)&&(l<m_GridnumZ); l++)
				{
					if (l<0)
					{
						continue;
					}
					num++;
				    sum+=m_tmpGridVelocity[(m-1)*m_GridnumZ+l];
					if (sum>10000000)
					{
						continue;
					}
				}
			}

			m_vecGridVelocity[(i-1)*m_GridnumZ+j]=sum/num;
			//////////////////////////////debug/////////////////////////////////////////
			if(m_vecGridVelocity[(i-1)*m_GridnumZ+j]<1000)
			{
				CString str1;
				str1.Format("velocity error,low: pt-%d,%d",i,j);
			//	AfxMessageBox(str1);
			}
			if(m_vecGridVelocity[(i-1)*m_GridnumZ+j]>6000)
			{
				CString str2;
				str2.Format("velocity error,high: pt-%d,%d",i,j);
				AfxMessageBox(str2);
			}
			//////////////////////////////////////////////////////////////////////////
		}
}

void FFT(complex<float> * TD, complex<float> * FD, int r)
{
	const float PI = 3.14159f;
	// 付立叶变换点数
	LONG	count;
	
	// 循环变量
	int		i,j,k;
	
	// 中间变量
	int		bfsize,p;
	
	// 角度
	float angle;
	//W为要用到的旋转因子
	//X1,X2分别为各级运算的输入和输出数据

	complex<float> *W,*X1,*X2,*X;
	
	// 计算付立叶变换点数
	count = 1 << r;
	
	// 分配运算所需存储器
	W  = new complex<float>[count / 2]; 
	X1 = new complex<float>[count];
	X2 = new complex<float>[count];
	
	// 计算加权系数
	for(i = 0; i < count / 2; i++)
	{
		angle = -i * PI * 2 / count;
//		W[i].real()=cos(angle);
//		W[i].image=sin(angle);	
		W[i]=complex<float>(cos(angle),sin(angle));
	}
	
	// 将时域点写入X1
	memcpy(X1, TD, sizeof(complex<float>) * count);
	
	// 采用蝶形算法进行快速付立叶变换
	for(k = 0; k < r; k++)  //K为当前迭代级数,也为当前级中蝶组个数
	{
		for(j = 0; j < 1 << k; j++) //j 为当前级中的蝶组序号
		{
			bfsize = 1 << (r-k);    //bfsize/2为蝶距,即每个蝶组中蝶型运算个数
			for(i = 0; i < bfsize / 2; i++)
			{
				p = j * bfsize;

	            //完成以下蝶型运算
				X2[i + p] = X1[i + p] + X1[i + p + bfsize / 2];
			    X2[i + p + bfsize / 2] = (X1[i + p] - X1[i + p + bfsize / 2]) * W[i * (1<<k)];						
			
			}
		}
		//前级输出变后级输入
		X  = X1;
		X1 = X2;
		X2 = X;
	}
	
	// 计算倒序数,重新排序
	for(j = 0; j < count; j++)
	{
		p = 0;
		for(i = 0; i < r; i++)
		{
			if (j&(1<<i))
			{
				p+=1<<(r-i-1);
			}
		}
		FD[j]=X1[p];
	}
	
	// 释放内存
	delete []W;
	delete []X1;
	delete []X2;
}

void IFFT(complex<float> * FD, complex<float> * TD, int r)
{
	// 付立叶变换点数
	LONG	count;
	
	// 循环变量
	int		i;
	
	complex<float> *X;
	
	// 计算付立叶变换点数
	count = 1 << r;
	
	// 分配运算所需存储器
	X = new complex<float>[count];
	
	// 将频域点写入X
	memcpy(X, FD, sizeof(complex<float>) * count);
	
	// 求共轭
	for(i = 0; i < count; i++)
	{
//		X[i].real=.real;
//		X[i].image=-X[i].image;
		X[i]=conj(X[i]);
	}
	
	// 调用快速付立叶变换
	FFT(X, TD, r);
	
	// 求时域点的共轭
	for(i = 0; i < count; i++)
	{
		TD[i]=conj(TD[i]/(float)count);	
		//TD[i]=conj(TD[i]);	
	}
	// 释放内存
	delete []X;
}

/////////////////////////////////////////////////////////////////////////////
// C2DModel message handlers
