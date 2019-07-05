/*************************************************************************
* All rights reserved.
* 
* �ļ����ƣ�EsModelUpdateServerGS.h
* ժ    Ҫ���ص��ֶ���ģ���Զ����·��������
* 
* ��    �ߣ��ο�ǿ
* ������ڣ�2019/03/27
*************************************************************************/

#pragma once
#include <windows.h>
#include "EsModelUpdateServerBase.h"
#include "tinyxml.h"


namespace gridworld
{
	namespace GS_TwoMFN
	{
		//ģ�����ݷ��������������
		const int DownLoadLimit = 10000; //�еص���������������
		const int UpLoadLimit = 1000; //���������ϴ����ޣ���Ϊ�趨��Ϊ�˽����ϴ�������
		const int SleepTime = 5000; //����ʱ��
		const string strDataType_int = "int"; //��������-int
		const string strDataType_double = "double"; //��������-double

		const string strMatchedDataCenter_DataUpdate = "DataUpdate"; //�ص��ָ���Ŀ¼�µĸ��������ļ�����
		const string strMatchedDataCenter_DataOrigin = "DataOrigin"; //�ص��ָ���Ŀ¼�µ�����ԭʼ�����ļ�����
		const string strMatchedDataCenter_InterResult = "InterResult"; //�ص��ָ���Ŀ¼�µĲ�ֵ�������ļ�����
		const string strMatchedDataCenter_ScaleUp = "ScaleUp"; //�ص��ָ���Ŀ¼�µĴֻ��������ļ�����
		const string strMatchedDataCenter_Config = "ModelUpdateConfig_TwoMFN.xml"; //�ص��ָ���Ŀ¼�µĸ��������ļ���

		//�Զ����������ļ��������
		const char* strConfigSetting = "setting";
		const char* strConfigSetting_RootURL = "RootURL";
		const char* strConfigSetting_MajorMatchedFieldName = "MajorMatchedFieldName"; //���ڿ��Ʒ�Χ���������У��������
		const char* strConfigSetting_MinorMatchedFieldName = "MinorMatchedFieldName"; //���ڿ��Ʒ�Χ�Ĵ������У�������

		const char* strConfigProperty = "property";
		const char* strConfigProperty_PropName = "PropName";
		const char* strConfigProperty_PropType = "PropType";
		const char* strConfigProperty_Lod = "Lod";
		const char* strConfigProperty_ModelID = "CoverageID";

		const char* strConfigMajor = "major";
		const char* strConfigMajor_MajorID = "MajorID";

		const char* strConfigMinor = "minor";
		const char* strConfigMinor_MinorID = "MinorID";
		const char* strConfigMinor_InterpolationMethod = "InterpolationMethod";

		const char* strConfigVariogram = "variogram";
		const char* strConfigVariogram_FaceCode = "FaceCode";
		const char* strConfigVariogram_VariogramType = "VariogramType";
		const char* strConfigVariogram_Sill = "Sill";
		const char* strConfigVariogram_Nugget = "Nugget";
		const char* strConfigVariogram_MajorRange = "MajorRange";
		const char* strConfigVariogram_MinorRange = "MinorRange";
		const char* strConfigVariogram_VerticalRange = "VerticalRange";
		const char* strConfigVariogram_Azimuth = "Azimuth";
		const char* strConfigVariogram_Dip = "Dip";

