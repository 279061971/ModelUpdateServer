#include <fstream>
#include <iomanip>
#include <algorithm>
#include "windows.h"
#include "EsInterpolation.h"
#include "EsModelInterCoreDll.h"

using std::ifstream;
using std::ofstream;
using std::ios;
using std::endl;
using std::fixed;
using std::setprecision;


bool gridworld::EsInterForModel(ModelInterPara& tmip)
{
	//初始化工作目录及过程文件
	tmip.strWorkPath = tmip.strWorkPath + "//TEMP";
	CreateDirectoryA(tmip.strWorkPath.c_str(), NULL);

	string strInterDataFile = ""; //插值样本数据文件
	string strInterParaFile = ""; //插值参数文件
	string strResultFile = ""; //插值结果文件

	//生成插值样本数据文件
	if(!EsInterDataFilePrep(tmip, strInterDataFile))
	{
		EsClearDir(tmip.strWorkPath);
		return false;
	}

	//生成插值参数文件
	if(!EsInterParaFilePrep(tmip, strInterDataFile, strInterParaFile, strResultFile))
	{
		EsClearDir(tmip.strWorkPath);
		return false;
	}

	//插值计算
	if(!EsInterpolation(tmip, strInterParaFile, strResultFile))
	{
		EsClearDir(tmip.strWorkPath);
		return false;
	}

#ifndef _DEBUG
	EsClearDir(tmip.strWorkPath);
#endif
	return true;
}

bool gridworld::EsInterDataFilePrep(ModelInterPara& tmip, string& strInterDataFile)
{
	strInterDataFile = tmip.strWorkPath + "//sample.dat";
	ofstream fout(strInterDataFile, ios::out);
	if(!fout.is_open())
		return false;

	fout << "Clustered " << tmip.vecCell.size() << " primary data\n";
	fout << "5\nXLocation\nYLocation\nZLocation\nPrimary\nIndex\n";

	tmip.gi.dZOri = tmip.gi.dZStep / 2.0;
	for(auto cell : tmip.vecCell)
	{
		double x = tmip.gi.dXOri + (cell.nI - tmip.gi.nIOri)*tmip.gi.dXStep;
		double y = tmip.gi.dYOri + (cell.nJ - tmip.gi.nJOri)*tmip.gi.dYStep;
		double z = tmip.gi.dZOri + (cell.nK - tmip.gi.nKOri)*tmip.gi.dZStep;

		if (tmip.nAttrType == Discrete)
		{
			fout << fixed << setprecision(2) << x << "\t" << fixed << setprecision(2) << y << "\t" 
				<< fixed << setprecision(2) << z << "\t" << cell.nValue << "\t" << cell.nK  << "\n";
		}
		else if (tmip.nAttrType == Continuous)
		{
			fout << fixed << setprecision(2) << x << "\t" << fixed << setprecision(2) << y << "\t" 
				<< fixed << setprecision(2) << z << "\t"<< fixed << setprecision(4) << cell.dValue << "\t" << cell.nK  << "\n";
		}		
	}

	fout.close();

	return true;
}

bool gridworld::EsInterParaFilePrep(ModelInterPara& tmip, const string& strInterDataFile, string& strInterParaFile, string& strResultFile)
{
	if(!EsCheckInterMethod(tmip.nAttrType, tmip.nInterMethod))
		return false;

	switch(tmip.nInterMethod)
	{
	case SequentialGaussianSimulation:
		EsPrepSgsimParaFile(tmip, strInterDataFile, strInterParaFile, strResultFile);
		break;
	case Kriging:
		EsPrepKt3dParaFile(tmip, strInterDataFile, strInterParaFile, strResultFile);
		break;
	case SequentialIndicatorSimulation:
		EsPrepSisimParaFile(tmip, strInterDataFile, strInterParaFile, strResultFile);
		break;
	case IndicatorKriging:
		EsPrepIk3dParaFile(tmip, strInterDataFile, strInterParaFile, strResultFile);
		break;
	default:
		return false;
	}
	return true;
}

bool gridworld::EsInterpolation(ModelInterPara& tmip, const string& strInterParaFile, const string& strResultFile)
{
	if(!EsInterFunc(strInterParaFile, tmip.nInterMethod))
		return false;

	if(tmip.nInterMethod == SequentialGaussianSimulation && tmip.distPara.bIsTrans == true)
	{
		string strRevertResultFile = strResultFile + ".trans";
		if(!EsRevertNormalScore(strRevertResultFile, strResultFile, tmip.strWorkPath, tmip.distPara.dOutputMin, tmip.distPara.dOutputMax))
			return false;
	}

	if(tmip.nInterMethod == IndicatorKriging)
	{
		if(!EsTransformIk3dResult(strResultFile))
			return false;
	}

	if(!EsResample(tmip, strResultFile))
		return false;

	return true;
}

int gridworld::EsGetDiscretePropertyByIJK(ModelInterPara& tmip, const int i, const int j, const int k)
{
	if(tmip.vecDiscreteProp.empty())
		return -1;

	if(tmip.vecDiscreteProp.size() != tmip.gi.nXNum*tmip.gi.nYNum*tmip.gi.nZNum) //属性值个数与网格数量要一致
		return -1;

	if ((i < tmip.gi.nIOri) || (i > (tmip.gi.nIOri + tmip.gi.nXNum - 1)) ||
		(j < tmip.gi.nJOri) || (j > (tmip.gi.nJOri + tmip.gi.nYNum - 1)) ||
		(k < tmip.gi.nKOri) || (k > (tmip.gi.nKOri + tmip.gi.nZNum - 1)))
		return -1;

	int nPos = (k - tmip.gi.nKOri)*tmip.gi.nYNum*tmip.gi.nXNum + (j-tmip.gi.nJOri)*tmip.gi.nXNum + (i - tmip.gi.nIOri); //先换算成局部IJK，在计算Position
	return tmip.vecDiscreteProp[nPos];
}

double gridworld::EsGetContinuousPropertyByIJK(ModelInterPara& tmip, const int i, const int j, const int k)
{
	if(tmip.vecContinuousProp.empty())
		return -1;

	if(tmip.vecContinuousProp.size() != tmip.gi.nXNum*tmip.gi.nYNum*tmip.gi.nZNum) //属性值个数与网格数量要一致
		return -1;

	if ((i < tmip.gi.nIOri) || (i > (tmip.gi.nIOri + tmip.gi.nXNum - 1)) ||
		(j < tmip.gi.nJOri) || (j > (tmip.gi.nJOri + tmip.gi.nYNum - 1)) ||
		(k < tmip.gi.nKOri) || (k > (tmip.gi.nKOri + tmip.gi.nZNum - 1)))
		return -1;

	int nPos = (k - tmip.gi.nKOri)*tmip.gi.nYNum*tmip.gi.nXNum + (j-tmip.gi.nJOri)*tmip.gi.nXNum + (i - tmip.gi.nIOri); //先换算成局部IJK，在计算Position
	return tmip.vecContinuousProp[nPos];
}

bool gridworld::EsCheckInterMethod(const AttrType nAttrType, const InterMethod nInterMethod)
{
	if(nAttrType == Continuous)
	{
		if(nInterMethod == SequentialGaussianSimulation || nInterMethod == Kriging)
			return true;
		else
			return false;
	}
	else if(nAttrType == Discrete)
	{
		if(nInterMethod == SequentialIndicatorSimulation || nInterMethod == IndicatorKriging)
			return true;
		else
			return false;
	}
	else
	{
		return false;
	}
}

