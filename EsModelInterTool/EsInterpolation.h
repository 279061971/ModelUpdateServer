#include "EsModelInterToolDll.h"

namespace gridworld
{
/*****************************内部过程接口**************************************/
	/**
	* @Function:    插值样本数据文件准备接口
	* @Param [in]:  tmip                    插值样本
	* @Param [out]: strInterDataFile        插值样本数据文件
	* @return:      true-准备成功;false-准备失败
	*/
	bool EsInterDataFilePrep(ModelInterPara& tmip, string& strInterDataFile);

	/**
	* @Function:    插值参数文件准备接口
	* @Param [in]:  tmip                    插值参数
	* @Param [in]:  strInterDataFile        插值样本数据文件
	* @Param [out]: strInterParaFile        插值参数文件
	* @Param [out]: strResultFile           插值结果保存文件
	* @return:      true-准备成功;false-准备失败
	*/
	bool EsInterParaFilePrep(ModelInterPara& tmip, const string& strInterDataFile, string& strInterParaFile, string& strResultFile);

	/**
	* @Function:    插值计算接口
	* @Param [in]:  tmip                                               插值参数
	* @Param [in]:  strInterParaFile                                   插值参数文件
	* @Param [in]:  strResultFile                                      插值结果保存文件
	* @Param [out]: tmip.vecContinuousProp/tmip.vecDiscreteProp        插值结果
	* @return:      true-插值成功;false-插值失败
	*/
	bool EsInterpolation(ModelInterPara& tmip, const string& strInterParaFile, const string& strResultFile);


/*****************************不同插值算法参数文件准备函数**************************************/
	/**
	* @Function:    准备序贯高斯模拟参数文件
	* @Param [in]:  tmip                    插值参数
	* @Param [in]:  strInterDataFile        插值样本数据文件
	* @Param [out]: strInterParaFile        插值参数文件
	* @Param [out]: strResultFile           插值结果保存文件
	* @return:      true-准备成功;false-准备失败
	*/
	bool EsPrepSgsimParaFile(ModelInterPara& tmip, const string& strInterDataFile, string& strInterParaFile, string& strResultFile);

	/**
	* @Function:    准备三维克里金参数文件
	* @Param [in]:  tmip                    插值参数
	* @Param [in]:  strInterDataFile        插值样本数据文件
	* @Param [out]: strInterParaFile        插值参数文件
	* @Param [out]: strResultFile           插值结果保存文件
	* @return:      true-准备成功;false-准备失败
	*/
	bool EsPrepKt3dParaFile(ModelInterPara& tmip, const string& strInterDataFile, string& strInterParaFile, string& strResultFile);

	/**
	* @Function:    准备序贯指示模拟参数文件
	* @Param [in]:  tmip                    插值参数
	* @Param [in]:  strInterDataFile        插值样本数据文件
	* @Param [out]: strInterParaFile        插值参数文件
	* @Param [out]: strResultFile           插值结果保存文件
	* @return:      true-准备成功;false-准备失败
	*/
	bool EsPrepSisimParaFile(ModelInterPara& tmip, const string& strInterDataFile, string& strInterParaFile, string& strResultFile);

	/**
	* @Function:    准备指示克里金参数文件
	* @Param [in]:  tmip                    插值参数
	* @Param [in]:  strInterDataFile        插值样本数据文件
	* @Param [out]: strInterParaFile        插值参数文件
	* @Param [out]: strResultFile           插值结果保存文件
	* @return:      true-准备成功;false-准备失败
	*/
	bool EsPrepIk3dParaFile(ModelInterPara& tmip, const string& strInterDataFile, string& strInterParaFile, string& strResultFile);


/*****************************辅助函数**************************************/
	/**
	* @Function:    检查属性类型与所选插值算法是否匹配
	* @Param [in]:  nAttrType               属性类型
	* @Param [in]:  nInterMethod            插值方法
	* @return:      true-匹配;false-不匹配
	*/
	bool EsCheckInterMethod(const AttrType nAttrType, const InterMethod nInterMethod);

	/**
	* @Function:    获取样本数据的最大和最小值
	* @Param [in]:  strInterDataFile        插值样本数据文件
	* @Param [out]: dMin                    样本最小值
	* @Param [out]: dMax                    样本最大值
	* @return:      true-获取成功;false-获取失败
	*/
	bool EsGetSampleDataRange(const string& strInterDataFile, double& dMin, double& dMax);

	/**
	* @Function:    计算当前相在样本数据中的比例
	* @Param [in]:  strInterDataFile        插值样本数据文件
	* @Param [out]: nCode                   相代码
	* @return:      相在样本数据中的比例
	*/
	double EsGetCodePercent(const string& strInterDataFile, const int nCode);

	/**
	* @Function:    生成奇数随机种子
	* @return:      一个奇数
	*/
	int EsGetOddRandomNum();

	/**
	* @Function:    字符串分割
	* @Param [in]:  source                  源字符串
	* @Param [out]: dest                    分割后的字符串数组
	*/
	void split(const string& source, vector<string>& dest);

	/**
	* @Function:    删除工作目录下的临时文件
	* @Param [in]:  strWorkPath             工作目录路径
	* @return:      true-删除成功;false-删除失败
	*/
	bool EsClearDir(const string& strWorkPath);

	/**
	* @Function:    ik3d插值结果不是直接可用的属性，需要进行转换	
	* @Param [in]:  strResultFile           ik3d插值结果保存文件
	* @Param [out]: strResultFile           插值结果转换后保存文件
	* @return:      true-转换成功;false-转换失败
	*/
	bool EsTransformIk3dResult(const string& strResultFile);

	/**
	* @Function:    正态变换
	* @Param [in]:  strInterDataFile        插值样本数据文件
	* @Param [in]:  strWorkPath             工作目录路径
	* @Param [in]:  dMin                    样本最小值
	* @Param [in]:  dMax                    样本最大值
	* @Param [out]: strOutputFile           样本正态变换结果保存文件
	* @return:      true-转换成功;false-转换失败
	*/
	bool EsNormalScore(const string& strOutputFile, const string& strInterDataFile, const string& strWorkPath, double& dMin, double& dMax);

	/**
	* @Function:    正态变换
	* @Param [in]:  strResultFile           插值结果数据文件
	* @Param [in]:  strWorkPath             工作目录路径
	* @Param [in]:  dMin                    插值结果最小值
	* @Param [in]:  dMax                    插值结果最大值
	* @Param [out]: strOutputFile           插值结果反正态变换的结果保存文件
	* @return:      true-转换成功;false-转换失败
	*/
	bool EsRevertNormalScore(const string& strOutputFile, const string& strResultFile, const string& strWorkPath, double& dMin, double& dMax);

	/**
	* @Function:    重采样
	* @Param [in]:  tmip                                               插值参数
	* @Param [in]:  strResultFile                                      插值结果保存文件
	* @Param [out]: tmip.vecContinuousProp/tmip.vecDiscreteProp        重采样保存的插值结果
	* @return:      true-重采样成功;false-重采样失败
	*/
	bool EsResample(ModelInterPara& tmip, const string& strResultFile); //
}