#include <fstream>
#include <math.h>
#include <direct.h>
#include <io.h>
#include "EsModelUpdateServerGS.h"
#include "g3dcpp.h"
#include "Grid.h"
#include "Field.h"
#include "Block.h"

using namespace g3dcpp;
using std::ifstream;
using std::ofstream;
using std::ios;
using std::pair;

gridworld::GS::CModelUpdateServerGS::CModelUpdateServerGS()
{

}

gridworld::GS::CModelUpdateServerGS::~CModelUpdateServerGS()
{

}

int gridworld::GS::CModelUpdateServerGS::EsStartServer(const string& strTargetDir)
{
	int nStatus = 0; //0-无异常；1-有异常
	string strMsg = "";
	char szMsg[1024];
	string strRootURL = ""; //模型数据服务中心根路径
	string strMatchedFieldName = ""; //网格所属地层ID属性列的列名称
	const string strConfigPath = strTargetDir + "\\" + strMatchedDataCenter_Config; //自动更新服务配置文件
	const string strLogFile = strTargetDir + "\\log.txt"; //日志文件

	//构建更新服务
	if (!EsReadServerConfigSetting(strConfigPath, strRootURL, strMatchedFieldName))
	{
		strMsg = EsGetErrorMsg();
		EsWriteToLog(strLogFile, strMsg);
		return 1;
	}

	/*********************启动服务************************/
	EsWriteToLog(strLogFile, strMsg_StartServer);

	API api;
	api.Init(strRootURL);

	/*********************从地质大数据中心检查钻孔数据是否有更新************************/
	EsWriteToLog(strLogFile, strMsg_CheckUpdate);

	vector<string> vecStrPropNameUpdate; //更新的属性名集
	if (!EsCheckIsUpdated(strTargetDir, vecStrPropNameUpdate))
	{
		EsWriteToLog(strLogFile, EsGetErrorMsg());
		return 1;
	}

	/*********************从地质大数据中心和模型数据服务中心获取更新的钻孔相关信息************************/
	strMsg = strMsg_CheckUpdate_first;
	for (auto propName : vecStrPropNameUpdate)
	{
		strMsg += propName + "\t";
	}
	strMsg += strMsg_CheckUpdate_second;
	EsWriteToLog(strLogFile, strMsg);

	vector<vector<AttrType>> vecAttrType; //更新的属性类型集         属性/Lod
	vector<vector<GridInfo>> vecGridInfo; //属性对应的网格信息集     属性/Lod
	vector<vector<string>> vecStrModelID; //属性对应的属性场ID集     属性/Lod
	vector<vector<int>> vecLayerIDSet; //属性对应的地层块ID集        属性/LayerId
	vector<vector<int>> vecLodSet; //属性对应的地层块ID集            属性/Lod
	vector<vector<vector<vector<Cell>>>> vecUpdateDataSet; //更新的网格样本    属性/Lod/地层
	vector<vector<vector<Cell>>> vecCellBeforeUpdate;      //更新前的网格样本  属性/Lod
	vector<vector<vector<Cell>>> vecCellAfterUpdate;       //更新后的网格样本  属性/Lod
	if (!EsGetUpdatedSampleData(strTargetDir, strConfigPath, strMatchedFieldName, vecStrPropNameUpdate, vecAttrType, vecUpdateDataSet, vecCellAfterUpdate, vecStrModelID, vecLodSet, vecLayerIDSet, vecGridInfo))
	{
		strMsg = EsGetErrorMsg();
		EsWriteToLog(strLogFile, strMsg);
		return 1;
	}

	EsWriteToLog(strLogFile, strMsg_UpdateBorehole_Success);

	/*********************为与地质大数据中心有更新的属性相匹配的数据库模型进行自动更新************************/
	for (size_t idxProp = 0; idxProp < vecStrPropNameUpdate.size(); idxProp++)
	{		
		sprintf_s(szMsg, strMsg_UpdateProp, idxProp + 1, vecStrPropNameUpdate[idxProp].c_str(), vecStrPropNameUpdate.size() - 1 - idxProp);
		EsWriteToLog(strLogFile, szMsg);

		/*********************不同LOD自动更新************************/
		for (size_t idxLod = 0; idxLod < vecLodSet[idxProp].size(); idxLod++)
		{
			sprintf_s(szMsg, strMsg_UpdateLod, idxLod + 1, vecLodSet[idxProp][idxLod], vecLodSet[idxProp].size() - 1 - idxLod);
			EsWriteToLog(strLogFile, szMsg);

			/*********************不同属性类型自动更新************************/
			if (vecAttrType[idxProp][idxLod] == Continuous)
			{
				/*********************不同地层自动更新************************/
				for (size_t idxLayer = 0; idxLayer < vecLayerIDSet[idxProp].size(); idxLayer++)
				{
					sprintf_s(szMsg, strMsg_UpdateLayer, idxLayer + 1, vecLayerIDSet[idxProp][idxLayer], vecLayerIDSet[idxProp].size() - 1 - idxLayer);
					EsWriteToLog(strLogFile, szMsg);

					//如果地层没有更新的样本，则跳过
					if (vecUpdateDataSet[idxProp][idxLod][idxLayer].empty())
					{
						sprintf_s(szMsg, strMsg_UpdateLayer_NoUpdate, vecLayerIDSet[idxProp][idxLayer]);
						EsWriteToLog(strLogFile, szMsg);

						continue;
					}

					//将更新的网格样本上传到数据库
					EsWriteToLog(strLogFile, strMsg_UpLoadingCell);

					if (!EsUpLoadData(vecStrPropNameUpdate[idxProp], vecAttrType[idxProp][idxLod], vecUpdateDataSet[idxProp][idxLod][idxLayer], vecStrModelID[idxProp][idxLod], vecLodSet[idxProp][idxLod]))
					{
						sprintf_s(szMsg, strMsg_UpLoad_Failed, EsGetErrorMsg().c_str(), vecLayerIDSet[idxProp][idxLayer]);
						EsWriteToLog(strLogFile, szMsg);
						nStatus = 1;
						continue;
					}

					//获取目标区域的变差函数，并转化为评估参数
					const string strLayerID = std::to_string(vecLayerIDSet[idxProp][idxLayer]);
					VariogramPara varPara;
					if (!EsReadServerConfigVariogram(strConfigPath, vecStrPropNameUpdate[idxProp], vecLodSet[idxProp][idxLod], strLayerID, varPara))
					{
						sprintf_s(szMsg, strMsg_GetVariogram_Failed, vecLayerIDSet[idxProp][idxLayer]);
						EsWriteToLog(strLogFile, szMsg);
						nStatus = 1;
					}

					int rangeI = ceil(varPara.dMajorRange / vecGridInfo[idxProp][idxLod].dXStep); //评估参数：I方向搜索步长
					int rangeJ = ceil(varPara.dMinorRange / vecGridInfo[idxProp][idxLod].dYStep); //评估参数：J方向搜索步长
					int rangeK = ceil(varPara.dVerticalRange / vecGridInfo[idxProp][idxLod].dZStep); //评估参数：K方向搜索步长

					//使用评估参数对更新的数据进行影响范围评估
					EsWriteToLog(strLogFile, strMsg_UpLoadCellSuccess);

					vector<GridInfo> vecGIforSampleDownload; //样本下载范围集
					vector<GridInfo> vecGIforUpdate; //待插范围集				
					if (!EsEvaluate(vecUpdateDataSet[idxProp][idxLod][idxLayer], rangeI, rangeJ, rangeK, vecGridInfo[idxProp][idxLod], vecGIforSampleDownload, vecGIforUpdate))
					{
						sprintf_s(szMsg, strMsg_EvaluateArea_Failed, vecLayerIDSet[idxProp][idxLayer]);
						EsWriteToLog(strLogFile, szMsg);
						nStatus = 1;
						continue;
					}

					if (vecGIforSampleDownload.size() != vecGIforUpdate.size())
					{
						sprintf_s(szMsg, strMsg_EvaluateArea_Failed, vecLayerIDSet[idxProp][idxLayer]);
						EsWriteToLog(strLogFile, szMsg);
						nStatus = 1;
						continue;
					}

					sprintf_s(szMsg, strMsg_EvaluateArea_Success, vecLayerIDSet[idxProp][idxLayer], vecGIforUpdate.size());
					EsWriteToLog(strLogFile, szMsg);

					//写出受该属性影响的其他属性的原始钻孔
					EsExportOriginDataOfOtherPropsEffectedByCurProp(strConfigPath, strTargetDir, vecStrPropNameUpdate[idxProp], strLayerID, vecLodSet[idxProp][idxLod], vecGridInfo[idxProp][idxLod], vecGIforUpdate);

					//从配置文件获取该区域插值使用的方法
					InterMethod nInterMethod;
					if (!EsReadServerConfigLayer(strConfigPath, vecStrPropNameUpdate[idxProp], vecLodSet[idxProp][idxLod], strLayerID, nInterMethod))
					{
						sprintf_s(szMsg, strMsg_GetInterMethod_Failed, vecLayerIDSet[idxProp][idxLayer]);
						EsWriteToLog(strLogFile, szMsg);
						nStatus = 1;
						continue;
					}

					/*********************对每个更新区域进行更新************************/
					for (size_t idxArea = 0; idxArea < vecGIforUpdate.size(); idxArea++)
					{
						sprintf_s(szMsg, strMsg_UpdateArea, idxArea + 1, vecGIforUpdate[idxArea].nIOri, vecGIforUpdate[idxArea].nJOri, vecGIforUpdate[idxArea].nKOri, vecGIforUpdate[idxArea].nXNum, 
							vecGIforUpdate[idxArea].nYNum, vecGIforUpdate[idxArea].nZNum, vecGIforUpdate.size() - 1 - idxArea);
						EsWriteToLog(strLogFile, szMsg);

						//准备插值参数
						ModelInterPara tmip; //插值参数
						tmip.strWorkPath = strTargetDir; //工作目录
						tmip.nAttrType = Continuous; //属性为连续
						tmip.gi = vecGIforUpdate[idxArea]; //待插网格			
						tmip.nInterMethod = nInterMethod; //插值算法
						tmip.varPara = varPara; //变差函数

						bool bStochastic = false;
						if (nInterMethod == SequentialGaussianSimulation)
						{
							bStochastic = true;
						}

						//从数据库获取目标范围内的样本	
						EsWriteToLog(strLogFile, strMsg_DownloadCell);

						if (!EsDownLoadData(strTargetDir, vecStrPropNameUpdate[idxProp], vecAttrType[idxProp][idxLod], strMatchedFieldName, strLayerID, vecGIforSampleDownload[idxArea], vecGIforUpdate[idxArea],
							bStochastic, vecStrModelID[idxProp][idxLod], vecLodSet[idxProp][idxLod], vecCellAfterUpdate[idxProp][idxLod], tmip.vecCell))
						{
							sprintf_s(szMsg, strMsg_DownloadingCell_Failed, EsGetErrorMsg().c_str());
							EsWriteToLog(strLogFile, szMsg);
							nStatus = 1;
							continue;
						}

						//如果是序贯高斯，则将模型的样本都用于插值计算
						if (nInterMethod == SequentialGaussianSimulation)
						{
							for (auto cell : vecCellAfterUpdate[idxProp][idxLod])
							{
								bool bCellExist = false;
								for (auto curCell : tmip.vecCell)
								{
									if (curCell.nI == cell.nI && curCell.nJ == cell.nJ && curCell.nK == cell.nK)
									{
										bCellExist = true;
										break;
									}
								}
								if (!bCellExist)
									tmip.vecCell.push_back(cell);
							}
						}

						//更新数据-插值计算
						EsWriteToLog(strLogFile, strMsg_DownloadCellSuccess);

						if (!EsUpdateData(tmip))
						{
							EsWriteToLog(strLogFile, strMsg_InterpolationFailed);
							nStatus = 1;
							continue;
						}

						//将更新后的网格属性写回数据库
						EsWriteToLog(strLogFile, strMsg_InterpolationSuccess);

						if (!EsUpLoadData(vecStrPropNameUpdate[idxProp], vecAttrType[idxProp][idxLod], strMatchedFieldName, strLayerID, tmip, vecStrModelID[idxProp][idxLod], vecLodSet[idxProp][idxLod]))
						{
							sprintf_s(szMsg, strMsg_UploadCell_Failed, EsGetErrorMsg().c_str());
							EsWriteToLog(strLogFile, szMsg);
							nStatus = 1;
							continue;
						}

						EsWriteToLog(strLogFile, strMsg_UploadCell_Success);

						//当前LOD都更新完之后，使用当前的模型向上粗化
						EsWriteToLog(strLogFile, strMsg_ExtractLodProp_Start);
						EsExtractPropFromCurrentLodToLowerLod(vecGIforUpdate[idxArea], vecStrPropNameUpdate[idxProp], vecStrModelID[idxProp][idxLod], vecLodSet[idxProp][idxLod]);
						EsWriteToLog(strLogFile, strMsg_ExtractLodProp_End);
					}
				}
			}
			else if (vecAttrType[idxProp][idxLod] == Discrete)
			{
				/*********************不同地层自动更新************************/
				for (size_t idxLayer = 0; idxLayer < vecLayerIDSet[idxProp].size(); idxLayer++)
				{
					sprintf_s(szMsg, strMsg_UpdateLayer, idxLayer + 1, vecLayerIDSet[idxProp][idxLayer], vecLayerIDSet[idxProp].size() - 1 - idxLayer);
					EsWriteToLog(strLogFile, szMsg);

					//如果地层没有更新的样本，则跳过
					if (vecUpdateDataSet[idxProp][idxLod][idxLayer].empty())
					{
						sprintf_s(szMsg, strMsg_UpdateLayer_NoUpdate, vecLayerIDSet[idxProp][idxLayer]);
						EsWriteToLog(strLogFile, szMsg);

						continue;
					}

					//将更新的网格样本上传到数据库
					EsWriteToLog(strLogFile, strMsg_UpLoadingCell);

					if (!EsUpLoadData(vecStrPropNameUpdate[idxProp], vecAttrType[idxProp][idxLod], vecUpdateDataSet[idxProp][idxLod][idxLayer], vecStrModelID[idxProp][idxLod], vecLodSet[idxProp][idxLod]))
					{
						sprintf_s(szMsg, strMsg_UpLoad_Failed, EsGetErrorMsg().c_str(), vecLayerIDSet[idxProp][idxLayer]);
						EsWriteToLog(strLogFile, szMsg);
						nStatus = 1;
						continue;
					}

					//遍历各相的变差函数，获取最大值为评估参数
					int rangeI = INT_MIN; //评估参数：I方向搜索步长
					int rangeJ = INT_MIN; //评估参数：J方向搜索步长
					int rangeK = INT_MIN; //评估参数：K方向搜索步长

					const string strLayerID = std::to_string(vecLayerIDSet[idxProp][idxLayer]);
					vector<int> vecFaceCode;
					if (!EsReadServerConfigFaceCode(strConfigPath, vecStrPropNameUpdate[idxProp], vecLodSet[idxProp][idxLod], strLayerID, vecFaceCode))
					{
						sprintf_s(szMsg, strMsg_GetFaceCode_Failed, vecStrPropNameUpdate[idxProp].c_str());
						EsWriteToLog(strLogFile, szMsg);
						nStatus = 1;
						continue;
					}

					if (vecFaceCode.empty())
					{
						sprintf_s(szMsg, strMsg_GetFaceCode_NULL, vecStrPropNameUpdate[idxProp].c_str());
						EsWriteToLog(strLogFile, szMsg);
						nStatus = 1;
						continue;
					}

					for (size_t idxCode = 0; idxCode < vecFaceCode.size(); idxCode++)
					{
						VariogramPara varPara;
						string strFaceCode = std::to_string(vecFaceCode[idxCode]);
						if (!EsReadServerConfigVariogramByFaceCode(strConfigPath, vecStrPropNameUpdate[idxProp], vecLodSet[idxProp][idxLod], strLayerID, strFaceCode, varPara))
							continue;

						int tempI = ceil(varPara.dMajorRange / vecGridInfo[idxProp][idxLod].dXStep);
						int tempJ = ceil(varPara.dMinorRange / vecGridInfo[idxProp][idxLod].dYStep);
						int tempK = ceil(varPara.dVerticalRange / vecGridInfo[idxProp][idxLod].dZStep);

						if (rangeI <= tempI)
							rangeI = tempI;
						if (rangeJ <= tempJ)
							rangeJ = tempJ;
						if (rangeK <= tempK)
							rangeK = tempK;
					}

					//对更新的数据进行影响范围评估
					EsWriteToLog(strLogFile, strMsg_UpLoadCellSuccess);

					vector<GridInfo> vecGIforSampleDownload; //样本下载范围集
					vector<GridInfo> vecGIforUpdate; //待插范围集				
					if (!EsEvaluate(vecUpdateDataSet[idxProp][idxLod][idxLayer], rangeI, rangeJ, rangeK, vecGridInfo[idxProp][idxLod], vecGIforSampleDownload, vecGIforUpdate))
					{
						sprintf_s(szMsg, strMsg_EvaluateArea_Failed, vecLayerIDSet[idxProp][idxLayer]);
						EsWriteToLog(strLogFile, szMsg);
						nStatus = 1;
						continue;
					}

					if (vecGIforSampleDownload.size() != vecGIforUpdate.size())
					{
						sprintf_s(szMsg, strMsg_EvaluateArea_Failed, vecLayerIDSet[idxProp][idxLayer]);
						EsWriteToLog(strLogFile, szMsg);
						nStatus = 1;
						continue;
					}
					sprintf_s(szMsg, strMsg_EvaluateArea_Success, vecLayerIDSet[idxProp][idxLayer], vecGIforUpdate.size());
					EsWriteToLog(strLogFile, szMsg);

					//写出受该属性影响的其他属性的原始钻孔
					EsExportOriginDataOfOtherPropsEffectedByCurProp(strConfigPath, strTargetDir, vecStrPropNameUpdate[idxProp], strLayerID, vecLodSet[idxProp][idxLod], vecGridInfo[idxProp][idxLod], vecGIforUpdate);

					//从配置文件获取该区域插值使用的方法
					InterMethod nInterMethod;
					if (!EsReadServerConfigLayer(strConfigPath, vecStrPropNameUpdate[idxProp], vecLodSet[idxProp][idxLod], strLayerID, nInterMethod))
					{
						sprintf_s(szMsg, strMsg_GetInterMethod_Failed, vecLayerIDSet[idxProp][idxLayer]);
						EsWriteToLog(strLogFile, szMsg);
						nStatus = 1;
						continue;
					}

					/*********************对每个更新区域进行更新************************/
					for (size_t idxArea = 0; idxArea < vecGIforUpdate.size(); idxArea++)
					{
						sprintf_s(szMsg, strMsg_UpdateArea, idxArea + 1, vecGIforUpdate[idxArea].nIOri, vecGIforUpdate[idxArea].nJOri, vecGIforUpdate[idxArea].nKOri, vecGIforUpdate[idxArea].nXNum,
							vecGIforUpdate[idxArea].nYNum, vecGIforUpdate[idxArea].nZNum, vecGIforUpdate.size() - 1 - idxArea);
						EsWriteToLog(strLogFile, szMsg);

						//准备插值参数
						ModelInterPara tmip; //插值参数
						tmip.strWorkPath = strTargetDir; //工作目录
						tmip.nAttrType = Discrete; //属性为连续
						tmip.gi = vecGIforUpdate[idxArea]; //待插网格			
						tmip.nInterMethod = nInterMethod; //插值算法

						//设置离散属性变差函数
						tmip.faciesPara.resize(vecFaceCode.size());
						for (size_t idxCode = 0; idxCode < vecFaceCode.size(); idxCode++)
						{
							tmip.faciesPara[idxCode].nCode = vecFaceCode[idxCode];

							VariogramPara varParaTemp;
							string strCode = std::to_string(vecFaceCode[idxCode]);
							EsReadServerConfigVariogramByFaceCode(strConfigPath, vecStrPropNameUpdate[idxProp], vecLodSet[idxProp][idxLod], strLayerID, strCode, varParaTemp);
							tmip.faciesPara[idxCode].varPara = varParaTemp;
						}

						bool bStochastic = false;
						if (nInterMethod == SequentialIndicatorSimulation)
						{
							bStochastic = true;
						}

						//从样本中选取目标范围内的样本
						EsWriteToLog(strLogFile, strMsg_DownloadCell);
						if (!EsDownLoadData(strTargetDir, vecStrPropNameUpdate[idxProp], vecAttrType[idxProp][idxLod], strMatchedFieldName, strLayerID, vecGIforSampleDownload[idxArea], vecGIforUpdate[idxArea],
							bStochastic, vecStrModelID[idxProp][idxLod], vecLodSet[idxProp][idxLod], vecCellAfterUpdate[idxProp][idxLod], tmip.vecCell))
						{
							sprintf_s(szMsg, strMsg_DownloadingCell_Failed, EsGetErrorMsg().c_str());
							EsWriteToLog(strLogFile, szMsg);
							nStatus = 1;
							continue;
						}

						//更新数据-插值计算
						EsWriteToLog(strLogFile, strMsg_DownloadCellSuccess);
						if (!EsUpdateData(tmip))
						{
							EsWriteToLog(strLogFile, strMsg_InterpolationFailed);
							nStatus = 1;
							continue;
						}

						//将更新后的网格属性写回数据库
						EsWriteToLog(strLogFile, strMsg_InterpolationSuccess);
						if (!EsUpLoadData(vecStrPropNameUpdate[idxProp], vecAttrType[idxProp][idxLod], strMatchedFieldName, strLayerID, tmip, vecStrModelID[idxProp][idxLod], vecLodSet[idxProp][idxLod]))
						{
							sprintf_s(szMsg, strMsg_UploadCell_Failed, EsGetErrorMsg().c_str());
							EsWriteToLog(strLogFile, szMsg);
							nStatus = 1;
							continue;
						}
						EsWriteToLog(strLogFile, strMsg_UploadCell_Success);

						//当前LOD都更新完之后，使用当前的模型向上粗化
						EsWriteToLog(strLogFile, strMsg_ExtractLodProp_Start);
						EsExtractPropFromCurrentLodToLowerLod(vecGIforUpdate[idxArea], vecStrPropNameUpdate[idxProp], vecStrModelID[idxProp][idxLod], vecLodSet[idxProp][idxLod]);
						EsWriteToLog(strLogFile, strMsg_ExtractLodProp_End);
					}
				}
			}
		}
	}

	EsWriteToLog(strLogFile, strMsg_CloseServer);
	return nStatus;
}