bool gridworld::EsPrepSgsimParaFile(ModelInterPara& tmip, const string& strInterDataFile, string& strInterParaFile, string& strResultFile)
{
	strInterParaFile = tmip.strWorkPath + "//sgsim.par";
	ofstream fout(strInterParaFile, ios::out);
	if (!fout.is_open())
		return false;

	fout << "\t\t\tParameters for SGSIM\n";
	fout << "\t\t\t********************\n";
	fout << "\n";
	fout << "START OF PARAMETERS:\n";

	EsGetSampleDataRange(strInterDataFile, tmip.distPara.dOutputMin, tmip.distPara.dOutputMax); //计算样本数据的最值
	// 插值需要的原始数据（这里指的是粗化后经过处理或经过数据分析后符合gslib库的数据文件）文件路径
	string strSamplePath =  tmip.strWorkPath + "//sample.trans";
	if (tmip.distPara.bIsTrans)
	{
		// 如果使用数据分析结果则直接调用数据分析的变换函数进行变换，并将则结果文件作为样本数据
		if (!EsNormalScore(strSamplePath, strInterDataFile, tmip.strWorkPath, tmip.distPara.dOutputMin, tmip.distPara.dOutputMax)) // 变换失败则使用原始数据文件
		{
			//strSamplePath = strInterDataFile;
			return false;
		}		
	}
	else
	{
		//如果使用粗化数据，则根据参数设置生成样本数据文件
		strSamplePath = strInterDataFile;
	}

	// 数据列信息		
	// 不使用第二变量，按照内部数据格式给定列信息:1 2 3 4 0 0
	fout << strSamplePath << "\n";
	fout << 1 << "\t" << 2 << "\t" << 3 << "\t" << 4 << "\t" << 0 << "\t" << 0
		<< "\t-columns for X,Y,Z,vr,wt,sec.var\n";

	fout << -1.0e21 << "\t" << 1.0e21 << "\t-trimming limits\n"; // 指定数据截取范围
	fout << (tmip.distPara.bIsTrans ? 0 : 1) << "\t-transform the data (0=no, 1=yes)\n"; // 是否进行转换
	fout << strSamplePath + ".sgsim.trn" << "\n"; // 转换文件路径
	fout << 0 << "\t-consider ref. dist (0=no, 1=yes)\n";
	fout << strSamplePath + ".histsmth.out" << "\n";
	fout << 1 << "\t" << 2 << "\t-columns for vr and wt\n";


	fout << tmip.distPara.dOutputMin << "\t" << tmip.distPara.dOutputMax << "\t-zmin,zmax (tail extrapolation)\n"; // 输出Output数据的最大最小值
	fout << 1 << "\t" << tmip.distPara.dOutputMin << "\t-lower tail option\n";
	fout << 1 << "\t" << tmip.distPara.dOutputMax << "\t-upper tail option\n";

	// 指定调试级别
	fout << 0 << "\t-debug level (0-3)\n";

	fout << strSamplePath + ".dbg" << "\n"; // 调试文件路径

	strResultFile = tmip.strWorkPath + "//sgsim.result";
	fout << strResultFile << "\n"; // 插值结果文件路径
	fout << 1 << "\t-number of realizations to generate\n"; // 插值结果数量

	// 网格信息
	fout << tmip.gi.nXNum << "\t" << tmip.gi.dXOri << "\t" << tmip.gi.dXStep << "\t-nx, xmin, xsize\n";
	fout << tmip.gi.nYNum << "\t" <<  tmip.gi.dYOri << "\t" << tmip.gi.dYStep << "\t-ny, ymin, ysize\n";
	fout << tmip.gi.nZNum << "\t" << tmip.gi.dZOri << "\t" << tmip.gi.dZStep << "\t-nz, zmin, zsize\n";

	// 随机数：要求必须为奇数
	if(tmip.expertPara.nSeed == 10001) //如果用户未设置，则由系统产生随机种子
	{
		tmip.expertPara.nSeed = EsGetOddRandomNum();
	}
	else if(tmip.expertPara.nSeed != 10001 && tmip.expertPara.nSeed%2 == 0) //如果用户自行设置，则检测是否为奇数
	{
		tmip.expertPara.nSeed++;
	}
	fout << tmip.expertPara.nSeed << "\t-random number seed\n";	

	fout << tmip.expertPara.nMinNumNodesUse << "\t" << tmip.expertPara.nMaxNumNodesUse << "\t-Min and max original data for sim\n"; // 样本数据使用的最大最小数量
	fout << tmip.expertPara.nSimulatedNodesUse << "\t-number of simulated nodes to use\n"; // 已生成属性网格的使用最大个数
	fout << 1 << "\t-assign data to nodes (0=no, 1=yes)\n"; // 是否将样本属性匹配到网格
	fout << tmip.expertPara.bIsUseMulti << "\t" << tmip.expertPara.nGridMultiStep << "\t-multiple grid search (0=no, 1=yes), num\n";

	if (!tmip.expertPara.bUseOctant)
	{
		fout << 0 << "\t-maximum data per octant (0=not used)\n";
	}
	else
	{
		fout << tmip.expertPara.nNumOctant << "\t-maximum data per octant (0=not used)\n";
	}

	fout << tmip.varPara.dMajorRange << "\t" << tmip.varPara.dMinorRange << "\t" << tmip.varPara.dVerticalRange << "\t-maximum search radii (hmax, hmin, vert)\n";
	fout << tmip.varPara.dAzimuth << "\t" << tmip.varPara.dDip-90 << "\t" << 0 << "\t-angles for search ellipsoid\n"; // 搜索椭圆参数

	// 方差查找表大小
	fout << tmip.gi.nXNum << "\t" << tmip.gi.nYNum << "\t" << tmip.gi.nZNum << "\t-size of covariance lookup table\n";

	fout << tmip.expertPara.kType << "\t" << 0 << "\t" << 1 << "\t-kType: 0=SK,1=OK,2=LVM,3=EXDR,4=COLC\n";// 算法类型，相关系数及方差折减系数在算法为简单克里金时不需要

	string strYdataFile = tmip.strWorkPath + "//ydata.dat";
	fout << strYdataFile << "\n"; // 第二变量文件
	fout << 1 << "\t" << 0 << "\t-column\n";
	fout << 1 << "\t"; // 目前只支持一个变差函数

	// 变差函数
	fout << tmip.varPara.dNugget << "\t-nst, nugget NOFILE\n"; // 块金值

	// it, cc, ang1, ang2, ang3
	fout << (int)tmip.varPara.varType << "\t" //变差类型
		<< tmip.varPara.dSill-tmip.varPara.dNugget << "\t"
		<< tmip.varPara.dAzimuth << "\t"
		<< tmip.varPara.dDip-90 << "\t"
		<< 0 << "\t-it, cc, ang1, ang2, ang3\n";

	// a_hmax, a_hmin, a_vert
	fout << tmip.varPara.dMajorRange << "\t"
		<< tmip.varPara.dMinorRange << "\t"
		<< tmip.varPara.dVerticalRange << "\t-a_hmax, a_hmin, a_vert\n";

	fout.close();
	return true;
}