		//�����Ϣ
		const string strMsg_NoUpdate = "δ��⵽����";
		const string strMsg_StartServer = "*********************�����Զ����·���************************";
		const string strMsg_CheckUpdate = "���ڼ����ʴ����������Ƿ��и���";
		const string strMsg_CheckUpdate_first = "��⵽���������и��£�";
		const string strMsg_CheckUpdate_second = ",���ڻ�ȡ���µ���ף����ֻ�������...";
		const string strMsg_UpdateBorehole_Success = "���µ���״ֻ��ɹ�";
		const string strMsg_UpLoadingCell = "\t\t\t\t���ڽ����µ�����ϴ������ݿ�...";
		const string strMsg_UpLoadCellSuccess = "\t\t\t\t���µ�����ϴ������ݿ�ɹ��������������µ���׵�Ӱ�췶Χ...";
		const string strMsg_DownloadCell = "\t\t\t\t\t�������ز�ֵ�õ���������ȴ�...";
		const string strMsg_DownloadCellSuccess = "\t\t\t\t\t��ֵ�������سɹ������ڽ��в�ֵ����...";
		const string strMsg_InterpolationFailed = "\t\t\t\t\t���󣺲�ֵ����ʧ�ܣ������·�Χ�ĸ��½�������";
		const string strMsg_InterpolationSuccess = "\t\t\t\t\t��ֵ����ɹ������ڸ��²�ֵ��������ݿ�...";
		const string strMsg_CloseServer = "*********************�Զ����½���������ر�************************\n";

		const char* strMsg_UpdateProp = "\t���ڸ��µ�%d�����ԣ�%s,ʣ��%d�����Դ�����";
		const char* strMsg_UpdateLod = "\t\t���ڸ��µ�%d��LOD��Lod=%d,ʣ��%d��LOD������";
		const char* strMsg_UpdateMajor = "\t\t\t���ڸ�����ɸѡ����%s�ĵ�%d�����Ʒ�Χ��%d,ʣ��%d�����Ʒ�Χ������";
		const char* strMsg_UpdateMinor = "\t\t\t\t���ڸ��´�ɸѡ����%s�ĵ�%d�����Ʒ�Χ��%d,ʣ��%d�����Ʒ�Χ������";
		const char* strMsg_UpdateMinor_NoUpdate = "\t\t\t\t��ɸѡ����%s�Ŀ��Ʒ�Χ��%dû�и��µ���ף�������";
		const char* strMsg_UpLoad_Failed = "\t\t\t\t%s ���󣺴�ɸѡ����%s�Ŀ��Ʒ�Χ��%d���µ�����޷����µ����ݿ⣬����Χ�ĸ��½�������";
		const char* strMsg_GetVariogram_Failed = "\t\t\t\t���棺��ɸѡ����%s�Ŀ��Ʒ�Χ��%d�ı�����ȡʧ�ܣ���ʹ��Ĭ�ϱ����м���";
		const char* strMsg_EvaluateArea_Failed = "\t\t\t\t���󣺴�ɸѡ����%s�Ŀ��Ʒ�Χ��%d�ĸ��·�Χ����ʧ�ܣ�����Χ�ĸ��½�������,�������������ı���Ƿ����";
		const char* strMsg_EvaluateArea_Success = "\t\t\t\t��ɸѡ����%s�Ŀ��Ʒ�Χ��%d�ĸ��·�Χ�����ɹ�������%d�������·�Χ";
		const char* strMsg_GetInterMethod_Failed = "\t\t\t\t���󣺴������ļ��л�ȡ��ɸѡ����%s�Ŀ��Ʒ�Χ��%d�Ĳ�ֵ�㷨ʧ�ܣ�����Χ�ĸ��½�������";
		const char* strMsg_UpdateArea = "\t\t\t\t\t����ִ�е�%d�����·�Χ�Ĳ�ֵ����,��ֵ��Χ��IOri=%d JOri=%d KOri=%d XNum=%d YNum=%d ZNum=%d��ʣ��%d�����·�Χ����ֵ";
		const char* strMsg_DownloadingCell_Failed = "\t\t\t\t\t%s �������ز�ֵ����ʧ�ܣ������·�Χ�ĸ��½�������";
		const char* strMsg_UploadCell_Failed = "\t\t\t\t\t%s ���󣺲�ֵ������µ����ݿ�ʧ�ܣ������·�Χ�ĸ��½�������";
		const char* strMsg_UploadCell_Success = "\t\t\t\t\t��ֵ������µ����ݿ�ɹ�";
		const char* strMsg_GetFaceCode_Failed = "\t\t\t\t���󣺴������ļ��л�ȡ���ԣ�%s����ɢ��ʧ�ܣ�����Χ�ĸ��½�������\n";
		const char* strMsg_GetFaceCode_NULL = "\t\t\t\t���棺���ԣ�%s����ɢ������Ϊ0������Χ�ĸ��½�������";
		const char* strMsg_ExtractLodProp_Start = "\t\t\t\t\t���ڽ���ǰLOD�Ĳ�ֵ����ֻ����μ�LOD";
		const char* strMsg_ExtractLodProp_End = "\t\t\t\t\t��ǰLOD�Ĳ�ֵ����ֻ����μ�LOD����";

