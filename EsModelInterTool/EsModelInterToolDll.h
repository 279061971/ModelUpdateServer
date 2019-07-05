/*************************************************************************
* Copyright (c) 2018,北京网格天地软件技术股份有限公司
* All rights reserved.
* 
* 文件名称：EsModelInterToolDll.h
* 摘    要：模型自动更新插值计算SDK
* 
* 作    者：何俊强
* 完成日期：2018年11月22日
*************************************************************************/

#include <vector>
#include <string>
#include <map>

using std::vector;
using std::string;
using std::map;


namespace gridworld
{
	//单次插值网格数量上限(暂时定为1千万)
	const int gLimit = 1e7;

	//属性类型集合
	enum AttrType
	{
		Continuous, //连续
		Discrete //离散
	};

	//变差函数类型
	enum VariogramType
	{
		Spherical = 1, //球型
		Exponential, //指数型
		Gaussian //高斯型	
	};

	//插值算法
	enum InterMethod
	{
		SequentialGaussianSimulation, //连续：序贯高斯模拟
		Kriging, //连续：克里金插值

		SequentialIndicatorSimulation, //离散：序贯指示模拟
		IndicatorKriging //离散：指示克里金
	};

	//克里金类型
	enum KrigingType
	{
		Kriging_SK, //简单克里金
		Kriging_OK //普通克里金 
	};

	//变差函数
	struct VariogramPara
	{
		VariogramType varType; //变差函数类型（必须）
		double dSill; //基台值（必须）
		double dNugget; //块金值（必须）
		double dMajorRange; //主轴搜索范围（必须）
		double dMinorRange; //次轴搜索范围（必须）
		double dVerticalRange; //垂直搜索范围（必须）
		double dAzimuth; //搜索角度（必须）
		double dDip; //倾角（必须）

		//初始化
		VariogramPara() :varType(Exponential), dSill(1.0), dNugget(0.0001), dMajorRange(2000), dMinorRange(1800), dVerticalRange(8), dAzimuth(0), dDip(90) {}
		VariogramPara(VariogramType type, double sill, double nugget, double mRange, double nRange, double vRange, double azimuth, double dip) :
			varType(type), dSill(sill), dNugget(nugget), dMajorRange(mRange), dMinorRange(nRange), dVerticalRange(vRange), dAzimuth(azimuth), dDip(dip) {}
	};

	//离散相变差函数
	struct CodeParaSet
	{
		int nCode; //相代码（离散属性插值时-必须）
		VariogramPara varPara; //相变差函数（离散属性插值时-必须）
	};

	//插值搜索参数
	struct ExpertPara
	{
		KrigingType kType; //克里金类型（可选）
		double dSearchMulti; //搜索范围的乘数（可选）
		int nMinNumNodesUse; //使用数据源的最小数量（可选）
		int nMaxNumNodesUse; //使用数据源的最大数量（可选）

		bool bUseOctant; //是否使用象限搜索（可选）
		int nNumOctant; //当使用象限搜索时每个象限的最大搜索个数（可选）

		int nSimulatedNodesUse; //使用已生成属性网格的最大数量（可选）

		bool bIsAssignToDataNodes; //是否将样本属性匹配到网格（可选）

		bool bIsUseMulti; //是否使用多网格搜索乘数（可选）
		int nGridMultiStep; //多网格搜索乘数（可选）

		int  nSeed; //随机模拟种子（可选，序贯高斯和序贯指示需要，若用户不设置则SDK会自动生成）

		//初始化
		ExpertPara() : kType(Kriging_SK), dSearchMulti(2.0), nMinNumNodesUse(0), nMaxNumNodesUse(12), bUseOctant(false), 
			nNumOctant(2), nSimulatedNodesUse(12), bIsAssignToDataNodes(true), bIsUseMulti(false), nGridMultiStep(3), nSeed(10001) {}
		ExpertPara(KrigingType type, double searchMulti, int minNumNodesUse, int maxNumNodesUse, bool usdOctant, 
			int numOctant, int simulateNodesUsd, bool assignToDataNodes, bool useMulti, int gridMultiStep, int seed) : 
		kType(type), dSearchMulti(searchMulti), nMinNumNodesUse(minNumNodesUse), nMaxNumNodesUse(maxNumNodesUse), bUseOctant(useMulti), nNumOctant(numOctant), 
			nSimulatedNodesUse(simulateNodesUsd), bIsAssignToDataNodes(assignToDataNodes), bIsUseMulti(useMulti), nGridMultiStep(gridMultiStep), nSeed(seed) {}
	};

	//序贯高斯模拟数据变换参数
	struct DistributedPara
	{
		bool bIsTrans; //是否对样本数据进行正态变换（默认为进行正态变换）
		double dOutputMin; //输出值最小值设置（默认，SDK自动计算）
		double dOutputMax; //输出值最大值设置（默认，SDK自动计算）

		DistributedPara(): bIsTrans(true), dOutputMin(0), dOutputMax(0){}
	};

	//待插范围网格信息
	struct GridInfo
	{
		int nXNum; //x方向个数
		int nYNum; //y方向个数
		int nZNum; //z方向个数

		double dXStep; //x方向步长
		double dYStep; //y方向步长
		double dZStep; //Z方向平均步长 = 地层平均厚度/nZNum

		double dXOri; //起始网格中心点x坐标 
		double dYOri; //起始网格中心点y坐标
		double dZOri; //起始网格中心点z坐标 = zStep/2

