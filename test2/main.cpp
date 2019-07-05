#include <iostream>
#include <fstream>
#include <iomanip>
#include "EsModelInterToolDll.h"
#include "math.h"

//#define SGSIM
//#define KT3D
//#define SISIM
#define IK3D
//#define ESTIMATE
//#define BLOCK

using namespace gridworld;
using namespace std;

//四舍五入
int round_double(double number)
{
	return (number > 0.0) ? (number + 0.5) : (number - 0.5);
}

//字符串分割
void split(const string source, vector<string>& dest)
{
	const char *szDelim = " \t,;\n";
	char *context;
	char *next_token = NULL;
	context = strtok_s((char *)source.c_str(), szDelim, &next_token);
	while (context)
	{
		dest.push_back(context);
		context = strtok_s(NULL, szDelim, &next_token);
	}
}

//将连续属性XYZ转化成IJK
void TransXYZtoIJK(const string& strXYZFile, const GridInfo& gi, vector<Cell>& vecCell)
{
	ifstream fin(strXYZFile, ios::in);
	if (!fin.is_open())
		return;

	string strLine;
	vector<string> vecStrData;

	for (int i = 0; i < 7; i++)
		getline(fin, strLine);

	vecCell.clear();
	while (!fin.eof())
	{
		vecStrData.clear();
		getline(fin, strLine);
		split(strLine, vecStrData);
		if (vecStrData.size() != 5)
			continue;

		double x = atof(vecStrData[0].c_str());
		double y = atof(vecStrData[1].c_str());
		double z = atof(vecStrData[2].c_str());
		double v = atof(vecStrData[3].c_str());

		int i = round_double((x - gi.dXOri) / gi.dXStep + gi.nIOri);
		int j = round_double((y - gi.dYOri) / gi.dYStep + gi.nJOri);
		int k = round_double((z - gi.dZOri) / gi.dZStep + gi.nKOri);

		Cell c(i, j, k, v);
		vecCell.push_back(c);
	}

	fin.close();
}

//将离散属性XYZ转化成IJK
void TransXYZtoIJKDiscrete(const string& strXYZFile, const GridInfo& gi, vector<Cell>& vecCell)
{
	ifstream fin(strXYZFile, ios::in);
	if (!fin.is_open())
		return;

	string strLine;
	vector<string> vecStrData;

	for (int i = 0; i < 7; i++)
		getline(fin, strLine);

	vecCell.clear();
	while (!fin.eof())
	{
		vecStrData.clear();
		getline(fin, strLine);
		split(strLine, vecStrData);
		if (vecStrData.size() != 5)
			continue;

		double x = atof(vecStrData[0].c_str());
		double y = atof(vecStrData[1].c_str());
		double z = atof(vecStrData[2].c_str());
		int v = atoi(vecStrData[3].c_str());

		int i = round_double((x - gi.dXOri) / gi.dXStep + gi.nIOri);
		int j = round_double((y - gi.dYOri) / gi.dYStep + gi.nJOri);
		int k = round_double((z - gi.dZOri) / gi.dZStep + gi.nKOri);

		Cell c(i, j, k, v);
		vecCell.push_back(c);
	}

	fin.close();
}


