#include <fstream>
#include <math.h>
#include <direct.h>
#include <io.h>
#include "EsModelUpdateServerGS_TwoMFN.h"
#include "g3dcpp.h"
#include "Grid.h"
#include "Field.h"
#include "Block.h"

using namespace g3dcpp;
using std::ifstream;
using std::ofstream;
using std::ios;
using std::pair;

gridworld::GS_TwoMFN::CModelUpdateServerGS_TwoMFN::CModelUpdateServerGS_TwoMFN()
{

}

gridworld::GS_TwoMFN::CModelUpdateServerGS_TwoMFN::~CModelUpdateServerGS_TwoMFN()
{

}

int gridworld::GS_TwoMFN::CModelUpdateServerGS_TwoMFN::EsStartServer(const string& strTargetDir)
{
	int nStatus = 0;
	string strMsg = "";
	char szMsg[1024];
	string strRootURL = ""; //ģ�����ݷ������ĸ�·��
	string strMajorMatchedFieldName = ""; //����ɸѡ�������������������
	string strMinorMatchedFieldName = ""; //����ɸѡ����Ĵ�������������
	const string strConfigPath = strTargetDir + "\\" + strMatchedDataCenter_Config; //�Զ����·��������ļ�
	const string strLogFile = strTargetDir + "\\log.txt"; //��־�ļ�

	//�������·���
	if (!EsReadServerConfigSetting(strConfigPath, strRootURL, strMajorMatchedFieldName, strMinorMatchedFieldName))
	{
		strMsg = EsGetErrorMsg();
		EsWriteToLog(strLogFile, strMsg);
		return 1;
	}

	/*********************��������************************/
	EsWriteToLog(strLogFile, strMsg_StartServer);

	API api;
	api.Init(strRootURL);

	/*********************�ӵ��ʴ��������ļ����������Ƿ��и���************************/
	EsWriteToLog(strLogFile, strMsg_CheckUpdate);

	vector<string> vecStrPropNameUpdate; //���µ���������
	if (!EsCheckIsUpdated(strTargetDir, vecStrPropNameUpdate))
	{
		EsWriteToLog(strLogFile, EsGetErrorMsg());
		return 1;
	}

	/*********************�ӵ��ʴ��������ĺ�ģ�����ݷ������Ļ�ȡ���µ���������Ϣ************************/
	strMsg = strMsg_CheckUpdate_first;
	for (auto propName : vecStrPropNameUpdate)
	{
		strMsg += propName + "\t";
	}
	strMsg += strMsg_CheckUpdate_second;
	EsWriteToLog(strLogFile, strMsg);

	vector<vector<AttrType>> vecAttrType;                                 //���µ��������ͼ�         ����/Lod
	vector<vector<GridInfo>> vecGridInfo;                                 //���Զ�Ӧ��������Ϣ��     ����/Lod
	vector<vector<string>> vecStrModelID;                                 //���Զ�Ӧ�����Գ�ID��     ����/Lod
	vector<vector<int>> vecLodSet;                                        //���Զ�Ӧ�ĵز��ID��     ����/Lod
	vector<vector<int>> vecMajorIDSet;                                    //���Զ�Ӧ��MajorID��      ����/MajorId
	vector<vector<vector<int>>> vecMinorIDSet;                            //���Զ�Ӧ��MinorID��      ����/Major/MinorId	
	vector<vector<vector<vector<vector<Cell>>>>> vecUpdateDataSet;        //���µ���������           ����/Lod/Major/Minor
	vector<vector<vector<Cell>>> vecCellBeforeUpdate;                     //����ǰ����������         ����/Lod
	vector<vector<vector<Cell>>> vecCellAfterUpdate;                      //���º����������         ����/Lod
	if (!EsGetUpdatedSampleData(strTargetDir, strConfigPath, strMajorMatchedFieldName, strMinorMatchedFieldName, vecStrPropNameUpdate, vecAttrType, vecUpdateDataSet, vecCellAfterUpdate, vecStrModelID, vecLodSet, vecMajorIDSet, vecMinorIDSet, vecGridInfo))
	{
		strMsg = EsGetErrorMsg();
		EsWriteToLog(strLogFile, strMsg);
		return 1;
	}
	EsWriteToLog(strLogFile, strMsg_UpdateBorehole_Success);

	/*********************Ϊ����ʴ����������и��µ�������ƥ������ݿ�ģ�ͽ����Զ�����************************/
	for (size_t idxProp = 0; idxProp < vecStrPropNameUpdate.size(); idxProp++)
	{
		sprintf_s(szMsg, strMsg_UpdateProp, idxProp + 1, vecStrPropNameUpdate[idxProp].c_str(), vecStrPropNameUpdate.size() - 1 - idxProp);
		EsWriteToLog(strLogFile, szMsg);

		/*********************��ͬLOD�Զ�����************************/
		for (size_t idxLod = 0; idxLod < vecLodSet[idxProp].size(); idxLod++)
		{
			sprintf_s(szMsg, strMsg_UpdateLod, idxLod + 1, vecLodSet[idxProp][idxLod], vecLodSet[idxProp].size() - 1 - idxLod);
			EsWriteToLog(strLogFile, szMsg);

			/*********************��ͬ���������Զ�����************************/
			if (vecAttrType[idxProp][idxLod] == Continuous)
			{
				/*********************��ͬ��ɸѡ���Կ��Ƹ���************************/
				for (size_t idxMajor = 0; idxMajor < vecMajorIDSet[idxProp].size(); idxMajor++)
				{
					sprintf_s(szMsg, strMsg_UpdateMajor, strMajorMatchedFieldName, idxMajor + 1, vecMajorIDSet[idxProp][idxMajor], vecMajorIDSet[idxProp].size() - 1 - idxMajor);
					EsWriteToLog(strLogFile, szMsg);

					/*********************��ͬ��ɸѡ���Կ��Ƹ���************************/
					for (size_t idxMinor = 0; idxMinor < vecMinorIDSet[idxProp][idxMajor].size(); idxMinor++)
					{
						sprintf_s(szMsg, strMsg_UpdateMinor, strMinorMatchedFieldName, idxMinor + 1, vecMinorIDSet[idxProp][idxMajor][idxMinor], vecMinorIDSet[idxProp][idxMajor].size() - 1 - idxMinor);
						EsWriteToLog(strLogFile, szMsg);

						//�����ɸѡ���Կ��Ʒ�Χ��û�и��µ�������������
						if (vecUpdateDataSet[idxProp][idxLod][idxMajor][idxMinor].empty())
						{
							sprintf_s(szMsg, strMsg_UpdateMinor_NoUpdate, strMinorMatchedFieldName, vecMinorIDSet[idxProp][idxMajor][idxMinor]);
							EsWriteToLog(strLogFile, szMsg);

							continue;
						}

						//�����µ����������ϴ������ݿ�
						EsWriteToLog(strLogFile, strMsg_UpLoadingCell);

						if (!EsUpLoadData(vecStrPropNameUpdate[idxProp], vecAttrType[idxProp][idxLod], vecUpdateDataSet[idxProp][idxLod][idxMajor][idxMinor], vecStrModelID[idxProp][idxLod], vecLodSet[idxProp][idxLod]))
						{
							sprintf_s(szMsg, strMsg_UpLoad_Failed, EsGetErrorMsg().c_str(), strMinorMatchedFieldName.c_str(), vecMinorIDSet[idxProp][idxMajor][idxMinor]);
							EsWriteToLog(strLogFile, szMsg);
							nStatus = 1;
							continue;
						}

						//��ȡĿ������ı�������ת��Ϊ��������
						const string strMajorID = std::to_string(vecMajorIDSet[idxProp][idxMajor]);
						const string strMinorID = std::to_string(vecMinorIDSet[idxProp][idxMajor][idxMinor]);
						VariogramPara varPara;
						if (!EsReadServerConfigVariogram(strConfigPath, vecStrPropNameUpdate[idxProp], vecLodSet[idxProp][idxLod], strMajorID, strMinorID, varPara))
						{
							sprintf_s(szMsg, strMsg_GetVariogram_Failed, strMinorMatchedFieldName.c_str(), vecMinorIDSet[idxProp][idxMajor][idxMinor]);
							EsWriteToLog(strLogFile, szMsg);
							nStatus = 1;
						}

						int rangeI = ceil(varPara.dMajorRange / vecGridInfo[idxProp][idxLod].dXStep); //����������I������������
						int rangeJ = ceil(varPara.dMinorRange / vecGridInfo[idxProp][idxLod].dYStep); //����������J������������
						int rangeK = ceil(varPara.dVerticalRange / vecGridInfo[idxProp][idxLod].dZStep); //����������K������������

						//ʹ�����������Ը��µ����ݽ���Ӱ�췶Χ����
						EsWriteToLog(strLogFile, strMsg_UpLoadCellSuccess);

						vector<GridInfo> vecGIforSampleDownload; //�������ط�Χ��
						vector<GridInfo> vecGIforUpdate; //���巶Χ��				
						if (!EsEvaluate(vecUpdateDataSet[idxProp][idxLod][idxMajor][idxMinor], rangeI, rangeJ, rangeK, vecGridInfo[idxProp][idxLod], vecGIforSampleDownload, vecGIforUpdate))
						{
							sprintf_s(szMsg, strMsg_EvaluateArea_Failed, strMinorMatchedFieldName.c_str(), vecMinorIDSet[idxProp][idxMajor][idxMinor]);
							EsWriteToLog(strLogFile, szMsg);
							nStatus = 1;
							continue;
						}

						if (vecGIforSampleDownload.size() != vecGIforUpdate.size())
						{
							sprintf_s(szMsg, strMsg_EvaluateArea_Failed, strMinorMatchedFieldName.c_str(), vecMinorIDSet[idxProp][idxMajor][idxMinor]);
							EsWriteToLog(strLogFile, szMsg);
							nStatus = 1;
							continue;
						}

						sprintf_s(szMsg, strMsg_EvaluateArea_Success, strMinorMatchedFieldName.c_str(), vecMinorIDSet[idxProp][idxMajor][idxMinor], vecGIforUpdate.size());
						EsWriteToLog(strLogFile, szMsg);

						//�������ļ���ȡ�������ֵʹ�õķ���
						InterMethod nInterMethod;
						if (!EsReadServerConfigMinor(strConfigPath, vecStrPropNameUpdate[idxProp], vecLodSet[idxProp][idxLod], strMajorID, strMinorID, nInterMethod))
						{
							sprintf_s(szMsg, strMsg_GetInterMethod_Failed, strMinorMatchedFieldName.c_str(), vecMinorIDSet[idxProp][idxMajor][idxMinor]);
							EsWriteToLog(strLogFile, szMsg);
							nStatus = 1;
							continue;
						}

						/*********************��ÿ������������и���************************/
						for (size_t idxArea = 0; idxArea < vecGIforUpdate.size(); idxArea++)
						{
							sprintf_s(szMsg, strMsg_UpdateArea, idxArea + 1, vecGIforUpdate[idxArea].nIOri, vecGIforUpdate[idxArea].nJOri, vecGIforUpdate[idxArea].nKOri, vecGIforUpdate[idxArea].nXNum,
								vecGIforUpdate[idxArea].nYNum, vecGIforUpdate[idxArea].nZNum, vecGIforUpdate.size() - 1 - idxArea);
							EsWriteToLog(strLogFile, szMsg);

							//׼����ֵ����
							ModelInterPara tmip; //��ֵ����
							tmip.strWorkPath = strTargetDir; //����Ŀ¼
							tmip.nAttrType = Continuous; //����Ϊ����
							tmip.gi = vecGIforUpdate[idxArea]; //��������			
							tmip.nInterMethod = nInterMethod; //��ֵ�㷨
							tmip.varPara = varPara; //����

							bool bStochastic = false;
							if (nInterMethod == SequentialGaussianSimulation)
							{
								bStochastic = true;
							}

							//�����ݿ��ȡĿ�귶Χ�ڵ�����	
							EsWriteToLog(strLogFile, strMsg_DownloadCell);

							if (!EsDownLoadData(strTargetDir, vecStrPropNameUpdate[idxProp], vecAttrType[idxProp][idxLod], strMajorMatchedFieldName, strMinorMatchedFieldName, strMajorID, strMinorID, vecGIforSampleDownload[idxArea],
								vecGIforUpdate[idxArea], bStochastic, vecStrModelID[idxProp][idxLod], vecLodSet[idxProp][idxLod], vecCellAfterUpdate[idxProp][idxLod], tmip.vecCell))
							{
								sprintf_s(szMsg, strMsg_DownloadingCell_Failed, EsGetErrorMsg().c_str());
								EsWriteToLog(strLogFile, szMsg);
								nStatus = 1;
								continue;
							}

							//���������˹����ģ�͵����������ڲ�ֵ����
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

							//��������-��ֵ����
							EsWriteToLog(strLogFile, strMsg_DownloadCellSuccess);

							if (!EsUpdateData(tmip))
							{
								EsWriteToLog(strLogFile, strMsg_InterpolationFailed);
								nStatus = 1;
								continue;
							}

							//�����º����������д�����ݿ�
							EsWriteToLog(strLogFile, strMsg_InterpolationSuccess);

							if (!EsUpLoadData(vecStrPropNameUpdate[idxProp], vecAttrType[idxProp][idxLod], strMajorMatchedFieldName, strMinorMatchedFieldName, strMajorID, strMinorID, tmip, vecStrModelID[idxProp][idxLod], vecLodSet[idxProp][idxLod]))
							{
								sprintf_s(szMsg, strMsg_UploadCell_Failed, EsGetErrorMsg().c_str());
								EsWriteToLog(strLogFile, szMsg);
								nStatus = 1;
								continue;
							}

							EsWriteToLog(strLogFile, strMsg_UploadCell_Success);

							//��ǰLOD��������֮��ʹ�õ�ǰ��ģ�����ϴֻ�
							EsWriteToLog(strLogFile, strMsg_ExtractLodProp_Start);
							EsExtractPropFromCurrentLodToLowerLod(vecGIforUpdate[idxArea], vecStrPropNameUpdate[idxProp], vecStrModelID[idxProp][idxLod], vecLodSet[idxProp][idxLod]);
							EsWriteToLog(strLogFile, strMsg_ExtractLodProp_End);
						}
					}
				}
			}
			else if (vecAttrType[idxProp][idxLod] == Discrete)
			{
				/*********************��ͬ�ز��Զ�����************************/
				for (size_t idxMajor = 0; idxMajor < vecMajorIDSet[idxProp].size(); idxMajor++)
				{
					sprintf_s(szMsg, strMsg_UpdateMajor, strMajorMatchedFieldName, idxMajor + 1, vecMajorIDSet[idxProp][idxMajor], vecMajorIDSet[idxProp].size() - 1 - idxMajor);
					EsWriteToLog(strLogFile, szMsg);

					for (size_t idxMinor = 0; idxMinor < vecMinorIDSet[idxProp][idxMajor].size(); idxMinor++)
					{
						sprintf_s(szMsg, strMsg_UpdateMinor, strMinorMatchedFieldName, idxMinor + 1, vecMinorIDSet[idxProp][idxMajor][idxMinor], vecMinorIDSet[idxProp][idxMajor].size() - 1 - idxMinor);
						EsWriteToLog(strLogFile, szMsg);

						//����ز�û�и��µ�������������
						if (vecUpdateDataSet[idxProp][idxLod][idxMajor].empty())
						{
							sprintf_s(szMsg, strMsg_UpdateMinor_NoUpdate, strMinorMatchedFieldName, vecMinorIDSet[idxProp][idxMajor][idxMinor]);
							EsWriteToLog(strLogFile, szMsg);

							continue;
						}

						//�����µ����������ϴ������ݿ�
						EsWriteToLog(strLogFile, strMsg_UpLoadingCell);

						if (!EsUpLoadData(vecStrPropNameUpdate[idxProp], vecAttrType[idxProp][idxLod], vecUpdateDataSet[idxProp][idxLod][idxMajor][idxMinor], vecStrModelID[idxProp][idxLod], vecLodSet[idxProp][idxLod]))
						{
							sprintf_s(szMsg, strMsg_UpLoad_Failed, EsGetErrorMsg().c_str(), strMinorMatchedFieldName.c_str(), vecMinorIDSet[idxProp][idxMajor][idxMinor]);
							EsWriteToLog(strLogFile, szMsg);
							nStatus = 1;
							continue;
						}

						//��������ı�������ȡ���ֵΪ��������
						int rangeI = INT_MIN; //����������I������������
						int rangeJ = INT_MIN; //����������J������������
						int rangeK = INT_MIN; //����������K������������

						const string strMajorID = std::to_string(vecMajorIDSet[idxProp][idxMajor]);
						const string strMinorID = std::to_string(vecMinorIDSet[idxProp][idxMajor][idxMinor]);
						vector<int> vecFaceCode;
						if (!EsReadServerConfigFaceCode(strConfigPath, vecStrPropNameUpdate[idxProp], vecLodSet[idxProp][idxLod], strMajorID, strMinorID, vecFaceCode))
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
							if (!EsReadServerConfigVariogramByFaceCode(strConfigPath, vecStrPropNameUpdate[idxProp], vecLodSet[idxProp][idxLod], strMajorID, strMinorID, strFaceCode, varPara))
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

						//�Ը��µ����ݽ���Ӱ�췶Χ����
						EsWriteToLog(strLogFile, strMsg_UpLoadCellSuccess);

						vector<GridInfo> vecGIforSampleDownload; //�������ط�Χ��
						vector<GridInfo> vecGIforUpdate; //���巶Χ��				
						if (!EsEvaluate(vecUpdateDataSet[idxProp][idxLod][idxMajor][idxMinor], rangeI, rangeJ, rangeK, vecGridInfo[idxProp][idxLod], vecGIforSampleDownload, vecGIforUpdate))
						{
							sprintf_s(szMsg, strMsg_EvaluateArea_Failed, strMinorMatchedFieldName.c_str(), vecMinorIDSet[idxProp][idxMajor][idxMinor]);
							EsWriteToLog(strLogFile, szMsg);
							nStatus = 1;
							continue;
						}

						if (vecGIforSampleDownload.size() != vecGIforUpdate.size())
						{
							sprintf_s(szMsg, strMsg_EvaluateArea_Failed, strMinorMatchedFieldName.c_str(), vecMinorIDSet[idxProp][idxMajor][idxMinor]);
							EsWriteToLog(strLogFile, szMsg);
							nStatus = 1;
							continue;
						}
						sprintf_s(szMsg, strMsg_EvaluateArea_Success, strMinorMatchedFieldName.c_str(), vecMinorIDSet[idxProp][idxMajor][idxMinor], vecGIforUpdate.size());
						EsWriteToLog(strLogFile, szMsg);

						//�������ļ���ȡ�������ֵʹ�õķ���
						InterMethod nInterMethod;
						if (!EsReadServerConfigMinor(strConfigPath, vecStrPropNameUpdate[idxProp], vecLodSet[idxProp][idxLod], strMajorID, strMinorID, nInterMethod))
						{
							sprintf_s(szMsg, strMsg_GetInterMethod_Failed, strMinorMatchedFieldName.c_str(), vecMinorIDSet[idxProp][idxMajor][idxMinor]);
							EsWriteToLog(strLogFile, szMsg);
							nStatus = 1;
							continue;
						}

						/*********************��ÿ������������и���************************/
						for (size_t idxArea = 0; idxArea < vecGIforUpdate.size(); idxArea++)
						{
							sprintf_s(szMsg, strMsg_UpdateArea, idxArea + 1, vecGIforUpdate[idxArea].nIOri, vecGIforUpdate[idxArea].nJOri, vecGIforUpdate[idxArea].nKOri, vecGIforUpdate[idxArea].nXNum,
								vecGIforUpdate[idxArea].nYNum, vecGIforUpdate[idxArea].nZNum, vecGIforUpdate.size() - 1 - idxArea);
							EsWriteToLog(strLogFile, szMsg);

							//׼����ֵ����
							ModelInterPara tmip; //��ֵ����
							tmip.strWorkPath = strTargetDir; //����Ŀ¼
							tmip.nAttrType = Discrete; //����Ϊ����
							tmip.gi = vecGIforUpdate[idxArea]; //��������			
							tmip.nInterMethod = nInterMethod; //��ֵ�㷨

															  //������ɢ���Ա���
							tmip.faciesPara.resize(vecFaceCode.size());
							for (size_t idxCode = 0; idxCode < vecFaceCode.size(); idxCode++)
							{
								tmip.faciesPara[idxCode].nCode = vecFaceCode[idxCode];

								VariogramPara varParaTemp;
								string strCode = std::to_string(vecFaceCode[idxCode]);
								EsReadServerConfigVariogramByFaceCode(strConfigPath, vecStrPropNameUpdate[idxProp], vecLodSet[idxProp][idxLod], strMajorID, strMinorID, strCode, varParaTemp);
								tmip.faciesPara[idxCode].varPara = varParaTemp;
							}

							bool bStochastic = false;
							if (nInterMethod == SequentialIndicatorSimulation)
							{
								bStochastic = true;
							}

							//��������ѡȡĿ�귶Χ�ڵ�����
							EsWriteToLog(strLogFile, strMsg_DownloadCell);

							if (!EsDownLoadData(strTargetDir, vecStrPropNameUpdate[idxProp], vecAttrType[idxProp][idxLod], strMajorMatchedFieldName, strMinorMatchedFieldName, strMajorID, strMinorID, vecGIforSampleDownload[idxArea],
								vecGIforUpdate[idxArea], bStochastic, vecStrModelID[idxProp][idxLod], vecLodSet[idxProp][idxLod], vecCellAfterUpdate[idxProp][idxLod], tmip.vecCell))
							{
								sprintf_s(szMsg, strMsg_DownloadingCell_Failed, EsGetErrorMsg().c_str());
								EsWriteToLog(strLogFile, szMsg);
								nStatus = 1;
								continue;
							}

							//��������-��ֵ����
							EsWriteToLog(strLogFile, strMsg_DownloadCellSuccess);

							if (!EsUpdateData(tmip))
							{
								EsWriteToLog(strLogFile, strMsg_InterpolationFailed);
								nStatus = 1;
								continue;
							}

							//�����º����������д�����ݿ�
							EsWriteToLog(strLogFile, strMsg_InterpolationSuccess);

							if (!EsUpLoadData(vecStrPropNameUpdate[idxProp], vecAttrType[idxProp][idxLod], strMajorMatchedFieldName, strMinorMatchedFieldName, strMajorID, strMinorID, tmip, vecStrModelID[idxProp][idxLod], vecLodSet[idxProp][idxLod]))
							{
								sprintf_s(szMsg, strMsg_UploadCell_Failed, EsGetErrorMsg().c_str());
								EsWriteToLog(strLogFile, szMsg);
								nStatus = 1;
								continue;
							}

							EsWriteToLog(strLogFile, strMsg_UploadCell_Success);

							//��ǰLOD��������֮��ʹ�õ�ǰ��ģ�����ϴֻ�
							EsWriteToLog(strLogFile, strMsg_ExtractLodProp_Start);
							EsExtractPropFromCurrentLodToLowerLod(vecGIforUpdate[idxArea], vecStrPropNameUpdate[idxProp], vecStrModelID[idxProp][idxLod], vecLodSet[idxProp][idxLod]);
							EsWriteToLog(strLogFile, strMsg_ExtractLodProp_End);
						}
					}
				}
			}
		}
	}

	EsWriteToLog(strLogFile, strMsg_CloseServer);
	return nStatus;
}