		int nIOri; //I，起始网格全局I
		int nJOri; //J，起始网格全局J
		int nKOri; //K，起始网格全局K

		GridInfo() : nXNum(0), nYNum(0), nZNum(0), dXStep(0), dYStep(0), dZStep(0), dXOri(0), dYOri(0), dZOri(0), nIOri(0), nJOri(0), nKOri(0) {}
		GridInfo(int xNum, int yNum, int zNum, double xStep, double yStep, double zStep, double xOri, double yOri, double zOri, int iOri, int jOri, int kOri) : 
			nXNum(xNum), nYNum(yNum), nZNum(zNum), dXStep(xStep), dYStep(yStep), dZStep(zStep), dXOri(xOri), dYOri(yOri), dZOri(zOri), nIOri(iOri), nJOri(jOri), nKOri(kOri) {}
	};

	//单一网格
	struct Cell
	{
		int nI; //I，从0开始
		int nJ; //J，从0开始
		int nK; //K，,从上至下，从0递增
		double dValue; //连续属性值
		int nValue; //离散属性相ID

		Cell() : nI(0), nJ(0), nK(0), dValue(0), nValue(0){}
		Cell(int i, int j, int k, double value) : nI(i), nJ(j), nK(k), dValue(value), nValue(0) {}
		Cell(int i, int j, int k, int value) : nI(i), nJ(j), nK(k), dValue(0), nValue(value) {}
	};

	//一条钻孔记录
	struct Record
	{
		double dX; //x坐标
		double dY; //y坐标
		double dZ; //z坐标
		string strValue; //属性值

		Record() : dX(0.0), dY(0.0), dZ(0.0) {}
		Record(double x, double y, double z, string value) : dX(x), dY(y), dZ(z), strValue(value) {}
	};

	//网格插值所需数据
	struct ModelInterPara
	{
		//工作目录
		string strWorkPath; //用户指定，SDK会在此目录下创建TEMP文件，用于存放插值计算过程中产生的临时文件（必须）,结束后TEMP文件夹会被删除

		//插值区域的网格与样本
		AttrType nAttrType; //属性类型（必须）
		GridInfo gi; //网格信息（必须）
		vector<Cell> vecCell; //样本网格集（必须）

		//插值参数
		InterMethod nInterMethod; //插值算法（必须）
		ExpertPara expertPara; //搜索参数（见具体参数）
		DistributedPara distPara; //连续属性正态变换参数（序贯高斯-必须，其他-默认）

		//变差函数
		VariogramPara varPara; //连续属性变差数据（连续-必选，离散-默认）		
		vector<CodeParaSet> faciesPara; //离散属性每个相的变差函数参数集合（离散-必须，连续-默认）

		//插值结果
		vector<double> vecContinuousProp; //用于存储全区网格连续属性值（默认）
		vector<int> vecDiscreteProp; //用于存储全区网格离散属性值（默认）
	};

	/**
	* @Function:    使用参数中的样本及插值参数计算参数网格中的每一个网格属性值
	* @Param [in]:  tmip                                                网格插值所需数据
	* @Param [out]: tmip.vecContinuousProp/tmip.vecDiscreteProp         存放连续/离散属性插值结果，vector大小与网格数量一致
	* @return:      true-插值成功;false-插值失败
	*/
	extern "C" __declspec(dllexport) bool EsInterForModel(ModelInterPara& tmip);

	/**
	* @Function:    从vecDiscreteProp中提取对应网格（I,J,K）的属性值
	* @Param [in]:  tmip.vecDiscreteProp         离散属性插值结果，vector大小与网格数量一致
	* @Param [in]:  i                            网格坐标I
	* @Param [in]:  j                            网格坐标J
	* @Param [in]:  k                            网格坐标K
	* @return:      离散属性值
	*/
	extern "C" __declspec(dllexport) int EsGetDiscretePropertyByIJK(ModelInterPara& tmip, const int i, const int j, const int k);

	/** 
	* @Function:    从vecContinuousProp中提取对应网格（I,J,K）的属性值
	* @Param [in]:  tmip.vecContinuousProp       连续属性插值结果，vector大小与网格数量一致
	* @Param [in]:  i                            网格坐标I
	* @Param [in]:  j                            网格坐标J
	* @Param [in]:  k                            网格坐标K
	* @return:      连续属性值
	*/
	extern "C" __declspec(dllexport) double EsGetContinuousPropertyByIJK(ModelInterPara& tmip, const int i, const int j, const int k);

	/**
	* @Function:    评估更新样本的影响范围,包括更新范围和所需样本范围
	* @Param [in]:  vecUpdateData                  更新的样本点集
	* @Param [in]:  rangeI                         I方向搜索范围（以网格数计）
	* @Param [in]:  rangeJ                         J方向搜索范围（以网格数计）
	* @Param [in]:  rangeK                         K方向搜索范围（以网格数计）
	* @Param [in]:  globalGI                       更新的最大范围网格信息（样本数据所属的区块范围）
	* @Param [out]: vecGIforSampleDownload         所需样本范围集合
	* @Param [out]: vecGIforSampleDownload         待插范围集合
	* @return:      true-评估成功;false-评估失败
	*/
	extern "C" __declspec(dllexport) bool EsEvaluateAreaForUpdate(vector<Cell>& vecUpdateData, const int rangeI, const int rangeJ, const int rangeK,
		const GridInfo& globalGI, vector<GridInfo>& vecGIforSampleDownload, vector<GridInfo>& vecGIforUpdate);

}