/**********连续属性序贯高斯模拟插值计算接口和插值结果提取接口调用Demo**********/
#ifdef SGSIM
void main()
{
	//准备OilWater属性插值的参数
	GridInfo gridInfo(45, 49, 1, 99.547226, 99.812538, 2.304501, 676067.647571, 5085799.081054, 1.152251, 0, 0, 0); //构建待插网格
	VariogramPara varPara(Exponential, 1, 0.0001, 2000, 1800, 8, 0, 90); //构建待插网格的变差函数
	string strXYZFile = "E:\\001_GridWorld\\004_TestData\\model_for_test\\SDK测试模型_bachang\\测试数据\\连续序贯高斯\\oilwater_xyz.dat"; //XYZ格式样本数据，用来转化成IJK进行测试

	ModelInterPara tmip; //定义插值参数结构体
	tmip.strWorkPath = "E:\\001_GridWorld\\004_TestData\\model_for_test\\SDK测试模型_bachang\\测试数据\\连续序贯高斯\\"; //设置工作目录（目录需存在）
	tmip.nAttrType = Continuous; //属性为连续
	tmip.gi = gridInfo; //定义待插网格
	TransXYZtoIJK(strXYZFile, gridInfo, tmip.vecCell); //定义IJK样本

	tmip.nInterMethod = SequentialGaussianSimulation; //定义插值算法为序贯高斯
	tmip.distPara.bIsTrans = false; //插值前不进行正态变换
	tmip.expertPara.nSeed = 30367; //设置随机模拟种子(与深探一致)
	tmip.varPara = varPara;	//定义变差函数

	//调用SDK进行插值
	if (!EsInterForModel(tmip))
		return;

	//使用IJK提取插值结果，这里以文本方式输出便于查看
	string strSaveFile = "E:\\001_GridWorld\\004_TestData\\model_for_test\\SDK测试模型_bachang\\测试数据\\连续序贯高斯\\oilwater_ijk.dat";
	ofstream fout(strSaveFile, ios::out);
	if(!fout.is_open())
		return;
	
	for(int k=0; k<tmip.gi.nZNum; k++)
	{
		for(int j=0; j<tmip.gi.nYNum; j++)
		{
			for(int i=0; i<tmip.gi.nXNum; i++)
			{
				double v = EsGetContinuousPropertyByIJK(tmip,i,j,k);
				fout << setw(4) << i << "\t" << setw(4) << j << "\t" << setw(4) << k << "\t" << setw(8) << fixed << setprecision(4) << v << endl;
			}
		}
	}
	
	fout.close();
}
#endif

/**********连续属性三维克里金插值计算接口和插值结果提取接口调用Demo**********/
#ifdef KT3D
void main()
{	
	//准备OilWater属性插值的参数
	GridInfo gridInfo(45, 49, 1, 99.547226, 99.812538, 2.304501, 676067.647571, 5085799.081054, 1.152251, 0, 0, 0); //构建待插网格
	VariogramPara varPara(Exponential, 1, 0.0001, 2000, 1800, 8, 0, 90); //构建待插网格的变差函数
	string strXYZFile = "E:\\001_GridWorld\\004_TestData\\model_for_test\\SDK测试模型_bachang\\测试数据\\连续克里金\\oilwater_xyz.dat"; //XYZ格式样本数据，用来转化成IJK进行测试

	ModelInterPara tmip; //定义插值参数结构体
	tmip.strWorkPath = "E:\\001_GridWorld\\004_TestData\\model_for_test\\SDK测试模型_bachang\\测试数据\\连续克里金\\"; //设置工作目录（目录需存在）
	tmip.nAttrType = Continuous; //属性为连续
	tmip.gi = gridInfo; //定义待插网格
	TransXYZtoIJK(strXYZFile, gridInfo, tmip.vecCell); //定义IJK样本

	tmip.nInterMethod = Kriging; //定义插值算法为三维克里金
	tmip.varPara= varPara;	//定义变差函数

	//调用SDK进行插值
	if(!EsInterForModel(tmip))
		return;

	//使用IJK提取插值结果，这里以文本方式输出便于查看
	string strSaveFile = "E:\\001_GridWorld\\004_TestData\\model_for_test\\SDK测试模型_bachang\\测试数据\\连续克里金\\oilwater_ijk.dat";
	ofstream fout(strSaveFile, ios::out);
	if(!fout.is_open())
		return;

	for(int k=0; k<tmip.gi.nZNum; k++)
	{
		for(int j=0; j<tmip.gi.nYNum; j++)
		{
			for(int i=0; i<tmip.gi.nXNum; i++)
			{
				double v = EsGetContinuousPropertyByIJK(tmip,i,j,k);
				fout << setw(4) << i << "\t" << setw(4) << j << "\t" << setw(4) << k << "\t" << setw(8) << fixed << setprecision(4) << v << endl;
			}
		}
	}

	fout.close();
}
#endif