bool gridworld::GS_TwoMFN::CModelUpdateServerGS_TwoMFN::EsCheckIsUpdated(const string& strTargetDir, vector<string>& vecStrUpdatePropName)
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

bool gridworld::GS_TwoMFN::CModelUpdateServerGS_TwoMFN::EsGetUpdatedSampleData(const string& strTargetDir, const string& strConfigPath, const string& strMajorMatchedFieldName, const string& strMinorMatchedFieldName, const vector<string>& vecStrUpdatePropName, vector<vector<AttrType>>& vecAttrType, vector<vector<vector<vector<vector<Cell>>>>>& vecCellSet,
	vector<vector<vector<Cell>>>& vecCellAfterUpdate, vector<vector<string>>& vecStrModelID, vector<vector<int>>& vecLodSet, vector<vector<int>>& vecMajorIDSet, vector<vector<vector<int>>>& vecMinorIDSet, vector<vector<GridInfo>>& vecGISet)
{
	//�������ļ���ȡ�������ƥ�����Ϣ
	vecAttrType.resize(vecStrUpdatePropName.size());
	vecStrModelID.resize(vecStrUpdatePropName.size());
	vecLodSet.resize(vecStrUpdatePropName.size());
	vecMajorIDSet.resize(vecStrUpdatePropName.size());
	vecMinorIDSet.resize(vecStrUpdatePropName.size());
	vecGISet.resize(vecStrUpdatePropName.size());
	for (size_t i = 0; i < vecStrUpdatePropName.size(); i++)
	{
		//��ȡ��������������͡��Լ������ݿ��ֶ�Ӧ�����Գ�ID�����γ�ID��Ҫ����ID
		if (!EsReadServerConfigProperty(strConfigPath, vecStrUpdatePropName[i], vecAttrType[i], vecLodSet[i], vecStrModelID[i]))
		{
			EsSetErrorMsg(strErrorMsg_FailedToReadConfig);
			return false;
		}
		
		vecGISet[i].resize(vecLodSet[i].size());
		for (size_t j = 0; j < vecLodSet[i].size(); j++)
		{
			//��ȡ�������ģ�͵�MajorID
			if (!EsReadServerConfigMajorID(strConfigPath, vecStrUpdatePropName[i], vecLodSet[i][j], vecMajorIDSet[i]))
			{
				EsSetErrorMsg(strErrorMsg_FailedToReadConfig);
				return false;
			}

			//��ȡ�������ģ�͵�MinorID
			if (!EsReadServerConfigMinorID(strConfigPath, vecStrUpdatePropName[i], vecLodSet[i][j], vecMinorIDSet[i]))
			{
				EsSetErrorMsg(strErrorMsg_FailedToReadConfig);
				return false;
			}

			//��ģ�����ݷ������Ļ�ȡģ���ʷֵ�������Ϣ
			if (!EsGetGlobalGridInfoFromModelDataBase(vecStrModelID[i][j], vecLodSet[i][j], vecGISet[i][j]))
				return false;

			//�����ݿ��д��������ԣ���������򲻻ᴴ�����������Ա�֤���ݿ��ж��и�����
			CreateGridFields createGridFields(vecStrModelID[i][j]);
			Field field;
			field.Name(vecStrUpdatePropName[i]);
			field.DataType(vecAttrType[i][j] == Discrete ? geo3dml::Field::ValueType::Integer : geo3dml::Field::ValueType::Double);
			createGridFields.AddField(field);
		}
	}

	//���������Ե�ԭʼ��״ֻ�������
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

			//�Ƚ�����ǰ�����ת������������
			bool bAddDiff = true;
			bool bFileMustExist = false;
			if (!EsGetPropDataFromGeoDataCenter(strDataOriginDir, vecStrUpdatePropName[i], vecAttrType[i][j], vecGISet[i][j], bFileMustExist, bAddDiff, mapCellIdAndCellSet))
			{
				EsSetErrorMsg(strErrorMsg_FailedToReadBoreholeFile);
				return false;
			}

			//�õ�����ǰ�Ĵֻ����
			if (!EsScaleUp(vecAttrType[i][j], mapCellIdAndCellSet, vecCellBeforeUpdate[i][j]))
				return false;

			//Ȼ�󽫸��º�����ת������������
			bFileMustExist = true;
			if (!EsGetPropDataFromGeoDataCenter(strDataUpdateDir, vecStrUpdatePropName[i], vecAttrType[i][j], vecGISet[i][j], bFileMustExist, bAddDiff, mapCellIdAndCellSet))
			{
				EsSetErrorMsg(strErrorMsg_FailedToReadBoreholeFile);
				return false;
			}
			
			//����ͬһ�����ڵ����ݽ��й�һ��			
			if (!EsScaleUp(vecAttrType[i][j], mapCellIdAndCellSet, vecCellAfterUpdate[i][j]))
				return false;

			//�ͷ��ڴ�
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

			//�Ƚ����µ����ת������������
			bool bAddDiff = true;
			bool bFileMustExist = true;
			if (!EsGetPropDataFromGeoDataCenter(strDataUpdateDir, vecStrUpdatePropName[i], vecAttrType[i][j], vecGISet[i][j], bFileMustExist, bAddDiff, mapCellIdAndCellSet))
			{
				EsSetErrorMsg(strErrorMsg_FailedToReadBoreholeFile);
				return false;
			}

			//Ȼ���ԭʼ����л�ȡ������������ļ�¼
			bAddDiff = false;
			bFileMustExist = false;
			if (!EsGetPropDataFromGeoDataCenter(strDataOriginDir, vecStrUpdatePropName[i], vecAttrType[i][j], vecGISet[i][j], bFileMustExist, bAddDiff, mapCellIdAndCellSet))
			{
				EsSetErrorMsg(strErrorMsg_FailedToReadBoreholeFile);
				return false;
			}

			//����ͬһ�����ڵ����ݽ��й�һ��			
			if (!EsScaleUp(vecAttrType[i][j], mapCellIdAndCellSet, vecCellTemp[i][j]))
				return false;

			//�ͷ��ڴ�
			for (auto pair : mapCellIdAndCellSet)
			{
				delete pair.second;
				pair.second = NULL;
			}

#ifdef EXPORTSCALEUPRESULT
			const string strScaleUpDir = strTargetDir + "\\" + strMatchedDataCenter_ScaleUp + "\\";
			if (_access(strScaleUpDir.c_str(), 0) == -1)
			{
				_mkdir(strScaleUpDir.c_str());
			}

			const string strScaleUpFile = strScaleUpDir + "modelid_" + vecStrModelID[i][j] + "_" + vecStrUpdatePropName[i] + "_Lod_" + std::to_string(vecLodSet[i][j]) + ".txt";
			EsExportScaleUpCell(strScaleUpFile, vecAttrType[i][j], vecCellTemp[i][j]);
#endif
		}
	}

	//���������԰��ز�����Խ��з���
	if (!EsSelectDataByMatchedFieldName(strMajorMatchedFieldName, strMinorMatchedFieldName, vecStrUpdatePropName, vecAttrType, vecStrModelID, vecLodSet, vecGISet, vecMajorIDSet, vecMinorIDSet, vecCellTemp, vecCellBeforeUpdate, vecCellSet))
		return false;

	return true;
}