bool gridworld::EsPrepKt3dParaFile(ModelInterPara& tmip, const string& strInterDataFile, string& strInterParaFile, string& strResultFile)
{
	strInterParaFile = tmip.strWorkPath + "//kt3d.par";
	ofstream fout(strInterParaFile, ios::out);
	if (!fout.is_open())
		return false;

	fout << "\t\t\tParameters for KT3D\n";
	fout << "\t\t\t********************\n";
	fout << "\n";
	fout << "START OF PARAMETERS:\n";

	fout << strInterDataFile << "\n"; //样本数据文件
	fout << 0 << "\t" << 1 << "\t" << 2 << "\t" << 3 << "\t" << 4 << "\t" << 0
		<< "\t-columns for DH,X,Y,Z,vr,sec.var\n";

	// 指定数据截取范围
	fout << -1.0e21 << "\t" << 1.0e21 << "\t-trimming limits\n";

	fout << 0 << "\t-option: 0=grid, 1=cross, 2=jackknife\n";
	fout << strInterDataFile + ".xvk.dat" << "\n";
	fout << 1 << "\t" << 2 << "\t" << 3 << "\t" << 4 << "\t" << 5 << "\n";

	// 指定调试级别
	fout << 0 << "\t-debug level (0-3)\n";
	// 调试文件路径
	fout << strInterDataFile + ".dbg" << "\n";

	// 插值结果文件路径
	strResultFile = tmip.strWorkPath + "//kt3d.result";
	fout << strResultFile << "\n";

	// 网格信息
	fout << tmip.gi.nXNum << "\t" << fixed << setprecision(6) << tmip.gi.dXOri << "\t" << tmip.gi.dXStep << "\t-nx, xmin, xsize\n";
	fout << tmip.gi.nYNum << "\t" << fixed << setprecision(6) <<  tmip.gi.dYOri << "\t" << tmip.gi.dYStep << "\t-ny, ymin, ysize\n";
	fout << tmip.gi.nZNum << "\t" << tmip.gi.dZOri << "\t" << tmip.gi.dZStep << "\t-nz, zmin, zsize\n";

	fout << 1 << "\t" << 1 << "\t" << 1 << "\t-x, y and z block discretization\n";
	fout << 4 << "\t" << 8 << "\t-Min and max data for kriging\n";
	fout << 0 << "\t-Max per octant (0-> not used)\n";

	// 搜索椭圆参数
	fout << tmip.varPara.dMajorRange << "\t" << tmip.varPara.dMinorRange << "\t" << tmip.varPara.dVerticalRange
		<< "\t-maximum search radii (hmax, hmin, vert)\n";
	fout << tmip.varPara.dAzimuth << "\t" << tmip.varPara.dDip-90 << "\t" << 0
		<< "\t-angles for search ellipsoid\n";

	// 克里金类型
	fout << 1 << "\t" << 0
		<< "\t-0=SK, 1=OK, 2=non-st SK,3=exdrift\n";

	fout << 0 << "\t" << 0 << "\t" << 0 << "\t"
		<< 0 << "\t" << 0 << "\t" << 0 << "\t"
		<< 0 << "\t" << 0 << "\t" << 0 << "\t"
		<< "-drift: x,y,z,xx,yy,zz,xy,xz,zy\n";
	fout << 0 << "\t-Variable; 1, estimate; trend\n";

	fout << strInterDataFile + ".extdrift.dat" << "\n";
	fout << 0 << "\n";

	// 变差函数结构套合数量
	fout << 1 << "\t";

	// 变差函数
	fout << tmip.varPara.dNugget << "\t-nst, nugget NOFILE\n"; // 块金值

	// it, cc, ang1, ang2, ang3
	fout << (int)tmip.varPara.varType << "\t"
		<< tmip.varPara.dSill-tmip.varPara.dNugget << "\t"
		<< tmip.varPara.dAzimuth << "\t"
		<< tmip.varPara.dDip-90 << "\t"
		<< 0 << "\t-it, cc, ang1, ang2, ang3\n";

	// a_hmax, a_hmin, a_vert
	fout << tmip.varPara.dMajorRange << "\t"
		<< tmip.varPara.dMinorRange << "\t"
		<< tmip.varPara.dVerticalRange << "\t-a_hmax, a_hmin, a_vert\n";

	fout.close();

	return true;
}

