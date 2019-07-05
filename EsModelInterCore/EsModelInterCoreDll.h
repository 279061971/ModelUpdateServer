/*************************************************************************
* Copyright (c) 2018,北京网格天地软件技术股份有限公司
* All rights reserved.
* 
* 文件名称：EsModelInterCoreDll.h
* 摘    要：模型自动更新插值计算SDK
* 
* 作    者：何俊强
* 完成日期：2018年11月22日
*************************************************************************/

#include <string>
using std::string;

namespace gridworld
{
	/**
	* @Function:    将插值参数文件传送给gslib程序进行插值计算
	* @Param [in]:  strInterParaFile               插值参数文件
	* @Param [in]:  nInterMethod                   插值方法
	* @return:      true-插值计算成功;false-插值计算失败
	*/
	extern "C" __declspec(dllexport) bool EsInterFunc(const string& strInterParaFile, int nInterMethod); //插值核心计算

}