bool gridworld::GS_TwoMFN::CModelUpdateServerGS_TwoMFN::EsDownLoadData(const string& strTargetDir, const string& strPropName, const AttrType& nAttrType, const string& strMajorMatchedFieldName, const string& strMinorMatchedFieldName, const string& strMajorID, 
	const string& strMinorID, const GridInfo& giforSampleDownload, const GridInfo& giforUpdate, bool bStochastic, const string& strModelID, const int nLod, const vector<Cell>& vecCellOrigin, vector<Cell>& vecCell)
{
	/*******************�����ݿ�����������Χ�ڵ���������********************/
	//��ʼ����
	//��ʼ����
	int nMinI, nMinJ, nMinK;
	nMinI = giforSampleDownload.nIOri;
	nMinJ = giforSampleDownload.nJOri;
	nMinK = giforSampleDownload.nKOri;

	//��ֹ����
	int nMaxI, nMaxJ, nMaxK;
	nMaxI = giforSampleDownload.nIOri + giforSampleDownload.nXNum - 1;
	nMaxJ = giforSampleDownload.nJOri + giforSampleDownload.nYNum - 1;
	nMaxK = giforSampleDownload.nKOri + giforSampleDownload.nZNum - 1;

	//���жϲ�ֵ��Χ�ڵ�Լ������ֵ�Ƿ���ڣ���������ڣ����Դ�����LOD��ȡ������ȡʧ�ܣ��򱾷�Χ����
	if (!EsCheckAndBuildMatchedField(giforSampleDownload, nMinI, nMinJ, nMinK, nMaxI, nMaxJ, nMaxK, strModelID, nLod, strMajorMatchedFieldName))
	{
		EsSetErrorMsg(strErrorMsg_DownLoadData_MajorMatchedFieldNoData);
		return false;
	}

	if (!EsCheckAndBuildMatchedField(giforSampleDownload, nMinI, nMinJ, nMinK, nMaxI, nMaxJ, nMaxK, strModelID, nLod, strMinorMatchedFieldName))
	{
		EsSetErrorMsg(strErrorMsg_DownLoadData_MinorMatchedFieldNoData);
		return false;
	}

	//��ȡ�������ط�Χ������,�ӿ�һ���������DEFAULT_LIMIT���������Ե÷���������
	QueryCellsInBoxOfIJK queryCellsInBoxOfIJK(strModelID, nLod);
	queryCellsInBoxOfIJK.Box(nMinI, nMinJ, nMaxI, nMaxJ);
	queryCellsInBoxOfIJK.ZRange(nMinK, nMaxK);

	queryCellsInBoxOfIJK.Select(strMajorMatchedFieldName);
	queryCellsInBoxOfIJK.Select(strMinorMatchedFieldName);
	queryCellsInBoxOfIJK.Select(strPropName);

	queryCellsInBoxOfIJK.Limit(g3dcpp::DEFAULT_LIMIT);

	vecCell.clear();
	while (queryCellsInBoxOfIJK.HasNextPage())
	{
		//��������ȡʧ�ܣ��򷵻�
		vector<Grid::Cell> vecCellTemp;
		if (!queryCellsInBoxOfIJK.NextPage(vecCellTemp))
		{
			EsSetErrorMsg(strErrorMsg_DownLoadData_FailedToDownLoadCell);
			return false;
		}
	
		/*******************�������ص����񣬻�ȡ����Ҫ�����������ֵ********************/	
		for (auto sGridCell : vecCellTemp)
		{
			//�������񣬼�¼IJK
			Cell cell;
			cell.nI = sGridCell.I();
			cell.nJ = sGridCell.J();
			cell.nK = sGridCell.K();

			//���������ɸѡ���Ժʹ�ɸѡ���Թ�ͬɸѡ����			
			FieldValue fieldValue_MajorMatchedFieldName;
			if(!sGridCell.FieldValueOf(strMajorMatchedFieldName, fieldValue_MajorMatchedFieldName))
				continue;

			FieldValue fieldValue_MinorMatchedFieldName;
			if(!sGridCell.FieldValueOf(strMinorMatchedFieldName, fieldValue_MinorMatchedFieldName))
				continue;

			bool bMatched = false;
			if (std::to_string(fieldValue_MajorMatchedFieldName.ValueAsInt()) == strMajorID && std::to_string(fieldValue_MinorMatchedFieldName.ValueAsInt()) == strMinorID)
				bMatched = true;

			//�������Ŀ������������
			if (!bMatched)
				continue;

			/*******************�����Ŀ����������ݲ�ֵ�㷨��ȡ��Ӧ����********************/
			//��������ģ���㷨�����ֵ��Χ��ֻȡԭʼ��������ֵ��Χ�⣨�������ط�Χ�ڣ�ȡ���ж�Ӧ����ֵ�������ȷ���Բ�ֵ�㷨����ȡ�������ط�Χ�ڵ�����ԭʼ����
			if (bStochastic) //���ģ��
			{
				if (cell.nI >= giforUpdate.nIOri && cell.nI <= (giforUpdate.nIOri + giforUpdate.nXNum - 1)
					&& cell.nJ >= giforUpdate.nJOri && cell.nJ <= (giforUpdate.nJOri + giforUpdate.nYNum - 1)
					&& cell.nK >= giforUpdate.nKOri && cell.nK <= (giforUpdate.nKOri + giforUpdate.nZNum - 1))
				{
					//�ж����������Ƿ���Դ�����
					bool bOrigin = false;
					for (auto cellOrigin : vecCellOrigin)
					{
						if (cell.nI == cellOrigin.nI && cell.nJ == cellOrigin.nJ && cell.nK == cellOrigin.nK)
						{
							bOrigin = true;
							break;
						}
					}

					//�������ԭʼ������������
					if (!bOrigin)
						continue;

					//�����Ŀ����������ԭʼ���������ȡĿ������ֵ
					FieldValue fieldValue_PropName;
					if(!sGridCell.FieldValueOf(strPropName, fieldValue_PropName))
						continue;
				
					//ת���ɶ�Ӧ��������
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
					//�����Ŀ���������ȡĿ������ֵ
					FieldValue fieldValue_PropName;
					if (!sGridCell.FieldValueOf(strPropName, fieldValue_PropName))
						continue;
				
					//ת���ɶ�Ӧ��������
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
			else //ȷ���Բ�ֵ
			{
				//�ж����������Ƿ���Դ�����
				bool bOrigin = false;
				for (auto cellOrigin : vecCellOrigin)
				{
					if (cell.nI == cellOrigin.nI && cell.nJ == cellOrigin.nJ && cell.nK == cellOrigin.nK)
					{
						bOrigin = true;
						break;
					}
				}

				//�������ԭʼ������������
				if (!bOrigin)
					continue;

				//�����Ŀ����������ԭʼ���������ȡĿ������ֵ
				FieldValue fieldValue_PropName;
				if (!sGridCell.FieldValueOf(strPropName, fieldValue_PropName))
					continue;
			
				//ת���ɶ�Ӧ��������
				if (nAttrType == Discrete)
				{
					cell.nValue = fieldValue_PropName.ValueAsInt();
				}
				else
				{
					cell.dValue = fieldValue_PropName.ValueAsDouble();
				}
			}

			//�洢����
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

bool gridworld::GS_TwoMFN::CModelUpdateServerGS_TwoMFN::EsUpLoadData(const string& strPropName, const AttrType nAttrType, const vector<Cell>& vecCell, const string& strModelID, const int& nLod)
{
	if (!EsUpdateDataValue(strPropName, nAttrType, vecCell, strModelID, nLod))
		return false;
	
	return true;
}

bool gridworld::GS_TwoMFN::CModelUpdateServerGS_TwoMFN::EsUpLoadData(const string& strPropName, const AttrType nAttrType, const string& strMajorMatchedFieldName, const string& strMinorMatchedFieldName,
	const string& strMajorID, const string& strMinorID, ModelInterPara& tmip, const string& strModelID, const int& nLod)
{
#ifdef EXPORTINTERRESULT
	//����ֵ���д�����ļ�
	const string strResultDir = tmip.strWorkPath.substr(0, tmip.strWorkPath.find_last_of("//") - 1) + "\\" + strMatchedDataCenter_InterResult;
	if (_access(strResultDir.c_str(), 0) == -1)
	{
		_mkdir(strResultDir.c_str());
	}

	const string strFilePath = strResultDir + "\\" + "modelid_" + strModelID + "_" + strPropName + "_Lod_" + std::to_string(nLod) + "_" +
		strMajorMatchedFieldName + "_" + strMajorID + "_" +
		strMinorMatchedFieldName + "_" + strMinorID + "_" +
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

	//��ʼ����
	int nMinI, nMinJ, nMinK;
	nMinI = tmip.gi.nIOri;
	nMinJ = tmip.gi.nJOri;
	nMinK = tmip.gi.nKOri;

	//��ֹ����
	int nMaxI, nMaxJ, nMaxK;
	nMaxI = tmip.gi.nIOri + tmip.gi.nXNum - 1;
	nMaxJ = tmip.gi.nJOri + tmip.gi.nYNum - 1;
	nMaxK = tmip.gi.nKOri + tmip.gi.nZNum - 1;

	//���жϸ��·�Χ�ڵ�Լ������ֵ�Ƿ���ڣ���������ڣ����Դ�����LOD��ȡ������ȡʧ�ܣ��򱾷�Χ����
	EsCheckAndBuildMatchedField(tmip.gi, nMinI, nMinJ, nMinK, nMaxI, nMaxJ, nMaxK, strModelID, nLod, strMajorMatchedFieldName);
	EsCheckAndBuildMatchedField(tmip.gi, nMinI, nMinJ, nMinK, nMaxI, nMaxJ, nMaxK, strModelID, nLod, strMinorMatchedFieldName);

	//��ȡ�������ط�Χ������,�ӿ�һ���������DEFAULT_LIMIT���������Ե÷���������
	QueryCellsInBoxOfIJK queryCellsInBoxOfIJK(strModelID, nLod);
	queryCellsInBoxOfIJK.Box(nMinI, nMinJ, nMaxI, nMaxJ);
	queryCellsInBoxOfIJK.ZRange(nMinK, nMaxK);

	queryCellsInBoxOfIJK.Select(strMajorMatchedFieldName);
	queryCellsInBoxOfIJK.Select(strMinorMatchedFieldName);

	queryCellsInBoxOfIJK.Limit(g3dcpp::DEFAULT_LIMIT);

	while (queryCellsInBoxOfIJK.HasNextPage())
	{
		//��������ȡʧ�ܣ��򷵻�
		vector<Grid::Cell> vecCellTemp;
		if (!queryCellsInBoxOfIJK.NextPage(vecCellTemp))
		{
			EsSetErrorMsg(strErrorMsg_DownLoadData_FailedToDownLoadCell);

#ifdef EXPORTINTERRESULT
			fout.close();
#endif // EXPORTINTERRESULT

			return false;
		}

		/*******************�������ص����񣬽���ǰ�ز��ڵĲ�ֵ���������������ݸ�ʽ********************/
		vector<Cell> vecCell;
		vecCell.clear();
		for (auto sGridCell : vecCellTemp)
		{
			//�������񣬼�¼IJK
			Cell cell;
			cell.nI = sGridCell.I();
			cell.nJ = sGridCell.J();
			cell.nK = sGridCell.K();

			//�ж��������ɸѡ����ֵ�ʹ�ɸѡ����ֵ�Ƿ���Ŀ��ֵ
			FieldValue fieldValue_MajorMatchedFieldName;
			if(!sGridCell.FieldValueOf(strMajorMatchedFieldName, fieldValue_MajorMatchedFieldName))
				continue;

			FieldValue fieldValue_MinorMatchedFieldName;
			if(!sGridCell.FieldValueOf(strMinorMatchedFieldName, fieldValue_MinorMatchedFieldName))
				continue;

			bool bMatched = false;
			if (std::to_string(fieldValue_MajorMatchedFieldName.ValueAsInt()) == strMajorID && std::to_string(fieldValue_MinorMatchedFieldName.ValueAsInt()) == strMinorID)
				bMatched = true;

			//�������Ŀ������������
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

		//�ٵ����������ݸ�ʽ���ϴ��ӿڸ��µ����ݿ�
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

bool gridworld::GS_TwoMFN::CModelUpdateServerGS_TwoMFN::EsReadServerConfigSetting(const string& strConfigPath, string& strRootURL, string& strMajorMatchedFieldName, string& strMinorMatchedFieldName)
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
	strMajorMatchedFieldName = pElem->Attribute(strConfigSetting_MajorMatchedFieldName);
	strMinorMatchedFieldName = pElem->Attribute(strConfigSetting_MinorMatchedFieldName);

	return true;
}

bool gridworld::GS_TwoMFN::CModelUpdateServerGS_TwoMFN::EsReadServerConfigProperty(const string& strConfigPath, const string& strPropName, vector<AttrType>& vecAttrType, vector<int>& vecLod, vector<string>& vecModelID)
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

bool gridworld::GS_TwoMFN::CModelUpdateServerGS_TwoMFN::EsReadServerConfigMinor(const string& strConfigPath, const string& strPropName, const int& nLod, const string& strMajorID, const string& strMinorID, InterMethod& nInterpolationMethod)
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
				if (pSecondElem && pSecondElem->ValueTStr() == strConfigMajor && pSecondElem->Attribute(strConfigMajor_MajorID) == strMajorID)
				{
					for (TiXmlNode* pThirdChild = pSecondElem->FirstChild(); pThirdChild; pThirdChild = pThirdChild->NextSibling())
					{
						TiXmlElement* pThirdElem = pThirdChild->ToElement();
						if (pThirdElem && pThirdElem->ValueTStr() == strConfigMinor && pThirdElem->Attribute(strConfigMinor_MinorID) == strMinorID)
						{
							int nMehtod = -1;
							pThirdElem->Attribute(strConfigMinor_InterpolationMethod, &nMehtod);

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
		}
	}

	return true;
}

bool gridworld::GS_TwoMFN::CModelUpdateServerGS_TwoMFN::EsReadServerConfigVariogram(const string& strConfigPath, const string& strPropName, const int& nLod, const string& strMajorID, const string& strMinorID, VariogramPara& varPara)
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
				if (pSecondElem && pSecondElem->ValueTStr() == strConfigMajor && pSecondElem->Attribute(strConfigMajor_MajorID) == strMajorID)
				{
					for (TiXmlNode* pThirdChild = pSecondElem->FirstChild(); pThirdChild; pThirdChild = pThirdChild->NextSibling())
					{
						TiXmlElement* pThirdElem = pThirdChild->ToElement();
						if (pThirdElem && pThirdElem->ValueTStr() == strConfigMinor && pThirdElem->Attribute(strConfigMinor_MinorID) == strMinorID)
						{
							for (TiXmlNode* pFourthChild = pThirdElem->FirstChild(); pFourthChild; pFourthChild = pFourthChild->NextSibling())
							{
								TiXmlElement* pFourthElem = pFourthChild->ToElement();
								if (pFourthElem && pFourthElem->ValueTStr() == strConfigVariogram)
								{
									int nType = -1;
									pFourthElem->Attribute(strConfigVariogram_VariogramType, &nType);

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

									pFourthElem->Attribute(strConfigVariogram_Sill, &varPara.dSill);
									pFourthElem->Attribute(strConfigVariogram_Nugget, &varPara.dNugget);
									pFourthElem->Attribute(strConfigVariogram_MajorRange, &varPara.dMajorRange);
									pFourthElem->Attribute(strConfigVariogram_MinorRange, &varPara.dMinorRange);
									pFourthElem->Attribute(strConfigVariogram_VerticalRange, &varPara.dVerticalRange);
									pFourthElem->Attribute(strConfigVariogram_Azimuth, &varPara.dAzimuth);
									pFourthElem->Attribute(strConfigVariogram_Dip, &varPara.dDip);
									break;
								}
							}						
						}						
					}
				}
			}
		}
	}

	return true;
}

bool gridworld::GS_TwoMFN::CModelUpdateServerGS_TwoMFN::EsReadServerConfigVariogramByFaceCode(const string& strConfigPath, const string& strPropName, const int& nLod, const string& strMajorID, const string& strMinorID, const string& strFaceCode, VariogramPara& varPara)
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
				if (pSecondElem && pSecondElem->ValueTStr() == strConfigMajor && pSecondElem->Attribute(strConfigMajor_MajorID) == strMajorID)
				{
					for (TiXmlNode* pThirdChild = pSecondElem->FirstChild(); pThirdChild; pThirdChild = pThirdChild->NextSibling())
					{
						TiXmlElement* pThirdElem = pThirdChild->ToElement();
						if (pThirdElem && pThirdElem->ValueTStr() == strConfigMinor && pThirdElem->Attribute(strConfigMinor_MinorID) == strMinorID)
						{
							for (TiXmlNode* pFourthChild = pThirdElem->FirstChild(); pFourthChild; pFourthChild = pFourthChild->NextSibling())
							{
								TiXmlElement* pFourthElem = pFourthChild->ToElement();
								if (pFourthElem && pFourthElem->ValueTStr() == strConfigVariogram && pFourthElem->Attribute(strConfigVariogram_FaceCode) == strFaceCode)
								{
									int nType = -1;
									pFourthElem->Attribute(strConfigVariogram_VariogramType, &nType);

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

									pFourthElem->Attribute(strConfigVariogram_Sill, &varPara.dSill);
									pFourthElem->Attribute(strConfigVariogram_Nugget, &varPara.dNugget);
									pFourthElem->Attribute(strConfigVariogram_MajorRange, &varPara.dMajorRange);
									pFourthElem->Attribute(strConfigVariogram_MinorRange, &varPara.dMinorRange);
									pFourthElem->Attribute(strConfigVariogram_VerticalRange, &varPara.dVerticalRange);
									pFourthElem->Attribute(strConfigVariogram_Azimuth, &varPara.dAzimuth);
									pFourthElem->Attribute(strConfigVariogram_Dip, &varPara.dDip);
									break;
								}
							}
						}
					}
				}
			}
		}
	}

	return true;
}

