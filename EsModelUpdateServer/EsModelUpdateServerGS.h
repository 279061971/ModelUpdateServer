/*************************************************************************
* All rights reserved.
* 
* 文件名称：EsModelUpdateServerGS.h
* 摘    要：地调局定制模型自动更新服务管理类
* 
* 作    者：何俊强
* 完成日期：2019/03/11
*************************************************************************/

#pragma once
#include <windows.h>
#include "EsModelUpdateServerBase.h"
#include "tinyxml.h"


namespace gridworld
{
	namespace GS
	{
		//模型数据服务中心相关设置
		const int DownLoadLimit = 5000; //中地单次网格下载上限
		const int UpLoadLimit = 5000; //单次网格上传上限，人为设定，为了降低上传出错率
		const int SleepTime = 5000; //休眠时长
		const string strDataType_int = "int"; //数据类型-int
		const string strDataType_double = "double"; //数据类型-double

		const string strMatchedDataCenter_DataUpdate = "DataUpdate"; //地调局更新目录下的更新数据文件夹名
		const string strMatchedDataCenter_DataOrigin = "DataOrigin"; //地调局更新目录下的所有原始数据文件夹名
		const string strMatchedDataCenter_InterResult = "InterResult"; //地调局更新目录下的插值结果存放文件夹名
		const string strMatchedDataCenter_ScaleUp = "ScaleUp"; //地调局更新目录下的粗化结果存放文件夹名
		const string strMatchedDataCenter_DataRelational = "DataRelational"; //地调局更新目录下的与更新属性相关的属性输出目录
		const string strMatchedDataCenter_Config = "ModelUpdateConfig.xml"; //地调局更新目录下的更新配置文件名

		//自动更新配置文件相关设置
		const char* strConfigSetting = "setting";
		const char* strConfigSetting_RootURL = "RootURL";
		const char* strConfigSetting_MatchedFieldName = "MatchedFieldName"; //用于控制范围的属性列，一般为所属地层ID

		const char* strConfigProperty = "property";
		const char* strConfigProperty_PropName = "PropName";
		const char* strConfigProperty_PropType = "PropType";
		const char* strConfigProperty_Lod = "Lod";
		const char* strConfigProperty_ModelID = "ModelID";

		const char* strConfigLayer = "layer";
		const char* strConfigLayer_LayerID = "LayerID";
		const char* strConfigLayer_InterpolationMethod = "InterpolationMethod";

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

		//输出信息
		const string strMsg_NoUpdate = "未检测到更新";
		const string strMsg_StartServer = "*********************启动自动更新服务************************";
		const string strMsg_CheckUpdate = "正在检测地质大数据中心是否有更新";
		const string strMsg_CheckUpdate_first = "检测到以下属性有更新：";
		const string strMsg_CheckUpdate_second = ",正在获取更新的钻孔，并粗化到网格...";
		const string strMsg_UpdateBorehole_Success = "更新的钻孔粗化成功";
		const string strMsg_UpLoadingCell = "\t\t\t正在将更新的钻孔上传到数据库...";
		const string strMsg_UpLoadCellSuccess = "\t\t\t更新的钻孔上传到数据库成功，正在评估更新的钻孔的影响范围...";
		const string strMsg_DownloadCell = "\t\t\t\t正在下载插值用的样本，请等待...";
		const string strMsg_DownloadCellSuccess = "\t\t\t\t插值样本下载成功，正在进行插值计算...";
		const string strMsg_InterpolationFailed = "\t\t\t\t错误：插值计算失败，本更新范围的更新将被跳过";
		const string strMsg_InterpolationSuccess = "\t\t\t\t插值计算成功，正在更新插值结果到数据库...";
		const string strMsg_CloseServer = "*********************自动更新结束，服务关闭************************\n";