bool gridworld::GS::CModelUpdateServerGS::EsCheckIsUpdated(const string& strTargetDir, vector<string>& vecStrUpdatePropName)
{
	const string strDataUpdateDir = strTargetDir + "\\" + strMatchedDataCenter_DataUpdate + "\\";

	vector<string> vecStrFilePath;
	vector<string> vecStrFileName;
	vecStrFileName.clear();
	vecStrFilePath.clear();
	MyFileRetrieveByExt(strDataUpdateDir, "txt", false, vecStrFileName, vecStrFilePath);
	if (vecStrFilePath.empty())
	{
		EsSetErrorMsg(strMsg_NoUpdate);
		return false;
	}

	vecStrUpdatePropName.resize(vecStrFilePath.size());
	for (size_t i = 0; i < vecStrFilePath.size(); i++)
	{
		vecStrUpdatePropName[i] = vecStrFileName[i].substr(0, vecStrFileName[i].find_last_of('.'));
	}

	return true;
}

bool gridworld::GS::CModelUpdateServerGS::EsGetUpdatedSampleData(const string& strTargetDir, const string& strConfigPath, const string& strMatchedFieldName, const vector<string>& vecStrUpdatePropName, vector<vector<AttrType>>& vecAttrType, vector<vector<vector<vector<Cell>>>>& vecCellSet,
	vector<vector<vector<Cell>>>& vecCellAfterUpdate, vector<vector<string>>& vecStrModelID, vector<vector<int>>& vecLodSet, vector<vector<int>>& vecLayerIDSet, vector<vector<GridInfo>>& vecGISet)
{
	//从配置文件获取与钻孔名匹配的信息
	vecAttrType.resize(vecStrUpdatePropName.size());
	vecStrModelID.resize(vecStrUpdatePropName.size());
	vecLodSet.resize(vecStrUpdatePropName.size());
	vecLayerIDSet.resize(vecStrUpdatePropName.size());
	vecGISet.resize(vecStrUpdatePropName.size());
	for (size_t i = 0; i < vecStrUpdatePropName.size(); i++)
	{
		//获取钻孔名的属性类型、以及在数据库种对应的属性场ID、几何场ID、要素类ID
		if (!EsReadServerConfigProperty(strConfigPath, vecStrUpdatePropName[i], vecAttrType[i], vecLodSet[i], vecStrModelID[i]))
		{
			EsSetErrorMsg(strErrorMsg_FailedToReadConfig);
			return false;
		}
		
		vecGISet[i].resize(vecLodSet[i].size());
		for (size_t j = 0; j < vecLodSet[i].size(); j++)
		{
			//获取钻孔所属模型的地层划分ID
			if (!EsReadServerConfigLayerID(strConfigPath, vecStrUpdatePropName[i], vecLodSet[i][j], vecLayerIDSet[i]))
			{
				EsSetErrorMsg(strErrorMsg_FailedToReadConfig);
				return false;
			}

			//从模型数据服务中心获取模型剖分的网格信息
			if (!EsGetGlobalGridInfoFromModelDataBase(vecStrModelID[i][j], vecLodSet[i][j], vecGISet[i][j]))
				return false;

			//在数据库中创建该属性，如果已有则不会创建，这样可以保证数据库中都有该属性
			CreateGridFields createGridFields(vecStrModelID[i][j]);
			Field field;
			field.Name(vecStrUpdatePropName[i]);
			field.Label(vecStrUpdatePropName[i]);
			field.DataType(vecAttrType[i][j] == Discrete ? geo3dml::Field::ValueType::Integer : geo3dml::Field::ValueType::Double);
			createGridFields.AddField(field);
			createGridFields.Create();
		}
	}

	//将各个属性的原始钻孔粗化到网格
	const string strDataUpdateDir = strTargetDir + "\\" + strMatchedDataCenter_DataUpdate + "\\";
	const string strDataOriginDir = strTargetDir + "\\" + strMatchedDataCenter_DataOrigin + "\\";

	vector<vector<vector<Cell>>> vecCellTemp;
	vector<vector<vector<Cell>>> vecCellBeforeUpdate;
	vecCellTemp.clear();
	vecCellTemp.resize(vecStrUpdatePropName.size());
	vecCellBeforeUpdate.clear();
	vecCellBeforeUpdate.resize(vecStrUpdatePropName.size());
	vecCellAfterUpdate.clear();
	vecCellAfterUpdate.resize(vecStrUpdatePropName.size());
	for (size_t i = 0; i < vecStrUpdatePropName.size(); i++)
	{
		vecCellBeforeUpdate[i].resize(vecLodSet[i].size());
		vecCellAfterUpdate[i].resize(vecLodSet[i].size());
		for (size_t j = 0; j < vecLodSet[i].size(); j++)
		{
			map<int, vector<Cell>*> mapCellIdAndCellSet;
			
			//先将更新前的钻孔转换到所属网格
			bool bAddDiff = true;
			bool bFileMustExist = false;
			if (!EsGetPropDataFromGeoDataCenter(strDataOriginDir, vecStrUpdatePropName[i], vecAttrType[i][j], vecGISet[i][j], bFileMustExist, bAddDiff, mapCellIdAndCellSet))
			{
				EsSetErrorMsg(strErrorMsg_FailedToReadBoreholeFile);
				return false;
			}
			
			//得到更新前的粗化结果
			if (!EsScaleUp(vecAttrType[i][j], mapCellIdAndCellSet, vecCellBeforeUpdate[i][j]))
				return false;
			
			//然后将更新后的钻孔转换到所属网格
			bFileMustExist = true;
			if (!EsGetPropDataFromGeoDataCenter(strDataUpdateDir, vecStrUpdatePropName[i], vecAttrType[i][j], vecGISet[i][j], bFileMustExist, bAddDiff, mapCellIdAndCellSet))
			{
				EsSetErrorMsg(strErrorMsg_FailedToReadBoreholeFile);
				return false;
			}
			
			//最后对同一网格内的数据进行归一化			
			if (!EsScaleUp(vecAttrType[i][j], mapCellIdAndCellSet, vecCellAfterUpdate[i][j]))
				return false;
			
			//释放内存
			for (auto pair : mapCellIdAndCellSet)
			{
				delete pair.second;
				pair.second = NULL;
			}
		}

		vecCellTemp[i].resize(vecLodSet[i].size());
		for (size_t j = 0; j < vecLodSet[i].size(); j++)
		{
			map<int, vector<Cell>*> mapCellIdAndCellSet;

			//先将更新的钻孔转换到所属网格
			bool bAddDiff = true;
			bool bFileMustExist = true;
			if (!EsGetPropDataFromGeoDataCenter(strDataUpdateDir, vecStrUpdatePropName[i], vecAttrType[i][j], vecGISet[i][j], bFileMustExist, bAddDiff, mapCellIdAndCellSet))
			{
				EsSetErrorMsg(strErrorMsg_FailedToReadBoreholeFile);
				return false;
			}

			//然后从原始钻孔中获取属于上述网格的记录
			bAddDiff = false;
			bFileMustExist = false;
			if (!EsGetPropDataFromGeoDataCenter(strDataOriginDir, vecStrUpdatePropName[i], vecAttrType[i][j], vecGISet[i][j], bFileMustExist, bAddDiff, mapCellIdAndCellSet))
			{
				EsSetErrorMsg(strErrorMsg_FailedToReadBoreholeFile);
				return false;
			}

			//最后对同一网格内的数据进行归一化			
			if (!EsScaleUp(vecAttrType[i][j], mapCellIdAndCellSet, vecCellTemp[i][j]))
				return false;

			//释放内存
			for (auto pair : mapCellIdAndCellSet)
			{
				delete pair.second;
				pair.second = NULL;
			}

#ifdef EXPORTSCALEUPRESULT
			const string strScaleUpDir = strTargetDir + "\\" + strMatchedDataCenter_ScaleUp+ "\\";
			if (_access(strScaleUpDir.c_str(), 0) == -1)
			{
				_mkdir(strScaleUpDir.c_str());
			}

			const string strScaleUpFile = strScaleUpDir + "modelid_" + vecStrModelID[i][j] + "_" + vecStrUpdatePropName[i] + "_Lod_" + std::to_string(vecLodSet[i][j]) + ".txt";
			EsExportScaleUpCell(strScaleUpFile, vecAttrType[i][j], vecCellTemp[i][j]);
#endif
		}
	}

	//将网格属性按地层分配
	if (!EsSelectDataByLayer(strMatchedFieldName, vecStrUpdatePropName, vecAttrType, vecStrModelID, vecLodSet, vecGISet, vecLayerIDSet, vecCellTemp, vecCellBeforeUpdate, vecCellSet))
		return false;

	return true;
}