		const string strErrorMsg_FailedToReadBoreholeFile = "��������ļ���ȡʧ�ܣ������ļ��Ƿ���ڻ����ļ���ʽ�Ƿ���ȷ";
		const string strErrorMsg_FailedToReadConfig = "����ģ���Զ����������ļ���ȡʧ�ܣ������ļ��Ƿ���ڻ����ļ���ʽ�Ƿ���ȷ";
		const string strErrorMsg_FailedToGetGridInfoCountFromDataBase_NoConnect = "���󣺴����ݿ��ȡ���Գ�����ʧ��";
		const string strErrorMsg_FailedToGetGridInfoFromDataBase_NoConnect = "���󣺴����ݿ��ȡ������Ϣʧ��";
		const string strErrorMsg_FailedToGetGridInfoFromDataBase_NoMatched = "�������ݿ���û��ƥ��ģ�͵�������Ϣ";
		const string strErrorMsg_SelectToLayer_FailedToGetMajorCellFromDataBase = "���󣺽���������ɸѡ���ز㲽��-�����ݿ�������ɸѡ���Ե�����ʧ��";
		const string strErrorMsg_SelectToLayer_FailedToGetMinorCellFromDataBase = "���󣺽���������ɸѡ���ز㲽��-�����ݿ����ش�ɸѡ���Ե�����ʧ��";
		const string strErrorMsg_SelectToLayer_FailedToGetPropCellFromDataBase = "���󣺽���������ɸѡ���ز㲽��-�����ݿ����ش��������Ե�����ʧ��";
		const string strErrorMsg_UpLoadData_FailedToUploadCell = "�����ϴ��������ݿ�ʧ��";
		const string strErrorMsg_UpLoadData_FailedToDownloadInterAreaCell = "�����ϴ��������ݿⲽ��-�����ݿ����ز�ֵ��Χ������ʧ��";
		const string strErrorMsg_DownLoadData_MajorMatchedFieldNoData = "�������ݿ��е�ǰ��Χû����Լ�����Ե�ֵ�����޷�������LOD��ȡ";
		const string strErrorMsg_DownLoadData_MinorMatchedFieldNoData = "�������ݿ��е�ǰ��Χû�д�Լ�����Ե�ֵ�����޷�������LOD��ȡ";
		const string strErrorMsg_DownLoadData_FailedToDownLoadCell = "���󣺴����ݿ���������ʧ��";
		const string strErrorMsg_DownLoadData_NoCellDownLoad = "���󣺴����ݿ����ص���������Ϊ0";


		class EXPORTMODELUPDATESERVER CModelUpdateServerGS_TwoMFN : public CModelUpdateServerBase
		{
		public:
			CModelUpdateServerGS_TwoMFN();
			~CModelUpdateServerGS_TwoMFN();

		public:

			virtual int EsStartServer(const string& strTargetDir);

		public:

			/**
			* @Function:    ��Ŀ��Ŀ¼�����������Ƿ��и���
			* @Param [in]:  strTargetDir                  ������и�����Ϣ��Ŀ��Ŀ¼
			* @Param [out]: vecStrUpdatePropName          ���µ���������
			* @return:      true-�и���;false-�޸���
			*/
			virtual bool EsCheckIsUpdated(const string& strTargetDir, vector<string>& vecStrUpdatePropName);