bool gridworld::EsPrepSisimParaFile(ModelInterPara& tmip, const string& strInterDataFile, string& strInterParaFile, string& strResultFile)
{
	strInterParaFile = tmip.strWorkPath + "//sisim.par";
	ofstream fout(strInterParaFile, ios::out);
	if (!fout.is_open())
		return false;

	//输出说明文件头
	fout << "START OF PARAMETERS:\n";
	int nSize = (int)tmip.faciesPara.size();
	map<int, double> FaciesInfo;
	vector<int> codes;

	//属性类型
	fout << 0 << "\t-1=continuous (cdf), 0=categorical (pdf)\n";
	fout << nSize << "\t-number thresholds/categories\n";

	for(int i=0; i<nSize; i++)
	{
		const CodeParaSet& cps = tmip.faciesPara[i];
		codes.push_back(cps.nCode);	
		FaciesInfo[cps.nCode] = EsGetCodePercent(strInterDataFile, cps.nCode);

	}

	if(FaciesInfo.empty())
		return false;

	//相代码
	double sum = 0.0;
	for(map<int, double>::iterator it=FaciesInfo.begin(); it!=FaciesInfo.end(); it++)
	{
		//相属性，所以都为整数
		fout << it->first << "\t";
		sum += it->second;
	}
	fout << "\n";

	//每种相的全局累计概率
	double cdf = 0.0;
	for (map<int, double>::iterator it=FaciesInfo.begin(); it!=FaciesInfo.end(); it++)
	{
		cdf += it->second;
		fout << cdf/sum << "\t";
	}
	fout << "\n" ;

	//数据路径
	fout << strInterDataFile << "\n";
	fout << 1 << "\t" << 2 << "\t" << 3 << "\t" << 4 << "\t-columns for X, Y, Z, vr\n";

	// 转换好的指示数据文件路径,暂时不考虑使用指示数据文件，所以都设为默认值
	fout << strInterDataFile << "---------" << "\n";
	fout << 1 << "\t" << 2 << "\t" << 3 << "\t" << 1 << "\t" << 1 <<"\n";
	fout << 0 << "\t-Markov-Bayes simulation (0=no, 1=yes)\n" ;

	// calibration B(z) values
	for (int i=0; i<nSize; i++)
	{
		fout << 0 << "\t";
	}
	fout << "\n";

	//数据的裁剪，读入数据时，当数据大于MAX或小于MIN时去掉此类数据,从新读下一行
	fout << -1.0e21 << "\t" << 1.0e21 << "\t-trimming limits\n";

	// 反变换允许的最大最小值
	sort(codes.begin(), codes.end());
	fout << codes.front() << "\t" << codes.back() << "\t-minimum and maximum data value\n";

	// -lower tail option
	fout << 1 << "\t" << codes.front() << "\t-lower tail option\n";

	// -middle tail option
	fout << 1 << "\t" << 1 << "\t-middle tail option\n";

	// -upper tail option
	fout << 1 << "\t" << codes.back() << "\t-upper tail option\n";

	// 以上三个选项有任意一个反变换的插值方法采用文件数据进行线性插值时需要
	fout << strInterDataFile << "---------" << "\n";
	fout << 3 << "\t" << 0 << "\t-columns for variable, weight\n" ;

	// 指定调试级别
	fout << 0 << "\t-debug level (0-3)\n" ;

	// 调试文件路径
	fout << strInterDataFile + ".dbg" << "\n";

	//输出文件路径
	strResultFile = tmip.strWorkPath + "//sisim.result";
	fout << strResultFile << "\n" ;

	//模拟实现个数
	fout << 1 << "\t-number of realizations to generate\n" ;

	// 网格参数
	fout << tmip.gi.nXNum << "\t" << tmip.gi.dXOri << "\t" << tmip.gi.dXStep << "\t-nx, xmin, xsize\n";
	fout << tmip.gi.nYNum << "\t" <<  tmip.gi.dYOri << "\t" << tmip.gi.dYStep << "\t-ny, ymin, ysize\n";
	fout << tmip.gi.nZNum << "\t" << tmip.gi.dZOri << "\t" << tmip.gi.dZStep << "\t-nz, zmin, zsize\n";

	//输出随机数
	if(tmip.expertPara.nSeed == 10001) //如果用户未设置，则由系统产生随机种子
	{
		tmip.expertPara.nSeed = EsGetOddRandomNum();
	}
	else if(tmip.expertPara.nSeed != 10001 && tmip.expertPara.nSeed%2 == 0) //如果用户自行设置，则检测是否为奇数
	{
		tmip.expertPara.nSeed++;
	}
	fout << tmip.expertPara.nSeed << "\t-random number seed\n";

	//用于序贯高斯模拟的最大已知数据量
	fout << 12 << "\t-maximum original data for each kriging\n" ;

	//序贯高斯模拟时将模拟得到的结果作为条件数据加入到后续节点的模拟
	fout << 12 << "\t-maximum previous nodes for each kriging\n" ;

	//利用指示数据文件时
	fout << 12 << "\t-maximum soft indicator nodes for kriging\n" ;

	//是否将原始数据赋给网格节点
	fout << 1 << "\t-assign data to nodes? (0=no, 1=yes)\n" ;

	//是否使用多网格搜索，搜索级别
	fout << tmip.expertPara.bIsUseMulti << "\t" << tmip.expertPara.nGridMultiStep << "\t-multiple grid search? (0=no,1=yes), num\n" ;

	//当使用象限搜索是每个象限的最大搜索个数,0为不使用
	if (tmip.expertPara.bUseOctant)
	{
		fout << tmip.expertPara.nNumOctant << "\t-maximum per octant (0=not used)\n";
	}
	else
	{
		fout << 0 << "\t-maximum per octant (0=not used)\n";
	}

	map<int, VariogramPara> vs;
	for (size_t i=0; i<tmip.faciesPara.size(); i++)
	{
		const int& code = tmip.faciesPara[i].nCode;
		const VariogramPara& v = tmip.faciesPara[i].varPara;

		//不使用数据分析结果或者读取失败时使用界面参数（数据分析结果已读取到界面）
		vs.insert(std::make_pair(code, v));
	}

	// 找出最大的变程设置搜索椭圆半径
	double r1 = 0, r2 = 0, r3 = 0;
	double a1 = 0, a2 = 0, a3 = 0;

	for (map<int, VariogramPara>::iterator it=vs.begin(); it!=vs.end(); it++)
	{
		if (it->second.dMajorRange > r1)
		{
			r1 = it->second.dMajorRange;
			r2 = it->second.dMinorRange;
			r3 = it->second.dVerticalRange;

			a1 = it->second.dAzimuth;
			a2 = 0;
			a3 = it->second.dDip-90;
		}
	}

	// 搜索半径 (hmax,hmin,vert)
	fout << r1*2 << "\t" << r2*2 << "\t" << r3*2 << "\t" << "\t-maximum search radii\n";

	// 搜索椭圆角度
	fout << a1 << "\t" << a2 << "\t" << a3 << "\t" << "\t-angles for search ellipsoid\n";

	// 方差查找表大小
	fout << tmip.gi.nXNum << "\t" << tmip.gi.nYNum << "\t" << tmip.gi.nZNum << "\n";

	//是否使用中值克里金
	fout << 0 << "\t" << 0 << "\t-0=full IK, 1=median IK (threshold num)\n";

	//解克里金方程类型
	fout << 0 << "\t-0=SK, 1=OK\n" ;

	// 每个离散变量的变异函数
	for (map<int, VariogramPara>::iterator it=vs.begin(); it!=vs.end(); it++)
	{
		const VariogramPara& v = it->second;

		fout << 1 << "\t" << v.dNugget << "\t-nst, nugget NOFILE\n"; //editor He: para1-the number of variogram
		fout << (int)v.varType << "\t" << v.dSill-v.dNugget << "\t" << v.dAzimuth << "\t" << 0 << "\t" << v.dDip-90 << "\t-it, cc, ang1, ang2, ang3\n";
		fout << v.dMajorRange << "\t" << v.dMinorRange << "\t" << v.dVerticalRange << "\t-a_hmax, a_hmin, a_vert\n";
	}

	fout.close();
	return true;
}

