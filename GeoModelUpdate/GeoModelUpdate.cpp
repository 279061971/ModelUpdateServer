#include <iostream>
#include <fstream>
#include "windows.h"
#include "EsModelUpdateServerGS.h"
using namespace gridworld;
using namespace gridworld::GS;
using namespace std;

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		cout << "错误：没有输入参数\n";
		return 1;
	}
	const string strTargetDir = argv[1];

	CModelUpdateServerGS server; //构建更新服务
	int res = server.EsStartServer(strTargetDir);
	return res;
}

//void main(int argc, char* argv[])
//{
//	/*********************初始化服务************************/
//	if (argc < 2)
//	{
//		cout << "错误：没有输入参数\n";
//		return;
//	}
//	const string strTargetDir = argv[1];
//
//	string strMsg = "";
//	string strRootURL = ""; //模型数据服务中心根路径
//	string strMatchedFieldName = ""; //网格所属地层ID属性列的列名称
//	const string strConfigPath = strTargetDir + "\\" + strMatchedDataCenter_Config; //自动更新服务配置文件
//	const string strLogFile = strTargetDir + "\\log.txt"; //日志文件
//
//	CModelUpdateServerGS server; //构建更新服务
//	if (!server.EsReadServerConfigSetting(strConfigPath, strRootURL, strMatchedFieldName))
//	{
//		strMsg = server.EsGetTime() + " " + server.EsGetMsg();
//		cout << strMsg << "\n";
//		server.EsWriteToLog(strLogFile, strMsg);
//		return;
//	}
//
//	/*********************启动服务************************/	
//	strMsg = server.EsGetTime() + "*********************启动自动更新服务************************";
//	cout << strMsg << "\n";
//	server.EsWriteToLog(strLogFile, strMsg);
//
//	/*********************从地质大数据中心检查钻孔数据是否有更新************************/	
//	strMsg = server.EsGetTime() + "正在检测地质大数据中心是否有更新";
//	cout << strMsg << "\n";
//	server.EsWriteToLog(strLogFile, strMsg);
//
//	vector<string> vecStrPropNameUpdate; //更新的属性名集
//	if (!server.EsCheckIsUpdated(strTargetDir, vecStrPropNameUpdate))
//	{		
//		strMsg = server.EsGetTime() + server.EsGetMsg();
//		cout << strMsg << "\n";
//		server.EsWriteToLog(strLogFile, strMsg);
//		return;
//	}
//
//	/*********************从地质大数据中心和模型数据服务中心获取更新的钻孔相关信息************************/
//	strMsg = server.EsGetTime() + "检测到以下属性有更新：";
//	for (auto propName : vecStrPropNameUpdate)
//	{
//		strMsg += propName + "\t";
//	}
//	cout << strMsg << ",正在获取更新的钻孔，并粗化到网格...\n";
//	server.EsWriteToLog(strLogFile, strMsg);
//
//	vector<vector<AttrType>> vecAttrType; //更新的属性类型集         属性/Lod
//	vector<vector<GridInfo>> vecGridInfo; //属性对应的网格信息集     属性/Lod
//	vector<vector<string>> vecStrCoverageID; //属性对应的属性场ID集  属性/Lod
//	vector<vector<string>> vecStrShapeID; //属性对应的几何场ID集     属性/Lod
//	vector<vector<string>> vecStrFeatureID; //属性对应的要素ID集     属性/Lod
//	vector<vector<string>> vecStrFeaClassID; //属性对应的要素类ID集  属性/Lod
//	vector<vector<int>> vecLayerIDSet; //属性对应的地层块ID集        属性/LayerId
//	vector<vector<int>> vecLodSet; //属性对应的地层块ID集            属性/Lod
//	vector<vector<vector<vector<Cell>>>> vecUpdateDataSet; //更新的网格样本    属性/Lod/地层
//	vector<vector<vector<Cell>>> vecCellBeforeUpdate;      //更新前的网格样本  属性/Lod
//	vector<vector<vector<Cell>>> vecCellAfterUpdate;       //更新后的网格样本  属性/Lod
//	if (!server.EsGetUpdatedSampleData(strTargetDir, strConfigPath, strRootURL, strMatchedFieldName, vecStrPropNameUpdate, vecAttrType, vecUpdateDataSet, vecCellAfterUpdate, vecStrCoverageID, vecStrShapeID, vecStrFeatureID, vecStrFeaClassID, vecLodSet, vecLayerIDSet, vecGridInfo))
//	{
//		strMsg = server.EsGetTime() + server.EsGetMsg();
//		cout << strMsg << "\n";
//		server.EsWriteToLog(strLogFile, strMsg);
//		return;
//	}
//	
//	strMsg = server.EsGetTime() + "更新的钻孔粗化成功";
//	server.EsWriteToLog(strLogFile, strMsg);
//
//	/*********************为与地质大数据中心有更新的属性相匹配的数据库模型进行自动更新************************/
//	for (size_t idxProp = 0; idxProp < vecStrPropNameUpdate.size(); idxProp++)
//	{
//		strMsg = server.EsGetTime() + "\t正在更新第" + to_string(idxProp + 1) + "个属性：" + vecStrPropNameUpdate[idxProp] + ",剩余" + to_string(vecStrPropNameUpdate.size() - 1 - idxProp) + "个属性待更新";
//		cout << strMsg << "\n";
//		server.EsWriteToLog(strLogFile, strMsg);
//
//		/*********************不同LOD自动更新************************/
//		for (size_t idxLod = 0; idxLod < vecLodSet[idxProp].size(); idxLod++)
//		{
//			strMsg = server.EsGetTime() + "\t\t正在更新第" + to_string(idxLod + 1) + "个LOD：Lod=" + to_string(vecLodSet[idxProp][idxLod]) + ",剩余" + to_string(vecLodSet[idxProp].size() - 1 - idxLod) + "个LOD待更新";
//			cout << strMsg << "\n";
//			server.EsWriteToLog(strLogFile, strMsg);
//
//			/*********************不同属性类型自动更新************************/
//			if (vecAttrType[idxProp][idxLod] == Continuous)
//			{
//				/*********************不同地层自动更新************************/
//				for (size_t idxLayer = 0; idxLayer < vecLayerIDSet[idxProp].size(); idxLayer++)
//				{				
//					strMsg = server.EsGetTime() + "\t\t\t正在更新第" + std::to_string(idxLayer + 1) + "个地层：" + std::to_string(vecLayerIDSet[idxProp][idxLayer]) + ",剩余" + std::to_string(vecLayerIDSet[idxProp].size() - 1 - idxLayer) + "个地层待更新";
//					cout << strMsg << "\n";
//					server.EsWriteToLog(strLogFile, strMsg);
//
//					//如果地层没有更新的样本，则跳过
//					if (vecUpdateDataSet[idxProp][idxLod][idxLayer].empty())
//					{					
//						strMsg = server.EsGetTime() + "\t\t\t地层：" + std::to_string(vecLayerIDSet[idxProp][idxLayer]) + "没有更新的钻孔，已跳过";
//						cout << strMsg << "\n";
//						server.EsWriteToLog(strLogFile, strMsg);
//
//						continue;
//					}
//
//					//将更新的网格样本上传到数据库
//					strMsg = server.EsGetTime() + "\t\t\t正在将更新的钻孔上传到数据库...";
//					cout << strMsg << "\n";
//					server.EsWriteToLog(strLogFile, strMsg);
//
//					if (!server.EsUpLoadData(vecStrPropNameUpdate[idxProp], vecAttrType[idxProp][idxLod], vecUpdateDataSet[idxProp][idxLod][idxLayer], strRootURL, vecStrShapeID[idxProp][idxLod], vecStrCoverageID[idxProp][idxLod], vecStrFeaClassID[idxProp][idxLod], vecLodSet[idxProp][idxLod]))
//					{
//						strMsg = server.EsGetTime() + "\t\t\t" + server.EsGetMsg() + " 错误：地层：" + std::to_string(vecLayerIDSet[idxProp][idxLayer]) + "更新的钻孔无法更新到数据库，本地层的更新将被跳过";
//						cout << strMsg << "\n";
//						server.EsWriteToLog(strLogFile, strMsg);
//
//						continue;
//					}
//
//					//获取目标区域的变差函数，并转化为评估参数
//					const string strLayerID = to_string(vecLayerIDSet[idxProp][idxLayer]);
//					VariogramPara varPara;
//					if (!server.EsReadServerConfigVariogram(strConfigPath, vecStrPropNameUpdate[idxProp], vecLodSet[idxProp][idxLod], strLayerID, varPara))
//					{
//						strMsg = server.EsGetTime() + "\t\t\t警告：地层：" + std::to_string(vecLayerIDSet[idxProp][idxLayer]) + "的变差函数获取失败，将使用默认变差进行计算";
//						cout << strMsg << "\n";
//						server.EsWriteToLog(strLogFile, strMsg);
//					}
//
//					int rangeI = ceil(varPara.dMajorRange / vecGridInfo[idxProp][idxLod].dXStep); //评估参数：I方向搜索步长
//					int rangeJ = ceil(varPara.dMinorRange / vecGridInfo[idxProp][idxLod].dYStep); //评估参数：J方向搜索步长
//					int rangeK = ceil(varPara.dVerticalRange / vecGridInfo[idxProp][idxLod].dZStep); //评估参数：K方向搜索步长
//
//					//使用评估参数对更新的数据进行影响范围评估
//					strMsg = server.EsGetTime() + "\t\t\t更新的钻孔上传到数据库成功，正在评估更新的钻孔的影响范围...";
//					cout << strMsg << "\n";
//					server.EsWriteToLog(strLogFile, strMsg);
//
//					vector<GridInfo> vecGIforSampleDownload; //样本下载范围集
//					vector<GridInfo> vecGIforUpdate; //待插范围集				
//					if (!server.EsEvaluate(vecUpdateDataSet[idxProp][idxLod][idxLayer], rangeI, rangeJ, rangeK, vecGridInfo[idxProp][idxLod], vecGIforSampleDownload, vecGIforUpdate))
//					{
//						strMsg = server.EsGetTime() + "\t\t\t错误：地层：" + std::to_string(vecLayerIDSet[idxProp][idxLayer]) + "的更新范围评估失败，本地层的更新将被跳过";
//						cout << strMsg << "\n";
//						server.EsWriteToLog(strLogFile, strMsg);
//
//						continue;
//					}
//
//					if (vecGIforSampleDownload.size() != vecGIforUpdate.size())
//					{
//						strMsg = server.EsGetTime() + "\t\t\t错误：地层：" + std::to_string(vecLayerIDSet[idxProp][idxLayer]) + "的更新范围评估失败，本地层的更新将被跳过";
//						cout << strMsg << "\n";
//						server.EsWriteToLog(strLogFile, strMsg);
//
//						continue;
//					}
//
//					strMsg = server.EsGetTime() + "\t\t\t地层：" + std::to_string(vecLayerIDSet[idxProp][idxLayer]) + "的更新范围评估成功，共有" + std::to_string(vecGIforUpdate.size()) + "个待更新范围";
//					cout << strMsg << "\n";
//					server.EsWriteToLog(strLogFile, strMsg);
//
//					//写出受该属性影响的其他属性的原始钻孔
//					server.EsExportOriginDataOfOtherPropsEffectedByCurProp(strConfigPath, strTargetDir, vecStrPropNameUpdate[idxProp], strLayerID, vecLodSet[idxProp][idxLod], vecGridInfo[idxProp][idxLod], vecGIforUpdate);
//
//					//从配置文件获取该区域插值使用的方法
//					InterMethod nInterMethod;
//					if (!server.EsReadServerConfigLayer(strConfigPath, vecStrPropNameUpdate[idxProp], vecLodSet[idxProp][idxLod], strLayerID, nInterMethod))
//					{
//						strMsg = server.EsGetTime() + "\t\t\t错误：从配置文件中获取地层：" + std::to_string(vecLayerIDSet[idxProp][idxLayer]) + "的插值算法失败，本地层的更新将被跳过";
//						cout << strMsg << "\n";
//						server.EsWriteToLog(strLogFile, strMsg);
//
//						continue;
//					}
//
//					/*********************对每个更新区域进行更新************************/
//					for (size_t idxArea = 0; idxArea < vecGIforUpdate.size(); idxArea++)
//					{
//						strMsg = server.EsGetTime() + "\t\t\t\t正在执行第" + std::to_string(idxArea + 1) + "个更新范围的插值计算,插值范围：IOri=" + std::to_string(vecGIforUpdate[idxArea].nIOri) + " JOri=" + std::to_string(vecGIforUpdate[idxArea].nJOri) + " KOri=" + std::to_string(vecGIforUpdate[idxArea].nKOri) +
//							" XNum=" + std::to_string(vecGIforUpdate[idxArea].nXNum) + " YNum=" + std::to_string(vecGIforUpdate[idxArea].nYNum) + " ZNum=" + std::to_string(vecGIforUpdate[idxArea].nZNum) + "，剩余" + std::to_string(vecGIforUpdate.size() - 1 - idxArea) + "个更新范围待插值";
//						cout << strMsg << "\n";
//						server.EsWriteToLog(strLogFile, strMsg);
//
//						//准备插值参数
//						ModelInterPara tmip; //插值参数
//						tmip.strWorkPath = strTargetDir; //工作目录
//						tmip.nAttrType = Continuous; //属性为连续
//						tmip.gi = vecGIforUpdate[idxArea]; //待插网格			
//						tmip.nInterMethod = nInterMethod; //插值算法
//						tmip.varPara = varPara; //变差函数
//
//						bool bStochastic = false;
//						if (nInterMethod == SequentialGaussianSimulation)
//						{
//							bStochastic = true;
//						}
//
//						//从数据库获取目标范围内的样本	
//						strMsg = server.EsGetTime() + "\t\t\t\t正在下载插值用的样本，请等待...";
//						cout << strMsg << "\n";
//						server.EsWriteToLog(strLogFile, strMsg);
//
//						if (!server.EsDownLoadData(strTargetDir, vecStrPropNameUpdate[idxProp], vecAttrType[idxProp][idxLod], strMatchedFieldName, strLayerID, vecGIforSampleDownload[idxArea], vecGIforUpdate[idxArea],
//							bStochastic, strRootURL, vecLodSet[idxProp][idxLod], vecStrFeaClassID[idxProp][idxLod], vecStrCoverageID[idxProp][idxLod], vecCellAfterUpdate[idxProp][idxLod], tmip.vecCell))
//						{
//							strMsg = server.EsGetTime() + "\t\t\t\t" + server.EsGetMsg() + " 错误：下载插值样本失败，本更新范围的更新将被跳过";
//							cout << strMsg << "\n";
//							server.EsWriteToLog(strLogFile, strMsg);
//
//							continue;
//						}
//
//						//更新数据-插值计算
//						strMsg = server.EsGetTime() + "\t\t\t\t插值样本下载成功，正在进行插值计算...";
//						cout << strMsg << "\n";
//						server.EsWriteToLog(strLogFile, strMsg);
//
//						if (!server.EsUpdateData(tmip))
//						{
//							strMsg = server.EsGetTime() + "\t\t\t\t错误：插值计算失败，本更新范围的更新将被跳过";
//							cout << strMsg << "\n";
//							server.EsWriteToLog(strLogFile, strMsg);
//
//							continue;
//						}
//
//						//将更新后的网格属性写回数据库
//						strMsg = server.EsGetTime() + "\t\t\t\t插值计算成功，正在更新插值结果到数据库...";
//						cout << strMsg << "\n";
//						server.EsWriteToLog(strLogFile, strMsg);
//
//						if (!server.EsUpLoadData(vecStrPropNameUpdate[idxProp], vecAttrType[idxProp][idxLod], strMatchedFieldName, strLayerID, tmip, strRootURL, vecStrShapeID[idxProp][idxLod], vecStrCoverageID[idxProp][idxLod], vecStrFeaClassID[idxProp][idxLod], vecLodSet[idxProp][idxLod]))
//						{
//							strMsg = server.EsGetTime() + "\t\t\t\t" + server.EsGetMsg() + " 错误：插值结果更新到数据库失败，本更新范围的更新将被跳过";
//							cout << strMsg << "\n";
//							server.EsWriteToLog(strLogFile, strMsg);
//
//							continue;
//						}
//
//						strMsg = server.EsGetTime() + "\t\t\t\t插值结果更新到数据库成功";
//						cout << strMsg << "\n";
//						server.EsWriteToLog(strLogFile, strMsg);
//					}
//				}
//			}
//			else if (vecAttrType[idxProp][idxLod] == Discrete)
//			{
//				/*********************不同地层自动更新************************/
//				for (size_t idxLayer = 0; idxLayer < vecLayerIDSet[idxProp].size(); idxLayer++)
//				{
//					strMsg = server.EsGetTime() + "\t\t\t正在更新第" + std::to_string(idxLayer + 1) + "个地层：" + std::to_string(vecLayerIDSet[idxProp][idxLayer]) + ",剩余" + std::to_string(vecLayerIDSet[idxProp].size() - 1 - idxLayer) + "个地层待更新";
//					cout << strMsg << "\n";
//					server.EsWriteToLog(strLogFile, strMsg);
//
//					//如果地层没有更新的样本，则跳过
//					if (vecUpdateDataSet[idxProp][idxLod][idxLayer].empty())
//					{
//						strMsg = server.EsGetTime() + "\t\t\t地层：" + std::to_string(vecLayerIDSet[idxProp][idxLayer]) + "没有更新的钻孔，已跳过";
//						cout << strMsg << "\n";
//						server.EsWriteToLog(strLogFile, strMsg);
//
//						continue;
//					}
//
//					//将更新的网格样本上传到数据库
//					strMsg = server.EsGetTime() + "\t\t\t正在将更新的钻孔上传到数据库...";
//					cout << strMsg << "\n";
//					server.EsWriteToLog(strLogFile, strMsg);
//
//					if (!server.EsUpLoadData(vecStrPropNameUpdate[idxProp], vecAttrType[idxProp][idxLod], vecUpdateDataSet[idxProp][idxLod][idxLayer], strRootURL, vecStrShapeID[idxProp][idxLod], vecStrCoverageID[idxProp][idxLod], vecStrFeaClassID[idxProp][idxLod], vecLodSet[idxProp][idxLod]))
//					{
//						strMsg = server.EsGetTime() + "\t\t\t" + server.EsGetMsg() + " 错误：地层：" + std::to_string(vecLayerIDSet[idxProp][idxLayer]) + "更新的钻孔无法更新到数据库，本地层的更新将被跳过";
//						cout << strMsg << "\n";
//						server.EsWriteToLog(strLogFile, strMsg);
//					
//						continue;
//					}
//
//					//遍历各相的变差函数，获取最大值为评估参数
//					int rangeI = INT_MIN; //评估参数：I方向搜索步长
//					int rangeJ = INT_MIN; //评估参数：J方向搜索步长
//					int rangeK = INT_MIN; //评估参数：K方向搜索步长
//
//					const string strLayerID = to_string(vecLayerIDSet[idxProp][idxLayer]);
//					vector<int> vecFaceCode;
//					if (!server.EsReadServerConfigFaceCode(strConfigPath, vecStrPropNameUpdate[idxProp], vecLodSet[idxProp][idxLod], strLayerID, vecFaceCode))
//					{
//						strMsg = server.EsGetTime() + "\t\t\t错误：从配置文件中获取属性：" + vecStrPropNameUpdate[idxProp] + "的离散相失败，本地层的更新将被跳过\n";
//						cout << strMsg << "\n";
//						server.EsWriteToLog(strLogFile, strMsg);
//
//						continue;
//					}
//
//					if (vecFaceCode.empty())
//					{
//						strMsg = server.EsGetTime() + "\t\t\t警告：属性：" + vecStrPropNameUpdate[idxProp] + "的离散相数量为0，本地层的更新将被跳过";
//						cout << strMsg << "\n";
//						server.EsWriteToLog(strLogFile, strMsg);
//
//						continue;
//					}
//
//					for (size_t idxCode = 0; idxCode < vecFaceCode.size(); idxCode++)
//					{
//						VariogramPara varPara;
//						string strFaceCode = to_string(vecFaceCode[idxCode]);
//						if (!server.EsReadServerConfigVariogramByFaceCode(strConfigPath, vecStrPropNameUpdate[idxProp], vecLodSet[idxProp][idxLod], strLayerID, strFaceCode, varPara))
//							continue;
//
//						int tempI = ceil(varPara.dMajorRange / vecGridInfo[idxProp][idxLod].dXStep);
//						int tempJ = ceil(varPara.dMinorRange / vecGridInfo[idxProp][idxLod].dYStep);
//						int tempK = ceil(varPara.dVerticalRange / vecGridInfo[idxProp][idxLod].dZStep);
//
//						if (rangeI <= tempI)
//							rangeI = tempI;
//						if (rangeJ <= tempJ)
//							rangeJ = tempJ;
//						if (rangeK <= tempK)
//							rangeK = tempK;
//					}
//
//					//对更新的数据进行影响范围评估
//					strMsg = server.EsGetTime() + "\t\t\t更新的钻孔上传到数据库成功，正在评估更新的钻孔的影响范围...";
//					cout << strMsg << "\n";
//					server.EsWriteToLog(strLogFile, strMsg);
//					vector<GridInfo> vecGIforSampleDownload; //样本下载范围集
//					vector<GridInfo> vecGIforUpdate; //待插范围集				
//					if (!server.EsEvaluate(vecUpdateDataSet[idxProp][idxLod][idxLayer], rangeI, rangeJ, rangeK, vecGridInfo[idxProp][idxLod], vecGIforSampleDownload, vecGIforUpdate))
//					{
//						strMsg = server.EsGetTime() + "\t\t\t错误：地层：" + std::to_string(vecLayerIDSet[idxProp][idxLayer]) + "的更新范围评估失败，本地层的更新将被跳过";
//						cout << strMsg << "\n";
//						server.EsWriteToLog(strLogFile, strMsg);
//
//						continue;
//					}
//
//					if (vecGIforSampleDownload.size() != vecGIforUpdate.size())
//					{
//						strMsg = server.EsGetTime() + "\t\t\t错误：地层：" + std::to_string(vecLayerIDSet[idxProp][idxLayer]) + "的更新范围评估失败，本地层的更新将被跳过";
//						cout << strMsg << "\n";
//						server.EsWriteToLog(strLogFile, strMsg);
//
//						continue;
//					}
//					strMsg = server.EsGetTime() + "\t\t\t地层：" + std::to_string(vecLayerIDSet[idxProp][idxLayer]) + "的更新范围评估成功，共有" + std::to_string(vecGIforUpdate.size()) + "个待更新范围";
//					cout << strMsg << "\n";
//					server.EsWriteToLog(strLogFile, strMsg);
//
//					//写出受该属性影响的其他属性的原始钻孔
//					server.EsExportOriginDataOfOtherPropsEffectedByCurProp(strConfigPath, strTargetDir, vecStrPropNameUpdate[idxProp], strLayerID, vecLodSet[idxProp][idxLod], vecGridInfo[idxProp][idxLod], vecGIforUpdate);
//
//					//从配置文件获取该区域插值使用的方法
//					InterMethod nInterMethod;
//					if (!server.EsReadServerConfigLayer(strConfigPath, vecStrPropNameUpdate[idxProp], vecLodSet[idxProp][idxLod], strLayerID, nInterMethod))
//					{
//						strMsg = server.EsGetTime() + "\t\t\t错误：从配置文件中获取地层：" + std::to_string(vecLayerIDSet[idxProp][idxLayer]) + "的插值算法失败，本地层的更新将被跳过";
//						cout << strMsg << "\n";
//						server.EsWriteToLog(strLogFile, strMsg);
//
//						continue;
//					}
//
//					/*********************对每个更新区域进行更新************************/
//					for (size_t idxArea = 0; idxArea < vecGIforUpdate.size(); idxArea++)
//					{
//						strMsg = server.EsGetTime() + "\t\t\t\t正在执行第" + std::to_string(idxArea + 1) + "个更新范围的插值计算,插值范围：IOri=" + std::to_string(vecGIforUpdate[idxArea].nIOri) + " JOri=" + std::to_string(vecGIforUpdate[idxArea].nJOri) + " KOri=" + std::to_string(vecGIforUpdate[idxArea].nKOri) +
//							" XNum=" + std::to_string(vecGIforUpdate[idxArea].nXNum) + " YNum=" + std::to_string(vecGIforUpdate[idxArea].nYNum) + " ZNum=" + std::to_string(vecGIforUpdate[idxArea].nZNum) + "，剩余" + std::to_string(vecGIforUpdate.size() - 1 - idxArea) + "个更新范围待插值";
//						cout << strMsg << "\n";
//						server.EsWriteToLog(strLogFile, strMsg);
//
//						//准备插值参数
//						ModelInterPara tmip; //插值参数
//						tmip.strWorkPath = strTargetDir; //工作目录
//						tmip.nAttrType = Discrete; //属性为连续
//						tmip.gi = vecGIforUpdate[idxArea]; //待插网格			
//						tmip.nInterMethod = nInterMethod; //插值算法
//
//						//设置离散属性变差函数
//						tmip.faciesPara.resize(vecFaceCode.size());
//						for (size_t idxCode = 0; idxCode < vecFaceCode.size(); idxCode++)
//						{
//							tmip.faciesPara[idxCode].nCode = vecFaceCode[idxCode];
//
//							VariogramPara varParaTemp;
//							string strCode = to_string(vecFaceCode[idxCode]);
//							server.EsReadServerConfigVariogramByFaceCode(strConfigPath, vecStrPropNameUpdate[idxProp], vecLodSet[idxProp][idxLod], strLayerID, strCode, varParaTemp);
//							tmip.faciesPara[idxCode].varPara = varParaTemp;
//						}
//
//						bool bStochastic = false;
//						if (nInterMethod == SequentialIndicatorSimulation)
//						{
//							bStochastic = true;
//						}
//
//						//从样本中选取目标范围内的样本
//						strMsg = server.EsGetTime() + "\t\t\t\t正在下载插值用的样本，请等待...";
//						cout << strMsg << "\n";
//						server.EsWriteToLog(strLogFile, strMsg);
//						if (!server.EsDownLoadData(strTargetDir, vecStrPropNameUpdate[idxProp], vecAttrType[idxProp][idxLod], strMatchedFieldName, strLayerID, vecGIforSampleDownload[idxArea], vecGIforUpdate[idxArea],
//							bStochastic, strRootURL, vecLodSet[idxProp][idxLod], vecStrFeaClassID[idxProp][idxLod], vecStrCoverageID[idxProp][idxLod], vecCellAfterUpdate[idxProp][idxLod], tmip.vecCell))
//						{
//							strMsg = server.EsGetTime() + "\t\t\t\t" + server.EsGetMsg() + " 错误：下载插值样本失败，本更新范围的更新将被跳过";
//							cout << strMsg << "\n";
//							server.EsWriteToLog(strLogFile, strMsg);
//
//							continue;
//						}
//
//						//更新数据-插值计算
//						strMsg = server.EsGetTime() + "\t\t\t\t插值样本下载成功，正在进行插值计算...";
//						cout << strMsg << "\n";
//						server.EsWriteToLog(strLogFile, strMsg);
//						if (!server.EsUpdateData(tmip))
//						{
//							strMsg = server.EsGetTime() + "\t\t\t\t错误：插值计算失败，本更新范围的更新将被跳过";
//							cout << strMsg << "\n";
//							server.EsWriteToLog(strLogFile, strMsg);
//
//							continue;
//						}
//
//						//将更新后的网格属性写回数据库
//						strMsg = server.EsGetTime() + "\t\t\t\t插值计算成功，正在更新插值结果到数据库...";
//						cout << strMsg << "\n";
//						server.EsWriteToLog(strLogFile, strMsg);
//						if (!server.EsUpLoadData(vecStrPropNameUpdate[idxProp], vecAttrType[idxProp][idxLod], strMatchedFieldName, strLayerID, tmip, strRootURL, vecStrShapeID[idxProp][idxLod], vecStrCoverageID[idxProp][idxLod], vecStrFeaClassID[idxProp][idxLod], vecLodSet[idxProp][idxLod]))
//						{
//							strMsg = server.EsGetTime() + "\t\t\t\t" + server.EsGetMsg() + " 错误：插值结果更新到数据库失败，本更新范围的更新将被跳过";
//							cout << strMsg << "\n";
//							server.EsWriteToLog(strLogFile, strMsg);
//
//							continue;
//						}
//						strMsg = server.EsGetTime() + "\t\t\t\t插值结果更新到数据库成功";
//						cout << strMsg << "\n";
//						server.EsWriteToLog(strLogFile, strMsg);
//					}
//				}
//			}
//		}
//	}
//
//	strMsg = server.EsGetTime() + "*********************自动更新结束，服务关闭************************\n";
//	cout << strMsg << "\n";
//	server.EsWriteToLog(strLogFile, strMsg);
//}