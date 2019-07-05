/*************************************************************************
* All rights reserved.
* 
* �ļ����ƣ�EsModelUpdateServerBase.h
* ժ    Ҫ��ģ���Զ����·�����࣬��ͬ����ʽ�ɴӴ����������ж���
* 
* ��    �ߣ��ο�ǿ
* ������ڣ�2018/12/27
*************************************************************************/

#pragma once
#include "EsGlobalDef.h"
#include "EsModelInterToolDll.h"
#include "windows.h"

#define  EXPORTINTERRESULT //����ֵ���д�����ļ�
#define  EXPORTSCALEUPRESULT //���ֻ����д�����ļ�
//#define MATCHVALUE //ֻ�е����������ֵ�б仯�ŵ�������

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

		//Step1: ��������Ƿ��и���
		virtual bool EsCheckIsUpdated();

		//Step2: ��ȡ���µ���������
		virtual bool EsGetUpdatedSampleData();

		/**
		* @Function:    Step3: ��������������Ӱ�췶Χ,�������·�Χ������������Χ
		* @Param [in]:  vecUpdateData                  ���µ������㼯
	    * @Param [in]:  rangeI                         I����������Χ�����������ƣ�
	    * @Param [in]:  rangeJ                         J����������Χ�����������ƣ�
	    * @Param [in]:  rangeK                         K����������Χ�����������ƣ�
	    * @Param [in]:  globalGI                       ���µ����Χ������Ϣ�������������������鷶Χ��
	    * @Param [out]: vecGIforSampleDownload         ����������Χ����
	    * @Param [out]: vecGIforSampleDownload         ���巶Χ����
	    * @return:      true-�����ɹ�;false-����ʧ��
	    */
		virtual bool EsEvaluate(vector<Cell>& vecUpdateData, const int rangeI, const int rangeJ, const int rangeK,
			const GridInfo& globalGI, vector<GridInfo>& vecGIforSampleDownload, vector<GridInfo>& vecGIforUpdate);

		//Step4: ��������������Χ�ڵ���������
		virtual bool EsDownLoadData();

		/**
		* @Function:    Step5: �Ը��·�Χ��������в�ֵ����
		* @Param [in]:  tmip                                                �����ֵ��������
		* @Param [out]: tmip.vecContinuousProp/tmip.vecDiscreteProp         �������/��ɢ���Բ�ֵ�����vector��С����������һ��
		* @return:      true-��ֵ�ɹ�;false-��ֵʧ��
		*/
		virtual bool EsUpdateData(ModelInterPara& tmip);

		//Step6: �����º�Ľ���ϴ�
		virtual bool EsUpLoadData(); //�ϴ����º����������

		/**
		* @Function:    д��־�ļ�
		* @Param [in]:  strLogFile                                          ��־�ļ�·��
		* @Param [in]:  strMsg                                              ��־��Ϣ  
		*/
		virtual void EsWriteToLog(const string& strLogFile, const string& strMsg);

		/**
		* @Function:    ��ô�����Ϣ
		*/
		virtual string EsGetErrorMsg() { return m_strErrorMsg; }

		/**
		* @Function:    ���ô�����Ϣ
		*/
		virtual void EsSetErrorMsg(const string& strErrorMsg) { m_strErrorMsg = strErrorMsg; }

		/**
		* @Function:    ��ȡʱ��
		*/
		virtual string EsGetTime();

	private:
		string m_strErrorMsg;

	};
}