/**********离散属性指示克里金插值计算接口和插值结果提取接口调用Demo**********/
#ifdef IK3D
void main()
{	
	//准备OilWater属性插值的参数
	GridInfo gridInfo(45, 49, 1, 99.547226, 99.812538, 2.304501, 676067.647571, 5085799.081054, 1.152251, 0, 0, 0); //构建待插网格
	VariogramPara varPara(Exponential, 1, 0.0001, 2000, 1800, 8, 0, 90); //构建待插网格的变差函数
	string strXYZFile = "E:\\001_GridWorld\\004_TestData\\model_for_test\\SDK测试模型_bachang\\测试数据\\离散指示克里金\\ylsy_xyz.dat"; //XYZ格式样本数据，用来转化成IJK进行测试

	ModelInterPara tmip; //定义插值参数结构体
	tmip.strWorkPath = "E:\\001_GridWorld\\004_TestData\\model_for_test\\SDK测试模型_bachang\\测试数据\\离散指示克里金\\"; //设置工作目录（目录需存在）
	tmip.nAttrType = Discrete; //属性为连续
	tmip.gi = gridInfo; //定义待插网格
	TransXYZtoIJKDiscrete(strXYZFile, gridInfo, tmip.vecCell); //定义IJK样本

	tmip.nInterMethod = IndicatorKriging; //定义插值算法为指示克里金

	//设置离散属性变差函数
	tmip.faciesPara.resize(2);	
	tmip.faciesPara[0].nCode = 1;
	tmip.faciesPara[0].varPara = varPara;
	tmip.faciesPara[1].nCode = 3;
	tmip.faciesPara[1].varPara = varPara;


	//调用SDK进行插值
	if(!EsInterForModel(tmip))
		return;

	//使用IJK提取插值结果，这里以文本方式输出便于查看
	string strSaveFile = "E:\\001_GridWorld\\004_TestData\\model_for_test\\SDK测试模型_bachang\\测试数据\\离散指示克里金\\ylsy_ijk.dat";
	ofstream fout(strSaveFile, ios::out);
	if(!fout.is_open())
		return;

	for(int k=0; k<tmip.gi.nZNum; k++)
	{
		for(int j=0; j<tmip.gi.nYNum; j++)
		{
			for(int i=0; i<tmip.gi.nXNum; i++)
			{
				int v = EsGetDiscretePropertyByIJK(tmip,i,j,k);
				fout << setw(4) << i << "\t" << setw(4) << j << "\t" << setw(4) << k << "\t" << setw(8) << fixed << setprecision(4) << v << endl;
			}
		}
	}

	fout.close();
}
#endif

/**********离散属性序贯指示模拟插值计算接口和插值结果提取接口调用Demo**********/
#ifdef SISIM
void main()
{	
	//准备OilWater属性插值的参数
	GridInfo gridInfo(45, 49, 1, 99.547226, 99.812538, 2.304501, 676067.647571, 5085799.081054, 1.152251, 0, 0, 0); //构建待插网格
	VariogramPara varPara(Exponential, 1, 0.0001, 2000, 1800, 8, 0, 90); //构建待插网格的变差函数
	string strXYZFile = "E:\\001_GridWorld\\004_TestData\\model_for_test\\SDK测试模型_bachang\\测试数据\\离散序贯指示\\ylsy_xyz.dat"; //XYZ格式样本数据，用来转化成IJK进行测试

	ModelInterPara tmip; //定义插值参数结构体
	tmip.strWorkPath = "E:\\001_GridWorld\\004_TestData\\model_for_test\\SDK测试模型_bachang\\测试数据\\离散序贯指示\\"; //设置工作目录（目录需存在）
	tmip.nAttrType = Discrete; //属性为连续
	tmip.gi = gridInfo; //定义待插网格
	TransXYZtoIJKDiscrete(strXYZFile, gridInfo, tmip.vecCell); //定义IJK样本

	tmip.nInterMethod = SequentialIndicatorSimulation; //定义插值算法为序贯指示模拟
	tmip.expertPara.nSeed = 21697; //设置随机模拟种子(与深探一致)

	//设置离散属性变差函数
	tmip.faciesPara.resize(2);	
	tmip.faciesPara[0].nCode = 1;
	tmip.faciesPara[0].varPara = varPara;
	tmip.faciesPara[1].nCode = 3;
	tmip.faciesPara[1].varPara = varPara;


	//调用SDK进行插值
	if(!EsInterForModel(tmip))
		return;

	//使用IJK提取插值结果，这里以文本方式输出便于查看
	string strSaveFile = "E:\\001_GridWorld\\004_TestData\\model_for_test\\SDK测试模型_bachang\\测试数据\\离散序贯指示\\ylsy_ijk.dat";
	ofstream fout(strSaveFile, ios::out);
	if(!fout.is_open())
		return;

	for(int k=0; k<tmip.gi.nZNum; k++)
	{
		for(int j=0; j<tmip.gi.nYNum; j++)
		{
			for(int i=0; i<tmip.gi.nXNum; i++)
			{
				int v = EsGetDiscretePropertyByIJK(tmip,i,j,k);
				fout << setw(4) << i << "\t" << setw(4) << j << "\t" << setw(4) << k << "\t" << setw(8) << fixed << setprecision(4) << v << endl;
			}
		}
	}

	fout.close();
}
#endif

