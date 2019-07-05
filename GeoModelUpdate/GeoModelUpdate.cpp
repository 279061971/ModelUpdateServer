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
		cout << "����û���������\n";
		return 1;
	}
	const string strTargetDir = argv[1];

	CModelUpdateServerGS server; //�������·���
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
//	string strMatchedFieldName = ""; //���������ز�ID�����е�������
//	const string strConfigPath = strTargetDir + "\\" + strMatchedDataCenter_Config; //�Զ����·��������ļ�
//	const string strLogFile = strTargetDir + "\\log.txt"; //��־�ļ�
//
//	CModelUpdateServerGS server; //�������·���
//	if (!server.EsReadServerConfigSetting(strConfigPath, strRootURL, strMatchedFieldName))
//	{
//		strMsg = server.EsGetTime() + " " + server.EsGetMsg();
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
//		strMsg = server.EsGetTime() + server.EsGetMsg();
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
//	vector<vector<AttrType>> vecAttrType; //���µ��������ͼ�         ����/Lod
//	vector<vector<GridInfo>> vecGridInfo; //���Զ�Ӧ��������Ϣ��     ����/Lod
//	vector<vector<string>> vecStrCoverageID; //���Զ�Ӧ�����Գ�ID��  ����/Lod
//	vector<vector<string>> vecStrShapeID; //���Զ�Ӧ�ļ��γ�ID��     ����/Lod
//	vector<vector<string>> vecStrFeatureID; //���Զ�Ӧ��Ҫ��ID��     ����/Lod
//	vector<vector<string>> vecStrFeaClassID; //���Զ�Ӧ��Ҫ����ID��  ����/Lod
//	vector<vector<int>> vecLayerIDSet; //���Զ�Ӧ�ĵز��ID��        ����/LayerId
//	vector<vector<int>> vecLodSet; //���Զ�Ӧ�ĵز��ID��            ����/Lod
//	vector<vector<vector<vector<Cell>>>> vecUpdateDataSet; //���µ���������    ����/Lod/�ز�
//	vector<vector<vector<Cell>>> vecCellBeforeUpdate;      //����ǰ����������  ����/Lod
//	vector<vector<vector<Cell>>> vecCellAfterUpdate;       //���º����������  ����/Lod
//	if (!server.EsGetUpdatedSampleData(strTargetDir, strConfigPath, strRootURL, strMatchedFieldName, vecStrPropNameUpdate, vecAttrType, vecUpdateDataSet, vecCellAfterUpdate, vecStrCoverageID, vecStrShapeID, vecStrFeatureID, vecStrFeaClassID, vecLodSet, vecLayerIDSet, vecGridInfo))
//	{
//		strMsg = server.EsGetTime() + server.EsGetMsg();
//		cout << strMsg << "\n";
//		server.EsWriteToLog(strLogFile, strMsg);
//		return;
//	}
//	
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
//				/*********************��ͬ�ز��Զ�����************************/
//				for (size_t idxLayer = 0; idxLayer < vecLayerIDSet[idxProp].size(); idxLayer++)
//				{				
//					strMsg = server.EsGetTime() + "\t\t\t���ڸ��µ�" + std::to_string(idxLayer + 1) + "���ز㣺" + std::to_string(vecLayerIDSet[idxProp][idxLayer]) + ",ʣ��" + std::to_string(vecLayerIDSet[idxProp].size() - 1 - idxLayer) + "���ز������";
//					cout << strMsg << "\n";
//					server.EsWriteToLog(strLogFile, strMsg);
//
//					//����ز�û�и��µ�������������
//					if (vecUpdateDataSet[idxProp][idxLod][idxLayer].empty())
//					{					
//						strMsg = server.EsGetTime() + "\t\t\t�ز㣺" + std::to_string(vecLayerIDSet[idxProp][idxLayer]) + "û�и��µ���ף�������";
//						cout << strMsg << "\n";
//						server.EsWriteToLog(strLogFile, strMsg);
//
//						continue;
//					}
//
//					//�����µ����������ϴ������ݿ�
//					strMsg = server.EsGetTime() + "\t\t\t���ڽ����µ�����ϴ������ݿ�...";
//					cout << strMsg << "\n";
//					server.EsWriteToLog(strLogFile, strMsg);
//
//					if (!server.EsUpLoadData(vecStrPropNameUpdate[idxProp], vecAttrType[idxProp][idxLod], vecUpdateDataSet[idxProp][idxLod][idxLayer], strRootURL, vecStrShapeID[idxProp][idxLod], vecStrCoverageID[idxProp][idxLod], vecStrFeaClassID[idxProp][idxLod], vecLodSet[idxProp][idxLod]))
//					{
//						strMsg = server.EsGetTime() + "\t\t\t" + server.EsGetMsg() + " ���󣺵ز㣺" + std::to_string(vecLayerIDSet[idxProp][idxLayer]) + "���µ�����޷����µ����ݿ⣬���ز�ĸ��½�������";
//						cout << strMsg << "\n";
//						server.EsWriteToLog(strLogFile, strMsg);
//
//						continue;
//					}
//
//					//��ȡĿ������ı�������ת��Ϊ��������
//					const string strLayerID = to_string(vecLayerIDSet[idxProp][idxLayer]);
//					VariogramPara varPara;
//					if (!server.EsReadServerConfigVariogram(strConfigPath, vecStrPropNameUpdate[idxProp], vecLodSet[idxProp][idxLod], strLayerID, varPara))
//					{
//						strMsg = server.EsGetTime() + "\t\t\t���棺�ز㣺" + std::to_string(vecLayerIDSet[idxProp][idxLayer]) + "�ı�����ȡʧ�ܣ���ʹ��Ĭ�ϱ����м���";
//						cout << strMsg << "\n";
//						server.EsWriteToLog(strLogFile, strMsg);
//					}
//
//					int rangeI = ceil(varPara.dMajorRange / vecGridInfo[idxProp][idxLod].dXStep); //����������I������������
//					int rangeJ = ceil(varPara.dMinorRange / vecGridInfo[idxProp][idxLod].dYStep); //����������J������������
//					int rangeK = ceil(varPara.dVerticalRange / vecGridInfo[idxProp][idxLod].dZStep); //����������K������������
//
//					//ʹ�����������Ը��µ����ݽ���Ӱ�췶Χ����
//					strMsg = server.EsGetTime() + "\t\t\t���µ�����ϴ������ݿ�ɹ��������������µ���׵�Ӱ�췶Χ...";
//					cout << strMsg << "\n";
//					server.EsWriteToLog(strLogFile, strMsg);
//
//					vector<GridInfo> vecGIforSampleDownload; //�������ط�Χ��
//					vector<GridInfo> vecGIforUpdate; //���巶Χ��				
//					if (!server.EsEvaluate(vecUpdateDataSet[idxProp][idxLod][idxLayer], rangeI, rangeJ, rangeK, vecGridInfo[idxProp][idxLod], vecGIforSampleDownload, vecGIforUpdate))
//					{
//						strMsg = server.EsGetTime() + "\t\t\t���󣺵ز㣺" + std::to_string(vecLayerIDSet[idxProp][idxLayer]) + "�ĸ��·�Χ����ʧ�ܣ����ز�ĸ��½�������";
//						cout << strMsg << "\n";
//						server.EsWriteToLog(strLogFile, strMsg);
//
//						continue;
//					}
//
//					if (vecGIforSampleDownload.size() != vecGIforUpdate.size())
//					{
//						strMsg = server.EsGetTime() + "\t\t\t���󣺵ز㣺" + std::to_string(vecLayerIDSet[idxProp][idxLayer]) + "�ĸ��·�Χ����ʧ�ܣ����ز�ĸ��½�������";
//						cout << strMsg << "\n";
//						server.EsWriteToLog(strLogFile, strMsg);
//
//						continue;
//					}
//
//					strMsg = server.EsGetTime() + "\t\t\t�ز㣺" + std::to_string(vecLayerIDSet[idxProp][idxLayer]) + "�ĸ��·�Χ�����ɹ�������" + std::to_string(vecGIforUpdate.size()) + "�������·�Χ";
//					cout << strMsg << "\n";
//					server.EsWriteToLog(strLogFile, strMsg);
//
//					//д���ܸ�����Ӱ����������Ե�ԭʼ���
//					server.EsExportOriginDataOfOtherPropsEffectedByCurProp(strConfigPath, strTargetDir, vecStrPropNameUpdate[idxProp], strLayerID, vecLodSet[idxProp][idxLod], vecGridInfo[idxProp][idxLod], vecGIforUpdate);
//
//					//�������ļ���ȡ�������ֵʹ�õķ���
//					InterMethod nInterMethod;
//					if (!server.EsReadServerConfigLayer(strConfigPath, vecStrPropNameUpdate[idxProp], vecLodSet[idxProp][idxLod], strLayerID, nInterMethod))
//					{
//						strMsg = server.EsGetTime() + "\t\t\t���󣺴������ļ��л�ȡ�ز㣺" + std::to_string(vecLayerIDSet[idxProp][idxLayer]) + "�Ĳ�ֵ�㷨ʧ�ܣ����ز�ĸ��½�������";
//						cout << strMsg << "\n";
//						server.EsWriteToLog(strLogFile, strMsg);
//
//						continue;
//					}
//
//					/*********************��ÿ������������и���************************/
//					for (size_t idxArea = 0; idxArea < vecGIforUpdate.size(); idxArea++)
//					{
//						strMsg = server.EsGetTime() + "\t\t\t\t����ִ�е�" + std::to_string(idxArea + 1) + "�����·�Χ�Ĳ�ֵ����,��ֵ��Χ��IOri=" + std::to_string(vecGIforUpdate[idxArea].nIOri) + " JOri=" + std::to_string(vecGIforUpdate[idxArea].nJOri) + " KOri=" + std::to_string(vecGIforUpdate[idxArea].nKOri) +
//							" XNum=" + std::to_string(vecGIforUpdate[idxArea].nXNum) + " YNum=" + std::to_string(vecGIforUpdate[idxArea].nYNum) + " ZNum=" + std::to_string(vecGIforUpdate[idxArea].nZNum) + "��ʣ��" + std::to_string(vecGIforUpdate.size() - 1 - idxArea) + "�����·�Χ����ֵ";
//						cout << strMsg << "\n";
//						server.EsWriteToLog(strLogFile, strMsg);
//
//						//׼����ֵ����
//						ModelInterPara tmip; //��ֵ����
//						tmip.strWorkPath = strTargetDir; //����Ŀ¼
//						tmip.nAttrType = Continuous; //����Ϊ����
//						tmip.gi = vecGIforUpdate[idxArea]; //��������			
//						tmip.nInterMethod = nInterMethod; //��ֵ�㷨
//						tmip.varPara = varPara; //����
//
//						bool bStochastic = false;
//						if (nInterMethod == SequentialGaussianSimulation)
//						{
//							bStochastic = true;
//						}
//
//						//�����ݿ��ȡĿ�귶Χ�ڵ�����	
//						strMsg = server.EsGetTime() + "\t\t\t\t�������ز�ֵ�õ���������ȴ�...";
//						cout << strMsg << "\n";
//						server.EsWriteToLog(strLogFile, strMsg);
//
//						if (!server.EsDownLoadData(strTargetDir, vecStrPropNameUpdate[idxProp], vecAttrType[idxProp][idxLod], strMatchedFieldName, strLayerID, vecGIforSampleDownload[idxArea], vecGIforUpdate[idxArea],
//							bStochastic, strRootURL, vecLodSet[idxProp][idxLod], vecStrFeaClassID[idxProp][idxLod], vecStrCoverageID[idxProp][idxLod], vecCellAfterUpdate[idxProp][idxLod], tmip.vecCell))
//						{
//							strMsg = server.EsGetTime() + "\t\t\t\t" + server.EsGetMsg() + " �������ز�ֵ����ʧ�ܣ������·�Χ�ĸ��½�������";
//							cout << strMsg << "\n";
//							server.EsWriteToLog(strLogFile, strMsg);
//
//							continue;
//						}
//
//						//��������-��ֵ����
//						strMsg = server.EsGetTime() + "\t\t\t\t��ֵ�������سɹ������ڽ��в�ֵ����...";
//						cout << strMsg << "\n";
//						server.EsWriteToLog(strLogFile, strMsg);
//
//						if (!server.EsUpdateData(tmip))
//						{
//							strMsg = server.EsGetTime() + "\t\t\t\t���󣺲�ֵ����ʧ�ܣ������·�Χ�ĸ��½�������";
//							cout << strMsg << "\n";
//							server.EsWriteToLog(strLogFile, strMsg);
//
//							continue;
//						}
//
//						//�����º����������д�����ݿ�
//						strMsg = server.EsGetTime() + "\t\t\t\t��ֵ����ɹ������ڸ��²�ֵ��������ݿ�...";
//						cout << strMsg << "\n";
//						server.EsWriteToLog(strLogFile, strMsg);
//
//						if (!server.EsUpLoadData(vecStrPropNameUpdate[idxProp], vecAttrType[idxProp][idxLod], strMatchedFieldName, strLayerID, tmip, strRootURL, vecStrShapeID[idxProp][idxLod], vecStrCoverageID[idxProp][idxLod], vecStrFeaClassID[idxProp][idxLod], vecLodSet[idxProp][idxLod]))
//						{
//							strMsg = server.EsGetTime() + "\t\t\t\t" + server.EsGetMsg() + " ���󣺲�ֵ������µ����ݿ�ʧ�ܣ������·�Χ�ĸ��½�������";
//							cout << strMsg << "\n";
//							server.EsWriteToLog(strLogFile, strMsg);
//
//							continue;
//						}
//
//						strMsg = server.EsGetTime() + "\t\t\t\t��ֵ������µ����ݿ�ɹ�";
//						cout << strMsg << "\n";
//						server.EsWriteToLog(strLogFile, strMsg);
//					}
//				}
//			}
//			else if (vecAttrType[idxProp][idxLod] == Discrete)
//			{
//				/*********************��ͬ�ز��Զ�����************************/
//				for (size_t idxLayer = 0; idxLayer < vecLayerIDSet[idxProp].size(); idxLayer++)
//				{
//					strMsg = server.EsGetTime() + "\t\t\t���ڸ��µ�" + std::to_string(idxLayer + 1) + "���ز㣺" + std::to_string(vecLayerIDSet[idxProp][idxLayer]) + ",ʣ��" + std::to_string(vecLayerIDSet[idxProp].size() - 1 - idxLayer) + "���ز������";
//					cout << strMsg << "\n";
//					server.EsWriteToLog(strLogFile, strMsg);
//
//					//����ز�û�и��µ�������������
//					if (vecUpdateDataSet[idxProp][idxLod][idxLayer].empty())
//					{
//						strMsg = server.EsGetTime() + "\t\t\t�ز㣺" + std::to_string(vecLayerIDSet[idxProp][idxLayer]) + "û�и��µ���ף�������";
//						cout << strMsg << "\n";
//						server.EsWriteToLog(strLogFile, strMsg);
//
//						continue;
//					}
//
//					//�����µ����������ϴ������ݿ�
//					strMsg = server.EsGetTime() + "\t\t\t���ڽ����µ�����ϴ������ݿ�...";
//					cout << strMsg << "\n";
//					server.EsWriteToLog(strLogFile, strMsg);
//
//					if (!server.EsUpLoadData(vecStrPropNameUpdate[idxProp], vecAttrType[idxProp][idxLod], vecUpdateDataSet[idxProp][idxLod][idxLayer], strRootURL, vecStrShapeID[idxProp][idxLod], vecStrCoverageID[idxProp][idxLod], vecStrFeaClassID[idxProp][idxLod], vecLodSet[idxProp][idxLod]))
//					{
//						strMsg = server.EsGetTime() + "\t\t\t" + server.EsGetMsg() + " ���󣺵ز㣺" + std::to_string(vecLayerIDSet[idxProp][idxLayer]) + "���µ�����޷����µ����ݿ⣬���ز�ĸ��½�������";
//						cout << strMsg << "\n";
//						server.EsWriteToLog(strLogFile, strMsg);
//					
//						continue;
//					}
//
//					//��������ı�������ȡ���ֵΪ��������
//					int rangeI = INT_MIN; //����������I������������
//					int rangeJ = INT_MIN; //����������J������������
//					int rangeK = INT_MIN; //����������K������������
//
//					const string strLayerID = to_string(vecLayerIDSet[idxProp][idxLayer]);
//					vector<int> vecFaceCode;
//					if (!server.EsReadServerConfigFaceCode(strConfigPath, vecStrPropNameUpdate[idxProp], vecLodSet[idxProp][idxLod], strLayerID, vecFaceCode))
//					{
//						strMsg = server.EsGetTime() + "\t\t\t���󣺴������ļ��л�ȡ���ԣ�" + vecStrPropNameUpdate[idxProp] + "����ɢ��ʧ�ܣ����ز�ĸ��½�������\n";
//						cout << strMsg << "\n";
//						server.EsWriteToLog(strLogFile, strMsg);
//
//						continue;
//					}
//
//					if (vecFaceCode.empty())
//					{
//						strMsg = server.EsGetTime() + "\t\t\t���棺���ԣ�" + vecStrPropNameUpdate[idxProp] + "����ɢ������Ϊ0�����ز�ĸ��½�������";
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
//					//�Ը��µ����ݽ���Ӱ�췶Χ����
//					strMsg = server.EsGetTime() + "\t\t\t���µ�����ϴ������ݿ�ɹ��������������µ���׵�Ӱ�췶Χ...";
//					cout << strMsg << "\n";
//					server.EsWriteToLog(strLogFile, strMsg);
//					vector<GridInfo> vecGIforSampleDownload; //�������ط�Χ��
//					vector<GridInfo> vecGIforUpdate; //���巶Χ��				
//					if (!server.EsEvaluate(vecUpdateDataSet[idxProp][idxLod][idxLayer], rangeI, rangeJ, rangeK, vecGridInfo[idxProp][idxLod], vecGIforSampleDownload, vecGIforUpdate))
//					{
//						strMsg = server.EsGetTime() + "\t\t\t���󣺵ز㣺" + std::to_string(vecLayerIDSet[idxProp][idxLayer]) + "�ĸ��·�Χ����ʧ�ܣ����ز�ĸ��½�������";
//						cout << strMsg << "\n";
//						server.EsWriteToLog(strLogFile, strMsg);
//
//						continue;
//					}
//
//					if (vecGIforSampleDownload.size() != vecGIforUpdate.size())
//					{
//						strMsg = server.EsGetTime() + "\t\t\t���󣺵ز㣺" + std::to_string(vecLayerIDSet[idxProp][idxLayer]) + "�ĸ��·�Χ����ʧ�ܣ����ز�ĸ��½�������";
//						cout << strMsg << "\n";
//						server.EsWriteToLog(strLogFile, strMsg);
//
//						continue;
//					}
//					strMsg = server.EsGetTime() + "\t\t\t�ز㣺" + std::to_string(vecLayerIDSet[idxProp][idxLayer]) + "�ĸ��·�Χ�����ɹ�������" + std::to_string(vecGIforUpdate.size()) + "�������·�Χ";
//					cout << strMsg << "\n";
//					server.EsWriteToLog(strLogFile, strMsg);
//
//					//д���ܸ�����Ӱ����������Ե�ԭʼ���
//					server.EsExportOriginDataOfOtherPropsEffectedByCurProp(strConfigPath, strTargetDir, vecStrPropNameUpdate[idxProp], strLayerID, vecLodSet[idxProp][idxLod], vecGridInfo[idxProp][idxLod], vecGIforUpdate);
//
//					//�������ļ���ȡ�������ֵʹ�õķ���
//					InterMethod nInterMethod;
//					if (!server.EsReadServerConfigLayer(strConfigPath, vecStrPropNameUpdate[idxProp], vecLodSet[idxProp][idxLod], strLayerID, nInterMethod))
//					{
//						strMsg = server.EsGetTime() + "\t\t\t���󣺴������ļ��л�ȡ�ز㣺" + std::to_string(vecLayerIDSet[idxProp][idxLayer]) + "�Ĳ�ֵ�㷨ʧ�ܣ����ز�ĸ��½�������";
//						cout << strMsg << "\n";
//						server.EsWriteToLog(strLogFile, strMsg);
//
//						continue;
//					}
//
//					/*********************��ÿ������������и���************************/
//					for (size_t idxArea = 0; idxArea < vecGIforUpdate.size(); idxArea++)
//					{
//						strMsg = server.EsGetTime() + "\t\t\t\t����ִ�е�" + std::to_string(idxArea + 1) + "�����·�Χ�Ĳ�ֵ����,��ֵ��Χ��IOri=" + std::to_string(vecGIforUpdate[idxArea].nIOri) + " JOri=" + std::to_string(vecGIforUpdate[idxArea].nJOri) + " KOri=" + std::to_string(vecGIforUpdate[idxArea].nKOri) +
//							" XNum=" + std::to_string(vecGIforUpdate[idxArea].nXNum) + " YNum=" + std::to_string(vecGIforUpdate[idxArea].nYNum) + " ZNum=" + std::to_string(vecGIforUpdate[idxArea].nZNum) + "��ʣ��" + std::to_string(vecGIforUpdate.size() - 1 - idxArea) + "�����·�Χ����ֵ";
//						cout << strMsg << "\n";
//						server.EsWriteToLog(strLogFile, strMsg);
//
//						//׼����ֵ����
//						ModelInterPara tmip; //��ֵ����
//						tmip.strWorkPath = strTargetDir; //����Ŀ¼
//						tmip.nAttrType = Discrete; //����Ϊ����
//						tmip.gi = vecGIforUpdate[idxArea]; //��������			
//						tmip.nInterMethod = nInterMethod; //��ֵ�㷨
//
//						//������ɢ���Ա���
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
//						//��������ѡȡĿ�귶Χ�ڵ�����
//						strMsg = server.EsGetTime() + "\t\t\t\t�������ز�ֵ�õ���������ȴ�...";
//						cout << strMsg << "\n";
//						server.EsWriteToLog(strLogFile, strMsg);
//						if (!server.EsDownLoadData(strTargetDir, vecStrPropNameUpdate[idxProp], vecAttrType[idxProp][idxLod], strMatchedFieldName, strLayerID, vecGIforSampleDownload[idxArea], vecGIforUpdate[idxArea],
//							bStochastic, strRootURL, vecLodSet[idxProp][idxLod], vecStrFeaClassID[idxProp][idxLod], vecStrCoverageID[idxProp][idxLod], vecCellAfterUpdate[idxProp][idxLod], tmip.vecCell))
//						{
//							strMsg = server.EsGetTime() + "\t\t\t\t" + server.EsGetMsg() + " �������ز�ֵ����ʧ�ܣ������·�Χ�ĸ��½�������";
//							cout << strMsg << "\n";
//							server.EsWriteToLog(strLogFile, strMsg);
//
//							continue;
//						}
//
//						//��������-��ֵ����
//						strMsg = server.EsGetTime() + "\t\t\t\t��ֵ�������سɹ������ڽ��в�ֵ����...";
//						cout << strMsg << "\n";
//						server.EsWriteToLog(strLogFile, strMsg);
//						if (!server.EsUpdateData(tmip))
//						{
//							strMsg = server.EsGetTime() + "\t\t\t\t���󣺲�ֵ����ʧ�ܣ������·�Χ�ĸ��½�������";
//							cout << strMsg << "\n";
//							server.EsWriteToLog(strLogFile, strMsg);
//
//							continue;
//						}
//
//						//�����º����������д�����ݿ�
//						strMsg = server.EsGetTime() + "\t\t\t\t��ֵ����ɹ������ڸ��²�ֵ��������ݿ�...";
//						cout << strMsg << "\n";
//						server.EsWriteToLog(strLogFile, strMsg);
//						if (!server.EsUpLoadData(vecStrPropNameUpdate[idxProp], vecAttrType[idxProp][idxLod], strMatchedFieldName, strLayerID, tmip, strRootURL, vecStrShapeID[idxProp][idxLod], vecStrCoverageID[idxProp][idxLod], vecStrFeaClassID[idxProp][idxLod], vecLodSet[idxProp][idxLod]))
//						{
//							strMsg = server.EsGetTime() + "\t\t\t\t" + server.EsGetMsg() + " ���󣺲�ֵ������µ����ݿ�ʧ�ܣ������·�Χ�ĸ��½�������";
//							cout << strMsg << "\n";
//							server.EsWriteToLog(strLogFile, strMsg);
//
//							continue;
//						}
//						strMsg = server.EsGetTime() + "\t\t\t\t��ֵ������µ����ݿ�ɹ�";
//						cout << strMsg << "\n";
//						server.EsWriteToLog(strLogFile, strMsg);
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