		const char* strMsg_UpdateProp = "\t正在更新第%d个属性：%s,剩余%d个属性待更新";
		const char* strMsg_UpdateLod = "\t\t正在更新第%d个LOD：Lod=%d,剩余%d个LOD待更新";
		const char* strMsg_UpdateLayer = "\t\t\t正在更新第%d个地层：%d,剩余%d个地层待更新";
		const char* strMsg_UpdateLayer_NoUpdate = "\t\t\t地层：%d没有更新的钻孔，已跳过";
		const char* strMsg_UpLoad_Failed = "\t\t\t%s 错误：地层：%d更新的钻孔无法更新到数据库，本地层的更新将被跳过";
		const char* strMsg_GetVariogram_Failed = "\t\t\t警告：地层：%d的变差函数获取失败，将使用默认变差进行计算";
		const char* strMsg_EvaluateArea_Failed = "\t\t\t错误：地层：%d的更新范围评估失败，本地层的更新将被跳过,请检查用于评估的变程是否合理";
		const char* strMsg_EvaluateArea_Success = "\t\t\t地层：%d的更新范围评估成功，共有%d个待更新范围";
		const char* strMsg_GetInterMethod_Failed = "\t\t\t错误：从配置文件中获取地层：%d的插值算法失败，本地层的更新将被跳过";
		const char* strMsg_UpdateArea = "\t\t\t\t正在执行第%d个更新范围的插值计算,插值范围：IOri=%d JOri=%d KOri=%d XNum=%d YNum=%d ZNum=%d，剩余%d个更新范围待插值";
		const char* strMsg_DownloadingCell_Failed = "\t\t\t\t%s 错误：下载插值样本失败，本更新范围的更新将被跳过";
		const char* strMsg_UploadCell_Failed = "\t\t\t\t%s 错误：插值结果更新到数据库失败，本更新范围的更新将被跳过";
		const char* strMsg_UploadCell_Success = "\t\t\t\t插值结果更新到数据库成功";
		const char* strMsg_GetFaceCode_Failed = "\t\t\t错误：从配置文件中获取属性：%s的离散相失败，本地层的更新将被跳过\n";
		const char* strMsg_GetFaceCode_NULL = "\t\t\t警告：属性：%s的离散相数量为0，本地层的更新将被跳过";
		const char* strMsg_ExtractLodProp_Start = "\t\t\t\t正在将当前LOD的插值结果粗化到次级LOD";
		const char* strMsg_ExtractLodProp_End = "\t\t\t\t当前LOD的插值结果粗化到次级LOD结束";

		const string strErrorMsg_FailedToReadBoreholeFile = "错误：钻孔文件读取失败，请检查文件是否存在或者文件格式是否正确";
		const string strErrorMsg_FailedToReadConfig = "错误：模型自动更新配置文件读取失败，请检查文件是否存在或者文件格式是否正确";
		const string strErrorMsg_FailedToGetGridInfoCountFromDataBase_NoConnect = "错误：从数据库获取属性场数量失败";
		const string strErrorMsg_FailedToGetGridInfoFromDataBase_NoConnect = "错误：从数据库获取网格信息失败";
		const string strErrorMsg_FailedToGetGridInfoFromDataBase_NoMatched = "错误：数据库中没有匹配模型的网格信息";
		const string strErrorMsg_SelectToLayer_FailedToGetMajorCellFromDataBase = "错误：将样本数据筛选到地层步骤-从数据库下载主筛选属性的网格失败";
		const string strErrorMsg_SelectToLayer_FailedToGetPropCellFromDataBase = "错误：将样本数据筛选到地层步骤-从数据库下载待更新属性的网格失败";
		const string strErrorMsg_UpLoadData_FailedToUploadCell = "错误：上传网格到数据库失败";
		const string strErrorMsg_UpLoadData_FailedToDownloadInterAreaCell = "错误：上传网格到数据库步骤-从数据库下载插值范围的网格失败";
		const string strErrorMsg_DownLoadData_MatchedFieldNoData = "错误：数据库中当前范围没有约束属性的值，且无法从其他LOD获取";
		const string strErrorMsg_DownLoadData_FailedToDownLoadCell = "错误：从数据库下载网格失败";
		const string strErrorMsg_DownLoadData_NoCellDownLoad = "错误：从数据库下载到的样本数为0";


		class EXPORTMODELUPDATESERVER CModelUpdateServerGS : public CModelUpdateServerBase
		{
		public:
			CModelUpdateServerGS();
			~CModelUpdateServerGS();

		public:

			virtual int EsStartServer(const string& strTargetDir);

		public:

			/**
			* @Function:    从目标目录检测钻孔数据是否有更新
			* @Param [in]:  strTargetDir                  存放所有更新信息的目标目录
			* @Param [out]: vecStrUpdatePropName          更新的属性名集
			* @return:      true-有更新;false-无更新
			*/
			virtual bool EsCheckIsUpdated(const string& strTargetDir, vector<string>& vecStrUpdatePropName);

			/**
			* @Function:    从目标目录获取更新钻孔，并粗化到网格
			* @Param [in]:  strTargetDir                  存放所有更新信息的目标目录
			* @Param [in]:  strConfigPath                 自动更新配置文件路径
			* @Param [in]:  strMatchedFieldName           用于筛选网格的属性列列名称，一般为地层
			* @Param [in]:  vecStrUpdatePropName          更新的属性名集
			* @Param [out]: vecAttrType                   更新的属性类型集            属性/Lod
			* @Param [out]: vecCellSet                    更新的属性样本集            属性/Lod
			* @Param [out]: vecCellAfterUpdate            更新后的属性样本集          属性/Lod
			* @Param [out]: vecStrCoverageID              更新的属性对应的属性场集    属性/Lod
			* @Param [out]: vecStrShapeID                 更新的属性对应的几何场集    属性/Lod
			* @Param [out]: vecStrFeatureID               更新的属性对应的要素ID集    属性/Lod
			* @Param [out]: vecLodSet                     更新的属性对应的LOD集       属性/Lod
			* @Param [out]: vecLayerIDSet                  更新的属性对应的地层ID集    属性/Lod
			* @Param [out]: vecGISet                      更新的属性对应的网格集      属性/Lod
			* @return:      true-获取成功;false-获取失败
			*/
			virtual bool EsGetUpdatedSampleData(const string& strTargetDir, const string& strConfigPath, const string& strMatchedFieldName, const vector<string>& vecStrUpdatePropName, vector<vector<AttrType>>& vecAttrType, vector<vector<vector<vector<Cell>>>>& vecCellSet,
				vector<vector<vector<Cell>>>& vecCellAfterUpdate, vector<vector<string>>& vecStrModelID, vector<vector<int>>& vecLodSet, vector<vector<int>>& vecLayerIDSet, vector<vector<GridInfo>>& vecGISet);

			/**
			* @Function:    从数据服务中心获取插值样本
			* @Param [in]:  strTargetDir                  存放所有更新信息的目标目录
			* @Param [in]:  strPropName                   属性名
			* @Param [in]:  nAttrType                     属性类型
			* @Param [in]:  strMatchedFieldName           用于筛选网格的属性列列名称，一般为地层
			* @Param [in]:  strLayerID                    地层ID
			* @Param [in]:  giforSampleDownload           样本下载网格范围
			* @Param [in]:  giforUpdate                   待更新的网格范围
			* @Param [in]:  bStochastic                   是否是随机模拟：true-是;false-否
			* @Param [in]:  strRootURL                    数据服务中心的根路径
			* @Param [in]:  strCoverageID                 属性场ID
			* @Param [in]:  vecCellOrigin                 属性插值前的所有样本集
			* @Param [out]: vecCell                       获取的插值样本
			* @return:      true-获取成功;false-获取失败
			*/
			virtual bool EsDownLoadData(const string& strTargetDir, const string& strPropName, const AttrType& nAttrType, const string& strMatchedFieldName, const string& strLayerID, 
				const GridInfo& giforSampleDownload, const GridInfo& giforUpdate, bool bStochastic, const string& strModelID, const int nLod, const vector<Cell>& vecCellOrigin, vector<Cell>& vecCell);

			/**
			* @Function:    将Cell格式的网格属性值更新到数据服务中心
			* @Param [in]:  strPropName                   属性名
			* @Param [in]:  nAttrType                     属性类型
			* @Param [in]:  vecCell                       待上传的样本
			* @Param [in]:  strRootURL                    数据服务中心的根路径
			* @Param [in]:  strCoverageID                 属性场ID
			* @Param [in]:  nLod                          Lod级数
			* @return:      true-上传成功;false-上传失败
			*/
			virtual bool EsUpLoadData(const string& strPropName, const AttrType nAttrType, const vector<Cell>& vecCell, const string& strModelID, const int& nLod);