/**********范围评估接口调用Demo**********/
#ifdef ESTIMATE
void main()
{	
	GridInfo gridInfo(45, 49, 1, 99.547226, 99.812538, 2.304501, 676067.647571, 5085799.081054, 1.152251, 0, 0, 0); //构建待插网格
	string strXYZFile = "E:\\001_GridWorld\\004_TestData\\model_for_test\\SDK测试模型_bachang\\测试数据\\范围评估\\ylsy_xyz.dat"; //XYZ格式样本数据，用来转化成IJK进行测试

	vector<Cell> vecUpdateData;
	TransXYZtoIJKDiscrete(strXYZFile, gridInfo, vecUpdateData); //得到IJK样本

	int rangeI = 1; //计算I方向搜索步长 = I方向实际搜索范围/I方向网格步长
	int rangeJ = 1; //计算J方向搜索步长 = J方向实际搜索范围/J方向网格步长
	int rangeK = 1; //计算K方向搜索步长 = K方向实际搜索范围/K方向网格步长

	vector<GridInfo> vecGIforSampleDownload; //样本下载范围集
	vector<GridInfo> vecGIforUpdate; //待插范围集
	EsEvaluateAreaForUpdate(vecUpdateData, rangeI, rangeJ, rangeK, gridInfo, vecGIforSampleDownload, vecGIforUpdate); //调用接口得到待插范围

	if(vecGIforSampleDownload.size() != vecGIforUpdate.size())
		return;
	
#ifdef BLOCK
	string strAreaFile = "E:\\001_GridWorld\\004_TestData\\model_for_test\\SDK测试模型_bachang\\测试数据\\范围评估\\area_block.dat"; //插值范围信息
#else
	string strAreaFile = "E:\\001_GridWorld\\004_TestData\\model_for_test\\SDK测试模型_bachang\\测试数据\\范围评估\\area.dat"; //插值范围信息
#endif

	ofstream fout(strAreaFile, ios::out);
	if(!fout.is_open())
		return;
	
	for(size_t i=0; i<vecGIforUpdate.size(); i++)
	{
		fout << "待插范围：" << "\t" << "起始I:" << vecGIforUpdate[i].nIOri << "\t" << "起始J:" << vecGIforUpdate[i].nJOri << "\t" << "起始K:" << vecGIforUpdate[i].nKOri << "\t"
			<< "I方向个数:" << vecGIforUpdate[i].nXNum << "\t" << "J方向个数:" << vecGIforUpdate[i].nYNum << "\t" << "K方向个数:" << vecGIforUpdate[i].nZNum << "\n";

		fout << "样本范围：" << "\t" << "起始I:" << vecGIforSampleDownload[i].nIOri << "\t" << "起始J:" << vecGIforSampleDownload[i].nJOri << "\t" << "起始K:" << vecGIforSampleDownload[i].nKOri << "\t"
			<< "I方向个数:" << vecGIforSampleDownload[i].nXNum << "\t" << "J方向个数:" << vecGIforSampleDownload[i].nYNum << "\t" << "K方向个数:" << vecGIforSampleDownload[i].nZNum << "\n";

		fout << "\n";
	}
	fout.close();
}
#endif