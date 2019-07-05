/*************************************************************************
* All rights reserved.
* 
* 文件名称：EsModelUpdateServerBase.h
* 摘    要：模型自动更新服务基类，不同管理方式可从此类派生进行定制
* 
* 作    者：何俊强
* 完成日期：2018/12/27
*************************************************************************/

#pragma once
#include "EsGlobalDef.h"
#include "EsModelInterToolDll.h"
#include "windows.h"

#define  EXPORTINTERRESULT //将插值结果写出到文件
#define  EXPORTSCALEUPRESULT //将粗化结果写出到文件
//#define MATCHVALUE //只有当网格的属性值有变化才当作更新

namespace gridworld
{
	class EXPORTMODELUPDATESERVER CModelUpdateServerBase
	{
	public:
		CModelUpdateServerBase();
		~CModelUpdateServerBase();

	public:

		virtual void EsStartServer();

	public:

		//Step1: 检测数据是否有更新
		virtual bool EsCheckIsUpdated();

		//Step2: 获取更新的样本数据
		virtual bool EsGetUpdatedSampleData();

		/**
		* @Function:    Step3: 评估更新样本的影响范围,包括更新范围和所需样本范围
		* @Param [in]:  vecUpdateData                  更新的样本点集
	    * @Param [in]:  rangeI                         I方向搜索范围（以网格数计）
	    * @Param [in]:  rangeJ                         J方向搜索范围（以网格数计）
	    * @Param [in]:  rangeK                         K方向搜索范围（以网格数计）
	    * @Param [in]:  globalGI                       更新的最大范围网格信息（样本数据所属的区块范围）
	    * @Param [out]: vecGIforSampleDownload         所需样本范围集合
	    * @Param [out]: vecGIforSampleDownload         待插范围集合
	    * @return:      true-评估成功;false-评估失败
	    */
		virtual bool EsEvaluate(vector<Cell>& vecUpdateData, const int rangeI, const int rangeJ, const int rangeK,
			const GridInfo& globalGI, vector<GridInfo>& vecGIforSampleDownload, vector<GridInfo>& vecGIforUpdate);

		//Step4: 下载所需样本范围内的样本数据
		virtual bool EsDownLoadData();

		/**
		* @Function:    Step5: 对更新范围的网格进行插值计算
		* @Param [in]:  tmip                                                网格插值所需数据
		* @Param [out]: tmip.vecContinuousProp/tmip.vecDiscreteProp         存放连续/离散属性插值结果，vector大小与网格数量一致
		* @return:      true-插值成功;false-插值失败
		*/
		virtual bool EsUpdateData(ModelInterPara& tmip);

		//Step6: 将更新后的结果上传
		virtual bool EsUpLoadData(); //上传更新后的属性数据

		/**
		* @Function:    写日志文件
		* @Param [in]:  strLogFile                                          日志文件路径
		* @Param [in]:  strMsg                                              日志信息  
		*/
		virtual void EsWriteToLog(const string& strLogFile, const string& strMsg);

		/**
		* @Function:    获得错误信息
		*/
		virtual string EsGetErrorMsg() { return m_strErrorMsg; }

		/**
		* @Function:    设置错误信息
		*/
		virtual void EsSetErrorMsg(const string& strErrorMsg) { m_strErrorMsg = strErrorMsg; }

		/**
		* @Function:    获取时间
		*/
		virtual string EsGetTime();

	private:
		string m_strErrorMsg;

	};
}