			/**
			* @Function:    ��Ŀ��Ŀ¼��ȡ������ף����ֻ�������
			* @Param [in]:  strTargetDir                  ������и�����Ϣ��Ŀ��Ŀ¼
			* @Param [in]:  strConfigPath                 �Զ����������ļ�·��
			* @Param [in]:  strRootURL                    ���ݷ������ĸ�·��
			* @Param [in]:  strMajorMatchedFieldName      ����ɸѡ������������������ƣ�һ��Ϊ�ز�
			* @Param [in]:  strMinorMatchedFieldName      ����ɸѡ����Ĵ������������ƣ�һ��Ϊ����
			* @Param [in]:  vecStrUpdatePropName          ���µ���������
			* @Param [out]: vecAttrType                   ���µ��������ͼ�            ����/Lod
			* @Param [out]: vecCellSet                    ���µ�����������            ����/Lod
			* @Param [out]: vecCellAfterUpdate            ���º������������          ����/Lod
			* @Param [out]: vecStrCoverageID              ���µ����Զ�Ӧ�����Գ���    ����/Lod
			* @Param [out]: vecStrShapeID                 ���µ����Զ�Ӧ�ļ��γ���    ����/Lod
			* @Param [out]: vecStrFeatureID               ���µ����Զ�Ӧ��Ҫ��ID��    ����/Lod
			* @Param [out]: vecLodSet                     ���µ����Զ�Ӧ��LOD��       ����/Lod
			* @Param [out]: vecLayerIDSet                  ���µ����Զ�Ӧ�ĵز�ID��    ����/Lod
			* @Param [out]: vecGISet                      ���µ����Զ�Ӧ������      ����/Lod
			* @return:      true-��ȡ�ɹ�;false-��ȡʧ��
			*/
			virtual bool EsGetUpdatedSampleData(const string& strTargetDir, const string& strConfigPath, const string& strMajorMatchedFieldName, const string& strMinorMatchedFieldName, const vector<string>& vecStrUpdatePropName, vector<vector<AttrType>>& vecAttrType, vector<vector<vector<vector<vector<Cell>>>>>& vecCellSet,
				vector<vector<vector<Cell>>>& vecCellAfterUpdate, vector<vector<string>>& vecStrModelID, vector<vector<int>>& vecLodSet, vector<vector<int>>& vecMajorIDSet, vector<vector<vector<int>>>& vecMinorIDSet, vector<vector<GridInfo>>& vecGISet);

			/**
			* @Function:    �����ݷ������Ļ�ȡ��ֵ����
			* @Param [in]:  strTargetDir                  ������и�����Ϣ��Ŀ��Ŀ¼
			* @Param [in]:  strPropName                   ������
			* @Param [in]:  nAttrType                     ��������
			* @Param [in]:  strMatchedFieldName           ����ɸѡ������������������ƣ�һ��Ϊ�ز�
			* @Param [in]:  strMinorMatchedFieldName      ����ɸѡ����Ĵ������������ƣ�һ��Ϊ����
			* @Param [in]:  strLayerID                    �ز�ID
			* @Param [in]:  giforSampleDownload           ������������Χ
			* @Param [in]:  giforUpdate                   �����µ�����Χ
			* @Param [in]:  bStochastic                   �Ƿ������ģ�⣺true-��;false-��
			* @Param [in]:  strRootURL                    ���ݷ������ĵĸ�·��
			* @Param [in]:  strCoverageID                 ���Գ�ID
			* @Param [in]:  vecCellOrigin                 ���Բ�ֵǰ������������
			* @Param [out]: vecCell                       ��ȡ�Ĳ�ֵ����
			* @return:      true-��ȡ�ɹ�;false-��ȡʧ��
			*/
			virtual bool EsDownLoadData(const string& strTargetDir, const string& strPropName, const AttrType& nAttrType, const string& strMajorMatchedFieldName, const string& strMinorMatchedFieldName, const string& strMajorID, 
				const string& strMinorID, const GridInfo& giforSampleDownload, const GridInfo& giforUpdate, bool bStochastic, const string& strModelID, const int nLod, const vector<Cell>& vecCellOrigin, vector<Cell>& vecCell);

