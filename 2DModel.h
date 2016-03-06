#if !defined(AFX_2DMODEL_H__1FE5CE63_9B15_49A4_A82E_DF54B6015C27__INCLUDED_)
#define AFX_2DMODEL_H__1FE5CE63_9B15_49A4_A82E_DF54B6015C27__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// 2DModel.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// C2DModel window

//使用容器
#include "vector"
#include "list"
#include "iterator"
#include <complex>
#include <string>
using namespace std;

/*正演算法中用到的结构体*/
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

/*反演算法中用到的结构体*/


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
	/*2D模型的基本参数*/
	double	m_GridcellX;//网格间隔
	double	m_GridcellZ;
	double  m_tsearch;//正演时间区域
	double  m_tstep;  //时间步长τ
	int     m_t;      //当前计算的时间位置，相当于k
	float   Sourcet;
	int     m_tnum;   //正演次数，即t/τ
	int 	m_GridnumX;
	int	    m_GridnumZ;
	int     PML;
	int     NS;

	vector<double>   m_vecGridRecord;//网格点的地震记录
	vector<double>   m_vecGridRecord_tmp;
	vector<double>   m_vecGridRecord_t;

	vector<double>   m_Source;
	vector<double>   m_Source_t;
	vector<double>   m_Source_tmp;

	vector<double>   m_vecGridVelocity;//网格点的速度
	vector<double>   m_tmpGridVelocity; 

	/*正演算法中用到的参数和函数*/
public:
	double	m_ShotX;
	double	m_ShotZ;
	int		m_ShotGridX;
	int		m_ShotGridZ;
	
	vector<double>   m_vecGridUOrigin;// 网格点前一时刻位移函数   总位移    中间区域（不加pml层的网格上下左右各减去一层） 
	vector<double>   m_vecGridUNow;//    网格点该时刻位移函数
	vector<double>   m_vecGridUNext;//   网格点下一时刻位移函数
	vector<double>   m_vecGridUA;
	vector<double>   m_vecGridUB;
	                           //PML层用到的中间变量，4个区域（上下左右），每个区域的交错网格点的x和z分量
	vector<float>   m_vecALX;//在x方向网格线上的交错网格点，只有x方向位移
	vector<float>   m_vecALZ;//在z方向网格线上的交错网格点，只有z方向位移
	vector<float>   m_vecARX;
	vector<float>   m_vecARZ;
	vector<float>   m_vecAUX;
	vector<float>   m_vecAUZ;
	vector<float>   m_vecADX;
	vector<float>   m_vecADZ;
	                         //PML层用到的位移值，4个区域，每个区域的正常网格点的x和z分量
	vector<float>   m_vecULX;//
	vector<float>   m_vecULZ;
	vector<float>   m_vecURX;
	vector<float>   m_vecURZ;
	vector<float>   m_vecUUX;
	vector<float>   m_vecUUZ;
	vector<float>   m_vecUDX;
	vector<float>   m_vecUDZ;

	GRIDCOOR Compoint;
	SeqList  myList;           //矩阵C	

public:
	inline int GetVelocitystore(GRIDCOOR origin);
	inline int GetUstore(int i,int j);
    inline int GetURstore(int i,int j,int t);
	inline int GetRecordstore(int i,int t);
	inline int Get2Ustore(int i,int j);

	BOOL    InitialMember();
	BOOL    InitialSource();
	void    InitialVelocity();
	BOOL    ConstVelocity(); //匀速模型
	BOOL    IncreaseVelocity();//匀速递增模型
	BOOL    ThreeLayer();//三层模型
	BOOL    Faultage();//断层模型
	BOOL    Highspeed_model();//中间高速体模型
	BOOL    Highspeed_model_tmp();//中间高速体模型
	BOOL    Step_model();
	BOOL    Step_model_tmp();
    
    void    RecordInitial();//正演初始化
	void    WaveRecord(double m_ShotX, double m_ShotZ,vector<double> m_Vel,int s,int a);//主程序
	void    UpdateNextU(vector<double> m_Vel,int s);//计算该时刻下各网格点的U值
	void    UpdateNextUH(vector<double> m_Vel,int s);
	void    ShiftUstore();//将三个时刻的网格点移位，以适应下一时刻计算
	double  SourceG(int t);//震源函数
	double  SourceGT(int t);//残差函数
	void    Filter(vector<double> &S,double freq,int t);

	double  U1(double a1, double a2, double x1, double x2, double z1, double z2, double s);//非接收点位移函数计算公式
	double  U0(double a1, double a2, double x1, double x2, double z1, double src, double s);//接收点位移函数计算公式
	double  U2(double a1, double a2, double x1, double x2, double x3, double x4, double z1, double z2, double z3, double z4, double v);
	double UPMLabsorb(double a1, double p1, double p2, double alpha, double h);

    BOOL    SaveVelocity(CString str, int n, vector<double> vec, int width, int depth);
	void	OpenVelocity(CString VelName);
	BOOL    CreateRecordmat();
	BOOL    Create_testmat(string s,int i);
	BOOL    CreateRecordmat(int n);
	BOOL    Createvelocitymat(int iter);

	/*反演算法中用到的参数和函数*/
public:
	double  *deta_V;     //速度修正值
	vector<double>  m_vecFrecord;    //网格点的实测地震记录
	vector<double>  m_vecArecord;    //网格点的观测地震记录
	vector<double>  m_vecdeta_record;//实测和观测地震记录差

    vector<double>  m_AvecArecord;
	vector<double>  m_AvecArecord0;
	vector<double>  m_AvecArecord1;
	vector<double>  m_AvecArecordY;
    vector<double>  m_DAvecArecord;

	double  *B1;         //向量第一项
	double  *B2;         //向量第二项
	double  *B3;         //向量第三项
	double  *b;          //b
	double  ** ADerivative;    //观测地震记录关于v的导数
	double  ** ADerivative_T;  //观测地震记录关于v的导数的转置
//	double  ** U;// 为了使用新的梯度法，保存两个时间导数的乘积的变量
	int     ** M1;             //光滑矩阵1
	int     ** M2;             //光滑矩阵2
	double  ** C1;             //系数矩阵的第一项
	double  ** C2;             //系数矩阵的第二项
	double  ** C3;             //系数矩阵的第三项


public:
	BOOL LSQR(int iterations);
	BOOL Test(int iterations);
	void LSQRInitial();
	void SmoothVelocity(int len);
	void umfpack(double *rhs,SeqList *myList, double *sol,int Lside,int Rside);


	void Matric_multi_Matric(double **matric,int size_A,int size_B,double **c,float alpha);//计算矩阵转置与矩阵点乘
	void Matric_multi_Matric(int **matric,int size_A,int size_B, double **c,float alpha);//矩阵为整型
	void Matric_multi_Vector(double **matric,vector<double> vec,int size_A,int size_B,double *b);//计算矩阵与向量点乘

	/*反演LSQR算法用到的函数*/
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
	**　　　　　　　vec_norm用于求向量的模
	**              input：   vector<double> m;
	*************************************************/	
    double vec_norm(double *m, int size_m);

	/*梯度法中用到的变量和函数*/
};

	void FFT(complex<float> * TD, complex<float> * FD, int r);
	void IFFT(complex<float> * FD, complex<float> * TD, int r);
#endif //!defined(AFX_2DMODEL_H__1FE5CE63_9B15_49A4_A82E_DF54B6015C27__INCLUDED_)