bool gridworld::GS::CModelUpdateServerGS::EsDownLoadData(const string& strTargetDir, const string& strPropName, const AttrType& nAttrType, const string& strMatchedFieldName, const string& strLayerID, 
	const GridInfo& giforSampleDownload, const GridInfo& giforUpdate, bool bStochastic, const string& strModelID, const int nLod, const vector<Cell>& vecCellOrigin, vector<Cell>& vecCell)
{
	/*******************从数据库下载样本范围内的所有网格********************/
	//起始网格
	int nMinI, nMinJ, nMinK;
	nMinI = giforSampleDownload.nIOri;
	nMinJ = giforSampleDownload.nJOri;
	nMinK = giforSampleDownload.nKOri;

	//终止网格
	int nMaxI, nMaxJ, nMaxK;
	nMaxI = giforSampleDownload.nIOri + giforSampleDownload.nXNum - 1;
	nMaxJ = giforSampleDownload.nJOri + giforSampleDownload.nYNum - 1;
	nMaxK = giforSampleDownload.nKOri + giforSampleDownload.nZNum - 1;

	//先判断插值范围内的约束属性值是否存在，如果不存在，尝试从相邻LOD获取，若获取失败，则本范围跳过
	if (!EsCheckAndBuildMatchedField(giforSampleDownload, nMinI, nMinJ, nMinK, nMaxI, nMaxJ, nMaxK, strModelID, nLod, strMatchedFieldName))
	{
		EsSetErrorMsg(strErrorMsg_DownLoadData_MatchedFieldNoData);
		return false;
	}

	//获取样本下载范围的网格,接口一次最多下载DEFAULT_LIMIT个网格，所以得分批次下载
	QueryCellsInBoxOfIJK queryCellsInBoxOfIJK(strModelID, nLod);
	queryCellsInBoxOfIJK.Box(nMinI, nMinJ, nMaxI, nMaxJ);
	queryCellsInBoxOfIJK.ZRange(nMinK, nMaxK);

	queryCellsInBoxOfIJK.Select(strMatchedFieldName);
	queryCellsInBoxOfIJK.Select(strPropName);

	queryCellsInBoxOfIJK.Limit(g3dcpp::DEFAULT_LIMIT);

	vecCell.clear();
	while (queryCellsInBoxOfIJK.HasNextPage())
	{
		//如果网格获取失败，则返回
		vector<Grid::Cell> vecCellTemp;
		if (!queryCellsInBoxOfIJK.NextPage(vecCellTemp))
		{
			EsSetErrorMsg(strErrorMsg_DownLoadData_FailedToDownLoadCell);
			return false;
		}

		/*******************遍历下载的网格，获取符合要求的网格属性值********************/		
		for (auto sGridCell : vecCellTemp)
		{
			//构建网格，记录IJK
			Cell cell;
			cell.nI = sGridCell.I();
			cell.nJ = sGridCell.J();
			cell.nK = sGridCell.K();

			//判断网格所属地层是否是目标地层
			FieldValue fieldValue_MatchedFieldName;
			if(!sGridCell.FieldValueOf(strMatchedFieldName, fieldValue_MatchedFieldName))
				continue;

			bool bMatched = false;
			if (std::to_string(fieldValue_MatchedFieldName.ValueAsInt()) == strLayerID)
				bMatched = true;

			//如果不是目标地层，则跳过
			if (!bMatched)
				continue;

			/*******************如果是目标地层，则根据插值算法获取相应数据********************/
			//如果是随机模拟算法，则插值范围内只取原始样本，插值范围外（样本下载范围内）取所有对应属性值；如果是确定性插值算法，则取样本下载范围内的所有原始样本
			if (bStochastic) //随机模拟
			{
				if (cell.nI >= giforUpdate.nIOri && cell.nI <= (giforUpdate.nIOri + giforUpdate.nXNum - 1)
					&& cell.nJ >= giforUpdate.nJOri && cell.nJ <= (giforUpdate.nJOri + giforUpdate.nYNum - 1)
					&& cell.nK >= giforUpdate.nKOri && cell.nK <= (giforUpdate.nKOri + giforUpdate.nZNum - 1))
				{
					//判断网格属性是否来源于钻孔
					bool bOrigin = false;
					for (auto cellOrigin : vecCellOrigin)
					{
						if (cell.nI == cellOrigin.nI && cell.nJ == cellOrigin.nJ && cell.nK == cellOrigin.nK)
						{
							bOrigin = true;
							break;
						}
					}

					//如果不是原始样本，则跳过
					if (!bOrigin)
						continue;

					//如果是目标地层且是原始样本，则获取目标属性值
					FieldValue fieldValue_PropName;
					if(!sGridCell.FieldValueOf(strPropName, fieldValue_PropName))
						continue;
				
					//转换成对应数据类型
					if (nAttrType == Discrete)
					{
						cell.nValue = fieldValue_PropName.ValueAsInt();
					}
					else
					{
						cell.dValue = fieldValue_PropName.ValueAsDouble();
					}				
				}
				else
				{
					//如果是目标地层，则获取目标属性值
					FieldValue fieldValue_PropName;
					if (!sGridCell.FieldValueOf(strPropName, fieldValue_PropName))
						continue;
				
					//转换成对应数据类型
					if (nAttrType == Discrete)
					{
						cell.nValue = fieldValue_PropName.ValueAsInt();
					}
					else
					{
						cell.dValue = fieldValue_PropName.ValueAsDouble();
					}
				}
			}
			else //确定性插值
			{
				//判断网格属性是否来源于钻孔
				bool bOrigin = false;
				for (auto cellOrigin : vecCellOrigin)
				{
					if (cell.nI == cellOrigin.nI && cell.nJ == cellOrigin.nJ && cell.nK == cellOrigin.nK)
					{
						bOrigin = true;
						break;
					}
				}

				//如果不是原始样本，则跳过
				if (!bOrigin)
					continue;

				//如果是目标地层且是原始样本，则获取目标属性值
				FieldValue fieldValue_PropName;
				if (!sGridCell.FieldValueOf(strPropName, fieldValue_PropName))
					continue;			
				//转换成对应数据类型
				if (nAttrType == Discrete)
				{
					cell.nValue = fieldValue_PropName.ValueAsInt();
				}
				else
				{
					cell.dValue = fieldValue_PropName.ValueAsDouble();
				}
			}

			//存储网格
			vecCell.push_back(cell);
		}
	}

	if (vecCell.empty())
	{
		EsSetErrorMsg(strErrorMsg_DownLoadData_NoCellDownLoad);
		return false;
	}

	return true;
}