			/**
			* @Function:    ��Cell��ʽ����������ֵ���µ����ݷ�������
			* @Param [in]:  strPropName                   ������
			* @Param [in]:  nAttrType                     ��������
			* @Param [in]:  vecCell                       ���ϴ�������
			* @Param [in]:  strRootURL                    ���ݷ������ĵĸ�·��
			* @Param [in]:  strShapeID                    ���γ�ID
			* @Param [in]:  strCoverageID                 ���Գ�ID
			* @Param [in]:  nLod                          Lod����
			* @return:      true-�ϴ��ɹ�;false-�ϴ�ʧ��
			*/
			virtual bool EsUpLoadData(const string& strPropName, const AttrType nAttrType, const vector<Cell>& vecCell, const string& strModelID, const int& nLod);

			/**
			* @Function:    ���²�ֵ�����ṹ���е�����ֵ�����ݷ�������
			* @Param [in]:  strPropName                   ������
			* @Param [in]:  nAttrType                     ��������
			* @Param [in]:  strMatchedFieldName           ����ɸѡ������������������ƣ�һ��Ϊ�ز�
			* @Param [in]:  strMinorMatchedFieldName      ����ɸѡ����Ĵ������������ƣ�һ��Ϊ����
			* @Param [in]:  strLayerID                    �ز�ID
			* @Param [in]:  tmip                          ��ֵ�����ṹ��
			* @Param [in]:  strRootURL                    ���ݷ������ĵĸ�·��
			* @Param [in]:  strShapeID                    ���γ�ID
			* @Param [in]:  strCoverageID                 ���Գ�ID
			* @Param [in]:  nLod                          Lod����
			* @return:      true-�ϴ��ɹ�;false-�ϴ�ʧ��
			*/
			virtual bool EsUpLoadData(const string& strPropName, const AttrType nAttrType, const string& strMajorMatchedFieldName, const string& strMinorMatchedFieldName, const string& strMajorID,
				const string& strMinorID, ModelInterPara& tmip, const string& strModelID, const int& nLod);

			/**
			* @Function:    ���Զ����������ļ���ȡ�������ò���
			* @Param [in]:  strConfigPath                 �����ļ�·��
			* @Param [out]: strRootURL                    ���ݷ������ĸ�·��
			* @Param [in]:  strMatchedFieldName           ����ɸѡ������������������ƣ�һ��Ϊ�ز�
			* @Param [in]:  strMinorMatchedFieldName      ����ɸѡ����Ĵ������������ƣ�һ��Ϊ����
			* @return:      true-��ȡ�ɹ�;false-��ȡʧ��
			*/
			bool EsReadServerConfigSetting(const string& strConfigPath, string& strRootURL, string& strMajorMatchedFieldName, string& strMinorMatchedFieldName);

			/**
			* @Function:    ���Զ����������ļ���ȡָ�����Ե������Ϣ
			* @Param [in]:  strConfigPath                 �����ļ�·��
			* @Param [in]:  strPropName                   ������
			* @Param [out]: vecAttrType                   �������ͼ�
			* @Param [out]: vecLod                        ���Զ�Ӧ��Lod��
			* @Param [out]: vecCoverageID                 ���Գ�ID��
			* @Param [out]: vecShapeID                    ���γ�ID��
			* @Param [out]: vecFeatureID                  Ҫ��ID��
			* @Param [out]: vecFeatureclassID             Ҫ����ID��
			* @return:      true-��ȡ�ɹ�;false-��ȡʧ��
			*/
			bool EsReadServerConfigProperty(const string& strConfigPath, const string& strPropName, vector<AttrType>& vecAttrType, vector<int>& vecLod, vector<string>& vecModelID);

			/**
			* @Function:    ���Զ����������ļ���ȡָ�����ԡ�ָ���ز�Ĳ�ֵ����
			* @Param [in]:  strConfigPath                 �����ļ�·��
			* @Param [in]:  strPropName                   ������
			* @Param [in]:  strMajorID                    �ز�ID
			* @Param [out]: nInterpolationMethod          ��ֵ����
			* @return:      true-��ȡ�ɹ�;false-��ȡʧ��
			*/
			bool EsReadServerConfigMinor(const string& strConfigPath, const string& strPropName, const int& nLod, const string& strMajorID, const string& strMinorID, InterMethod& nInterpolationMethod);

