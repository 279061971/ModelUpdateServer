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
		cout << "����û���������\n";
		return 1;
	}
	const string strTargetDir = argv[1];

	CModelUpdateServerGS_TwoMFN server; //�������·���
	int res = server.EsStartServer(strTargetDir);
	return res;
}

//void main(int argc, char* argv[])
//{
//	/*********************��ʼ������************************/
//	if (argc < 2)
//	{
//		cout << "����û���������\n";
//		return;
//	}
//	const string strTargetDir = argv[1];
//
//	string strMsg = "";
//	string strRootURL = ""; //ģ�����ݷ������ĸ�·��
//	string strMajorMatchedFieldName = ""; //����ɸѡ�������������������
//	string strMinorMatchedFieldName = ""; //����ɸѡ����Ĵ�������������
//	const string strConfigPath = strTargetDir + "\\" + strMatchedDataCenter_Config; //�Զ����·��������ļ�
//	const string strLogFile = strTargetDir + "\\log.txt"; //��־�ļ�
//
//	CModelUpdateServerGS_TwoMFN server; //�������·���
//	if (!server.EsReadServerConfigSetting(strConfigPath, strRootURL, strMajorMatchedFieldName, strMinorMatchedFieldName))
//	{
//		strMsg = server.EsGetTime() + " " + server.EsGetErrorMsg();
//		cout << strMsg << "\n";
//		server.EsWriteToLog(strLogFile, strMsg);
//		return;
//	}
//
//	/*********************��������************************/
//	strMsg = server.EsGetTime() + "*********************�����Զ����·���************************";
//	cout << strMsg << "\n";
//	server.EsWriteToLog(strLogFile, strMsg);
//
//	/*********************�ӵ��ʴ��������ļ����������Ƿ��и���************************/
//	strMsg = server.EsGetTime() + "���ڼ����ʴ����������Ƿ��и���";
//	cout << strMsg << "\n";
//	server.EsWriteToLog(strLogFile, strMsg);
//
//	vector<string> vecStrPropNameUpdate; //���µ���������
//	if (!server.EsCheckIsUpdated(strTargetDir, vecStrPropNameUpdate))
//	{
//		strMsg = server.EsGetTime() + server.EsGetErrorMsg();
//		cout << strMsg << "\n";
//		server.EsWriteToLog(strLogFile, strMsg);
//		return;
//	}
//
//	/*********************�ӵ��ʴ��������ĺ�ģ�����ݷ������Ļ�ȡ���µ���������Ϣ************************/
//	strMsg = server.EsGetTime() + "��⵽���������и��£�";
//	for (auto propName : vecStrPropNameUpdate)
//	{
//		strMsg += propName + "\t";
//	}
//	cout << strMsg << ",���ڻ�ȡ���µ���ף����ֻ�������...\n";
//	server.EsWriteToLog(strLogFile, strMsg);
//
//	vector<vector<AttrType>> vecAttrType;                                 //���µ��������ͼ�         ����/Lod
//	vector<vector<GridInfo>> vecGridInfo;                                 //���Զ�Ӧ��������Ϣ��     ����/Lod
//	vector<vector<string>> vecStrCoverageID;                              //���Զ�Ӧ�����Գ�ID��     ����/Lod
//	vector<vector<string>> vecStrShapeID;                                 //���Զ�Ӧ�ļ��γ�ID��     ����/Lod
//	vector<vector<string>> vecStrFeatureID;                               //���Զ�Ӧ��Ҫ��ID��       ����/Lod
//	vector<vector<string>> vecStrFeaClassID;                              //���Զ�Ӧ��Ҫ����ID��     ����/Lod
//	vector<vector<int>> vecLodSet;                                        //���Զ�Ӧ�ĵز��ID��     ����/Lod
//	vector<vector<int>> vecMajorIDSet;                                    //���Զ�Ӧ��MajorID��      ����/MajorId
//	vector<vector<vector<int>>> vecMinorIDSet;                            //���Զ�Ӧ��MinorID��      ����/Major/MinorId	
//	vector<vector<vector<vector<vector<Cell>>>>> vecUpdateDataSet;        //���µ���������           ����/Lod/Major/Minor
//	vector<vector<vector<Cell>>> vecCellBeforeUpdate;                     //����ǰ����������         ����/Lod
//	vector<vector<vector<Cell>>> vecCellAfterUpdate;                      //���º����������         ����/Lod
//	if (!server.EsGetUpdatedSampleData(strTargetDir, strConfigPath, strRootURL, strMajorMatchedFieldName, strMinorMatchedFieldName, vecStrPropNameUpdate, vecAttrType, vecUpdateDataSet, vecCellAfterUpdate, vecStrCoverageID, vecStrShapeID, vecStrFeatureID, vecStrFeaClassID, vecLodSet, vecMajorIDSet, vecMinorIDSet, vecGridInfo))
//	{
//		strMsg = server.EsGetTime() + server.EsGetErrorMsg();
//		cout << strMsg << "\n";
//		server.EsWriteToLog(strLogFile, strMsg);
//		return;
//	}
//	strMsg = server.EsGetTime() + "���µ���״ֻ��ɹ�";
//	server.EsWriteToLog(strLogFile, strMsg);
//
//	/*********************Ϊ����ʴ����������и��µ�������ƥ������ݿ�ģ�ͽ����Զ�����************************/
//	for (size_t idxProp = 0; idxProp < vecStrPropNameUpdate.size(); idxProp++)
//	{
//		strMsg = server.EsGetTime() + "\t���ڸ��µ�" + to_string(idxProp + 1) + "�����ԣ�" + vecStrPropNameUpdate[idxProp] + ",ʣ��" + to_string(vecStrPropNameUpdate.size() - 1 - idxProp) + "�����Դ�����";
//		cout << strMsg << "\n";
//		server.EsWriteToLog(strLogFile, strMsg);
//
//		/*********************��ͬLOD�Զ�����************************/
//		for (size_t idxLod = 0; idxLod < vecLodSet[idxProp].size(); idxLod++)
//		{
//			strMsg = server.EsGetTime() + "\t\t���ڸ��µ�" + to_string(idxLod + 1) + "��LOD��Lod=" + to_string(vecLodSet[idxProp][idxLod]) + ",ʣ��" + to_string(vecLodSet[idxProp].size() - 1 - idxLod) + "��LOD������";
//			cout << strMsg << "\n";
//			server.EsWriteToLog(strLogFile, strMsg);
//
//			/*********************��ͬ���������Զ�����************************/
//			if (vecAttrType[idxProp][idxLod] == Continuous)
//			{
//				/*********************��ͬ��ɸѡ���Կ��Ƹ���************************/
//				for (size_t idxMajor = 0; idxMajor < vecMajorIDSet[idxProp].size(); idxMajor++)
//				{
//					strMsg = server.EsGetTime() + "\t\t\t���ڸ�����ɸѡ����" + strMajorMatchedFieldName + "�ĵ�" + std::to_string(idxMajor + 1) + "�����Ʒ�Χ��" + std::to_string(vecMajorIDSet[idxProp][idxMajor]) + ",ʣ��" + std::to_string(vecMajorIDSet[idxProp].size() - 1 - idxMajor) + "�������Ʒ�Χ������";
//					cout << strMsg << "\n";
//					server.EsWriteToLog(strLogFile, strMsg);
//
//					/*********************��ͬ��ɸѡ���Կ��Ƹ���************************/
//					for (size_t idxMinor = 0; idxMinor < vecMinorIDSet[idxProp][idxMajor].size(); idxMinor++)
//					{
//						strMsg = server.EsGetTime() + "\t\t\t\t���ڸ��´�ɸѡ����" + strMinorMatchedFieldName + "�ĵ�" + std::to_string(idxMinor + 1) + "�����Ʒ�Χ��" + std::to_string(vecMinorIDSet[idxProp][idxMajor][idxMinor]) + ",ʣ��" + std::to_string(vecMinorIDSet[idxProp][idxMajor].size() - 1 - idxMinor) + "���ο��Ʒ�Χ������";
//						cout << strMsg << "\n";
//						server.EsWriteToLog(strLogFile, strMsg);
//
//						//�����ɸѡ���Կ��Ʒ�Χ��û�и��µ�������������
//						if (vecUpdateDataSet[idxProp][idxLod][idxMajor][idxMinor].empty())
//						{
//							strMsg = server.EsGetTime() + "\t\t\t\t��ɸѡ����" + strMinorMatchedFieldName + "�Ŀ��Ʒ�Χ��" + std::to_string(vecMinorIDSet[idxProp][idxMajor][idxMinor]) + "û�и��µ���ף�������";
//							cout << strMsg << "\n";
//							server.EsWriteToLog(strLogFile, strMsg);
//
//							continue;
//						}
//
//						//�����µ����������ϴ������ݿ�
//						strMsg = server.EsGetTime() + "\t\t\t\t���ڽ����µ�����ϴ������ݿ�...";
//						cout << strMsg << "\n";
//						server.EsWriteToLog(strLogFile, strMsg);
//
//						if (!server.EsUpLoadData(vecStrPropNameUpdate[idxProp], vecAttrType[idxProp][idxLod], vecUpdateDataSet[idxProp][idxLod][idxMajor][idxMinor], strRootURL, vecStrShapeID[idxProp][idxLod], vecStrCoverageID[idxProp][idxLod], vecStrFeaClassID[idxProp][idxLod], vecLodSet[idxProp][idxLod]))
//						{
//							strMsg = server.EsGetTime() + "\t\t\t\t" + server.EsGetErrorMsg() + " ���󣺴�ɸѡ����" + strMinorMatchedFieldName + "�Ŀ��Ʒ�Χ��" + std::to_string(vecMinorIDSet[idxProp][idxMajor][idxMinor]) + "���µ�����޷����µ����ݿ⣬����Χ�ĸ��½�������";
//							cout << strMsg << "\n";
//							server.EsWriteToLog(strLogFile, strMsg);
//
//							continue;
//						}
//
//						//��ȡĿ������ı�������ת��Ϊ��������
//						const string strMajorID = to_string(vecMajorIDSet[idxProp][idxMajor]);
//						const string strMinorID = to_string(vecMinorIDSet[idxProp][idxMajor][idxMinor]);
//						VariogramPara varPara;
//						if (!server.EsReadServerConfigVariogram(strConfigPath, vecStrPropNameUpdate[idxProp], vecLodSet[idxProp][idxLod], strMajorID, strMinorID, varPara))
//						{
//							strMsg = server.EsGetTime() + "\t\t\t\t���棺��ɸѡ����" + strMinorMatchedFieldName + "�Ŀ��Ʒ�Χ��" + std::to_string(vecMinorIDSet[idxProp][idxMajor][idxMinor]) + "�ı�����ȡʧ�ܣ���ʹ��Ĭ�ϱ����м���";
//							cout << strMsg << "\n";
//							server.EsWriteToLog(strLogFile, strMsg);
//						}
//
//						int rangeI = ceil(varPara.dMajorRange / vecGridInfo[idxProp][idxLod].dXStep); //����������I������������
//						int rangeJ = ceil(varPara.dMinorRange / vecGridInfo[idxProp][idxLod].dYStep); //����������J������������
//						int rangeK = ceil(varPara.dVerticalRange / vecGridInfo[idxProp][idxLod].dZStep); //����������K������������
//
//						//ʹ�����������Ը��µ����ݽ���Ӱ�췶Χ����
//						strMsg = server.EsGetTime() + "\t\t\t\t���µ�����ϴ������ݿ�ɹ��������������µ���׵�Ӱ�췶Χ...";
//						cout << strMsg << "\n";
//						server.EsWriteToLog(strLogFile, strMsg);
//
//						vector<GridInfo> vecGIforSampleDownload; //�������ط�Χ��
//						vector<GridInfo> vecGIforUpdate; //���巶Χ��				
//						if (!server.EsEvaluate(vecUpdateDataSet[idxProp][idxLod][idxMajor][idxMinor], rangeI, rangeJ, rangeK, vecGridInfo[idxProp][idxLod], vecGIforSampleDownload, vecGIforUpdate))
//						{
//							strMsg = server.EsGetTime() + "\t\t\t\t���󣺴�ɸѡ����" + strMinorMatchedFieldName + "�Ŀ��Ʒ�Χ��" + std::to_string(vecMinorIDSet[idxProp][idxMajor][idxMinor]) + "�ĸ��·�Χ����ʧ�ܣ�����Χ�ĸ��½�������";
//							cout << strMsg << "\n";
//							server.EsWriteToLog(strLogFile, strMsg);
//
//							continue;
//						}
//
//						if (vecGIforSampleDownload.size() != vecGIforUpdate.size())
//						{
//							strMsg = server.EsGetTime() + "\t\t\t\t���󣺴�ɸѡ����" + strMinorMatchedFieldName + "�Ŀ��Ʒ�Χ��" + std::to_string(vecMinorIDSet[idxProp][idxMajor][idxMinor]) + "�ĸ��·�Χ����ʧ�ܣ�����Χ�ĸ��½�������";
//							cout << strMsg << "\n";
//							server.EsWriteToLog(strLogFile, strMsg);
//
//							continue;
//						}
//
//						strMsg = server.EsGetTime() + "\t\t\t\t��ɸѡ����" + strMinorMatchedFieldName + "�Ŀ��Ʒ�Χ��" + std::to_string(vecMinorIDSet[idxProp][idxMajor][idxMinor]) + "�ĸ��·�Χ�����ɹ�������" + std::to_string(vecGIforUpdate.size()) + "�������·�Χ";
//						cout << strMsg << "\n";
//						server.EsWriteToLog(strLogFile, strMsg);
//
//						//�������ļ���ȡ�������ֵʹ�õķ���
//						InterMethod nInterMethod;
//						if (!server.EsReadServerConfigMinor(strConfigPath, vecStrPropNameUpdate[idxProp], vecLodSet[idxProp][idxLod], strMajorID, strMinorID, nInterMethod))
//						{
//							strMsg = server.EsGetTime() + "\t\t\t\t���󣺴������ļ��л�ȡ��ɸѡ����" + strMinorMatchedFieldName + "�Ŀ��Ʒ�Χ��" + std::to_string(vecMinorIDSet[idxProp][idxMajor][idxMinor]) + "�Ĳ�ֵ�㷨ʧ�ܣ�����Χ�ĸ��½�������";
//							cout << strMsg << "\n";
//							server.EsWriteToLog(strLogFile, strMsg);
//
//							continue;
//						}
//
//						/*********************��ÿ������������и���************************/
//						for (size_t idxArea = 0; idxArea < vecGIforUpdate.size(); idxArea++)
//						{
//							strMsg = server.EsGetTime() + "\t\t\t\t\t����ִ�е�" + std::to_string(idxArea + 1) + "�����·�Χ�Ĳ�ֵ����,��ֵ��Χ��IOri=" + std::to_string(vecGIforUpdate[idxArea].nIOri) + " JOri=" + std::to_string(vecGIforUpdate[idxArea].nJOri) + " KOri=" + std::to_string(vecGIforUpdate[idxArea].nKOri) +
//								" XNum=" + std::to_string(vecGIforUpdate[idxArea].nXNum) + " YNum=" + std::to_string(vecGIforUpdate[idxArea].nYNum) + " ZNum=" + std::to_string(vecGIforUpdate[idxArea].nZNum) + "��ʣ��" + std::to_string(vecGIforUpdate.size() - 1 - idxArea) + "�����·�Χ����ֵ";
//							cout << strMsg << "\n";
//							server.EsWriteToLog(strLogFile, strMsg);
//
//							//׼����ֵ����
//							ModelInterPara tmip; //��ֵ����
//							tmip.strWorkPath = strTargetDir; //����Ŀ¼
//							tmip.nAttrType = Continuous; //����Ϊ����
//							tmip.gi = vecGIforUpdate[idxArea]; //��������			
//							tmip.nInterMethod = nInterMethod; //��ֵ�㷨
//							tmip.varPara = varPara; //����
//
//							bool bStochastic = false;
//							if (nInterMethod == SequentialGaussianSimulation)
//							{
//								bStochastic = true;
//							}
//
//							//�����ݿ��ȡĿ�귶Χ�ڵ�����	
//							strMsg = server.EsGetTime() + "\t\t\t\t\t�������ز�ֵ�õ���������ȴ�...";
//							cout << strMsg << "\n";
//							server.EsWriteToLog(strLogFile, strMsg);
//
//							if (!server.EsDownLoadData(strTargetDir, vecStrPropNameUpdate[idxProp], vecAttrType[idxProp][idxLod], strMajorMatchedFieldName, strMinorMatchedFieldName, strMajorID, strMinorID, vecGIforSampleDownload[idxArea], 
//								vecGIforUpdate[idxArea], bStochastic, strRootURL, vecLodSet[idxProp][idxLod], vecStrFeaClassID[idxProp][idxLod], vecStrCoverageID[idxProp][idxLod], vecCellAfterUpdate[idxProp][idxLod], tmip.vecCell))
//							{
//								strMsg = server.EsGetTime() + "\t\t\t\t\t" + server.EsGetErrorMsg() + " �������ز�ֵ����ʧ�ܣ������·�Χ�ĸ��½�������";
//								cout << strMsg << "\n";
//								server.EsWriteToLog(strLogFile, strMsg);
//
//								continue;
//							}
//
//							//��������-��ֵ����
//							strMsg = server.EsGetTime() + "\t\t\t\t\t��ֵ�������سɹ������ڽ��в�ֵ����...";
//							cout << strMsg << "\n";
//							server.EsWriteToLog(strLogFile, strMsg);
//
//							if (!server.EsUpdateData(tmip))
//							{
//								strMsg = server.EsGetTime() + "\t\t\t\t\t���󣺲�ֵ����ʧ�ܣ������·�Χ�ĸ��½�������";
//								cout << strMsg << "\n";
//								server.EsWriteToLog(strLogFile, strMsg);
//
//								continue;
//							}
//
//							//�����º����������д�����ݿ�
//							strMsg = server.EsGetTime() + "\t\t\t\t\t��ֵ����ɹ������ڸ��²�ֵ��������ݿ�...";
//							cout << strMsg << "\n";
//							server.EsWriteToLog(strLogFile, strMsg);
//
//							if (!server.EsUpLoadData(vecStrPropNameUpdate[idxProp], vecAttrType[idxProp][idxLod], strMajorMatchedFieldName, strMinorMatchedFieldName, strMajorID, strMinorID, tmip, strRootURL, vecStrShapeID[idxProp][idxLod], vecStrCoverageID[idxProp][idxLod], vecStrFeaClassID[idxProp][idxLod], vecLodSet[idxProp][idxLod]))
//							{
//								strMsg = server.EsGetTime() + "\t\t\t\t\t" + server.EsGetErrorMsg() + " ���󣺲�ֵ������µ����ݿ�ʧ�ܣ������·�Χ�ĸ��½�������";
//								cout << strMsg << "\n";
//								server.EsWriteToLog(strLogFile, strMsg);
//
//								continue;
//							}
//
//							strMsg = server.EsGetTime() + "\t\t\t\t\t��ֵ������µ����ݿ�ɹ�";
//							cout << strMsg << "\n";
//							server.EsWriteToLog(strLogFile, strMsg);
//						}
//					}					
//				}
//			}
//			else if (vecAttrType[idxProp][idxLod] == Discrete)
//			{
//				/*********************��ͬ�ز��Զ�����************************/
//				for (size_t idxMajor = 0; idxMajor < vecMajorIDSet[idxProp].size(); idxMajor++)
//				{
//					strMsg = server.EsGetTime() + "\t\t\t���ڸ�����ɸѡ����" + strMajorMatchedFieldName + "�ĵ�" + std::to_string(idxMajor + 1) + "�����Ʒ�Χ��" + std::to_string(vecMajorIDSet[idxProp][idxMajor]) + ",ʣ��" + std::to_string(vecMajorIDSet[idxProp].size() - 1 - idxMajor) + "�������Ʒ�Χ������";
//					cout << strMsg << "\n";
//					server.EsWriteToLog(strLogFile, strMsg);
//
//					for (size_t idxMinor = 0; idxMinor < vecMinorIDSet[idxProp][idxMajor].size(); idxMinor++)
//					{
//						strMsg = server.EsGetTime() + "\t\t\t\t���ڸ��´�ɸѡ����" + strMinorMatchedFieldName + "�ĵ�" + std::to_string(idxMinor + 1) + "�����Ʒ�Χ��" + std::to_string(vecMinorIDSet[idxProp][idxMajor][idxMinor]) + ",ʣ��" + std::to_string(vecMinorIDSet[idxProp][idxMajor].size() - 1 - idxMinor) + "���ο��Ʒ�Χ������";
//						cout << strMsg << "\n";
//						server.EsWriteToLog(strLogFile, strMsg);
//
//						//����ز�û�и��µ�������������
//						if (vecUpdateDataSet[idxProp][idxLod][idxMajor].empty())
//						{
//							strMsg = server.EsGetTime() + "\t\t\t\t��ɸѡ����" + strMinorMatchedFieldName + "�Ŀ��Ʒ�Χ��" + std::to_string(vecMinorIDSet[idxProp][idxMajor][idxMinor]) + "û�и��µ���ף�������";
//							cout << strMsg << "\n";
//							server.EsWriteToLog(strLogFile, strMsg);
//
//							continue;
//						}
//
//						//�����µ����������ϴ������ݿ�
//						strMsg = server.EsGetTime() + "\t\t\t\t���ڽ����µ�����ϴ������ݿ�...";
//						cout << strMsg << "\n";
//						server.EsWriteToLog(strLogFile, strMsg);
//
//						if (!server.EsUpLoadData(vecStrPropNameUpdate[idxProp], vecAttrType[idxProp][idxLod], vecUpdateDataSet[idxProp][idxLod][idxMajor][idxMinor], strRootURL, vecStrShapeID[idxProp][idxLod], vecStrCoverageID[idxProp][idxLod], vecStrFeaClassID[idxProp][idxLod], vecLodSet[idxProp][idxLod]))
//						{
//							strMsg = server.EsGetTime() + "\t\t\t\t" + server.EsGetErrorMsg() + "���󣺴�ɸѡ����" + strMinorMatchedFieldName + "�Ŀ��Ʒ�Χ��" + std::to_string(vecMinorIDSet[idxProp][idxMajor][idxMinor]) + "���µ�����޷����µ����ݿ⣬����Χ�ĸ��½�������";
//							cout << strMsg << "\n";
//							server.EsWriteToLog(strLogFile, strMsg);
//
//							continue;
//						}
//
//						//��������ı�������ȡ���ֵΪ��������
//						int rangeI = INT_MIN; //����������I������������
//						int rangeJ = INT_MIN; //����������J������������
//						int rangeK = INT_MIN; //����������K������������
//
//						const string strMajorID = to_string(vecMajorIDSet[idxProp][idxMajor]);
//						const string strMinorID = to_string(vecMinorIDSet[idxProp][idxMajor][idxMinor]);
//						vector<int> vecFaceCode;
//						if (!server.EsReadServerConfigFaceCode(strConfigPath, vecStrPropNameUpdate[idxProp], vecLodSet[idxProp][idxLod], strMajorID, strMinorID, vecFaceCode))
//						{
//							strMsg = server.EsGetTime() + "\t\t\t\t���󣺴������ļ��л�ȡ���ԣ�" + vecStrPropNameUpdate[idxProp] + "����ɢ��ʧ�ܣ�����Χ�ĸ��½�������\n";
//							cout << strMsg << "\n";
//							server.EsWriteToLog(strLogFile, strMsg);
//
//							continue;
//						}
//
//						if (vecFaceCode.empty())
//						{
//							strMsg = server.EsGetTime() + "\t\t\t\t���棺���ԣ�" + vecStrPropNameUpdate[idxProp] + "����ɢ������Ϊ0������Χ�ĸ��½�������";
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
//						//�Ը��µ����ݽ���Ӱ�췶Χ����
//						strMsg = server.EsGetTime() + "\t\t\t\t���µ�����ϴ������ݿ�ɹ��������������µ���׵�Ӱ�췶Χ...";
//						cout << strMsg << "\n";
//						server.EsWriteToLog(strLogFile, strMsg);
//						vector<GridInfo> vecGIforSampleDownload; //�������ط�Χ��
//						vector<GridInfo> vecGIforUpdate; //���巶Χ��				
//						if (!server.EsEvaluate(vecUpdateDataSet[idxProp][idxLod][idxMajor][idxMinor], rangeI, rangeJ, rangeK, vecGridInfo[idxProp][idxLod], vecGIforSampleDownload, vecGIforUpdate))
//						{
//							strMsg = server.EsGetTime() + "\t\t\t\t���󣺴�ɸѡ����" + strMinorMatchedFieldName + "�Ŀ��Ʒ�Χ��" + std::to_string(vecMinorIDSet[idxProp][idxMajor][idxMinor]) + "�ĸ��·�Χ����ʧ�ܣ�����Χ�ĸ��½�������";
//							cout << strMsg << "\n";
//							server.EsWriteToLog(strLogFile, strMsg);
//
//							continue;
//						}
//
//						if (vecGIforSampleDownload.size() != vecGIforUpdate.size())
//						{
//							strMsg = server.EsGetTime() + "\t\t\t\t���󣺴�ɸѡ����" + strMinorMatchedFieldName + "�Ŀ��Ʒ�Χ��" + std::to_string(vecMinorIDSet[idxProp][idxMajor][idxMinor]) + "�ĸ��·�Χ����ʧ�ܣ�����Χ�ĸ��½�������";
//							cout << strMsg << "\n";
//							server.EsWriteToLog(strLogFile, strMsg);
//
//							continue;
//						}
//						strMsg = server.EsGetTime() + "\t\t\t\t��ɸѡ����" + strMinorMatchedFieldName + "�Ŀ��Ʒ�Χ��" + std::to_string(vecMinorIDSet[idxProp][idxMajor][idxMinor]) + "�ĸ��·�Χ�����ɹ�������" + std::to_string(vecGIforUpdate.size()) + "�������·�Χ";
//						cout << strMsg << "\n";
//						server.EsWriteToLog(strLogFile, strMsg);
//
//						//�������ļ���ȡ�������ֵʹ�õķ���
//						InterMethod nInterMethod;
//						if (!server.EsReadServerConfigMinor(strConfigPath, vecStrPropNameUpdate[idxProp], vecLodSet[idxProp][idxLod], strMajorID, strMinorID, nInterMethod))
//						{
//							strMsg = server.EsGetTime() + "\t\t\t\t���󣺴������ļ��л�ȡ��ɸѡ����" + strMinorMatchedFieldName + "�Ŀ��Ʒ�Χ��" + std::to_string(vecMinorIDSet[idxProp][idxMajor][idxMinor]) + "�Ĳ�ֵ�㷨ʧ�ܣ�����Χ�ĸ��½�������";
//							cout << strMsg << "\n";
//							server.EsWriteToLog(strLogFile, strMsg);
//
//							continue;
//						}
//
//						/*********************��ÿ������������и���************************/
//						for (size_t idxArea = 0; idxArea < vecGIforUpdate.size(); idxArea++)
//						{
//							strMsg = server.EsGetTime() + "\t\t\t\t\t����ִ�е�" + std::to_string(idxArea + 1) + "�����·�Χ�Ĳ�ֵ����,��ֵ��Χ��IOri=" + std::to_string(vecGIforUpdate[idxArea].nIOri) + " JOri=" + std::to_string(vecGIforUpdate[idxArea].nJOri) + " KOri=" + std::to_string(vecGIforUpdate[idxArea].nKOri) +
//								" XNum=" + std::to_string(vecGIforUpdate[idxArea].nXNum) + " YNum=" + std::to_string(vecGIforUpdate[idxArea].nYNum) + " ZNum=" + std::to_string(vecGIforUpdate[idxArea].nZNum) + "��ʣ��" + std::to_string(vecGIforUpdate.size() - 1 - idxArea) + "�����·�Χ����ֵ";
//							cout << strMsg << "\n";
//							server.EsWriteToLog(strLogFile, strMsg);
//
//							//׼����ֵ����
//							ModelInterPara tmip; //��ֵ����
//							tmip.strWorkPath = strTargetDir; //����Ŀ¼
//							tmip.nAttrType = Discrete; //����Ϊ����
//							tmip.gi = vecGIforUpdate[idxArea]; //��������			
//							tmip.nInterMethod = nInterMethod; //��ֵ�㷨
//
//															  //������ɢ���Ա���
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
//							//��������ѡȡĿ�귶Χ�ڵ�����
//							strMsg = server.EsGetTime() + "\t\t\t\t\t�������ز�ֵ�õ���������ȴ�...";
//							cout << strMsg << "\n";
//							server.EsWriteToLog(strLogFile, strMsg);
//							if (!server.EsDownLoadData(strTargetDir, vecStrPropNameUpdate[idxProp], vecAttrType[idxProp][idxLod], strMajorMatchedFieldName, strMinorMatchedFieldName, strMajorID, strMinorID, vecGIforSampleDownload[idxArea],
//								vecGIforUpdate[idxArea], bStochastic, strRootURL, vecLodSet[idxProp][idxLod], vecStrFeaClassID[idxProp][idxLod], vecStrCoverageID[idxProp][idxLod], vecCellAfterUpdate[idxProp][idxLod], tmip.vecCell))
//							{
//								strMsg = server.EsGetTime() + "\t\t\t\t\t" + server.EsGetErrorMsg() + " �������ز�ֵ����ʧ�ܣ������·�Χ�ĸ��½�������";
//								cout << strMsg << "\n";
//								server.EsWriteToLog(strLogFile, strMsg);
//
//								continue;
//							}
//
//							//��������-��ֵ����
//							strMsg = server.EsGetTime() + "\t\t\t\t\t��ֵ�������سɹ������ڽ��в�ֵ����...";
//							cout << strMsg << "\n";
//							server.EsWriteToLog(strLogFile, strMsg);
//							if (!server.EsUpdateData(tmip))
//							{
//								strMsg = server.EsGetTime() + "\t\t\t\t\t���󣺲�ֵ����ʧ�ܣ������·�Χ�ĸ��½�������";
//								cout << strMsg << "\n";
//								server.EsWriteToLog(strLogFile, strMsg);
//
//								continue;
//							}
//
//							//�����º����������д�����ݿ�
//							strMsg = server.EsGetTime() + "\t\t\t\t\t��ֵ����ɹ������ڸ��²�ֵ��������ݿ�...";
//							cout << strMsg << "\n";
//							server.EsWriteToLog(strLogFile, strMsg);
//							if (!server.EsUpLoadData(vecStrPropNameUpdate[idxProp], vecAttrType[idxProp][idxLod], strMajorMatchedFieldName, strMinorMatchedFieldName, strMajorID, strMinorID, tmip, strRootURL, vecStrShapeID[idxProp][idxLod], vecStrCoverageID[idxProp][idxLod], vecStrFeaClassID[idxProp][idxLod], vecLodSet[idxProp][idxLod]))
//							{
//								strMsg = server.EsGetTime() + "\t\t\t\t\t" + server.EsGetErrorMsg() + " ���󣺲�ֵ������µ����ݿ�ʧ�ܣ������·�Χ�ĸ��½�������";
//								cout << strMsg << "\n";
//								server.EsWriteToLog(strLogFile, strMsg);
//
//								continue;
//							}
//							strMsg = server.EsGetTime() + "\t\t\t\t\t��ֵ������µ����ݿ�ɹ�";
//							cout << strMsg << "\n";
//							server.EsWriteToLog(strLogFile, strMsg);
//						}
//					}				
//				}
//			}
//		}
//	}
//
//	strMsg = server.EsGetTime() + "*********************�Զ����½���������ر�************************\n";
//	cout << strMsg << "\n";
//	server.EsWriteToLog(strLogFile, strMsg);
//}