bool gridworld::GS::CModelUpdateServerGS::EsUpLoadData(const string& strPropName, const AttrType nAttrType, const vector<Cell>& vecCell, const string& strModelID, const int& nLod)
{
	if (!EsUpdateDataValue(strPropName, nAttrType, vecCell, strModelID, nLod))
		return false;
	
	return true;
}

bool gridworld::GS::CModelUpdateServerGS::EsUpLoadData(const string& strPropName, const AttrType nAttrType, const string& strMatchedFieldName, 
	const string& strLayerID, ModelInterPara& tmip, const string& strModelID, const int& nLod)
{
#ifdef EXPORTINTERRESULT
	//将插值结果写出到文件
	const string strResultDir = tmip.strWorkPath.substr(0, tmip.strWorkPath.find_last_of("//") - 1) + "\\" + strMatchedDataCenter_InterResult;
	if (_access(strResultDir.c_str(), 0) == -1)
	{
		_mkdir(strResultDir.c_str());
	}

	const string strFilePath = strResultDir + "\\" + "modelid_" + strModelID + "_" + strPropName + "_Lod_" + std::to_string(nLod) + "_" + 
		strMatchedFieldName + "_" + strLayerID + "_" +
		"IOri" + "_" + std::to_string(tmip.gi.nIOri) + "_" +
		"JOri" + "_" + std::to_string(tmip.gi.nJOri) + "_" +
		"KOri" + "_" + std::to_string(tmip.gi.nKOri) + "_" +
		"XNum" + "_" + std::to_string(tmip.gi.nXNum) + "_" +
		"YNum" + "_" + std::to_string(tmip.gi.nYNum) + "_" +
		"ZNum" + "_" + std::to_string(tmip.gi.nZNum) + ".txt";

	ofstream fout(strFilePath, ios::app);
	if (!fout.is_open())
		return false;
#endif

	//起始网格
	int nMinI, nMinJ, nMinK;
	nMinI = tmip.gi.nIOri;
	nMinJ = tmip.gi.nJOri;
	nMinK = tmip.gi.nKOri;

	//终止网格
	int nMaxI, nMaxJ, nMaxK;
	nMaxI = tmip.gi.nIOri + tmip.gi.nXNum - 1;
	nMaxJ = tmip.gi.nJOri + tmip.gi.nYNum - 1;
	nMaxK = tmip.gi.nKOri + tmip.gi.nZNum - 1;

	//先判断更新范围内的约束属性值是否存在，如果不存在，尝试从相邻LOD获取，若获取失败，则本范围跳过
	EsCheckAndBuildMatchedField(tmip.gi, nMinI, nMinJ, nMinK, nMaxI, nMaxJ, nMaxK, strModelID, nLod, strMatchedFieldName);

	//获取样本下载范围的网格,接口一次最多下载DEFAULT_LIMIT个网格，所以得分批次下载
	QueryCellsInBoxOfIJK queryCellsInBoxOfIJK(strModelID, nLod);
	queryCellsInBoxOfIJK.Box(nMinI, nMinJ, nMaxI, nMaxJ);
	queryCellsInBoxOfIJK.ZRange(nMinK, nMaxK);

	queryCellsInBoxOfIJK.Select(strMatchedFieldName);
	queryCellsInBoxOfIJK.Select(strPropName);

	queryCellsInBoxOfIJK.Limit(g3dcpp::DEFAULT_LIMIT);

	while (queryCellsInBoxOfIJK.HasNextPage())
	{
		//如果网格获取失败，则返回
		vector<Grid::Cell> vecCellTemp;
		if (!queryCellsInBoxOfIJK.NextPage(vecCellTemp))
		{
			EsSetErrorMsg(strErrorMsg_DownLoadData_FailedToDownLoadCell);

#ifdef EXPORTINTERRESULT
			fout.close();
#endif // EXPORTINTERRESULT

			return false;
		}

		/*******************遍历下载的网格，将当前地层内的插值结果整理成网格数据格式********************/
		vector<Cell> vecCell;
		vecCell.clear();
		for (auto sGridCell : vecCellTemp)
		{
			//构建网格，记录IJK
			Cell cell;
			cell.nI = sGridCell.I();
			cell.nJ = sGridCell.J();
			cell.nK = sGridCell.K();

			//判断网格所属地层是否是目标地层
			FieldValue fieldValue_MatchedFieldName;
			if(!sGridCell.FieldValueOf(strMatchedFieldName, fieldValue_MatchedFieldName))
				continue;

			bool bMatched = false;
			if (std::to_string(fieldValue_MatchedFieldName.ValueAsInt()) == strLayerID)
				bMatched = true;

			//如果不是目标地层，则跳过
			if (bMatched)
			{
				if (nAttrType == Discrete)
				{
					cell.nValue = EsGetDiscretePropertyByIJK(tmip, cell.nI, cell.nJ, cell.nK);

#ifdef EXPORTINTERRESULT
					fout << cell.nI << "\t" << cell.nJ << "\t" << cell.nK << "\t" << cell.nValue << "\n";
#endif // EXPORTINTERRESULT

					if (cell.nValue == -1)
						continue;
				}
				else
				{
					cell.dValue = EsGetContinuousPropertyByIJK(tmip, cell.nI, cell.nJ, cell.nK);

#ifdef EXPORTINTERRESULT
					fout << cell.nI << "\t" << cell.nJ << "\t" << cell.nK << "\t" << cell.dValue << "\n";
#endif // EXPORTINTERRESULT

					if (abs(cell.dValue + 1) < 10e-3)
						continue;
				}

				vecCell.push_back(cell);
			}
		}

		//再调用网格数据格式的上传接口更新到数据库
		if (!EsUpdateDataValue(strPropName, nAttrType, vecCell, strModelID, nLod))
		{

#ifdef EXPORTINTERRESULT
			fout.close();
#endif // EXPORTINTERRESULT

			return false;
		}
	}

#ifdef EXPORTINTERRESULT
	fout.close();
#endif // EXPORTINTERRESULT

	return true;
}

