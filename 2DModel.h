#if !defined(AFX_2DMODEL_H__1FE5CE63_9B15_49A4_A82E_DF54B6015C27__INCLUDED_)
#define AFX_2DMODEL_H__1FE5CE63_9B15_49A4_A82E_DF54B6015C27__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// 2DModel.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// C2DModel window

//ʹ������
#include "vector"
#include "list"
#include "iterator"
#include <complex>
#include <string>
using namespace std;

/*�����㷨���õ��Ľṹ��*/
typedef struct point{
	double lx;
	double ly;
	double lz;
} TDPOINT;

typedef struct gridpoint{
	int i;
	int j;
	int k;
} GRIDCOOR;

/*�����㷨���õ��Ľṹ��*/


struct DataType
{
	int i;
	int j;
	double val;
};

struct SeqList
{
	DataType *list;
	int size;
};

class C2DModel 
{
public:
	C2DModel();
	virtual ~C2DModel();
public:
	/*2Dģ�͵Ļ�������*/
	double	m_GridcellX;//������
	double	m_GridcellZ;
	double  m_tsearch;//����ʱ������
	double  m_tstep;  //ʱ�䲽����
	int     m_t;      //��ǰ�����ʱ��λ�ã��൱��k
	float   Sourcet;
	int     m_tnum;   //���ݴ�������t/��
	int 	m_GridnumX;
	int	    m_GridnumZ;
	int     PML;
	int     NS;

	vector<double>   m_vecGridRecord;//�����ĵ����¼
	vector<double>   m_vecGridRecord_tmp;
	vector<double>   m_vecGridRecord_t;

	vector<double>   m_Source;
	vector<double>   m_Source_t;
	vector<double>   m_Source_tmp;

	vector<double>   m_vecGridVelocity;//�������ٶ�
	vector<double>   m_tmpGridVelocity; 

	/*�����㷨���õ��Ĳ����ͺ���*/
public:
	double	m_ShotX;
	double	m_ShotZ;
	int		m_ShotGridX;
	int		m_ShotGridZ;
	
	vector<double>   m_vecGridUOrigin;// �����ǰһʱ��λ�ƺ���   ��λ��    �м����򣨲���pml��������������Ҹ���ȥһ�㣩 
	vector<double>   m_vecGridUNow;//    ������ʱ��λ�ƺ���
	vector<double>   m_vecGridUNext;//   �������һʱ��λ�ƺ���
	vector<double>   m_vecGridUA;
	vector<double>   m_vecGridUB;
	                           //PML���õ����м������4�������������ң���ÿ������Ľ���������x��z����
	vector<float>   m_vecALX;//��x�����������ϵĽ�������㣬ֻ��x����λ��
	vector<float>   m_vecALZ;//��z�����������ϵĽ�������㣬ֻ��z����λ��
	vector<float>   m_vecARX;
	vector<float>   m_vecARZ;
	vector<float>   m_vecAUX;
	vector<float>   m_vecAUZ;
	vector<float>   m_vecADX;
	vector<float>   m_vecADZ;
	                         //PML���õ���λ��ֵ��4������ÿ�����������������x��z����
	vector<float>   m_vecULX;//
	vector<float>   m_vecULZ;
	vector<float>   m_vecURX;
	vector<float>   m_vecURZ;
	vector<float>   m_vecUUX;
	vector<float>   m_vecUUZ;
	vector<float>   m_vecUDX;
	vector<float>   m_vecUDZ;

	GRIDCOOR Compoint;
	SeqList  myList;           //����C	

public:
	inline int GetVelocitystore(GRIDCOOR origin);
	inline int GetUstore(int i,int j);
    inline int GetURstore(int i,int j,int t);
	inline int GetRecordstore(int i,int t);
	inline int Get2Ustore(int i,int j);

	BOOL    InitialMember();
	BOOL    InitialSource();
	void    InitialVelocity();
	BOOL    ConstVelocity(); //����ģ��
	BOOL    IncreaseVelocity();//���ٵ���ģ��
	BOOL    ThreeLayer();//����ģ��
	BOOL    Faultage();//�ϲ�ģ��
	BOOL    Highspeed_model();//�м������ģ��
	BOOL    Highspeed_model_tmp();//�м������ģ��
	BOOL    Step_model();
	BOOL    Step_model_tmp();
    
    void    RecordInitial();//���ݳ�ʼ��
	void    WaveRecord(double m_ShotX, double m_ShotZ,vector<double> m_Vel,int s,int a);//������
	void    UpdateNextU(vector<double> m_Vel,int s);//�����ʱ���¸�������Uֵ
	void    UpdateNextUH(vector<double> m_Vel,int s);
	void    ShiftUstore();//������ʱ�̵��������λ������Ӧ��һʱ�̼���
	double  SourceG(int t);//��Դ����
	double  SourceGT(int t);//�в��
	void    Filter(vector<double> &S,double freq,int t);