bool gridworld::EsPrepIk3dParaFile(ModelInterPara& tmip, const string& strInterDataFile, string& strInterParaFile, string& strResultFile)
{
	strInterParaFile = tmip.strWorkPath + "//ik3d.par";
	ofstream fout(strInterParaFile, ios::out);
	if (!fout.is_open())
		return false;

	//输出说明文件头
	fout << "START OF PARAMETERS:\n";
	int nSize = (int)tmip.faciesPara.size();
	map<int, double> FaciesInfo;

	if (tmip.nAttrType == Discrete)//输出相代码和相频率
	{
		fout << 0 << "\t-1=continuous (cdf), 0=categorical (pdf)\n";
		fout << 0 << "\t-option: 0=grid, 1=cross, 2=jackknife\n";
		fout << "jack.dat" << "\n";
		fout << 1 << "\t" << 2 << "\t" << 0 << "\t" << 3 << "\t-columns for X, Y, Z, variable\n";
		fout << nSize << "\t-number thresholds/categories\n";

		for (int i=0; i<nSize; i++)
		{
			const CodeParaSet& cps = tmip.faciesPara[i];			
			FaciesInfo[cps.nCode] = EsGetCodePercent(strInterDataFile, cps.nCode);
		}

		if (FaciesInfo.empty()) return false;


		// 相代码 hjq 2018/07/10
		double sum = 0.0;
		for (map<int, double>::iterator it=FaciesInfo.begin(); it!=FaciesInfo.end(); it++)
		{
			//相属性，所以都为整数
			fout << it->first << "\t";
			sum += it->second;
		}
		fout << "\n";

		// 每种相的全局累计概率 hjq 2018/07/10
		double cdf = 0.0;
		for (map<int, double>::iterator it=FaciesInfo.begin(); it!=FaciesInfo.end(); it++)
		{
			cdf += it->second;
			fout << cdf/sum << "\t";
		}

		fout << "\n" ;
	}

	//数据路径
	fout << strInterDataFile << "\n";

	fout << 0 << "\t" << 1 << "\t" << 2 << "\t" << 3 << "\t" << 4 << "\t-columns for DH, X, Y, Z, variable\n";

	// 转换好的指示数据文件路径,暂时不考虑使用指示数据文件，所以都设为默认值
	fout << strInterDataFile << "---------" << "\n";
	fout << 1 << "\t" << 2 << "\t" << 3 << "\t" << 1 << "\t" << 1 <<"\n";

	//数据的裁剪，读入数据时，当数据大于MAX或小于MIN时去掉此类数据,从新读下一行
	fout << -1.0e21 << "\t" << 1.0e21 << "\t-trimming limits\n";

	// 指定调试级别
	fout << 0 << "\t-debug level (0-3)\n" ;

	// 调试文件路径
	fout << strInterDataFile + ".dbg" << "\n";

	//输出文件路径
	strResultFile = tmip.strWorkPath + "//ik3d.result";
	fout << strResultFile << "\n" ;

	// 网格参数
	fout << tmip.gi.nXNum << "\t" << tmip.gi.dXOri << "\t" << tmip.gi.dXStep << "\t-nx, xmin, xsize\n";
	fout << tmip.gi.nYNum << "\t" <<  tmip.gi.dYOri << "\t" << tmip.gi.dYStep << "\t-ny, ymin, ysize\n";
	fout << tmip.gi.nZNum << "\t" << tmip.gi.dZOri << "\t" << tmip.gi.dZStep << "\t-nz, zmin, zsize\n";

	fout << 4 << "\t" << 8 << "\t-Min, Max Data for kriging\n";

	// 找出最大的变程设置搜索椭圆半径
	double maxRange = 0;
	int index = 0;
	for (size_t j=0; j<tmip.faciesPara.size(); j++)
	{
		if (tmip.faciesPara[j].varPara.dMajorRange > maxRange)
		{
			maxRange = tmip.faciesPara[j].varPara.dMajorRange;
			index = (int)j;
		}
	}

	// 搜索半径 (hmax,hmin,vert)
	fout << tmip.faciesPara[index].varPara.dMajorRange*2 << "\t" << tmip.faciesPara[index].varPara.dMinorRange*2 << "\t" 
		<< tmip.faciesPara[index].varPara.dVerticalRange*2 << "\t-maximum search radii\n";

	// 搜索椭圆角度
	fout << tmip.faciesPara[index].varPara.dAzimuth << "\t" << 0 << "\t" << tmip.faciesPara[index].varPara.dDip-90 << "\t-angles for search ellipsoid\n";

	//当使用象限搜索是每个象限的最大搜索个数,0为不使用
	if (tmip.expertPara.bUseOctant)
	{
		fout << tmip.expertPara.nNumOctant << "\t-maximum per octant (0=not used)\n";
	}
	else
	{
		fout << 0 << "\t-maximum per octant (0=not used)\n";
	}

	//是否使用中值克里金
	fout << 0 << "\t" << 0 << "\t-0=full IK, 1=median IK (threshold num)\n";

	//解克里金方程类型
	fout << 0 << "\t-0=SK, 1=OK\n" ;

	//每个离散变量的变异函数
	for (map<int, double>::iterator it=FaciesInfo.begin();
		it!=FaciesInfo.end(); it++)
	{
		for (size_t j=0; j<tmip.faciesPara.size(); j++)
		{
			if (it->first == tmip.faciesPara[j].nCode)
			{ 
				fout << 1 << "\t" << tmip.faciesPara[j].varPara.dNugget << "\t-nst, nugget NOFILE\n"; //para1-the number of variogram
				fout << tmip.faciesPara[j].varPara.varType << "\t" << tmip.faciesPara[j].varPara.dSill-tmip.faciesPara[j].varPara.dNugget << "\t" 
					<< tmip.faciesPara[j].varPara.dAzimuth << "\t" << 0 << "\t" << tmip.faciesPara[j].varPara.dDip-90 << "\t-it, cc, ang1, ang2, ang3\n";
				fout << tmip.faciesPara[j].varPara.dMajorRange << "\t" << tmip.faciesPara[j].varPara.dMinorRange << "\t" << tmip.faciesPara[j].varPara.dVerticalRange << "\t-a_hmax, a_hmin, a_vert\n";

				break;
			}
		}
	}

	fout.close();
	return true;
}

double gridworld::EsGetCodePercent(const string& strInterDataFile, const int nCode)
{
	ifstream fin(strInterDataFile, ios::in);
	if (!fin.is_open())
		return false;

	string strLine;
	getline(fin, strLine);

	getline(fin, strLine);
	int nColNum = atoi(strLine.c_str());

	for(int i = 0; i < nColNum; i++)
		getline(fin, strLine);

	int nSum = 0;
	int nNum = 0;
	while(!fin.eof())
	{
		getline(fin, strLine);
		vector<string> vecStrData;
		split(strLine, vecStrData);

		if(vecStrData.size() != 5)
			continue;

		nSum++;
		if(atoi(vecStrData[3].c_str()) == nCode)
			nNum++;		
	}

	fin.close();

	if(nSum == 0)
		return 0.0;
	else
		return (double)nNum/(double)nSum;
}

bool gridworld::EsGetSampleDataRange(const string& strInterDataFile, double& dMin, double& dMax)
{
	ifstream fin(strInterDataFile, ios::in);
	if (!fin.is_open())
		return false;

	string strLine;
	getline(fin, strLine);

	getline(fin, strLine);
	int nColNum = atoi(strLine.c_str());

	for(int i = 0; i < nColNum; i++)
		getline(fin, strLine);

	dMin = DBL_MAX;
	dMax = DBL_MIN;
	while(!fin.eof())
	{
		getline(fin, strLine);
		vector<string> vecStrData;
		split(strLine, vecStrData);

		if(vecStrData.size() != 5)
			continue;

		double dValue = atof(vecStrData[3].c_str());
		if( dValue > dMax)
			dMax = dValue;
		if(dValue < dMin)
			dMin = dValue;
	}

	dMin = floor(dMin);
	dMax = floor(dMax);

	fin.close();
	return true;
}

bool gridworld::EsNormalScore(const string& strSamplePath, const string& strInterDataFile, const string& strWorkPath, double& dMin, double& dMax)
{	
	string strNormalScoreParaFile = strWorkPath + "//nscore.par";
	string strOutputFile = strWorkPath + "//nscore_data.dat";
	string strTransTableFile = strWorkPath + "//trn_table.out";

	ofstream fout(strNormalScoreParaFile, ios::out);
	if (!fout.is_open())
		return false;

	fout << "START OF PARAMETERS:" << endl
		<< strInterDataFile << endl
		<< 4 << " " << 0 << endl
		<< dMin << "\t" << dMax + 1 << endl
		<< 0 << endl
		<< "histsmth.out" << endl
		<< 1 << " " << 2 << endl
		<< strOutputFile << endl
		<< strTransTableFile << endl;

	fout.close();

	SHELLEXECUTEINFOA ShExecInfo = {0};
	ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFOA);
	ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	ShExecInfo.hwnd = NULL;
	ShExecInfo.lpVerb = NULL;
	ShExecInfo.lpParameters = strNormalScoreParaFile.c_str(); //参数
	ShExecInfo.lpDirectory = NULL;
	ShExecInfo.nShow = SW_HIDE;
	ShExecInfo.hInstApp = NULL;

	char chpath[MAX_PATH];
	GetModuleFileNameA(NULL, chpath, sizeof(chpath));
	string strExePath = (string)chpath;
	int pos = (int)strExePath.find_last_of('\\', strExePath.length());
	string strCurrentDir = strExePath.substr(0, pos);
	strExePath = strCurrentDir + "\\nscore.exe";
	ShExecInfo.lpFile = strExePath.c_str(); //exe路径   

	ShellExecuteExA(&ShExecInfo);
	WaitForSingleObject(ShExecInfo.hProcess, INFINITE);

	ifstream fin(strOutputFile, ios::in);
	if(!fin.is_open())
		return false;

	ofstream fsp(strSamplePath, ios::out);
	if (!fsp.is_open())
		return false;

	string strLine;
	getline(fin, strLine);

	fsp << strLine <<"\n";
	fsp << "4\nXLocation\nYLocation\nZLocation\nPrimary\n";

	while(!fin.eof())
	{
		getline(fin, strLine);
		vector<string> vecStrData;
		split(strLine, vecStrData);

		if(vecStrData.size() != 6)
			continue;

		double dx = atof(vecStrData[0].c_str());
		double dy = atof(vecStrData[1].c_str());
		double dz = atof(vecStrData[2].c_str());
		double dp = atof(vecStrData[5].c_str());

		fsp << dx << "\t" << dy << "\t" << dz << "\t" << dp << "\n";
	}

	fsp.close();
	fin.close();

	return true;
}