bool gridworld::GS::CModelUpdateServerGS::EsReadServerConfigSetting(const string& strConfigPath, string& strRootURL, string& strMatchedFieldName)
{
	TiXmlDocument doc(strConfigPath.c_str());
	if (!doc.LoadFile())
		return false;

	TiXmlHandle hDoc(&doc);
	TiXmlElement* pRoot = hDoc.FirstChildElement().Element();
	if (pRoot == NULL)
		return false;

	TiXmlElement* pElem = pRoot->FirstChild()->ToElement();
	if (pElem == NULL)
		return false;

	strRootURL = pElem->Attribute(strConfigSetting_RootURL);
	strMatchedFieldName = pElem->Attribute(strConfigSetting_MatchedFieldName);

	return true;
}

bool gridworld::GS::CModelUpdateServerGS::EsReadServerConfigProperty(const string& strConfigPath, const string& strPropName, vector<AttrType>& vecAttrType, vector<int>& vecLod, vector<string>& vecModelID)
{
	vecAttrType.clear();
	vecLod.clear();
	vecModelID.clear();

	TiXmlDocument doc(strConfigPath.c_str());
	if (!doc.LoadFile())
		return false;

	TiXmlHandle hDoc(&doc);
	TiXmlElement* pRoot = hDoc.FirstChildElement().Element();
	if (pRoot == NULL)
		return false;

	for (TiXmlNode* pChild = pRoot->FirstChild(); pChild; pChild = pChild->NextSibling())
	{
		TiXmlElement* pElem = pChild->ToElement();
		if (pElem && pElem->ValueTStr() == strConfigProperty && pElem->Attribute(strConfigProperty_PropName) == strPropName)
		{
			int nType = -1;
			pElem->Attribute(strConfigProperty_PropType, &nType);
			if (nType == 0)
				vecAttrType.push_back(Continuous);
			else if (nType == 1)
				vecAttrType.push_back(Discrete);

			int lod;
			pElem->Attribute(strConfigProperty_Lod, &lod);
			vecLod.push_back(lod);

			vecModelID.push_back(pElem->Attribute(strConfigProperty_ModelID));
		}
	}

	if (vecLod.empty() || vecModelID.empty())
		return false;

	return true;
}

bool gridworld::GS::CModelUpdateServerGS::EsReadServerConfigLayer(const string& strConfigPath, const string& strPropName, const int& nLod, const string& strLayerID, InterMethod& nInterpolationMethod)
{
	TiXmlDocument doc(strConfigPath.c_str());
	if (!doc.LoadFile())
		return false;

	TiXmlHandle hDoc(&doc);
	TiXmlElement* pRoot = hDoc.FirstChildElement().Element();
	if (pRoot == NULL)
		return false;

	for (TiXmlNode* pChild = pRoot->FirstChild(); pChild; pChild = pChild->NextSibling())
	{
		TiXmlElement* pElem = pChild->ToElement();
		if (pElem && pElem->ValueTStr() == strConfigProperty && pElem->Attribute(strConfigProperty_PropName) == strPropName && pElem->Attribute(strConfigProperty_Lod) == std::to_string(nLod))
		{
			for (TiXmlNode* pSecondChild = pElem->FirstChild(); pSecondChild; pSecondChild = pSecondChild->NextSibling())
			{
				TiXmlElement* pSecondElem = pSecondChild->ToElement();
				if (pSecondElem && pSecondElem->ValueTStr() == strConfigLayer && pSecondElem->Attribute(strConfigLayer_LayerID) == strLayerID)
				{
					int nMehtod = -1;
					pSecondElem->Attribute(strConfigLayer_InterpolationMethod, &nMehtod);

					switch (nMehtod)
					{
					case 0:
						nInterpolationMethod = SequentialGaussianSimulation;
						break;
					case 1:
						nInterpolationMethod = Kriging;
						break;
					case 2:
						nInterpolationMethod = SequentialIndicatorSimulation;
						break;
					case 3:
						nInterpolationMethod = IndicatorKriging;
						break;
					default:
						doc.SaveFile();
						return false;
					}
					break;
				}
			}
		}
	}

	return true;
}

bool gridworld::GS::CModelUpdateServerGS::EsReadServerConfigVariogram(const string& strConfigPath, const string& strPropName, const int& nLod, const string& strLayerID, VariogramPara& varPara)
{
	TiXmlDocument doc(strConfigPath.c_str());
	if (!doc.LoadFile())
		return false;

	TiXmlHandle hDoc(&doc);
	TiXmlElement* pRoot = hDoc.FirstChildElement().Element();
	if (pRoot == NULL)
		return false;

	for (TiXmlNode* pChild = pRoot->FirstChild(); pChild; pChild = pChild->NextSibling())
	{
		TiXmlElement* pElem = pChild->ToElement();
		if (pElem && pElem->ValueTStr() == strConfigProperty && pElem->Attribute(strConfigProperty_PropName) == strPropName && pElem->Attribute(strConfigProperty_Lod) == std::to_string(nLod))
		{
			for (TiXmlNode* pSecondChild = pElem->FirstChild(); pSecondChild; pSecondChild = pSecondChild->NextSibling())
			{
				TiXmlElement* pSecondElem = pSecondChild->ToElement();
				if (pSecondElem && pSecondElem->ValueTStr() == strConfigLayer && pSecondElem->Attribute(strConfigLayer_LayerID) == strLayerID)
				{
					for (TiXmlNode* pThirdChild = pSecondElem->FirstChild(); pThirdChild; pThirdChild = pThirdChild->NextSibling())
					{
						TiXmlElement* pThirdElem = pThirdChild->ToElement();
						if (pThirdElem && pThirdElem->ValueTStr() == strConfigVariogram)
						{
							int nType = -1;
							pThirdElem->Attribute(strConfigVariogram_VariogramType, &nType);

							switch (nType)
							{
							case 1:
								varPara.varType = Spherical;
								break;
							case 2:
								varPara.varType = Exponential;
								break;
							case 3:
								varPara.varType = Gaussian;
								break;
							default:
								doc.SaveFile();
								return false;
							}

							pThirdElem->Attribute(strConfigVariogram_Sill, &varPara.dSill);
							pThirdElem->Attribute(strConfigVariogram_Nugget, &varPara.dNugget);
							pThirdElem->Attribute(strConfigVariogram_MajorRange, &varPara.dMajorRange);
							pThirdElem->Attribute(strConfigVariogram_MinorRange, &varPara.dMinorRange);
							pThirdElem->Attribute(strConfigVariogram_VerticalRange, &varPara.dVerticalRange);
							pThirdElem->Attribute(strConfigVariogram_Azimuth, &varPara.dAzimuth);
							pThirdElem->Attribute(strConfigVariogram_Dip, &varPara.dDip);
							break;
						}
					}
				}
			}
		}
	}

	return true;
}

bool gridworld::GS::CModelUpdateServerGS::EsReadServerConfigVariogramByFaceCode(const string& strConfigPath, const string& strPropName, const int& nLod, const string& strLayerID, const string& strFaceCode, VariogramPara& varPara)
{
	TiXmlDocument doc(strConfigPath.c_str());
	if (!doc.LoadFile())
		return false;

	TiXmlHandle hDoc(&doc);
	TiXmlElement* pRoot = hDoc.FirstChildElement().Element();
	if (pRoot == NULL)
		return false;

	for (TiXmlNode* pChild = pRoot->FirstChild(); pChild; pChild = pChild->NextSibling())
	{
		TiXmlElement* pElem = pChild->ToElement();
		if (pElem && pElem->ValueTStr() == strConfigProperty && pElem->Attribute(strConfigProperty_PropName) == strPropName && pElem->Attribute(strConfigProperty_Lod) == std::to_string(nLod))
		{
			for (TiXmlNode* pSecondChild = pElem->FirstChild(); pSecondChild; pSecondChild = pSecondChild->NextSibling())
			{
				TiXmlElement* pSecondElem = pSecondChild->ToElement();
				if (pSecondElem && pSecondElem->ValueTStr() == strConfigLayer && pSecondElem->Attribute(strConfigLayer_LayerID) == strLayerID)
				{
					for (TiXmlNode* pThirdChild = pSecondElem->FirstChild(); pThirdChild; pThirdChild = pThirdChild->NextSibling())
					{
						TiXmlElement* pThirdElem = pThirdChild->ToElement();
						if (pThirdElem && pThirdElem->ValueTStr() == strConfigVariogram && pThirdElem->Attribute(strConfigVariogram_FaceCode) == strFaceCode)
						{
							int nType = -1;
							pThirdElem->Attribute(strConfigVariogram_VariogramType, &nType);

							switch (nType)
							{
							case 1:
								varPara.varType = Spherical;
								break;
							case 2:
								varPara.varType = Exponential;
								break;
							case 3:
								varPara.varType = Gaussian;
								break;
							default:
								doc.SaveFile();
								return false;
							}

							pThirdElem->Attribute(strConfigVariogram_Sill, &varPara.dSill);
							pThirdElem->Attribute(strConfigVariogram_Nugget, &varPara.dNugget);
							pThirdElem->Attribute(strConfigVariogram_MajorRange, &varPara.dMajorRange);
							pThirdElem->Attribute(strConfigVariogram_MinorRange, &varPara.dMinorRange);
							pThirdElem->Attribute(strConfigVariogram_VerticalRange, &varPara.dVerticalRange);
							pThirdElem->Attribute(strConfigVariogram_Azimuth, &varPara.dAzimuth);
							pThirdElem->Attribute(strConfigVariogram_Dip, &varPara.dDip);
							break;
						}
					}
				}
			}
		}
	}

	return true;
}

bool gridworld::GS::CModelUpdateServerGS::EsReadServerConfigLayerID(const string& strConfigPath, const string& strPropName, const int& nLod, vector<int>& vecLayerID)
{
	TiXmlDocument doc(strConfigPath.c_str());
	if (!doc.LoadFile())
		return false;

	TiXmlHandle hDoc(&doc);
	TiXmlElement* pRoot = hDoc.FirstChildElement().Element();
	if (pRoot == NULL)
		return false;

	vecLayerID.clear();
	for (TiXmlNode* pChild = pRoot->FirstChild(); pChild; pChild = pChild->NextSibling())
	{
		TiXmlElement* pElem = pChild->ToElement();
		if (pElem && pElem->ValueTStr() == strConfigProperty && pElem->Attribute(strConfigProperty_PropName) == strPropName && pElem->Attribute(strConfigProperty_Lod) == std::to_string(nLod))
		{
			for (TiXmlNode* pSecondChild = pElem->FirstChild(); pSecondChild; pSecondChild = pSecondChild->NextSibling())
			{
				TiXmlElement* pSecondElem = pSecondChild->ToElement();
				if (pSecondElem && pSecondElem->ValueTStr() == strConfigLayer)
				{
					int id = -1;
					pSecondElem->Attribute(strConfigLayer_LayerID, &id);
					vecLayerID.push_back(id);
				}
			}
		}
	}

	if (vecLayerID.empty())
		return false;

	return true;
}