bool gridworld::GS_TwoMFN::CModelUpdateServerGS_TwoMFN::EsReadServerConfigMajorID(const string& strConfigPath, const string& strPropName, const int& nLod, vector<int>& vecMajorID)
{
	TiXmlDocument doc(strConfigPath.c_str());
	if (!doc.LoadFile())
		return false;

	TiXmlHandle hDoc(&doc);
	TiXmlElement* pRoot = hDoc.FirstChildElement().Element();
	if (pRoot == NULL)
		return false;

	vecMajorID.clear();
	for (TiXmlNode* pChild = pRoot->FirstChild(); pChild; pChild = pChild->NextSibling())
	{
		TiXmlElement* pElem = pChild->ToElement();
		if (pElem && pElem->ValueTStr() == strConfigProperty && pElem->Attribute(strConfigProperty_PropName) == strPropName && pElem->Attribute(strConfigProperty_Lod) == std::to_string(nLod))
		{
			for (TiXmlNode* pSecondChild = pElem->FirstChild(); pSecondChild; pSecondChild = pSecondChild->NextSibling())
			{
				TiXmlElement* pSecondElem = pSecondChild->ToElement();
				if (pSecondElem && pSecondElem->ValueTStr() == strConfigMajor)
				{
					int id = -1;
					pSecondElem->Attribute(strConfigMajor_MajorID, &id);
					vecMajorID.push_back(id);
				}
			}
		}
	}

	if (vecMajorID.empty())
		return false;

	return true;
}