			/**
			* @Function:    更新插值参数结构体中的属性值到数据服务中心
			* @Param [in]:  strPropName                   属性名
			* @Param [in]:  nAttrType                     属性类型
			* @Param [in]:  strMatchedFieldName           用于筛选网格的属性列列名称，一般为地层
			* @Param [in]:  strLayerID                    地层ID
			* @Param [in]:  tmip                          插值参数结构体
			* @Param [in]:  strModelID                 属性场ID
			* @Param [in]:  nLod                          Lod级数
			* @return:      true-上传成功;false-上传失败
			*/
			virtual bool EsUpLoadData(const string& strPropName, const AttrType nAttrType, const string& strMatchedFieldName, const string& strLayerID, 
				ModelInterPara& tmip, const string& strModelID, const int& nLod);

			/**
			* @Function:    从自动更新配置文件获取基本设置参数
			* @Param [in]:  strConfigPath                 配置文件路径
			* @Param [out]: strRootURL                    数据服务中心根路径
			* @Param [out]: strMatchedFieldName           用于筛选网格的属性列列名称，一般为地层
			* @return:      true-获取成功;false-获取失败
			*/
			bool EsReadServerConfigSetting(const string& strConfigPath, string& strRootURL, string& strMatchedFieldName);

			/**
			* @Function:    从自动更新配置文件获取指定属性的相关信息
			* @Param [in]:  strConfigPath                 配置文件路径
			* @Param [in]:  strPropName                   属性名
			* @Param [out]: vecAttrType                   属性类型集
			* @Param [out]: vecLod                        属性对应的Lod集
			* @Param [out]: vecModelID                    模型ID集
			* @return:      true-获取成功;false-获取失败
			*/
			bool EsReadServerConfigProperty(const string& strConfigPath, const string& strPropName, vector<AttrType>& vecAttrType, vector<int>& vecLod, vector<string>& vecModelID);

			/**
			* @Function:    从自动更新配置文件获取指定属性、指定地层的插值方法
			* @Param [in]:  strConfigPath                 配置文件路径
			* @Param [in]:  strPropName                   属性名
			* @Param [in]:  strLayerID                    地层ID
			* @Param [out]: nInterpolationMethod          插值方法
			* @return:      true-获取成功;false-获取失败
			*/
			bool EsReadServerConfigLayer(const string& strConfigPath, const string& strPropName, const int& nLod, const string& strLayerID, InterMethod& nInterpolationMethod);

			/**
			* @Function:    从自动更新配置文件获取指定属性、指定地层的变差函数-适用于连续属性
			* @Param [in]:  strConfigPath                 配置文件路径
			* @Param [in]:  strPropName                   属性名
			* @Param [in]:  strLayerID                    地层ID
			* @Param [out]: varPara                       变差函数
			* @return:      true-获取成功;false-获取失败
			*/
			bool EsReadServerConfigVariogram(const string& strConfigPath, const string& strPropName, const int& nLod, const string& strLayerID, VariogramPara& varPara);

			/**
			* @Function:    从自动更新配置文件获取指定属性、指定地层、指定相的变差函数-适用于离散属性
			* @Param [in]:  strConfigPath                 配置文件路径
			* @Param [in]:  strPropName                   属性名
			* @Param [in]:  strLayerID                    地层ID
			* @Param [in]:  strFaceCode                   相ID
			* @Param [out]: varPara                       变差函数
			* @return:      true-获取成功;false-获取失败
			*/
			bool EsReadServerConfigVariogramByFaceCode(const string& strConfigPath, const string& strPropName, const int& nLod, const string& strLayerID, const string& strFaceCode, VariogramPara& varPara);

			/**
			* @Function:    从自动更新配置文件获取指定属性的地层ID集
			* @Param [in]:  strConfigPath                 配置文件路径
			* @Param [in]:  strPropName                   属性名
			* @Param [out]: vecLayerID                    地层ID集
			* @return:      true-获取成功;false-获取失败
			*/
			bool EsReadServerConfigLayerID(const string& strConfigPath, const string& strPropName, const int& nLod, vector<int>& vecLayerID);