bool gridworld::GS::CModelUpdateServerGS::EsReadServerConfigFaceCode(const string& strConfigPath, const string& strPropName, const int& nLod, const string& strLayerID, vector<int>& vecFaceCode)
{
	TiXmlDocument doc(strConfigPath.c_str());
	if (!doc.LoadFile())
		return false;

	TiXmlHandle hDoc(&doc);
	TiXmlElement* pRoot = hDoc.FirstChildElement().Element();
	if (pRoot == NULL)
		return false;

	vecFaceCode.clear();
	for (TiXmlNode* pChild = pRoot->FirstChild(); pChild; pChild = pChild->NextSibling())
	{
		TiXmlElement* pElem = pChild->ToElement();
		if (pElem && pElem->ValueTStr() == strConfigProperty && pElem->Attribute(strConfigProperty_PropName) == strPropName && pElem->Attribute(strConfigProperty_Lod) == std::to_string(nLod))
		{
			for (TiXmlNode* pSecondChild = pElem->FirstChild(); pSecondChild; pSecondChild = pSecondChild->NextSibling())
			{
				TiXmlElement* pSecondElem = pSecondChild->ToElement();
				if (pSecondElem && pSecondElem->ValueTStr() == strConfigLayer && pSecondElem->Attribute(strConfigLayer_LayerID) == strLayerID)
				{
					for (TiXmlNode* pThirdChild = pSecondElem->FirstChild(); pThirdChild; pThirdChild = pThirdChild->NextSibling())
					{
						TiXmlElement* pThirdElem = pThirdChild->ToElement();
						if (pThirdElem && pThirdElem->ValueTStr() == strConfigVariogram)
						{
							int nCode = -1;
							pThirdElem->Attribute(strConfigVariogram_FaceCode, &nCode);
							vecFaceCode.push_back(nCode);
						}
					}
				}
			}
		}
	}

	if (vecFaceCode.empty())
		return false;

	return true;
}

bool gridworld::GS::CModelUpdateServerGS::EsExportOriginDataOfOtherPropsEffectedByCurProp(const string& strConfigPath, const string& strTargetDir, const string& strCurPropName, const string& strLayerID, const int& nLod, 
	const GridInfo& gi, const vector<GridInfo>& vecGiforUpdate)
{
	const string strDataOriginDir = strTargetDir + "\\" + strMatchedDataCenter_DataOrigin + "\\";

	vector<string> vecStrFilePath;
	vector<string> vecStrFileName;
	vecStrFileName.clear();
	vecStrFilePath.clear();
	MyFileRetrieveByExt(strDataOriginDir, "txt", false, vecStrFileName, vecStrFilePath);
	if (vecStrFilePath.empty())
		return false;

	for (size_t idxProp = 0; idxProp < vecStrFilePath.size(); idxProp++)
	{
		const string strPropName = vecStrFileName[idxProp].substr(0, vecStrFileName[idxProp].find_last_of('.'));
		if(strPropName == strCurPropName)
			continue;

		vector<AttrType> vecAttrType;
		vector<int> vecLod;
		vector<string> vecModelID;
		if(!EsReadServerConfigProperty(strConfigPath, strPropName, vecAttrType, vecLod, vecModelID))
			continue;

		int idx = 0;
		bool bExist = false;
		for (idx; idx < vecLod.size(); idx++)
		{
			if (vecLod[idx] == nLod)
			{
				bExist = true;
				break;
			}
		}
		if(!bExist)
			continue;

		int rangeI = INT_MIN; //评估参数：I方向搜索步长
		int rangeJ = INT_MIN; //评估参数：J方向搜索步长
		int rangeK = INT_MIN; //评估参数：K方向搜索步长
		if (vecAttrType[idx] == Continuous)
		{
			VariogramPara varPara;
			if (!EsReadServerConfigVariogram(strConfigPath, strPropName, nLod, strLayerID, varPara))
				continue;

			rangeI = ceil(varPara.dMajorRange / gi.dXStep);
			rangeJ = ceil(varPara.dMinorRange / gi.dYStep);
			rangeK = ceil(varPara.dVerticalRange / gi.dZStep);
		}
		else
		{
			vector<int> vecFaceCode;
			if (!EsReadServerConfigFaceCode(strConfigPath, strPropName, nLod, strLayerID, vecFaceCode))
				continue;

			if (vecFaceCode.empty())
				continue;

			for (size_t idxCode = 0; idxCode < vecFaceCode.size(); idxCode++)
			{
				VariogramPara varPara;
				string strFaceCode = std::to_string(vecFaceCode[idxCode]);
				if (!EsReadServerConfigVariogramByFaceCode(strConfigPath, strPropName, nLod, strLayerID, strFaceCode, varPara))
					continue;

				int tempI = ceil(varPara.dMajorRange / gi.dXStep);
				int tempJ = ceil(varPara.dMinorRange / gi.dYStep);
				int tempK = ceil(varPara.dVerticalRange / gi.dZStep);

				if (rangeI <= tempI)
					rangeI = tempI;
				if (rangeJ <= tempJ)
					rangeJ = tempJ;
				if (rangeK <= tempK)
					rangeK = tempK;
			}
		}

		vector<int> vecXOri;
		vecXOri.resize(vecGiforUpdate.size());
		vector<int> vecYOri;
		vecYOri.resize(vecGiforUpdate.size());
		vector<int> vecZOri;
		vecZOri.resize(vecGiforUpdate.size());
		vector<int> vecXEnd;
		vecXEnd.resize(vecGiforUpdate.size());
		vector<int> vecYEnd;
		vecYEnd.resize(vecGiforUpdate.size());
		vector<int> vecZEnd;
		vecZEnd.resize(vecGiforUpdate.size());
		for (size_t idxGI = 0; idxGI < vecGiforUpdate.size(); idxGI++)
		{
			vecXOri[idxGI] = vecGiforUpdate[idxGI].nIOri - rangeI;
			vecYOri[idxGI] = vecGiforUpdate[idxGI].nJOri - rangeJ;
			vecZOri[idxGI] = vecGiforUpdate[idxGI].nKOri - rangeK;

			vecXEnd[idxGI] = vecGiforUpdate[idxGI].nIOri + vecGiforUpdate[idxGI].nXNum - 1 + rangeI;
			vecYEnd[idxGI] = vecGiforUpdate[idxGI].nJOri + vecGiforUpdate[idxGI].nYNum - 1 + rangeJ;
			vecZEnd[idxGI] = vecGiforUpdate[idxGI].nKOri + vecGiforUpdate[idxGI].nZNum - 1 + rangeK;
		}

		ifstream fin(vecStrFilePath[idxProp], ios::in);
		if (!fin.is_open())
			continue;

		const string strOutDir = strTargetDir + "\\" + strMatchedDataCenter_DataRelational;
		if (_access(strOutDir.c_str(), 0) == -1)
		{
			_mkdir(strOutDir.c_str());
		}

		const string strFileOut = strOutDir + "\\" + strPropName + ".txt";
		ofstream fout(strFileOut, ios::app);
		if (!fout.is_open())
		{
			fin.close();
			continue;
		}

		while (!fin.eof())
		{
			string strLine;
			vector<string> vecStrData;

			getline(fin, strLine);
			split(strLine, vecStrData);
			if (vecStrData.size() != 4)
				continue;

			double x = atof(vecStrData[0].c_str());
			double y = atof(vecStrData[1].c_str());
			double z = atof(vecStrData[2].c_str());

			int nI = floor((x - gi.dXOri + 0.5 * gi.dXStep) / gi.dXStep) + gi.nIOri;
			int nJ = floor((y - gi.dYOri + 0.5 * gi.dYStep) / gi.dYStep) + gi.nJOri;
			int nK = floor((z - gi.dZOri + 0.5 * gi.dZStep) / gi.dZStep) + gi.nKOri;

			bool bInArea = false;
			for (size_t idxGI = 0; idxGI < vecGiforUpdate.size(); idxGI++)
			{
				if (nI >= vecXOri[idxGI] && nI <= vecXEnd[idxGI] && nJ >= vecYOri[idxGI] && nJ <= vecYEnd[idxGI] && nK >= vecZOri[idxGI] && nK <= vecZEnd[idxGI])
				{
					bInArea = true;
					break;
				}
			}

			if (bInArea)
				fout << strLine << "\n";
		}

		fin.close();
		fout.close();
	}

	return true;
}

bool gridworld::GS::CModelUpdateServerGS::EsExtractPropFromCurrentLodToLowerLod(const GridInfo& gi, const string &strPropName, const string& strModelID, const int nLod)
{
	Point3D ptMin, ptMax;
	ptMin.X(gi.dXOri + (gi.nIOri - gi.nIOri)*gi.dXStep);
	ptMin.Y(gi.dYOri + (gi.nJOri - gi.nJOri)*gi.dYStep);
	ptMin.Z(gi.dZOri + (gi.nKOri - gi.nKOri)*gi.dZStep);
	ptMax.X(gi.dXOri + gi.nXNum*gi.dXStep);
	ptMax.Y(gi.dYOri + gi.nYNum*gi.dYStep);
	ptMax.Z(gi.dZOri + gi.nZNum*gi.dZStep);

	BlockExtractor blockExtractor(strModelID, nLod);
	blockExtractor.SetBox(ptMin, ptMax);
	if (!blockExtractor.Extract(strPropName))
	{
		EsSetErrorMsg(blockExtractor.Error());
		return false;
	}

	return true;
}