void gridworld::split(const string& source, vector<string>& dest)
{
	const char *szDelim = " \t,;\n";
	char *context;
	char *next_token = NULL;
	context = strtok_s((char *)source.c_str(), szDelim, &next_token);
	while(context)
	{
		dest.push_back(context);
		context = strtok_s(NULL, szDelim, &next_token);
	}
}

bool gridworld::EsClearDir(const string& strWorkPath)
{
	string strDir = strWorkPath;
	strDir.append("\\*");

	WIN32_FIND_DATAA FindFileData;
	HANDLE hFind = FindFirstFileA(strDir.c_str(), &FindFileData);

	if (hFind == INVALID_HANDLE_VALUE)
		return false;

	while (FindNextFileA(hFind, &FindFileData))
	{
		if (FindFileData.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY)
		{
			string strFilePath = strWorkPath + "\\" + FindFileData.cFileName;
			DeleteFileA(strFilePath.c_str());
		}
		else if (FindFileData.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY
			&& strcmp(FindFileData.cFileName, ".") != 0
			&& strcmp(FindFileData.cFileName, "..") != 0)
		{
			std::string strSubDir = strWorkPath;
			strSubDir.append("\\");
			strSubDir.append(FindFileData.cFileName);

			DeleteFileA(strSubDir.c_str());
		}
	}

	FindClose(hFind);

	RemoveDirectoryA(strWorkPath.c_str());

	return true;
}

bool gridworld::EsTransformIk3dResult(const string& strResultFile)
{
	string strTemp = strResultFile + ".temp";
	ifstream fin(strResultFile,ios::in);
	if(!fin.is_open())
		return false;

	ofstream fout(strTemp, ios::out);
	if(!fout.is_open())
		return false;

	string strLine;
	vector<string> vecStrData;

	//读写第一行
	getline(fin, strLine);
	fout << strLine << "\n";

	//读取列数
	getline(fin, strLine);
	fout << strLine << "\n";
	vecStrData.clear();
	split(strLine, vecStrData);
	if(vecStrData.empty())
		return false;
	int nColumn = atoi(vecStrData[0].c_str());
	vector<int> vecAttr;
	vecAttr.resize(nColumn);

	//读取每一列的相代码
	for(int i=0; i<nColumn; i++)
	{
		getline(fin, strLine);
		fout << strLine << "\n";
		vecStrData.clear();
		split(strLine, vecStrData);
		vecAttr[i] = atoi(vecStrData.back().c_str());
	}

	//读取相概率，并转化为相
	while(!fin.eof())
	{
		getline(fin, strLine);
		vecStrData.clear();
		split(strLine, vecStrData);
		if(vecStrData.size() != nColumn)
			continue;
		vector<double> vecProb;
		vecProb.resize(nColumn);
		int idx = 0;
		double value = 0;
		for(int i=0; i<nColumn; i++)
		{
			vecProb[i] = atof(vecStrData[i].c_str());
			if(vecProb[i] > value)
			{
				value = vecProb[i];
				idx = i;
			}
		}
		fout << vecAttr[idx] << "\n";
	}

	fin.close();
	fout.close();

	DeleteFileA(strResultFile.c_str());
	rename(strTemp.c_str(),strResultFile.c_str());
	return true;
}

int gridworld::EsGetOddRandomNum()
{
	SYSTEMTIME st;
	GetSystemTime(&st);
	srand(st.wMilliseconds + st.wSecond*1000);
	int nSeed = rand()%100000 + 10000;
	return nSeed%2 == 0 ? nSeed + 1 : nSeed;
}

bool gridworld::EsRevertNormalScore(const string& strRevertResultFile, const string& strResultFile, const string& strWorkPath, double& dMin, double& dMax)
{

/**********将result结果转换成backtr.exe所需数据格式**********/
	ifstream fResultFile(strResultFile, ios::in);
	if(!fResultFile.is_open())
		return false;

	string strLine;
	getline(fResultFile, strLine);

	string strGridInfo;
	getline(fResultFile, strGridInfo);
	vector<string> vecStrData;
	split(strGridInfo, vecStrData);

	if(vecStrData.size() != 11)
		return false;

	int nSkip = atoi(vecStrData[0].c_str());

	int nXNum = atoi(vecStrData[1].c_str());
	int nYNum = atoi(vecStrData[2].c_str());
	int nZNum = atoi(vecStrData[3].c_str());

	double xOri = atof(vecStrData[4].c_str());
	double yOri = atof(vecStrData[5].c_str());
	double zOri = atof(vecStrData[6].c_str());

	double xStep = atof(vecStrData[7].c_str());
	double yStep = atof(vecStrData[8].c_str());
	double zStep = atof(vecStrData[9].c_str());

	int nRealNum = atoi(vecStrData[10].c_str());

	for(int i=0; i<nSkip; i++)
		getline(fResultFile, strLine);

	string strRawDataFile = strWorkPath + "//normal.dat";
	ofstream fRawDataFile(strRawDataFile, ios::out);

	fRawDataFile << "Clustered " << nXNum*nYNum*nZNum << " primary data\n";
	fRawDataFile << "5\nXLocation\nYLocation\nZLocation\nPrimary\nIndex\n";
	
	for(int i=0; i<nRealNum; i++)
	{
		for(int z=0; z<nZNum; z++)
		{
			for(int y=0; y<nYNum; y++)
			{
				for(int x=0; x<nXNum; x++)
				{
					double tempX = xOri + xStep*x;
					double tempY = yOri + yStep*y;
					double tempZ = zOri + zStep*z;

					getline(fResultFile, strLine);
					vector<string> vecStrData;
					split(strLine, vecStrData);
					double tempV = atof(vecStrData[0].c_str());
					int tempN = (z*nXNum*nYNum + y*nXNum + x)/(nXNum*nYNum);

					fRawDataFile << tempX << "\t" << tempY << "\t" <<tempZ << "\t" <<tempV << "\t" << tempN << "\n";
				}
			}
		}
	}

	fRawDataFile.close();
	fResultFile.close();

/**********准备backtr.exe所需参数文件**********/
	string strRevertNormalScoreParaFile = strWorkPath + "//backtr.par";
	string strOutDataFile = strWorkPath + "//backtr_data.dat";
	string strTransTableFile = strWorkPath + "//trn_table.out";

	ofstream fRevertParaFile(strRevertNormalScoreParaFile, ios::out);
	if (!fRevertParaFile.is_open())
		return false;

	fRevertParaFile << "START OF PARAMETERS:" << endl
		<< strRawDataFile << endl
		<< 4 << endl
		<< "-1E+21\t1E+21" << endl
		<< strOutDataFile << endl
		<< strTransTableFile << endl
		//<< m_min << "\t" << m_max << endl     //
		<< dMin << "\t" << dMax + 1 << endl   //editor He:keep every value has a table
		<< "1\t" << dMin << endl
		<< "1\t" << dMax << endl;

	fRevertParaFile.close();

/**********调用backtr.exe进行反正态变换**********/
	SHELLEXECUTEINFOA ShExecInfo = {0};
	ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFOA);
	ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	ShExecInfo.hwnd = NULL;
	ShExecInfo.lpVerb = NULL;
	ShExecInfo.lpParameters = strRevertNormalScoreParaFile.c_str(); //参数
	ShExecInfo.lpDirectory = NULL;
	ShExecInfo.nShow = SW_HIDE;
	ShExecInfo.hInstApp = NULL;

	char chpath[MAX_PATH];
	GetModuleFileNameA(NULL, chpath, sizeof(chpath));
	string strExePath = (string)chpath;
	int pos = (int)strExePath.find_last_of('\\', strExePath.length());
	string strCurrentDir = strExePath.substr(0, pos);
	strExePath = strCurrentDir + "\\backtr.exe";
	ShExecInfo.lpFile = strExePath.c_str(); //exe路径   

	ShellExecuteExA(&ShExecInfo);
	WaitForSingleObject(ShExecInfo.hProcess, INFINITE);