			/**
			* @Function:    ���Զ����������ļ���ȡָ�����ԡ�ָ���ز�ı���-��������������
			* @Param [in]:  strConfigPath                 �����ļ�·��
			* @Param [in]:  strPropName                   ������
			* @Param [in]:  strLayerID                    �ز�ID
			* @Param [out]: varPara                       ����
			* @return:      true-��ȡ�ɹ�;false-��ȡʧ��
			*/
			bool EsReadServerConfigVariogram(const string& strConfigPath, const string& strPropName, const int& nLod, const string& strMajorID, const string& strMinorID, VariogramPara& varPara);

			/**
			* @Function:    ���Զ����������ļ���ȡָ�����ԡ�ָ���ز㡢ָ����ı���-��������ɢ����
			* @Param [in]:  strConfigPath                 �����ļ�·��
			* @Param [in]:  strPropName                   ������
			* @Param [in]:  strLayerID                    �ز�ID
			* @Param [in]:  strFaceCode                   ��ID
			* @Param [out]: varPara                       ����
			* @return:      true-��ȡ�ɹ�;false-��ȡʧ��
			*/
			bool EsReadServerConfigVariogramByFaceCode(const string& strConfigPath, const string& strPropName, const int& nLod, const string& strMajorID, const string& strMinorID, const string& strFaceCode, VariogramPara& varPara);

			/**
			* @Function:    ���Զ����������ļ���ȡָ�����Եĵز�ID��
			* @Param [in]:  strConfigPath                 �����ļ�·��
			* @Param [in]:  strPropName                   ������
			* @Param [out]: vecLayerID                    �ز�ID��
			* @return:      true-��ȡ�ɹ�;false-��ȡʧ��
			*/
			bool EsReadServerConfigMajorID(const string& strConfigPath, const string& strPropName, const int& nLod, vector<int>& vecMajorID);

			/**
			* @Function:    ���Զ����������ļ���ȡָ�����Եĵز�ID��
			* @Param [in]:  strConfigPath                 �����ļ�·��
			* @Param [in]:  strPropName                   ������
			* @Param [out]: vecLayerID                    �ز�ID��
			* @return:      true-��ȡ�ɹ�;false-��ȡʧ��
			*/
			bool EsReadServerConfigMinorID(const string& strConfigPath, const string& strPropName, const int& nLod, vector<vector<int>>& vecMinorID);

			/**
			* @Function:    ���Զ����������ļ���ȡָ�����ԡ�ָ���ز����ID��
			* @Param [in]:  strConfigPath                 �����ļ�·��
			* @Param [in]:  strPropName                   ������
			* @Param [in]:  strLayerID                    �ز�ID
			* @Param [out]: vecFaceCode                   ��ID��
			* @return:      true-��ȡ�ɹ�;false-��ȡʧ��
			*/
			bool EsReadServerConfigFaceCode(const string& strConfigPath, const string& strPropName, const int& nLod, const string& strMajorID, const string& strMinorID, vector<int>& vecFaceCode);

			bool EsExtractPropFromCurrentLodToLowerLod(const GridInfo& gi, const string &strPropName, const string& strModelID, const int nLod);

		private:

			/**
			* @Function:    ��ģ�����ݷ������Ļ�ȡ���γ�������Ϣ
			* @Param [in]:  strRootURL                    ���ݷ������ĵĸ�·��
			* @Param [in]:  strShapeID                    ���γ�ID
			* @Param [in]:  nLod                          Lod����
			* @Param [out]: gi                            ���γ���Ӧ��������Ϣ
			* @return:      true-��ȡ�ɹ�;false-��ȡʧ��
			*/
			bool EsGetGlobalGridInfoFromModelDataBase(const string& strModelID, const int& nLod, GridInfo& gi);