bool gridworld::GS::CModelUpdateServerGS::EsGetGlobalGridInfoFromModelDataBase(const string& strModelID, const int& nLod, GridInfo& gi)
{
	if (nLod == 0)
	{
		GetGridLOD getGridLod;
		Grid::LOD lod;
		if (!getGridLod.Get(strModelID, nLod, lod))
		{
			EsSetErrorMsg(strErrorMsg_FailedToGetGridInfoFromDataBase_NoConnect);
			return false;
		}

		Grid::CellIJK cellMinIJK = lod.MinIJK();
		Grid::CellIJK cellMaxIJK = lod.MaxIJK();
		gi.nIOri = cellMinIJK.I();
		gi.nJOri = cellMinIJK.J();
		gi.nKOri = cellMinIJK.K();

		gi.nXNum = cellMaxIJK.I() - cellMinIJK.I();
		gi.nYNum = cellMaxIJK.J() - cellMinIJK.J();
		gi.nZNum = cellMaxIJK.K() - cellMinIJK.K();

		Point3D ptStep = lod.Step();
		gi.dXStep = ptStep.X();
		gi.dYStep = ptStep.Y();
		gi.dZStep = ptStep.Z();

		GetGrid getGrid;
		Grid grid;
		if(!getGrid.Get(strModelID, grid))
		{
			EsSetErrorMsg(strErrorMsg_FailedToGetGridInfoFromDataBase_NoConnect);
			return false;
		}

		Point3D ptOri = grid.Origin();
		gi.dXOri = ptOri.X() + (0.5 + gi.nIOri)*gi.dXStep;
		gi.dYOri = ptOri.Y() + (0.5 + gi.nJOri)*gi.dYStep;
		gi.dZOri = ptOri.Z() + (0.5 + gi.nKOri)*gi.dZStep;
	}
	else
	{
		//先获取LOD=0的网格信息
		GetGridLOD getGridLod;
		Grid::LOD lod_zero;
		if (!getGridLod.Get(strModelID, 0, lod_zero))
		{
			EsSetErrorMsg(strErrorMsg_FailedToGetGridInfoFromDataBase_NoConnect);
			return false;
		}

		Grid::CellIJK lod_zero_cellMinIJK = lod_zero.MinIJK();
		Grid::CellIJK lod_zero_cellMaxIJK = lod_zero.MaxIJK();
		int lod_zero_nIOri = lod_zero_cellMinIJK.I();
		int lod_zero_nJOri = lod_zero_cellMinIJK.J();
		int lod_zero_nKOri = lod_zero_cellMinIJK.K();

		int lod_zero_nIEnd = lod_zero_cellMaxIJK.I();
		int lod_zero_nJEnd = lod_zero_cellMaxIJK.J();
		int lod_zero_nKEnd = lod_zero_cellMaxIJK.K();

		Point3D lod_zero_ptStep = lod_zero.Step();
		double lod_zero_dXStep = lod_zero_ptStep.X();
		double lod_zero_dYStep = lod_zero_ptStep.Y();
		double lod_zero_dZStep = lod_zero_ptStep.Z();

		//再把LOD=0的最大最小点转化成xyz格式
		double lod_zero_DeltaXOri = lod_zero_nIOri*lod_zero_dXStep;
		double lod_zero_DeltaYOri = lod_zero_nJOri*lod_zero_dYStep;
		double lod_zero_DeltaZOri = lod_zero_nKOri*lod_zero_dZStep;

		double lod_zero_DeltaXEnd = lod_zero_nIEnd*lod_zero_dXStep;
		double lod_zero_DeltaYEnd = lod_zero_nJEnd*lod_zero_dYStep;
		double lod_zero_DeltaZEnd = lod_zero_nKEnd*lod_zero_dZStep;

		//然后获取当前LOD的步长
		Grid::LOD lod;
		if (!getGridLod.Get(strModelID, nLod, lod))
		{
			EsSetErrorMsg(strErrorMsg_FailedToGetGridInfoFromDataBase_NoConnect);
			return false;
		}

		Point3D ptStep = lod.Step();
		gi.dXStep = ptStep.X();
		gi.dYStep = ptStep.Y();
		gi.dZStep = ptStep.Z();

		gi.nIOri = int(lod_zero_DeltaXOri / gi.dXStep);
		gi.nJOri = int(lod_zero_DeltaYOri / gi.dYStep);
		gi.nKOri = int(lod_zero_DeltaZOri / gi.dZStep);

		gi.nXNum = int(lod_zero_DeltaXEnd / gi.dXStep) - gi.nIOri;
		gi.nYNum = int(lod_zero_DeltaYEnd / gi.dYStep) - gi.nJOri;
		gi.nZNum = int(lod_zero_DeltaZEnd / gi.dZStep) - gi.nKOri;

		GetGrid getGrid;
		Grid grid;
		if(!getGrid.Get(strModelID, grid))
		{
			EsSetErrorMsg(strErrorMsg_FailedToGetGridInfoFromDataBase_NoConnect);
			return false;
		}

		Point3D ptOri = grid.Origin();
		gi.dXOri = ptOri.X() + (0.5 + gi.nIOri)*gi.dXStep;
		gi.dYOri = ptOri.Y() + (0.5 + gi.nJOri)*gi.dYStep;
		gi.dZOri = ptOri.Z() + (0.5 + gi.nKOri)*gi.dZStep;
	}
	
	return true;
}

bool gridworld::GS::CModelUpdateServerGS::EsSelectRecordToCell(const AttrType& nAttrType, const vector<Record>& vecRecord, const GridInfo& gi, bool& bAdd, map<int, vector<Cell>*>& mapCellIdAndCellSet)
{
	if (vecRecord.empty())
		return false;

	//将钻孔记录匹配到对应的网格
	int nCellCount = 0;
	for (auto record : vecRecord)
	{
		Cell cell;
		cell.nI = floor((record.dX - gi.dXOri + 0.5 * gi.dXStep) / gi.dXStep) + gi.nIOri;
		cell.nJ = floor((record.dY - gi.dYOri + 0.5 * gi.dYStep) / gi.dYStep) + gi.nJOri;
		cell.nK = floor((record.dZ - gi.dZOri + 0.5 * gi.dZStep) / gi.dZStep) + gi.nKOri;
		if(nAttrType == Discrete)
			cell.nValue = atoi(record.strValue.c_str());
		else
			cell.dValue = atof(record.strValue.c_str());

		bool bExist = false;
		for (auto pair : mapCellIdAndCellSet)
		{
			Cell tempCell = (*pair.second).back();
			if (tempCell.nI == cell.nI && tempCell.nJ == cell.nJ && tempCell.nK == cell.nK)
			{
				pair.second->push_back(cell);
				bExist = true;
				break;
			}
		}
		if ((!bExist) && bAdd)
		{
			vector<Cell>* pTempCellSet = new vector<Cell>;
			pTempCellSet->push_back(cell);
			mapCellIdAndCellSet[nCellCount] = pTempCellSet;
			nCellCount++;
		}
	}

	if (mapCellIdAndCellSet.empty())
		return false;

	return true;
}

bool gridworld::GS::CModelUpdateServerGS::EsScaleUp(const AttrType& nAttrType, map<int, vector<Cell>*>& mapCellIdAndCellSet, vector<Cell>& vecCell)
{	
	if (nAttrType == Discrete)
	{
		//离散：对网格数据进行粗化-取最多
		vecCell.resize(mapCellIdAndCellSet.size());
		int nCellCount = 0;
		for (auto pair : mapCellIdAndCellSet)
		{
			vecCell[nCellCount] = (*pair.second).back();

			vector<int> vecValue;
			vecValue.resize((*pair.second).size());
			for (size_t i = 0; i < (*pair.second).size(); i++)
			{
				vecValue[i] = (*pair.second).at(i).nValue;
			}

			int nMost = INT_MIN;
			size_t idx = 0;
			for (size_t i = 0; i < vecValue.size(); i++)
			{
				int nCount = count(vecValue.begin(), vecValue.end(), vecValue.at(i));
				if (nCount > nMost)
				{
					idx = i;
					nMost = nCount;
				}
			}

			vecCell[nCellCount].nValue = (*pair.second).at(idx).nValue;
			nCellCount++;
		}
	}
	else
	{
		//连续：对网格数据进行粗化-取平均
		vecCell.resize(mapCellIdAndCellSet.size());
		int nCellCount = 0;
		for (auto pair : mapCellIdAndCellSet)
		{
			vecCell[nCellCount] = (*pair.second).back();
			double dSum = 0;
			for (auto cell : (*pair.second))
				dSum += cell.dValue;

			int nNum = (int)(*pair.second).size();
			vecCell[nCellCount].dValue = dSum / nNum;
			nCellCount++;
		}		
	}

	return true;
}

bool gridworld::GS::CModelUpdateServerGS::EsUpdateDataValue(const string& strPropName, const AttrType& nAttrType, const vector<Cell>& vecCell, const string& strModelID, const int& nLod)
{
	int nCellNum = (int)vecCell.size();
	int nLoopNum = nCellNum / UpLoadLimit;
	int nCellNum_Rest = nCellNum % UpLoadLimit;
	
	//先循环nLoopNum次上传UpLoadLimit个的网格
	for (int idxLoop = 0; idxLoop < nLoopNum; idxLoop++)
	{
		UpdateGridCells updateGridCells(strModelID, nLod);
		for (int i = 0; i < UpLoadLimit; i++)
		{
			Grid::Cell cell;
			cell.IsValid(true);
			cell.I(vecCell[i + idxLoop*UpLoadLimit].nI);
			cell.J(vecCell[i + idxLoop*UpLoadLimit].nJ);
			cell.K(vecCell[i + idxLoop*UpLoadLimit].nK);

			FieldValue fieldValue;
			fieldValue.FieldName(strPropName);

			if (nAttrType == Discrete)
				fieldValue.ValueAsInt(vecCell[i + idxLoop*UpLoadLimit].nValue);
			else
				fieldValue.ValueAsDouble(vecCell[i + idxLoop*UpLoadLimit].dValue);
				
			cell.AddFieldValue(fieldValue);

			updateGridCells.AddCell(cell);		
		}

		//如果上传失败，先休眠一段时间然后尝试重新上传，如果还是失败，则报错退出
		if (!updateGridCells.UpdateByIJK())
		{
			Sleep(SleepTime);
			if (!updateGridCells.UpdateByIJK())
			{
				EsSetErrorMsg(strErrorMsg_UpLoadData_FailedToUploadCell);
				return false;
			}
		}
	}

	//然后上传剩余不足UpLoadLimit个的网格
	if (nCellNum_Rest > 0)
	{
		UpdateGridCells updateGridCells(strModelID, nLod);
		for (int i = 0; i < nCellNum_Rest; i++)
		{
			Grid::Cell cell;
			cell.IsValid(true);
			cell.I(vecCell[i + nLoopNum*UpLoadLimit].nI);
			cell.J(vecCell[i + nLoopNum*UpLoadLimit].nJ);
			cell.K(vecCell[i + nLoopNum*UpLoadLimit].nK);

			FieldValue fieldValue;
			fieldValue.FieldName(strPropName);

			if (nAttrType == Discrete)
				fieldValue.ValueAsInt(vecCell[i + nLoopNum*UpLoadLimit].nValue);
			else
				fieldValue.ValueAsDouble(vecCell[i + nLoopNum*UpLoadLimit].dValue);

			cell.AddFieldValue(fieldValue);

			updateGridCells.AddCell(cell);
		}

		//如果上传失败，先休眠一段时间然后尝试重新上传，如果还是失败，则报错退出
		if (!updateGridCells.UpdateByIJK())
		{
			Sleep(SleepTime);
			if (!updateGridCells.UpdateByIJK())
			{
				EsSetErrorMsg(strErrorMsg_UpLoadData_FailedToUploadCell);
				return false;
			}
		}
	}

	return true;
}