/**********调用backtr.exe输出的转换结果重新编辑成result格式**********/
	ifstream fOutDataFile(strOutDataFile, ios::in);
	if(!fOutDataFile.is_open())
		return false;

	ofstream fRevertFile(strRevertResultFile, ios::out);
	if (!fRevertFile.is_open())
		return false;

	strLine;
	getline(fOutDataFile, strLine);

	getline(fResultFile, strLine);
	split(strLine, vecStrData);

	nSkip = atoi(vecStrData[0].c_str());
	for(int i=0; i<nSkip; i++)
		getline(fResultFile, strLine);

	fRevertFile << "SGSIM Realizations\n";
	fRevertFile << nSkip << "\t" << nXNum << "\t" << nYNum << "\t" << nZNum << "\t" << xOri << "\t" << yOri << "\t" << zOri << "\t" << xStep << "\t" << yStep << "\t" << zStep << "\t" << nRealNum << "\n";
	fRevertFile << "value\n";

	while(!fOutDataFile.eof())
	{
		getline(fOutDataFile, strLine);
		vector<string> vecStrData;
		split(strLine, vecStrData);

		if(vecStrData.size() != 6)
			continue;

		double dp = atof(vecStrData[5].c_str());

		fRevertFile <<  dp << "\n";
	}

	fRevertFile.close();
	fOutDataFile.close();

#ifndef DEBUG
	string strBackup = strResultFile + ".backup";
	rename(strResultFile.c_str(), strBackup.c_str());
#else
	DeleteFileA(strResultFile.c_str());
#endif // DEBUG	
	rename(strRevertResultFile.c_str(), strResultFile.c_str());

	return true;
}

bool gridworld::EsResample(ModelInterPara& tmip, const string& strResultFile)
{
	ifstream fin(strResultFile, ios::in);
	if(!fin.is_open())
		return false;

	string strLine;
	getline(fin, strLine);

	getline(fin, strLine);
	vector<string> vecStrData;
	split(strLine, vecStrData);

	int nSpan = atoi(vecStrData[0].c_str());

	GridInfo gi;
	gi.nXNum = atoi(vecStrData[1].c_str());
	gi.nYNum = atoi(vecStrData[2].c_str());
	gi.nZNum = atoi(vecStrData[3].c_str());

	for(int i=0; i<nSpan; i++)
		getline(fin, strLine);

	int nTotal = gi.nXNum*gi.nYNum*gi.nZNum;
	if(nTotal <= 0 || nTotal > 1e10)
		return false;

	if(nTotal > 0)
	{
		if (tmip.nAttrType == Discrete)
			tmip.vecDiscreteProp.resize(nTotal);
		else if(tmip.nAttrType == Continuous)
			tmip.vecContinuousProp.resize(nTotal);

		for(int i = 0; i < nTotal; i++)
		{
			if(tmip.nAttrType == Discrete)
			{
				getline(fin, strLine);
				vector<string> vecStrData;
				split(strLine, vecStrData);
				int facisvalue = atoi(vecStrData[0].c_str());
				tmip.vecDiscreteProp[i] = facisvalue;
			}
			else if(tmip.nAttrType == Continuous)
			{
				getline(fin, strLine);
				vector<string> vecStrData;
				split(strLine, vecStrData);
				double vt = atof(vecStrData[0].c_str());
				tmip.vecContinuousProp[i] = vt;
			}
		}
	}

	fin.close();

	return true;
}