			/**
			* @Function:    从自动更新配置文件获取指定属性、指定地层的相ID集
			* @Param [in]:  strConfigPath                 配置文件路径
			* @Param [in]:  strPropName                   属性名
			* @Param [in]:  strLayerID                    地层ID
			* @Param [out]: vecFaceCode                   相ID集
			* @return:      true-获取成功;false-获取失败
			*/
			bool EsReadServerConfigFaceCode(const string& strConfigPath, const string& strPropName, const int& nLod, const string& strLayerID, vector<int>& vecFaceCode);

			bool EsExportOriginDataOfOtherPropsEffectedByCurProp(const string& strConfigPath, const string& strTargetDir, const string& strCurPropName, const string& strLayerID, const int& nLod,
				const GridInfo& gi, const vector<GridInfo>& vecGiforUpdate);

			bool EsExtractPropFromCurrentLodToLowerLod(const GridInfo& gi, const string &strMatchedFieldName, const string& strModelID, const int nLod);

		private:

			/**
			* @Function:    从模型数据服务中心获取几何场网格信息
			* @Param [in]:  strModelID                    模型ID
			* @Param [in]:  nLod                          Lod级数
			* @Param [out]: gi                            几何场对应的网格信息
			* @return:      true-获取成功;false-获取失败
			*/
			bool EsGetGlobalGridInfoFromModelDataBase(const string& strModelID, const int& nLod, GridInfo& gi);

			/**
			* @Function:    将钻孔数据筛选到网格
			* @Param [in]:  nAttrType                     属性类型
			* @Param [in]:  vecRecord                     属性钻孔数据
			* @Param [in]:  gi                            用于筛选的网格
			* @Param [in]:  bAdd                          是否增加map表中没有的数据对：true-增加；false-不增加
			* @Param [out]: mapCellIdAndCellSet           网格ID和对应的钻孔数据集
			* @return:      true-筛选成功;false-筛选失败
			*/
			bool EsSelectRecordToCell(const AttrType& nAttrType, const vector<Record>& vecRecord, const GridInfo& gi, bool& bAdd, map<int, vector<Cell>*>& mapCellIdAndCellSet);

			/**
			* @Function:    将属于同一个网格的连续属性数据归一化
			* @Param [in]:  nAttrType                     属性类型
			* @Param [in]:  mapCellIdAndCellSet           网格ID和对应的钻孔数据集
			* @Param [out]: vecCell                       归一化后的网格数据集
			* @return:      true-归一化成功;false-归一化失败
			*/
			bool EsScaleUp(const AttrType& nAttrType, map<int, vector<Cell>*>& mapCellIdAndCellSet, vector<Cell>& vecCell);

			/**
			* @Function:    上传网格属性值
			* @Param [in]:  strPropName                   属性名
			* @Param [in]:  nAttrType                     属性类型
			* @Param [in]:  vecCell                       归一化后的网格数据集
			* @Param [in]:  strRootURL                    数据服务中心的根路径
			* @Param [in]:  strCoverageID                 属性场ID
			* @Param [in]:  nLod                          Lod级数
			* @return:      true-上传成功;false-上传失败
			*/
			bool EsUpdateDataValue(const string& strPropName, const AttrType& nAttrType, const vector<Cell>& vecCell, const string& strCoverageID, const int& nLod);

			/**
			* @Function:    将样本数据筛选到对应的地层
			* @Param [in]:  strRootURL                    数据服务中心的根路径
			* @Param [in]:  strMatchedFieldName           用于筛选网格的属性列列名称，一般为地层
			* @Param [in]:  vecStrUpdatePropName          更新的属性名集
			* @Param [out]: vecAttrType                   更新的属性类型集            属性/Lod
			* @Param [in]:  vecStrCoverageID              属性场ID集
			* @Param [in]:  vecLayerIDSet                 地层ID集
			* @Param [in]:  vecCellIn                     归一化后的网格数据集
			* @Param [in]:  vecCellBeforeUpdate           更新前的网格数据集
			* @Param [out]: vecCellOut                    按地层分选后的网格数据集
			* @return:      true-筛选成功;false-筛选失败
			*/
			bool EsSelectDataByLayer(const string& strMatchedFieldName, const vector<string>& vecStrUpdatePropName, const vector<vector<AttrType>>& vecAttrType, const vector<vector<string>>& vecStrModelID, vector<vector<int>>& vecLodSet, vector<vector<GridInfo>>& vecGISet,
				const vector<vector<int>>& vecLayerIDSet, const vector<vector<vector<Cell>>>& vecCellIn, const vector<vector<vector<Cell>>>& vecCellBeforeUpdate, vector<vector<vector<vector<Cell>>>>& vecCellOut);