bool gridworld::GS::CModelUpdateServerGS::EsSelectDataByLayer(const string& strMatchedFieldName, const vector<string>& vecStrUpdatePropName, const vector<vector<AttrType>>& vecAttrType, const vector<vector<string>>& vecStrModelID, vector<vector<int>>& vecLodSet, 
	vector<vector<GridInfo>>& vecGISet, const vector<vector<int>>& vecLayerIDSet, const vector<vector<vector<Cell>>>& vecCellIn, const vector<vector<vector<Cell>>>& vecCellBeforeUpdate, vector<vector<vector<vector<Cell>>>>& vecCellOut)
{
	vecCellOut.clear();
	vecCellOut.resize(vecCellIn.size());
	for (size_t idxProp = 0; idxProp < vecCellIn.size(); idxProp++)
	{
		vecCellOut[idxProp].resize(vecCellIn[idxProp].size());
		for (size_t idxLod = 0; idxLod < vecCellIn[idxProp].size(); idxLod++)
		{			
			int nMinI, nMinJ, nMinK;
			int nMaxI, nMaxJ, nMaxK;
			nMinI = nMinJ = nMinK = INT_MAX;
			nMaxI = nMaxJ = nMaxK = INT_MIN;

			QueryCellsByIJK queryCellsByIJK(vecStrModelID[idxProp][idxLod], vecLodSet[idxProp][idxLod]);				
			for (size_t idxCell = 0; idxCell < vecCellIn[idxProp][idxLod].size(); idxCell++)
			{
				queryCellsByIJK.AddCell(vecCellIn[idxProp][idxLod][idxCell].nI, vecCellIn[idxProp][idxLod][idxCell].nJ, vecCellIn[idxProp][idxLod][idxCell].nK);

				if (vecCellIn[idxProp][idxLod][idxCell].nI < nMinI)
					nMinI = vecCellIn[idxProp][idxLod][idxCell].nI;
				if (vecCellIn[idxProp][idxLod][idxCell].nJ < nMinJ)
					nMinJ = vecCellIn[idxProp][idxLod][idxCell].nJ;
				if (vecCellIn[idxProp][idxLod][idxCell].nK < nMinK)
					nMinK = vecCellIn[idxProp][idxLod][idxCell].nK;
				if (vecCellIn[idxProp][idxLod][idxCell].nI > nMaxI)
					nMaxI = vecCellIn[idxProp][idxLod][idxCell].nI;
				if (vecCellIn[idxProp][idxLod][idxCell].nJ > nMaxJ)
					nMaxJ = vecCellIn[idxProp][idxLod][idxCell].nJ;
				if (vecCellIn[idxProp][idxLod][idxCell].nK > nMaxK)
					nMaxK = vecCellIn[idxProp][idxLod][idxCell].nK;
			}
			
			queryCellsByIJK.Select(strMatchedFieldName);

			if (!EsCheckAndBuildMatchedField(vecGISet[idxProp][idxLod], nMinI, nMinJ, nMinK, nMaxI, nMaxJ, nMaxK, vecStrModelID[idxProp][idxLod], vecLodSet[idxProp][idxLod], strMatchedFieldName))
				continue;

#ifdef MATCHVALUE
			queryCellsByIJK.Select(vecStrUpdatePropName[idxProp]);
#endif // MATCHVALUE	

			vector<Grid::Cell> vecCellTemp;
			if (!queryCellsByIJK.Query(vecCellTemp))
			{
				EsSetErrorMsg(strErrorMsg_SelectToLayer_FailedToGetMajorCellFromDataBase);
				return false;
			}

			vecCellOut[idxProp][idxLod].resize(vecLayerIDSet[idxProp].size());
			for (size_t idxCell = 0; idxCell < vecCellIn[idxProp][idxLod].size(); idxCell++)
			{

#ifdef MATCHVALUE
				if (vecSGridVolumeMatchedField[idxCell].cellFields.empty() || vecSGridVolumePropValue[idxCell].cellFields.empty())
					continue;
#else
				//if (vecSGridVolumeMatchedField[idxCell].cellFields.empty())
				//	continue;
#endif // MATCHVALUE				
				FieldValue fieldValue_MatchedFieldName;
				if (!vecCellTemp[idxCell].FieldValueOf(strMatchedFieldName, fieldValue_MatchedFieldName))
					continue;

#ifdef MATCHVALUE
				bool bOrigin = false;
				for (size_t idx = 0; idx < vecCellBeforeUpdate[idxProp][idxLod].size(); idx++)
				{
					if (vecCellIn[idxProp][idxLod][idxCell].nI == vecCellBeforeUpdate[idxProp][idxLod][idx].nI
						&& vecCellIn[idxProp][idxLod][idxCell].nJ == vecCellBeforeUpdate[idxProp][idxLod][idx].nJ
						&& vecCellIn[idxProp][idxLod][idxCell].nK == vecCellBeforeUpdate[idxProp][idxLod][idx].nK)
					{
						bOrigin = true;
						break;
					}
				}

				if (bOrigin)
				{
					if (vecAttrType[idxProp][idxLod] == Discrete)
					{
						if (vecSGridVolumePropValue[idxCell].cellFields[0].strFieldValue != "")
						{
							int nValue = atoi(vecSGridVolumePropValue[idxCell].cellFields[0].strFieldValue.c_str());
							if (nValue == vecCellIn[idxProp][idxLod][idxCell].nValue)
								continue;
						}
					}
					else
					{
						if (vecSGridVolumePropValue[idxCell].cellFields[0].strFieldValue != "")
						{
							int dValue = atof(vecSGridVolumePropValue[idxCell].cellFields[0].strFieldValue.c_str());
							if (abs(dValue - vecCellIn[idxProp][idxLod][idxCell].nValue) < 10e-5)
								continue;
						}
					}
				}
#endif // MATCHVALUE				

				int nLayerId = fieldValue_MatchedFieldName.ValueAsInt();
				for (size_t idxLayer = 0; idxLayer < vecLayerIDSet[idxProp].size(); idxLayer++)
				{
					if (nLayerId == vecLayerIDSet[idxProp][idxLayer])
					{
						vecCellOut[idxProp][idxLod][idxLayer].push_back(vecCellIn[idxProp][idxLod][idxCell]);
						break;
					}
				}
			}
		}
	}

	return true;
}

void gridworld::GS::CModelUpdateServerGS::split(const string source, vector<string>& dest)
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

void gridworld::GS::CModelUpdateServerGS::MyFileRetrieveByExt(const string& strRootDir, const string& strExt, bool bDoWithSubDir, vector<string>& vecStrFileName, vector<string>& vecStrFilePath)
{
	string strDir = strRootDir;
	strDir.append("\\*");

	WIN32_FIND_DATAA FindFileData;
	HANDLE hFind = FindFirstFileA(strDir.c_str(), &FindFileData);

	if (hFind == INVALID_HANDLE_VALUE)
		return;

	while (FindNextFileA(hFind, &FindFileData))
	{
		if (FindFileData.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY)
		{
			string strFileName = FindFileData.cFileName;
			string strFileExt = strFileName.substr(strFileName.find_last_of(".") + 1);
			if (strExt == "*" || _stricmp(strFileExt.c_str(), strExt.c_str()) == 0)
			{
				vecStrFileName.push_back(strFileName);
				vecStrFilePath.push_back(strRootDir + "\\" + strFileName);
			}
			continue;
		}
		else if (FindFileData.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY
			&& strcmp(FindFileData.cFileName, ".") != 0
			&& strcmp(FindFileData.cFileName, "..") != 0
			&& bDoWithSubDir)
		{
			string strSubDir = strRootDir;
			strSubDir.append("\\");
			strSubDir.append(FindFileData.cFileName);

			MyFileRetrieveByExt(strSubDir, strExt, true, vecStrFileName, vecStrFilePath);
		}
	}

	FindClose(hFind);
}

bool gridworld::GS::CModelUpdateServerGS::EsGetPropDataFromGeoDataCenter(const string& strDataDir, const string& strPropName, const AttrType& nAttrType, const GridInfo& gi, bool& bFileMustExit, bool& bAddDiff, map<int, vector<Cell>*>& mapCellIdAndCellSet)
{
	const string& strPropFile = strDataDir + strPropName + ".txt";

	vector<Record> vecRecord;
	if (!EsGetBoreholeRecordFromFile(strPropFile, vecRecord))
	{
		if (bFileMustExit)
			return false;
		else
			return true;
	}

	//如果文件无数据，如果文件不需要存在，则返回true，如果必须存在，则返回false
	if (vecRecord.empty())
	{
		if (bFileMustExit)
			return false;
		else
			return true;
	}

	if (!EsSelectRecordToCell(nAttrType, vecRecord, gi, bAddDiff, mapCellIdAndCellSet))
		return false;	

	return true;
}

bool gridworld::GS::CModelUpdateServerGS::EsGetBoreholeRecordFromFile(const string& strBoreholeFile, vector<Record>& vecRecord)
{
	ifstream fin(strBoreholeFile, ios::in);
	if (!fin.is_open())
		return false;

	string strLine;
	vector<string> vecStrData;
	while (!fin.eof())
	{
		vecStrData.clear();
		getline(fin, strLine);
		split(strLine, vecStrData);
		if (vecStrData.size() < 4)
			continue;

		Record record;
		record.dX = atof(vecStrData[0].c_str());
		record.dY = atof(vecStrData[1].c_str());
		record.dZ = atof(vecStrData[2].c_str());
		record.strValue = vecStrData[3].c_str();

		vecRecord.push_back(record);
	}

	return true;
}

bool gridworld::GS::CModelUpdateServerGS::EsExportScaleUpCell(const string& strSacleUpFilePath, const AttrType nAttrType, const vector<Cell>& vecCell)
{
	ofstream fout(strSacleUpFilePath, ios::out);
	if (!fout.is_open())
		return false;

	if (nAttrType == Discrete)
	{
		for (auto cell : vecCell)
		{
			fout << cell.nI << "\t" << cell.nJ << "\t" << cell.nK << "\t" << cell.nValue << "\n";
		}
	}
	else
	{
		for (auto cell : vecCell)
		{
			fout << cell.nI << "\t" << cell.nJ << "\t" << cell.nK << "\t" << cell.dValue << "\n";
		}
	}

	fout.close();
	return true;	
}

bool gridworld::GS::CModelUpdateServerGS::EsCheckAndBuildMatchedField(const GridInfo& gi, const int nMinI, const int nMinJ, const int nMinK, const int nMaxI, const int nMaxJ, const int nMaxK,
	const string strModelID, const int nLod, const string& strMatchedFieldName)
{
	Point3D ptMin, ptMax;
	ptMin.X(gi.dXOri + (nMinI - gi.nIOri)*gi.dXStep);
	ptMin.Y(gi.dYOri + (nMinJ - gi.nJOri)*gi.dYStep);
	ptMin.Z(gi.dZOri + (nMinK - gi.nKOri)*gi.dZStep);
	ptMax.X(gi.dXOri + (nMaxI - gi.nIOri)*gi.dXStep);
	ptMax.Y(gi.dYOri + (nMaxJ - gi.nJOri)*gi.dYStep);
	ptMax.Z(gi.dZOri + (nMaxK - gi.nKOri)*gi.dZStep);

	BlockBuilder blockBuilder(strModelID, nLod);
	blockBuilder.SetBox(ptMin, ptMax);
	if (!blockBuilder.CheckAndBuild(strMatchedFieldName))
	{
		EsSetErrorMsg(blockBuilder.Error());
		return false;
	}

	return true;
}