			/**
			* @Function:    ���������ɸѡ������
			* @Param [in]:  nAttrType                     ��������
			* @Param [in]:  vecRecord                     �����������
			* @Param [in]:  gi                            ����ɸѡ������
			* @Param [in]:  bAdd                          �Ƿ�����map����û�е����ݶԣ�true-���ӣ�false-������
			* @Param [out]: mapCellIdAndCellSet           ����ID�Ͷ�Ӧ��������ݼ�
			* @return:      true-ɸѡ�ɹ�;false-ɸѡʧ��
			*/
			bool EsSelectRecordToCell(const AttrType& nAttrType, const vector<Record>& vecRecord, const GridInfo& gi, bool& bAdd, map<int, vector<Cell>*>& mapCellIdAndCellSet);

			/**
			* @Function:    ������ͬһ������������������ݹ�һ��
			* @Param [in]:  nAttrType                     ��������
			* @Param [in]:  mapCellIdAndCellSet           ����ID�Ͷ�Ӧ��������ݼ�
			* @Param [out]: vecCell                       ��һ������������ݼ�
			* @return:      true-��һ���ɹ�;false-��һ��ʧ��
			*/
			bool EsScaleUp(const AttrType& nAttrType, map<int, vector<Cell>*>& mapCellIdAndCellSet, vector<Cell>& vecCell);

			/**
			* @Function:    �ϴ���������ֵ
			* @Param [in]:  strPropName                   ������
			* @Param [in]:  nAttrType                     ��������
			* @Param [in]:  vecCell                       ��һ������������ݼ�
			* @Param [in]:  strRootURL                    ���ݷ������ĵĸ�·��
			* @Param [in]:  strShapeID                    ���γ�ID
			* @Param [in]:  strCoverageID                 ���Գ�ID
			* @Param [in]:  nLod                          Lod����
			* @return:      true-�ϴ��ɹ�;false-�ϴ�ʧ��
			*/
			bool EsUpdateDataValue(const string& strPropName, const AttrType& nAttrType, const vector<Cell>& vecCell, const string& strModelID, const int& nLod);

			/**
			* @Function:    ����������ɸѡ����Ӧ�ĵز�
			* @Param [in]:  strRootURL                    ���ݷ������ĵĸ�·��
			* @Param [in]:  strMatchedFieldName           ����ɸѡ������������������ƣ�һ��Ϊ�ز�
			* @Param [in]:  strMinorMatchedFieldName      ����ɸѡ����Ĵ������������ƣ�һ��Ϊ����
			* @Param [in]:  vecStrUpdatePropName          ���µ���������
			* @Param [out]: vecAttrType                   ���µ��������ͼ�            ����/Lod
			* @Param [in]:  vecStrCoverageID              ���Գ�ID��
			* @Param [in]:  vecMajorIDSet                 ��Լ������ID��
			* @Param [in]:  vecMinorIDSet                 ��Լ������ID��
			* @Param [in]:  vecCellIn                     ��һ������������ݼ�
			* @Param [in]:  vecCellBeforeUpdate           ����ǰ���������ݼ�
			* @Param [out]: vecCellOut                    ���ز��ѡ����������ݼ�
			* @return:      true-ɸѡ�ɹ�;false-ɸѡʧ��
			*/
			bool EsSelectDataByMatchedFieldName(const string& strMajorMatchedFieldName, const string& strMinorMatchedFieldName, const vector<string>& vecStrUpdatePropName, const vector<vector<AttrType>>& vecAttrType, const vector<vector<string>>& vecStrModelID, vector<vector<int>>& vecLodSet, vector<vector<GridInfo>>& vecGISet,
				const vector<vector<int>>& vecMajorIDSet, const vector<vector<vector<int>>>& vecMinorIDSet, const vector<vector<vector<Cell>>>& vecCellIn, const vector<vector<vector<Cell>>>& vecCellBeforeUpdate, vector<vector<vector<vector<vector<Cell>>>>>& vecCellOut);

			/**
			* @Function:    �ַ����ָ�
			* @Param [in]:  source                  Դ�ַ���
			* @Param [out]: dest                    �ָ����ַ�������
			*/
			void split(const string source, vector<string>& dest);

