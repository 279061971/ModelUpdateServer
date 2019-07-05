#include <iostream>
#include <fstream>
#include "windows.h"
#include "EsModelUpdateServerGS_TwoMFN.h"
using namespace gridworld;
using namespace gridworld::GS_TwoMFN;
using namespace std;

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		cout << "错误：没有输入参数\n";
		return 1;
	}
	const string strTargetDir = argv[1];

	CModelUpdateServerGS_TwoMFN server; //构建更新服务
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
//	string strMajorMatchedFieldName = ""; //用于筛选网格的主属性列列名称
//	string strMinorMatchedFieldName = ""; //用于筛选网格的次属性列列名称
//	const string strConfigPath = strTargetDir + "\\" + strMatchedDataCenter_Config; //自动更新服务配置文件
//	const string strLogFile = strTargetDir + "\\log.txt"; //日志文件
//
//	CModelUpdateServerGS_TwoMFN server; //构建更新服务
//	if (!server.EsReadServerConfigSetting(strConfigPath, strRootURL, strMajorMatchedFieldName, strMinorMatchedFieldName))
//	{
//		strMsg = server.EsGetTime() + " " + server.EsGetErrorMsg();
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
//		strMsg = server.EsGetTime() + server.EsGetErrorMsg();
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
//	vector<vector<AttrType>> vecAttrType;                                 //更新的属性类型集         属性/Lod
//	vector<vector<GridInfo>> vecGridInfo;                                 //属性对应的网格信息集     属性/Lod
//	vector<vector<string>> vecStrCoverageID;                              //属性对应的属性场ID集     属性/Lod
//	vector<vector<string>> vecStrShapeID;                                 //属性对应的几何场ID集     属性/Lod
//	vector<vector<string>> vecStrFeatureID;                               //属性对应的要素ID集       属性/Lod
//	vector<vector<string>> vecStrFeaClassID;                              //属性对应的要素类ID集     属性/Lod
//	vector<vector<int>> vecLodSet;                                        //属性对应的地层块ID集     属性/Lod
//	vector<vector<int>> vecMajorIDSet;                                    //属性对应的MajorID集      属性/MajorId
//	vector<vector<vector<int>>> vecMinorIDSet;                            //属性对应的MinorID集      属性/Major/MinorId	
//	vector<vector<vector<vector<vector<Cell>>>>> vecUpdateDataSet;        //更新的网格样本           属性/Lod/Major/Minor
//	vector<vector<vector<Cell>>> vecCellBeforeUpdate;                     //更新前的网格样本         属性/Lod
//	vector<vector<vector<Cell>>> vecCellAfterUpdate;                      //更新后的网格样本         属性/Lod
//	if (!server.EsGetUpdatedSampleData(strTargetDir, strConfigPath, strRootURL, strMajorMatchedFieldName, strMinorMatchedFieldName, vecStrPropNameUpdate, vecAttrType, vecUpdateDataSet, vecCellAfterUpdate, vecStrCoverageID, vecStrShapeID, vecStrFeatureID, vecStrFeaClassID, vecLodSet, vecMajorIDSet, vecMinorIDSet, vecGridInfo))
//	{
//		strMsg = server.EsGetTime() + server.EsGetErrorMsg();
//		cout << strMsg << "\n";
//		server.EsWriteToLog(strLogFile, strMsg);
//		return;
//	}
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
//				/*********************不同主筛选属性控制更新************************/
//				for (size_t idxMajor = 0; idxMajor < vecMajorIDSet[idxProp].size(); idxMajor++)
//				{
//					strMsg = server.EsGetTime() + "\t\t\t正在更新主筛选属性" + strMajorMatchedFieldName + "的第" + std::to_string(idxMajor + 1) + "个控制范围：" + std::to_string(vecMajorIDSet[idxProp][idxMajor]) + ",剩余" + std::to_string(vecMajorIDSet[idxProp].size() - 1 - idxMajor) + "个主控制范围待更新";
//					cout << strMsg << "\n";
//					server.EsWriteToLog(strLogFile, strMsg);
//
//					/*********************不同次筛选属性控制更新************************/
//					for (size_t idxMinor = 0; idxMinor < vecMinorIDSet[idxProp][idxMajor].size(); idxMinor++)
//					{
//						strMsg = server.EsGetTime() + "\t\t\t\t正在更新次筛选属性" + strMinorMatchedFieldName + "的第" + std::to_string(idxMinor + 1) + "个控制范围：" + std::to_string(vecMinorIDSet[idxProp][idxMajor][idxMinor]) + ",剩余" + std::to_string(vecMinorIDSet[idxProp][idxMajor].size() - 1 - idxMinor) + "个次控制范围待更新";
//						cout << strMsg << "\n";
//						server.EsWriteToLog(strLogFile, strMsg);
//
//						//如果次筛选属性控制范围内没有更新的样本，则跳过
//						if (vecUpdateDataSet[idxProp][idxLod][idxMajor][idxMinor].empty())
//						{
//							strMsg = server.EsGetTime() + "\t\t\t\t次筛选属性" + strMinorMatchedFieldName + "的控制范围：" + std::to_string(vecMinorIDSet[idxProp][idxMajor][idxMinor]) + "没有更新的钻孔，已跳过";
//							cout << strMsg << "\n";
//							server.EsWriteToLog(strLogFile, strMsg);
//
//							continue;
//						}
//
//						//将更新的网格样本上传到数据库
//						strMsg = server.EsGetTime() + "\t\t\t\t正在将更新的钻孔上传到数据库...";
//						cout << strMsg << "\n";
//						server.EsWriteToLog(strLogFile, strMsg);
//
//						if (!server.EsUpLoadData(vecStrPropNameUpdate[idxProp], vecAttrType[idxProp][idxLod], vecUpdateDataSet[idxProp][idxLod][idxMajor][idxMinor], strRootURL, vecStrShapeID[idxProp][idxLod], vecStrCoverageID[idxProp][idxLod], vecStrFeaClassID[idxProp][idxLod], vecLodSet[idxProp][idxLod]))
//						{
//							strMsg = server.EsGetTime() + "\t\t\t\t" + server.EsGetErrorMsg() + " 错误：次筛选属性" + strMinorMatchedFieldName + "的控制范围：" + std::to_string(vecMinorIDSet[idxProp][idxMajor][idxMinor]) + "更新的钻孔无法更新到数据库，本范围的更新将被跳过";
//							cout << strMsg << "\n";
//							server.EsWriteToLog(strLogFile, strMsg);
//
//							continue;
//						}
//
//						//获取目标区域的变差函数，并转化为评估参数
//						const string strMajorID = to_string(vecMajorIDSet[idxProp][idxMajor]);
//						const string strMinorID = to_string(vecMinorIDSet[idxProp][idxMajor][idxMinor]);
//						VariogramPara varPara;
//						if (!server.EsReadServerConfigVariogram(strConfigPath, vecStrPropNameUpdate[idxProp], vecLodSet[idxProp][idxLod], strMajorID, strMinorID, varPara))
//						{
//							strMsg = server.EsGetTime() + "\t\t\t\t警告：次筛选属性" + strMinorMatchedFieldName + "的控制范围：" + std::to_string(vecMinorIDSet[idxProp][idxMajor][idxMinor]) + "的变差函数获取失败，将使用默认变差进行计算";
//							cout << strMsg << "\n";
//							server.EsWriteToLog(strLogFile, strMsg);
//						}
//
//						int rangeI = ceil(varPara.dMajorRange / vecGridInfo[idxProp][idxLod].dXStep); //评估参数：I方向搜索步长
//						int rangeJ = ceil(varPara.dMinorRange / vecGridInfo[idxProp][idxLod].dYStep); //评估参数：J方向搜索步长
//						int rangeK = ceil(varPara.dVerticalRange / vecGridInfo[idxProp][idxLod].dZStep); //评估参数：K方向搜索步长
//
//						//使用评估参数对更新的数据进行影响范围评估
//						strMsg = server.EsGetTime() + "\t\t\t\t更新的钻孔上传到数据库成功，正在评估更新的钻孔的影响范围...";
//						cout << strMsg << "\n";
//						server.EsWriteToLog(strLogFile, strMsg);
//
//						vector<GridInfo> vecGIforSampleDownload; //样本下载范围集
//						vector<GridInfo> vecGIforUpdate; //待插范围集				
//						if (!server.EsEvaluate(vecUpdateDataSet[idxProp][idxLod][idxMajor][idxMinor], rangeI, rangeJ, rangeK, vecGridInfo[idxProp][idxLod], vecGIforSampleDownload, vecGIforUpdate))
//						{
//							strMsg = server.EsGetTime() + "\t\t\t\t错误：次筛选属性" + strMinorMatchedFieldName + "的控制范围：" + std::to_string(vecMinorIDSet[idxProp][idxMajor][idxMinor]) + "的更新范围评估失败，本范围的更新将被跳过";
//							cout << strMsg << "\n";
//							server.EsWriteToLog(strLogFile, strMsg);
//
//							continue;
//						}
//
//						if (vecGIforSampleDownload.size() != vecGIforUpdate.size())
//						{
//							strMsg = server.EsGetTime() + "\t\t\t\t错误：次筛选属性" + strMinorMatchedFieldName + "的控制范围：" + std::to_string(vecMinorIDSet[idxProp][idxMajor][idxMinor]) + "的更新范围评估失败，本范围的更新将被跳过";
//							cout << strMsg << "\n";
//							server.EsWriteToLog(strLogFile, strMsg);
//
//							continue;
//						}
//
//						strMsg = server.EsGetTime() + "\t\t\t\t次筛选属性" + strMinorMatchedFieldName + "的控制范围：" + std::to_string(vecMinorIDSet[idxProp][idxMajor][idxMinor]) + "的更新范围评估成功，共有" + std::to_string(vecGIforUpdate.size()) + "个待更新范围";
//						cout << strMsg << "\n";
//						server.EsWriteToLog(strLogFile, strMsg);
//
//						//从配置文件获取该区域插值使用的方法
//						InterMethod nInterMethod;
//						if (!server.EsReadServerConfigMinor(strConfigPath, vecStrPropNameUpdate[idxProp], vecLodSet[idxProp][idxLod], strMajorID, strMinorID, nInterMethod))
//						{
//							strMsg = server.EsGetTime() + "\t\t\t\t错误：从配置文件中获取次筛选属性" + strMinorMatchedFieldName + "的控制范围：" + std::to_string(vecMinorIDSet[idxProp][idxMajor][idxMinor]) + "的插值算法失败，本范围的更新将被跳过";
//							cout << strMsg << "\n";
//							server.EsWriteToLog(strLogFile, strMsg);
//
//							continue;
//						}
//
//						/*********************对每个更新区域进行更新************************/
//						for (size_t idxArea = 0; idxArea < vecGIforUpdate.size(); idxArea++)
//						{
//							strMsg = server.EsGetTime() + "\t\t\t\t\t正在执行第" + std::to_string(idxArea + 1) + "个更新范围的插值计算,插值范围：IOri=" + std::to_string(vecGIforUpdate[idxArea].nIOri) + " JOri=" + std::to_string(vecGIforUpdate[idxArea].nJOri) + " KOri=" + std::to_string(vecGIforUpdate[idxArea].nKOri) +
//								" XNum=" + std::to_string(vecGIforUpdate[idxArea].nXNum) + " YNum=" + std::to_string(vecGIforUpdate[idxArea].nYNum) + " ZNum=" + std::to_string(vecGIforUpdate[idxArea].nZNum) + "，剩余" + std::to_string(vecGIforUpdate.size() - 1 - idxArea) + "个更新范围待插值";
//							cout << strMsg << "\n";
//							server.EsWriteToLog(strLogFile, strMsg);
//
//							//准备插值参数
//							ModelInterPara tmip; //插值参数
//							tmip.strWorkPath = strTargetDir; //工作目录
//							tmip.nAttrType = Continuous; //属性为连续
//							tmip.gi = vecGIforUpdate[idxArea]; //待插网格			
//							tmip.nInterMethod = nInterMethod; //插值算法
//							tmip.varPara = varPara; //变差函数
//
//							bool bStochastic = false;
//							if (nInterMethod == SequentialGaussianSimulation)
//							{
//								bStochastic = true;
//							}
//
//							//从数据库获取目标范围内的样本	
//							strMsg = server.EsGetTime() + "\t\t\t\t\t正在下载插值用的样本，请等待...";
//							cout << strMsg << "\n";
//							server.EsWriteToLog(strLogFile, strMsg);
//
//							if (!server.EsDownLoadData(strTargetDir, vecStrPropNameUpdate[idxProp], vecAttrType[idxProp][idxLod], strMajorMatchedFieldName, strMinorMatchedFieldName, strMajorID, strMinorID, vecGIforSampleDownload[idxArea], 
//								vecGIforUpdate[idxArea], bStochastic, strRootURL, vecLodSet[idxProp][idxLod], vecStrFeaClassID[idxProp][idxLod], vecStrCoverageID[idxProp][idxLod], vecCellAfterUpdate[idxProp][idxLod], tmip.vecCell))
//							{
//								strMsg = server.EsGetTime() + "\t\t\t\t\t" + server.EsGetErrorMsg() + " 错误：下载插值样本失败，本更新范围的更新将被跳过";
//								cout << strMsg << "\n";
//								server.EsWriteToLog(strLogFile, strMsg);
//
//								continue;
//							}
//
//							//更新数据-插值计算
//							strMsg = server.EsGetTime() + "\t\t\t\t\t插值样本下载成功，正在进行插值计算...";
//							cout << strMsg << "\n";
//							server.EsWriteToLog(strLogFile, strMsg);
//
//							if (!server.EsUpdateData(tmip))
//							{
//								strMsg = server.EsGetTime() + "\t\t\t\t\t错误：插值计算失败，本更新范围的更新将被跳过";
//								cout << strMsg << "\n";
//								server.EsWriteToLog(strLogFile, strMsg);
//
//								continue;
//							}
//
//							//将更新后的网格属性写回数据库
//							strMsg = server.EsGetTime() + "\t\t\t\t\t插值计算成功，正在更新插值结果到数据库...";
//							cout << strMsg << "\n";
//							server.EsWriteToLog(strLogFile, strMsg);
//
//							if (!server.EsUpLoadData(vecStrPropNameUpdate[idxProp], vecAttrType[idxProp][idxLod], strMajorMatchedFieldName, strMinorMatchedFieldName, strMajorID, strMinorID, tmip, strRootURL, vecStrShapeID[idxProp][idxLod], vecStrCoverageID[idxProp][idxLod], vecStrFeaClassID[idxProp][idxLod], vecLodSet[idxProp][idxLod]))
//							{
//								strMsg = server.EsGetTime() + "\t\t\t\t\t" + server.EsGetErrorMsg() + " 错误：插值结果更新到数据库失败，本更新范围的更新将被跳过";
//								cout << strMsg << "\n";
//								server.EsWriteToLog(strLogFile, strMsg);
//
//								continue;
//							}
//
//							strMsg = server.EsGetTime() + "\t\t\t\t\t插值结果更新到数据库成功";
//							cout << strMsg << "\n";
//							server.EsWriteToLog(strLogFile, strMsg);
//						}
//					}					
//				}
//			}
//			else if (vecAttrType[idxProp][idxLod] == Discrete)
//			{
//				/*********************不同地层自动更新************************/
//				for (size_t idxMajor = 0; idxMajor < vecMajorIDSet[idxProp].size(); idxMajor++)
//				{
//					strMsg = server.EsGetTime() + "\t\t\t正在更新主筛选属性" + strMajorMatchedFieldName + "的第" + std::to_string(idxMajor + 1) + "个控制范围：" + std::to_string(vecMajorIDSet[idxProp][idxMajor]) + ",剩余" + std::to_string(vecMajorIDSet[idxProp].size() - 1 - idxMajor) + "个主控制范围待更新";
//					cout << strMsg << "\n";
//					server.EsWriteToLog(strLogFile, strMsg);
//
//					for (size_t idxMinor = 0; idxMinor < vecMinorIDSet[idxProp][idxMajor].size(); idxMinor++)
//					{
//						strMsg = server.EsGetTime() + "\t\t\t\t正在更新次筛选属性" + strMinorMatchedFieldName + "的第" + std::to_string(idxMinor + 1) + "个控制范围：" + std::to_string(vecMinorIDSet[idxProp][idxMajor][idxMinor]) + ",剩余" + std::to_string(vecMinorIDSet[idxProp][idxMajor].size() - 1 - idxMinor) + "个次控制范围待更新";
//						cout << strMsg << "\n";
//						server.EsWriteToLog(strLogFile, strMsg);
//
//						//如果地层没有更新的样本，则跳过
//						if (vecUpdateDataSet[idxProp][idxLod][idxMajor].empty())
//						{
//							strMsg = server.EsGetTime() + "\t\t\t\t次筛选属性" + strMinorMatchedFieldName + "的控制范围：" + std::to_string(vecMinorIDSet[idxProp][idxMajor][idxMinor]) + "没有更新的钻孔，已跳过";
//							cout << strMsg << "\n";
//							server.EsWriteToLog(strLogFile, strMsg);
//
//							continue;
//						}
//
//						//将更新的网格样本上传到数据库
//						strMsg = server.EsGetTime() + "\t\t\t\t正在将更新的钻孔上传到数据库...";
//						cout << strMsg << "\n";
//						server.EsWriteToLog(strLogFile, strMsg);
//
//						if (!server.EsUpLoadData(vecStrPropNameUpdate[idxProp], vecAttrType[idxProp][idxLod], vecUpdateDataSet[idxProp][idxLod][idxMajor][idxMinor], strRootURL, vecStrShapeID[idxProp][idxLod], vecStrCoverageID[idxProp][idxLod], vecStrFeaClassID[idxProp][idxLod], vecLodSet[idxProp][idxLod]))
//						{
//							strMsg = server.EsGetTime() + "\t\t\t\t" + server.EsGetErrorMsg() + "错误：次筛选属性" + strMinorMatchedFieldName + "的控制范围：" + std::to_string(vecMinorIDSet[idxProp][idxMajor][idxMinor]) + "更新的钻孔无法更新到数据库，本范围的更新将被跳过";
//							cout << strMsg << "\n";
//							server.EsWriteToLog(strLogFile, strMsg);
//
//							continue;
//						}
//
//						//遍历各相的变差函数，获取最大值为评估参数
//						int rangeI = INT_MIN; //评估参数：I方向搜索步长
//						int rangeJ = INT_MIN; //评估参数：J方向搜索步长
//						int rangeK = INT_MIN; //评估参数：K方向搜索步长
//
//						const string strMajorID = to_string(vecMajorIDSet[idxProp][idxMajor]);
//						const string strMinorID = to_string(vecMinorIDSet[idxProp][idxMajor][idxMinor]);
//						vector<int> vecFaceCode;
//						if (!server.EsReadServerConfigFaceCode(strConfigPath, vecStrPropNameUpdate[idxProp], vecLodSet[idxProp][idxLod], strMajorID, strMinorID, vecFaceCode))
//						{
//							strMsg = server.EsGetTime() + "\t\t\t\t错误：从配置文件中获取属性：" + vecStrPropNameUpdate[idxProp] + "的离散相失败，本范围的更新将被跳过\n";
//							cout << strMsg << "\n";
//							server.EsWriteToLog(strLogFile, strMsg);
//
//							continue;
//						}
//
//						if (vecFaceCode.empty())
//						{
//							strMsg = server.EsGetTime() + "\t\t\t\t警告：属性：" + vecStrPropNameUpdate[idxProp] + "的离散相数量为0，本范围的更新将被跳过";
//							cout << strMsg << "\n";
//							server.EsWriteToLog(strLogFile, strMsg);
//
//							continue;
//						}
//
//						for (size_t idxCode = 0; idxCode < vecFaceCode.size(); idxCode++)
//						{
//							VariogramPara varPara;
//							string strFaceCode = to_string(vecFaceCode[idxCode]);
//							if (!server.EsReadServerConfigVariogramByFaceCode(strConfigPath, vecStrPropNameUpdate[idxProp], vecLodSet[idxProp][idxLod], strMajorID, strMinorID, strFaceCode, varPara))
//								continue;
//
//							int tempI = ceil(varPara.dMajorRange / vecGridInfo[idxProp][idxLod].dXStep);
//							int tempJ = ceil(varPara.dMinorRange / vecGridInfo[idxProp][idxLod].dYStep);
//							int tempK = ceil(varPara.dVerticalRange / vecGridInfo[idxProp][idxLod].dZStep);
//
//							if (rangeI <= tempI)
//								rangeI = tempI;
//							if (rangeJ <= tempJ)
//								rangeJ = tempJ;
//							if (rangeK <= tempK)
//								rangeK = tempK;
//						}
//
//						//对更新的数据进行影响范围评估
//						strMsg = server.EsGetTime() + "\t\t\t\t更新的钻孔上传到数据库成功，正在评估更新的钻孔的影响范围...";
//						cout << strMsg << "\n";
//						server.EsWriteToLog(strLogFile, strMsg);
//						vector<GridInfo> vecGIforSampleDownload; //样本下载范围集
//						vector<GridInfo> vecGIforUpdate; //待插范围集				
//						if (!server.EsEvaluate(vecUpdateDataSet[idxProp][idxLod][idxMajor][idxMinor], rangeI, rangeJ, rangeK, vecGridInfo[idxProp][idxLod], vecGIforSampleDownload, vecGIforUpdate))
//						{
//							strMsg = server.EsGetTime() + "\t\t\t\t错误：次筛选属性" + strMinorMatchedFieldName + "的控制范围：" + std::to_string(vecMinorIDSet[idxProp][idxMajor][idxMinor]) + "的更新范围评估失败，本范围的更新将被跳过";
//							cout << strMsg << "\n";
//							server.EsWriteToLog(strLogFile, strMsg);
//
//							continue;
//						}
//
//						if (vecGIforSampleDownload.size() != vecGIforUpdate.size())
//						{
//							strMsg = server.EsGetTime() + "\t\t\t\t错误：次筛选属性" + strMinorMatchedFieldName + "的控制范围：" + std::to_string(vecMinorIDSet[idxProp][idxMajor][idxMinor]) + "的更新范围评估失败，本范围的更新将被跳过";
//							cout << strMsg << "\n";
//							server.EsWriteToLog(strLogFile, strMsg);
//
//							continue;
//						}
//						strMsg = server.EsGetTime() + "\t\t\t\t次筛选属性" + strMinorMatchedFieldName + "的控制范围：" + std::to_string(vecMinorIDSet[idxProp][idxMajor][idxMinor]) + "的更新范围评估成功，共有" + std::to_string(vecGIforUpdate.size()) + "个待更新范围";
//						cout << strMsg << "\n";
//						server.EsWriteToLog(strLogFile, strMsg);
//
//						//从配置文件获取该区域插值使用的方法
//						InterMethod nInterMethod;
//						if (!server.EsReadServerConfigMinor(strConfigPath, vecStrPropNameUpdate[idxProp], vecLodSet[idxProp][idxLod], strMajorID, strMinorID, nInterMethod))
//						{
//							strMsg = server.EsGetTime() + "\t\t\t\t错误：从配置文件中获取次筛选属性" + strMinorMatchedFieldName + "的控制范围：" + std::to_string(vecMinorIDSet[idxProp][idxMajor][idxMinor]) + "的插值算法失败，本范围的更新将被跳过";
//							cout << strMsg << "\n";
//							server.EsWriteToLog(strLogFile, strMsg);
//
//							continue;
//						}
//
//						/*********************对每个更新区域进行更新************************/
//						for (size_t idxArea = 0; idxArea < vecGIforUpdate.size(); idxArea++)
//						{
//							strMsg = server.EsGetTime() + "\t\t\t\t\t正在执行第" + std::to_string(idxArea + 1) + "个更新范围的插值计算,插值范围：IOri=" + std::to_string(vecGIforUpdate[idxArea].nIOri) + " JOri=" + std::to_string(vecGIforUpdate[idxArea].nJOri) + " KOri=" + std::to_string(vecGIforUpdate[idxArea].nKOri) +
//								" XNum=" + std::to_string(vecGIforUpdate[idxArea].nXNum) + " YNum=" + std::to_string(vecGIforUpdate[idxArea].nYNum) + " ZNum=" + std::to_string(vecGIforUpdate[idxArea].nZNum) + "，剩余" + std::to_string(vecGIforUpdate.size() - 1 - idxArea) + "个更新范围待插值";
//							cout << strMsg << "\n";
//							server.EsWriteToLog(strLogFile, strMsg);
//
//							//准备插值参数
//							ModelInterPara tmip; //插值参数
//							tmip.strWorkPath = strTargetDir; //工作目录
//							tmip.nAttrType = Discrete; //属性为连续
//							tmip.gi = vecGIforUpdate[idxArea]; //待插网格			
//							tmip.nInterMethod = nInterMethod; //插值算法
//
//															  //设置离散属性变差函数
//							tmip.faciesPara.resize(vecFaceCode.size());
//							for (size_t idxCode = 0; idxCode < vecFaceCode.size(); idxCode++)
//							{
//								tmip.faciesPara[idxCode].nCode = vecFaceCode[idxCode];
//
//								VariogramPara varParaTemp;
//								string strCode = to_string(vecFaceCode[idxCode]);
//								server.EsReadServerConfigVariogramByFaceCode(strConfigPath, vecStrPropNameUpdate[idxProp], vecLodSet[idxProp][idxLod], strMajorID, strMinorID, strCode, varParaTemp);
//								tmip.faciesPara[idxCode].varPara = varParaTemp;
//							}
//
//							bool bStochastic = false;
//							if (nInterMethod == SequentialIndicatorSimulation)
//							{
//								bStochastic = true;
//							}
//
//							//从样本中选取目标范围内的样本
//							strMsg = server.EsGetTime() + "\t\t\t\t\t正在下载插值用的样本，请等待...";
//							cout << strMsg << "\n";
//							server.EsWriteToLog(strLogFile, strMsg);
//							if (!server.EsDownLoadData(strTargetDir, vecStrPropNameUpdate[idxProp], vecAttrType[idxProp][idxLod], strMajorMatchedFieldName, strMinorMatchedFieldName, strMajorID, strMinorID, vecGIforSampleDownload[idxArea],
//								vecGIforUpdate[idxArea], bStochastic, strRootURL, vecLodSet[idxProp][idxLod], vecStrFeaClassID[idxProp][idxLod], vecStrCoverageID[idxProp][idxLod], vecCellAfterUpdate[idxProp][idxLod], tmip.vecCell))
//							{
//								strMsg = server.EsGetTime() + "\t\t\t\t\t" + server.EsGetErrorMsg() + " 错误：下载插值样本失败，本更新范围的更新将被跳过";
//								cout << strMsg << "\n";
//								server.EsWriteToLog(strLogFile, strMsg);
//
//								continue;
//							}
//
//							//更新数据-插值计算
//							strMsg = server.EsGetTime() + "\t\t\t\t\t插值样本下载成功，正在进行插值计算...";
//							cout << strMsg << "\n";
//							server.EsWriteToLog(strLogFile, strMsg);
//							if (!server.EsUpdateData(tmip))
//							{
//								strMsg = server.EsGetTime() + "\t\t\t\t\t错误：插值计算失败，本更新范围的更新将被跳过";
//								cout << strMsg << "\n";
//								server.EsWriteToLog(strLogFile, strMsg);
//
//								continue;
//							}
//
//							//将更新后的网格属性写回数据库
//							strMsg = server.EsGetTime() + "\t\t\t\t\t插值计算成功，正在更新插值结果到数据库...";
//							cout << strMsg << "\n";
//							server.EsWriteToLog(strLogFile, strMsg);
//							if (!server.EsUpLoadData(vecStrPropNameUpdate[idxProp], vecAttrType[idxProp][idxLod], strMajorMatchedFieldName, strMinorMatchedFieldName, strMajorID, strMinorID, tmip, strRootURL, vecStrShapeID[idxProp][idxLod], vecStrCoverageID[idxProp][idxLod], vecStrFeaClassID[idxProp][idxLod], vecLodSet[idxProp][idxLod]))
//							{
//								strMsg = server.EsGetTime() + "\t\t\t\t\t" + server.EsGetErrorMsg() + " 错误：插值结果更新到数据库失败，本更新范围的更新将被跳过";
//								cout << strMsg << "\n";
//								server.EsWriteToLog(strLogFile, strMsg);
//
//								continue;
//							}
//							strMsg = server.EsGetTime() + "\t\t\t\t\t插值结果更新到数据库成功";
//							cout << strMsg << "\n";
//							server.EsWriteToLog(strLogFile, strMsg);
//						}
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