bool gridworld::GS_TwoMFN::CModelUpdateServerGS_TwoMFN::EsReadServerConfigMinorID(const string& strConfigPath, const string& strPropName, const int& nLod, vector<vector<int>>& vecMinorID)
{
	vector<int> vecMajorID;
	if (!EsReadServerConfigMajorID(strConfigPath, strPropName, nLod, vecMajorID))
		return false;

	TiXmlDocument doc(strConfigPath.c_str());
	if (!doc.LoadFile())
		return false;

	TiXmlHandle hDoc(&doc);
	TiXmlElement* pRoot = hDoc.FirstChildElement().Element();
	if (pRoot == NULL)
		return false;	

	vecMinorID.resize(vecMajorID.size());
	for (TiXmlNode* pChild = pRoot->FirstChild(); pChild; pChild = pChild->NextSibling())
	{
		TiXmlElement* pElem = pChild->ToElement();
		if (pElem && pElem->ValueTStr() == strConfigProperty && pElem->Attribute(strConfigProperty_PropName) == strPropName && pElem->Attribute(strConfigProperty_Lod) == std::to_string(nLod))
		{
			int idxMajor = 0;
			for (TiXmlNode* pSecondChild = pElem->FirstChild(); pSecondChild; pSecondChild = pSecondChild->NextSibling())
			{
				TiXmlElement* pSecondElem = pSecondChild->ToElement();
				if (pSecondElem && pSecondElem->ValueTStr() == strConfigMajor)
				{
					for (TiXmlNode* pThirdChild = pSecondElem->FirstChild(); pThirdChild; pThirdChild = pThirdChild->NextSibling())
					{
						TiXmlElement* pThirdElem = pThirdChild->ToElement();
						if (pThirdElem && pThirdElem->ValueTStr() == strConfigMinor)
						{
							int id = -1;
							pThirdElem->Attribute(strConfigMinor_MinorID, &id);
							vecMinorID[idxMajor].push_back(id);
						}
					}

					idxMajor++;
				}
			}
		}
	}

	return true;
}