			/**
			* @Function:    ��Ŀ¼�л�ȡָ����׺�����ļ�
			* @Param [in]:  strRootDir              ��Ŀ¼
			* @Param [in]:  strExt                  ָ�����ļ���׺
			* @Param [in]:  bDoWithSubDir           �Ƿ��ѯ��Ŀ¼��true-������Ŀ¼��false-��������Ŀ¼
			* @Param [out]: vecStrFileName          ����ָ����׺���ļ�����
			* @Param [out]: vecStrFilePath          ����ָ����׺���ļ�·����
			*/
			void MyFileRetrieveByExt(const string& strRootDir, const string& strExt, bool bDoWithSubDir, vector<string>& vecStrFileName, vector<string>& vecStrFilePath);

			/**
			* @Function:    ��Ŀ¼��ָ�����Ե������ļ��л�ȡ���µ�����
			* @Param [in]:  strDataDir              Ŀ��Ŀ¼
			* @Param [in]:  strPropName             ������
			* @Param [in]:  nAttrType               ��������
			* @Param [in]:  gi                      ������Ϣ
			* @Param [in]:  bFileMustExit           �ļ��Ƿ������ڣ�true-���룬�ļ�������ʱ����false��false-�Ǳ��룬�ļ�������ʱ����true
			* @Param [in]:  bAddDiff                �Ƿ�����map���в����ڵ����ݶԣ�true-���ӣ�false-������
			* @Param [out]: vecStrUpdatePropName    ���µ���������
			* @Param [out]: mapCellIdAndCellSet     ����ID�Ͷ�Ӧ��������ݼ�
			* @return:      true-��ȡ�ɹ�;false-��ȡʧ��
			*/
			bool EsGetPropDataFromGeoDataCenter(const string& strDataDir, const string& strPropName, const AttrType& nAttrType, const GridInfo& gi, bool& bFileMustExit, bool& bAddDiff, map<int, vector<Cell>*>& mapCellIdAndCellSet);

			/**
			* @Function:    ������ļ��л�ȡ���µ����
			* @Param [in]:  strBoreholeFile         ����ļ�·��
			* @Param [out]: vecRecord               �������
			* @return:      true-��ȡ�ɹ�;false-��ȡʧ��
			*/
			bool EsGetBoreholeRecordFromFile(const string& strBoreholeFile, vector<Record>& vecRecord);

			/**
			* @Function:    ���ֻ����������ļ�
			* @Param [in]:  strSacleUpFilePath      ��״ֻ�����ļ�·��
			* @Param [in]:  nAttrType               ��������
			* @Param [in]:  vecCell                 �ֻ��������
			* @return:      true-����ɹ�;false-���ʧ��
			*/
			bool EsExportScaleUpCell(const string& strSacleUpFilePath, const AttrType nAttrType, const vector<Cell>& vecCell);

			/**
			* @Function:    �������Լ�������������Ƿ���ڣ���������������ٽ�LOD�Ľ����ֵ
			* @Param [in]:  gi                      ģ��������Ϣ
			* @Param [in]:  nMinI                   ��ʼ����I
			* @Param [in]:  nMinJ                   ��ʼ����J
			* @Param [in]:  nMinK                   ��ʼ����K
			* @Param [in]:  nMaxI                   ��ʼ����I
			* @Param [in]:  nMaxJ                   ��ʼ����J
			* @Param [in]:  nMaxK                   ��ʼ����K
			* @Param [in]:  strModelID              ģ��ID
			* @Param [in]:  nLod                    LOD
			* @Param [in]:  strMatchedFieldName     Լ��������
			* @return:      true-��Ⲣ���³ɹ�;false-��Ⲣ����ʧ��
			*/
			bool EsCheckAndBuildMatchedField(const GridInfo& gi, const int nMinI, const int nMinJ, const int nMinK, const int nMaxI, const int nMaxJ, const int nMaxK,
				const string strModelID, const int nLod, const string& strMatchedFieldName);

		};
	}
}