			/**
			* @Function:    字符串分割
			* @Param [in]:  source                  源字符串
			* @Param [out]: dest                    分割后的字符串数组
			*/
			void split(const string source, vector<string>& dest);

			/**
			* @Function:    从目录中获取指定后缀名的文件
			* @Param [in]:  strRootDir              根目录
			* @Param [in]:  strExt                  指定的文件后缀
			* @Param [in]:  bDoWithSubDir           是否查询子目录：true-访问子目录；false-不访问子目录
			* @Param [out]: vecStrFileName          符合指定后缀的文件名集
			* @Param [out]: vecStrFilePath          符合指定后缀的文件路径集
			*/
			void MyFileRetrieveByExt(const string& strRootDir, const string& strExt, bool bDoWithSubDir, vector<string>& vecStrFileName, vector<string>& vecStrFilePath);

			/**
			* @Function:    从目录中指定属性的样本文件中获取更新的样本
			* @Param [in]:  strDataDir              目标目录
			* @Param [in]:  strPropName             属性名
			* @Param [in]:  nAttrType               属性类型
			* @Param [in]:  gi                      网格信息
			* @Param [in]:  bFileMustExit           文件是否必须存在：true-必须，文件不存在时返回false；false-非必须，文件不存在时返回true
			* @Param [in]:  bAddDiff                是否增加map表中不存在的数据对：true-增加；false-不增加
			* @Param [out]: vecStrUpdatePropName    更新的属性名集
			* @Param [out]: mapCellIdAndCellSet     网格ID和对应的钻孔数据集
			* @return:      true-获取成功;false-获取失败
			*/
			bool EsGetPropDataFromGeoDataCenter(const string& strDataDir, const string& strPropName, const AttrType& nAttrType, const GridInfo& gi, bool& bFileMustExit, bool& bAddDiff, map<int, vector<Cell>*>& mapCellIdAndCellSet);

			/**
			* @Function:    从钻孔文件中获取更新的钻孔
			* @Param [in]:  strBoreholeFile         钻孔文件路径
			* @Param [out]: vecRecord               钻孔数据
			* @return:      true-获取成功;false-获取失败
			*/
			bool EsGetBoreholeRecordFromFile(const string& strBoreholeFile, vector<Record>& vecRecord);

			/**
			* @Function:    将粗化结果输出到文件
			* @Param [in]:  strSacleUpFilePath      钻孔粗化结果文件路径
			* @Param [in]:  nAttrType               属性类型
			* @Param [in]:  vecCell                 粗化结果数组
			* @return:      true-输出成功;false-输出失败
			*/
			bool EsExportScaleUpCell(const string& strSacleUpFilePath, const AttrType nAttrType, const vector<Cell>& vecCell);

			/**
			* @Function:    检测用于约束的网格属性是否存在，如果不存在则用临近LOD的结果赋值
			* @Param [in]:  gi                      模型网格信息
			* @Param [in]:  nMinI                   起始网格I
			* @Param [in]:  nMinJ                   起始网格J
			* @Param [in]:  nMinK                   起始网格K
			* @Param [in]:  nMaxI                   起始网格I
			* @Param [in]:  nMaxJ                   起始网格J
			* @Param [in]:  nMaxK                   起始网格K
			* @Param [in]:  strModelID              模型ID
			* @Param [in]:  nLod                    LOD
			* @Param [in]:  strMatchedFieldName     约束属性名
			* @return:      true-检测并更新成功;false-检测并更新失败
			*/
			bool EsCheckAndBuildMatchedField(const GridInfo& gi, const int nMinI, const int nMinJ, const int nMinK, const int nMaxI, const int nMaxJ, const int nMaxK, 
				const string strModelID, const int nLod, const string& strMatchedFieldName);

		};
	}
}