bool gridworld::EsEvaluateAreaForUpdate(vector<Cell>& vecUpdateData, const int rangeI, const int rangeJ, const int rangeK,
	const GridInfo& globalGI, vector<GridInfo>& vecGIforSampleDownload, vector<GridInfo>& vecGIforUpdate)
{
	//如果搜索范围为负，返回失败
	if (rangeI <= 0 || rangeJ <= 0 || rangeK <= 0)
		return false;

	//搜索范围确定了最小的更新范围，如果该最小范围比单次插值上限还多，则返回失败
	int nNumLeast = (2 * rangeI + 1)*(2 * rangeJ + 1)*(2 * rangeK + 1);
	if (nNumLeast > gLimit)
		return false;

	int nMinI, nMinJ, nMinK;
	int nMaxI, nMaxJ, nMaxK;
	nMinI = nMinJ = nMinK = INT_MAX;
	nMaxI = nMaxJ = nMaxK = INT_MIN;

	//统计更新的数据网格起止
	for (auto cell : vecUpdateData)
	{
		if (cell.nI < nMinI)
			nMinI = cell.nI;
		if (cell.nJ < nMinJ)
			nMinJ = cell.nJ;
		if (cell.nK < nMinK)
			nMinK = cell.nK;
		if (cell.nI > nMaxI)
			nMaxI = cell.nI;
		if (cell.nJ > nMaxJ)
			nMaxJ = cell.nJ;
		if (cell.nK > nMaxK)
			nMaxK = cell.nK;
	}

	//结合变差搜索范围确定更新范围
	GridInfo GIforUpdate;

	//样本网格起始点减少一倍搜索范围即为待插范围网格起始点，判断待插网格起始点是否为负值，如果为负，则用实际起始点（0，0，0）
	GIforUpdate.nIOri = nMinI - rangeI;
	if (GIforUpdate.nIOri < globalGI.nIOri)
		GIforUpdate.nIOri = globalGI.nIOri;

	GIforUpdate.nJOri = nMinJ - rangeJ;
	if (GIforUpdate.nJOri < globalGI.nJOri)
		GIforUpdate.nJOri = globalGI.nJOri;

	GIforUpdate.nKOri = nMinK - rangeK;
	if (GIforUpdate.nKOri < globalGI.nKOri)
		GIforUpdate.nKOri = globalGI.nKOri;

	//样本网格最远点增加一倍搜索范围即为待插范围网格最远点，判断待插网格最远点是否超出实际范围，如果超出，则用实际最大范围
	if (nMaxI + rangeI > globalGI.nIOri + globalGI.nXNum - 1)
		GIforUpdate.nXNum = globalGI.nIOri + globalGI.nXNum - GIforUpdate.nIOri;
	else
		GIforUpdate.nXNum = nMaxI + rangeI - GIforUpdate.nIOri + 1;

	if (nMaxJ + rangeJ > globalGI.nJOri + globalGI.nYNum - 1)
		GIforUpdate.nYNum = globalGI.nJOri + globalGI.nYNum - GIforUpdate.nJOri;
	else
		GIforUpdate.nYNum = nMaxJ + rangeJ - GIforUpdate.nJOri + 1;

	if (nMaxK + rangeK > globalGI.nKOri + globalGI.nZNum - 1)
		GIforUpdate.nZNum = globalGI.nKOri + globalGI.nZNum - GIforUpdate.nKOri;
	else
		GIforUpdate.nZNum = nMaxK + rangeK - GIforUpdate.nKOri + 1;

	//计算待插网格总数
	int nTotal = GIforUpdate.nXNum*GIforUpdate.nYNum*GIforUpdate.nZNum;

	//如果待插总量未超过单次计算上限，则输出该范围,并输出对应的所需样本数据范围
	if (nTotal <= gLimit)
	{
		GIforUpdate.dXStep = globalGI.dXStep;
		GIforUpdate.dYStep = globalGI.dYStep;
		GIforUpdate.dZStep = globalGI.dZStep;

		GIforUpdate.dXOri = globalGI.dXOri + globalGI.dXStep*(GIforUpdate.nIOri - globalGI.nIOri);
		GIforUpdate.dYOri = globalGI.dYOri + globalGI.dYStep*(GIforUpdate.nJOri - globalGI.nJOri);
		GIforUpdate.dZOri = globalGI.dZOri;

		vecGIforUpdate.push_back(GIforUpdate);

		//输出待插范围需要的样本数据范围，计算思路与待插范围的确定思路一致，即再外扩一个搜索范围
		GridInfo GIforSampleDownload;
		GIforSampleDownload.nIOri = GIforUpdate.nIOri - rangeI;
		if (GIforSampleDownload.nIOri < globalGI.nIOri)
			GIforSampleDownload.nIOri = globalGI.nIOri;

		GIforSampleDownload.nJOri = GIforUpdate.nJOri - rangeJ;
		if (GIforSampleDownload.nJOri < globalGI.nJOri)
			GIforSampleDownload.nJOri = globalGI.nJOri;

		GIforSampleDownload.nKOri = GIforUpdate.nKOri - rangeK;
		if (GIforSampleDownload.nKOri < globalGI.nKOri)
			GIforSampleDownload.nKOri = globalGI.nKOri;

		if (GIforUpdate.nIOri + GIforUpdate.nXNum - 1 + rangeI > globalGI.nIOri + globalGI.nXNum - 1)
			GIforSampleDownload.nXNum = globalGI.nIOri + globalGI.nXNum - GIforSampleDownload.nIOri;
		else
			GIforSampleDownload.nXNum = GIforUpdate.nIOri + GIforUpdate.nXNum + rangeI - GIforSampleDownload.nIOri;

		if (GIforUpdate.nJOri + GIforUpdate.nYNum - 1 + rangeJ > globalGI.nJOri + globalGI.nYNum - 1)
			GIforSampleDownload.nYNum = globalGI.nJOri + globalGI.nYNum - GIforSampleDownload.nJOri;
		else
			GIforSampleDownload.nYNum = GIforUpdate.nJOri + GIforUpdate.nYNum + rangeI - GIforSampleDownload.nJOri;

		if (GIforUpdate.nKOri + GIforUpdate.nZNum - 1 + rangeJ > globalGI.nKOri + globalGI.nZNum - 1)
			GIforSampleDownload.nZNum = globalGI.nKOri + globalGI.nZNum - GIforSampleDownload.nKOri;
		else
			GIforSampleDownload.nZNum = GIforUpdate.nKOri + GIforUpdate.nZNum + rangeI - GIforSampleDownload.nKOri;

		GIforSampleDownload.dXStep = globalGI.dXStep;
		GIforSampleDownload.dYStep = globalGI.dYStep;
		GIforSampleDownload.dZStep = globalGI.dZStep;

		GIforSampleDownload.dXOri = globalGI.dXOri + globalGI.dXStep*(GIforSampleDownload.nIOri - globalGI.nIOri);
		GIforSampleDownload.dYOri = globalGI.dYOri + globalGI.dYStep*(GIforSampleDownload.nJOri - globalGI.nJOri);
		GIforSampleDownload.dZOri = globalGI.dZOri;

		vecGIforSampleDownload.push_back(GIforSampleDownload);
	}
	else //如果总量超过上限，均分网格为不同区域块（一分四）
	{
		int nBlockStepI = (nMaxI - nMinI + 1) / 2;
		int nBlockStepJ = (nMaxJ - nMinJ + 1) / 2;
		int nBlockStepK = (nMaxK - nMinK + 1) / 2;

		for (int k = 0; k < 2; k++)
		{
			for (int j = 0; j < 2; j++)
			{
				for (int i = 0; i < 2; i++)
				{
					//确定每个区域块的范围
					int nMinI_Block, nMinJ_Block, nMinK_Block, nMaxI_Block, nMaxJ_Block, nMaxK_Block;

					nMinI_Block = nMinI + i*nBlockStepI;
					nMinJ_Block = nMinJ + j*nBlockStepJ;
					nMinK_Block = nMinK + k*nBlockStepK;

					//田字型块：左侧块最远点用步长定，右侧块则为范围的边界
					if (i == 0)
						nMaxI_Block = nMinI_Block + nBlockStepI - 1;
					else
						nMaxI_Block = nMaxI;

					if (j == 0)
						nMaxJ_Block = nMinJ_Block + nBlockStepJ - 1;
					else
						nMaxJ_Block = nMaxJ;

					if (k == 0)
						nMaxK_Block = nMinK_Block + nBlockStepK - 1;
					else
						nMaxK_Block = nMaxK;

					//将更新的样本数据分配到每个区域块
					vector<Cell> vecUpdateData_Block;
					for (auto cell : vecUpdateData)
					{
						if (cell.nI >= nMinI_Block && cell.nI <= nMaxI_Block
							&& cell.nJ >= nMinJ_Block && cell.nJ <= nMaxJ_Block
							&& cell.nK >= nMinK_Block && cell.nK <= nMaxK_Block)
						{
							vecUpdateData_Block.push_back(cell);
						}
					}

					//如果该区域块没有样本数据，则跳过
					if (vecUpdateData_Block.empty())
						continue;

					//如果区块含有样本数据，则继续评估该区块样本数据（递归本函数）
					EsEvaluateAreaForUpdate(vecUpdateData_Block, rangeI, rangeJ, rangeK, globalGI, vecGIforSampleDownload, vecGIforUpdate);
				}
			}
		}
	}

	return true;
}