	double  U1(double a1, double a2, double x1, double x2, double z1, double z2, double s);//�ǽ��յ�λ�ƺ������㹫ʽ
	double  U0(double a1, double a2, double x1, double x2, double z1, double src, double s);//���յ�λ�ƺ������㹫ʽ
	double  U2(double a1, double a2, double x1, double x2, double x3, double x4, double z1, double z2, double z3, double z4, double v);
	double UPMLabsorb(double a1, double p1, double p2, double alpha, double h);

    BOOL    SaveVelocity(CString str, int n, vector<double> vec, int width, int depth);
	void	OpenVelocity(CString VelName);
	BOOL    CreateRecordmat();
	BOOL    Create_testmat(string s,int i);
	BOOL    CreateRecordmat(int n);
	BOOL    Createvelocitymat(int iter);

	/*�����㷨���õ��Ĳ����ͺ���*/
public:
	double  *deta_V;     //�ٶ�����ֵ
	vector<double>  m_vecFrecord;    //������ʵ������¼
	vector<double>  m_vecArecord;    //�����Ĺ۲�����¼
	vector<double>  m_vecdeta_record;//ʵ��͹۲�����¼��

    vector<double>  m_AvecArecord;
	vector<double>  m_AvecArecord0;
	vector<double>  m_AvecArecord1;
	vector<double>  m_AvecArecordY;
    vector<double>  m_DAvecArecord;

	double  *B1;         //������һ��
	double  *B2;         //�����ڶ���
	double  *B3;         //����������
	double  *b;          //b
	double  ** ADerivative;    //�۲�����¼����v�ĵ���
	double  ** ADerivative_T;  //�۲�����¼����v�ĵ�����ת��
//	double  ** U;// Ϊ��ʹ���µ��ݶȷ�����������ʱ�䵼���ĳ˻��ı���
	int     ** M1;             //�⻬����1
	int     ** M2;             //�⻬����2
	double  ** C1;             //ϵ������ĵ�һ��
	double  ** C2;             //ϵ������ĵڶ���
	double  ** C3;             //ϵ������ĵ�����


public:
	BOOL LSQR(int iterations);
	BOOL Test(int iterations);
	void LSQRInitial();
	void SmoothVelocity(int len);
	void umfpack(double *rhs,SeqList *myList, double *sol,int Lside,int Rside);


	void Matric_multi_Matric(double **matric,int size_A,int size_B,double **c,float alpha);//�������ת���������
	void Matric_multi_Matric(int **matric,int size_A,int size_B, double **c,float alpha);//����Ϊ����
	void Matric_multi_Vector(double **matric,vector<double> vec,int size_A,int size_B,double *b);//����������������

	/*����LSQR�㷨�õ��ĺ���*/
	void ListInitiate(SeqList *L, int maxSize);
	
	int ListInsert(SeqList *L, int i, DataType x);
	int ListGet(SeqList *L, int i, DataType *x);
    void ListDelete(SeqList *L);
	double Fcost(double alpha,double *grad,vector<double> v,vector<double> m_vecOFrecord);
	double ParaFit(double x[], double y[], int n);
	/*****************************************************
	*         function:    solve A * x = b;	
	****************************************************/	
	void lsqr_test(double *x, SeqList *myList, double *b, int size_A, 
		int size_b, double tol, int maxit);
	//typedef vector<double> DVEC;
	//int lsqr_test(DVEC& x, DVEC &A, DVEC &b, double tol, int maxit);	
	/*****************************************************
	*        function:   if mode = 1,solve A * b
	*                       if mode = 0, solve  A'* b;
	*******************************************************/	
	void Matric_multi(int mode, SeqList *myList,
		double *b, 
		int size_A,
		int size_b,
		int c, 
		float num,
		double *rt_vec);
		/*int Matric_Multi(int mode, DVEC& A, 
		DVEC& b, int size_A,
	int size_b, DVEC& rt_vec);*/
	/**************************************************
	*        function :  calculate the abs of a
	*        
	***************************************************/
	double jueduizhi(double a);	
	/***************************************************
	**��������������vec_norm������������ģ
	**              input��   vector<double> m;
	*************************************************/	
    double vec_norm(double *m, int size_m);

	/*�ݶȷ����õ��ı����ͺ���*/
};

	void FFT(complex<float> * TD, complex<float> * FD, int r);
	void IFFT(complex<float> * FD, complex<float> * TD, int r);
#endif //!defined(AFX_2DMODEL_H__1FE5CE63_9B15_49A4_A82E_DF54B6015C27__INCLUDED_)