bool gridworld::GS_TwoMFN::CModelUpdateServerGS_TwoMFN::EsReadServerConfigFaceCode(const string& strConfigPath, const string& strPropName, const int& nLod, const string& strMajorID, const string& strMinorID, vector<int>& vecFaceCode)
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
				if (pSecondElem && pSecondElem->ValueTStr() == strConfigMajor && pSecondElem->Attribute(strConfigMajor_MajorID) == strMajorID)
				{
					for (TiXmlNode* pThirdChild = pSecondElem->FirstChild(); pThirdChild; pThirdChild = pThirdChild->NextSibling())
					{
						TiXmlElement* pThirdElem = pThirdChild->ToElement();
						if (pThirdElem && pThirdElem->ValueTStr() == strConfigMinor && pThirdElem->Attribute(strConfigMinor_MinorID) == strMinorID)
						{
							for (TiXmlNode* pFourthChild = pThirdElem->FirstChild(); pFourthChild; pFourthChild = pFourthChild->NextSibling())
							{
								TiXmlElement* pFourthElem = pFourthChild->ToElement();
								if (pFourthElem && pFourthElem->ValueTStr() == strConfigVariogram)
								{
									int nCode = -1;
									pFourthElem->Attribute(strConfigVariogram_FaceCode, &nCode);
									vecFaceCode.push_back(nCode);
								}
							}
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

bool gridworld::GS_TwoMFN::CModelUpdateServerGS_TwoMFN::EsExtractPropFromCurrentLodToLowerLod(const GridInfo& gi, const string &strPropName, const string& strModelID, const int nLod)
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

bool gridworld::GS_TwoMFN::CModelUpdateServerGS_TwoMFN::EsGetGlobalGridInfoFromModelDataBase(const string& strModelID, const int& nLod, GridInfo& gi)
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
		//�Ȼ�ȡLOD=0��������Ϣ
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

		//�ٰ�LOD=0�������С��ת����xyz��ʽ
		double lod_zero_DeltaXOri = lod_zero_nIOri*lod_zero_dXStep;
		double lod_zero_DeltaYOri = lod_zero_nJOri*lod_zero_dYStep;
		double lod_zero_DeltaZOri = lod_zero_nKOri*lod_zero_dZStep;

		double lod_zero_DeltaXEnd = lod_zero_nIEnd*lod_zero_dXStep;
		double lod_zero_DeltaYEnd = lod_zero_nJEnd*lod_zero_dYStep;
		double lod_zero_DeltaZEnd = lod_zero_nKEnd*lod_zero_dZStep;

		//Ȼ���ȡ��ǰLOD�Ĳ���
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

bool gridworld::GS_TwoMFN::CModelUpdateServerGS_TwoMFN::EsSelectRecordToCell(const AttrType& nAttrType, const vector<Record>& vecRecord, const GridInfo& gi, bool& bAdd, map<int, vector<Cell>*>& mapCellIdAndCellSet)
{
	if (vecRecord.empty())
		return false;

	//����׼�¼ƥ�䵽��Ӧ������
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

bool gridworld::GS_TwoMFN::CModelUpdateServerGS_TwoMFN::EsScaleUp(const AttrType& nAttrType, map<int, vector<Cell>*>& mapCellIdAndCellSet, vector<Cell>& vecCell)
{	
	if (nAttrType == Discrete)
	{
		//��ɢ�����������ݽ��дֻ�-ȡ���
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
		//���������������ݽ��дֻ�-ȡƽ��
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

bool gridworld::GS_TwoMFN::CModelUpdateServerGS_TwoMFN::EsUpdateDataValue(const string& strPropName, const AttrType& nAttrType, const vector<Cell>& vecCell, const string& strModelID, const int& nLod)
{
	int nCellNum = (int)vecCell.size();
	int nLoopNum = nCellNum / UpLoadLimit;
	int nCellNum_Rest = nCellNum % UpLoadLimit;

	//��ѭ��nLoopNum���ϴ�UpLoadLimit��������
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

		//����ϴ�ʧ�ܣ�������һ��ʱ��Ȼ���������ϴ����������ʧ�ܣ��򱨴��˳�
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

	//Ȼ���ϴ�ʣ�಻��UpLoadLimit��������
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

		//����ϴ�ʧ�ܣ�������һ��ʱ��Ȼ���������ϴ����������ʧ�ܣ��򱨴��˳�
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

bool gridworld::GS_TwoMFN::CModelUpdateServerGS_TwoMFN::EsSelectDataByMatchedFieldName(const string& strMajorMatchedFieldName, const string& strMinorMatchedFieldName, const vector<string>& vecStrUpdatePropName, const vector<vector<AttrType>>& vecAttrType, const vector<vector<string>>& vecStrModelID, 
	vector<vector<int>>& vecLodSet, vector<vector<GridInfo>>& vecGISet, const vector<vector<int>>& vecMajorIDSet, const vector<vector<vector<int>>>& vecMinorIDSet, const vector<vector<vector<Cell>>>& vecCellIn, const vector<vector<vector<Cell>>>& vecCellBeforeUpdate, vector<vector<vector<vector<vector<Cell>>>>>& vecCellOut)
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

			queryCellsByIJK.Select(strMajorMatchedFieldName);
			queryCellsByIJK.Select(strMinorMatchedFieldName);

			if (!EsCheckAndBuildMatchedField(vecGISet[idxProp][idxLod], nMinI, nMinJ, nMinK, nMaxI, nMaxJ, nMaxK, vecStrModelID[idxProp][idxLod], vecLodSet[idxProp][idxLod], strMajorMatchedFieldName))
				continue;

			if (!EsCheckAndBuildMatchedField(vecGISet[idxProp][idxLod], nMinI, nMinJ, nMinK, nMaxI, nMaxJ, nMaxK, vecStrModelID[idxProp][idxLod], vecLodSet[idxProp][idxLod], strMinorMatchedFieldName))
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

			//���������Ժʹ����Ե�ID����Ϊ����������ռ�
			vecCellOut[idxProp][idxLod].resize(vecMajorIDSet[idxProp].size());
			for (size_t idxMajor = 0; idxMajor < vecMajorIDSet[idxProp].size(); idxMajor++)
			{
				vecCellOut[idxProp][idxLod][idxMajor].resize(vecMinorIDSet[idxProp][idxMajor].size());
			}

			//�������񣬰�������з���
			for (size_t idxCell = 0; idxCell < vecCellIn[idxProp][idxLod].size(); idxCell++)
			{

#ifdef MATCHVALUE
				//�����ǰ����������ԡ������Ի�ǰ������һ�����������Գ����������ĸ���������Ч�������ں�������
				if (vecSGridVolumeMajorMatchedField[idxCell].cellFields.empty() || vecSGridVolumeMinorMatchedField[idxCell].cellFields.empty() || vecSGridVolumePropValue[idxCell].cellFields.empty())
					continue;
#else
				//�����ǰ����������ԡ������Ի�ǰ������һ�����������Գ����������ĸ���������Ч�������ں�������
				//if (vecSGridVolumeMajorMatchedField[idxCell].cellFields.empty() || vecSGridVolumeMinorMatchedField[idxCell].cellFields.empty())
				//	continue;
#endif // MATCHVALUE			

				//�����ǰ����������ԡ������Դ������Գ�����������һ������Ϊ�գ��������ĸ���������Ч�������ں�������
				FieldValue fieldValue_MajorMatchedFieldName;
				if(!vecCellTemp[idxCell].FieldValueOf(strMajorMatchedFieldName, fieldValue_MajorMatchedFieldName))
					continue;

				FieldValue fieldValue_MinorMatchedFieldName;
				if(!vecCellTemp[idxCell].FieldValueOf(strMinorMatchedFieldName, fieldValue_MinorMatchedFieldName))
					continue;

#ifdef MATCHVALUE
				//�жϵ�ǰ���µ������Ƿ��Ǹ���ǰ����������
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

				//����Ǹ���ǰ��������������Ҫ�жϸ��º�����ݺ͸���ǰ�������Ƿ���ͬ�������ͬ�����������������Ч�ĸ��£������ں�������
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

				//����һ������Ч���£������µ����������Ժʹ����Խ��з���
				int nMajorId = fieldValue_MajorMatchedFieldName.ValueAsInt();
				int nMinorId = fieldValue_MinorMatchedFieldName.ValueAsInt();
				for (size_t idxMajor = 0; idxMajor < vecMajorIDSet[idxProp].size(); idxMajor++)
				{
					if (nMajorId == vecMajorIDSet[idxProp][idxMajor])
					{
						for (size_t idxMinor = 0; idxMinor < vecMinorIDSet[idxProp][idxMajor].size(); idxMinor++)
						{
							if (nMinorId == vecMinorIDSet[idxProp][idxMajor][idxMinor])
							{
								vecCellOut[idxProp][idxLod][idxMajor][idxMinor].push_back(vecCellIn[idxProp][idxLod][idxCell]);
								break;
							}
						}

						break;
					}
				}				
			}		
		}
	}

	return true;
}

void gridworld::GS_TwoMFN::CModelUpdateServerGS_TwoMFN::split(const string source, vector<string>& dest)
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

void gridworld::GS_TwoMFN::CModelUpdateServerGS_TwoMFN::MyFileRetrieveByExt(const string& strRootDir, const string& strExt, bool bDoWithSubDir, vector<string>& vecStrFileName, vector<string>& vecStrFilePath)
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

bool gridworld::GS_TwoMFN::CModelUpdateServerGS_TwoMFN::EsGetPropDataFromGeoDataCenter(const string& strDataDir, const string& strPropName, const AttrType& nAttrType, const GridInfo& gi, bool& bFileMustExit, bool& bAddDiff, map<int, vector<Cell>*>& mapCellIdAndCellSet)
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

	//����ļ������ݣ�����ļ�����Ҫ���ڣ��򷵻�true�����������ڣ��򷵻�false
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

bool gridworld::GS_TwoMFN::CModelUpdateServerGS_TwoMFN::EsGetBoreholeRecordFromFile(const string& strBoreholeFile, vector<Record>& vecRecord)
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

bool gridworld::GS_TwoMFN::CModelUpdateServerGS_TwoMFN::EsExportScaleUpCell(const string& strSacleUpFilePath, const AttrType nAttrType, const vector<Cell>& vecCell)
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

bool gridworld::GS_TwoMFN::CModelUpdateServerGS_TwoMFN::EsCheckAndBuildMatchedField(const GridInfo& gi, const int nMinI, const int nMinJ, const int nMinK, const int nMaxI, const int nMaxJ, const int nMaxK,
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