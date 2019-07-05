#include <iostream>
#include <fstream>
#include <iomanip>
#include "EsModelInterToolDll.h"
#include "math.h"

//#define SGSIM
//#define KT3D
//#define SISIM
#define IK3D
//#define ESTIMATE
//#define BLOCK

using namespace gridworld;
using namespace std;

//��������
int round_double(double number)
{
	return (number > 0.0) ? (number + 0.5) : (number - 0.5);
}

//�ַ����ָ�
void split(const string source, vector<string>& dest)
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

//����������XYZת����IJK
void TransXYZtoIJK(const string& strXYZFile, const GridInfo& gi, vector<Cell>& vecCell)
{
	ifstream fin(strXYZFile, ios::in);
	if (!fin.is_open())
		return;

	string strLine;
	vector<string> vecStrData;

	for (int i = 0; i < 7; i++)
		getline(fin, strLine);

	vecCell.clear();
	while (!fin.eof())
	{
		vecStrData.clear();
		getline(fin, strLine);
		split(strLine, vecStrData);
		if (vecStrData.size() != 5)
			continue;

		double x = atof(vecStrData[0].c_str());
		double y = atof(vecStrData[1].c_str());
		double z = atof(vecStrData[2].c_str());
		double v = atof(vecStrData[3].c_str());

		int i = round_double((x - gi.dXOri) / gi.dXStep + gi.nIOri);
		int j = round_double((y - gi.dYOri) / gi.dYStep + gi.nJOri);
		int k = round_double((z - gi.dZOri) / gi.dZStep + gi.nKOri);

		Cell c(i, j, k, v);
		vecCell.push_back(c);
	}

	fin.close();
}

//����ɢ����XYZת����IJK
void TransXYZtoIJKDiscrete(const string& strXYZFile, const GridInfo& gi, vector<Cell>& vecCell)
{
	ifstream fin(strXYZFile, ios::in);
	if (!fin.is_open())
		return;

	string strLine;
	vector<string> vecStrData;

	for (int i = 0; i < 7; i++)
		getline(fin, strLine);

	vecCell.clear();
	while (!fin.eof())
	{
		vecStrData.clear();
		getline(fin, strLine);
		split(strLine, vecStrData);
		if (vecStrData.size() != 5)
			continue;

		double x = atof(vecStrData[0].c_str());
		double y = atof(vecStrData[1].c_str());
		double z = atof(vecStrData[2].c_str());
		int v = atoi(vecStrData[3].c_str());

		int i = round_double((x - gi.dXOri) / gi.dXStep + gi.nIOri);
		int j = round_double((y - gi.dYOri) / gi.dYStep + gi.nJOri);
		int k = round_double((z - gi.dZOri) / gi.dZStep + gi.nKOri);

		Cell c(i, j, k, v);
		vecCell.push_back(c);
	}

	fin.close();
}


/**********������������˹ģ���ֵ����ӿںͲ�ֵ�����ȡ�ӿڵ���Demo**********/
#ifdef SGSIM
void main()
{
	//׼��OilWater���Բ�ֵ�Ĳ���
	GridInfo gridInfo(45, 49, 1, 99.547226, 99.812538, 2.304501, 676067.647571, 5085799.081054, 1.152251, 0, 0, 0); //������������
	VariogramPara varPara(Exponential, 1, 0.0001, 2000, 1800, 8, 0, 90); //������������ı���
	string strXYZFile = "E:\\001_GridWorld\\004_TestData\\model_for_test\\SDK����ģ��_bachang\\��������\\��������˹\\oilwater_xyz.dat"; //XYZ��ʽ�������ݣ�����ת����IJK���в���

	ModelInterPara tmip; //�����ֵ�����ṹ��
	tmip.strWorkPath = "E:\\001_GridWorld\\004_TestData\\model_for_test\\SDK����ģ��_bachang\\��������\\��������˹\\"; //���ù���Ŀ¼��Ŀ¼����ڣ�
	tmip.nAttrType = Continuous; //����Ϊ����
	tmip.gi = gridInfo; //�����������
	TransXYZtoIJK(strXYZFile, gridInfo, tmip.vecCell); //����IJK����

	tmip.nInterMethod = SequentialGaussianSimulation; //�����ֵ�㷨Ϊ����˹
	tmip.distPara.bIsTrans = false; //��ֵǰ��������̬�任
	tmip.expertPara.nSeed = 30367; //�������ģ������(����̽һ��)
	tmip.varPara = varPara;	//�������

	//����SDK���в�ֵ
	if (!EsInterForModel(tmip))
		return;

	//ʹ��IJK��ȡ��ֵ������������ı���ʽ������ڲ鿴
	string strSaveFile = "E:\\001_GridWorld\\004_TestData\\model_for_test\\SDK����ģ��_bachang\\��������\\��������˹\\oilwater_ijk.dat";
	ofstream fout(strSaveFile, ios::out);
	if(!fout.is_open())
		return;
	
	for(int k=0; k<tmip.gi.nZNum; k++)
	{
		for(int j=0; j<tmip.gi.nYNum; j++)
		{
			for(int i=0; i<tmip.gi.nXNum; i++)
			{
				double v = EsGetContinuousPropertyByIJK(tmip,i,j,k);
				fout << setw(4) << i << "\t" << setw(4) << j << "\t" << setw(4) << k << "\t" << setw(8) << fixed << setprecision(4) << v << endl;
			}
		}
	}
	
	fout.close();
}
#endif

/**********����������ά������ֵ����ӿںͲ�ֵ�����ȡ�ӿڵ���Demo**********/
#ifdef KT3D
void main()
{	
	//׼��OilWater���Բ�ֵ�Ĳ���
	GridInfo gridInfo(45, 49, 1, 99.547226, 99.812538, 2.304501, 676067.647571, 5085799.081054, 1.152251, 0, 0, 0); //������������
	VariogramPara varPara(Exponential, 1, 0.0001, 2000, 1800, 8, 0, 90); //������������ı���
	string strXYZFile = "E:\\001_GridWorld\\004_TestData\\model_for_test\\SDK����ģ��_bachang\\��������\\���������\\oilwater_xyz.dat"; //XYZ��ʽ�������ݣ�����ת����IJK���в���

	ModelInterPara tmip; //�����ֵ�����ṹ��
	tmip.strWorkPath = "E:\\001_GridWorld\\004_TestData\\model_for_test\\SDK����ģ��_bachang\\��������\\���������\\"; //���ù���Ŀ¼��Ŀ¼����ڣ�
	tmip.nAttrType = Continuous; //����Ϊ����
	tmip.gi = gridInfo; //�����������
	TransXYZtoIJK(strXYZFile, gridInfo, tmip.vecCell); //����IJK����

	tmip.nInterMethod = Kriging; //�����ֵ�㷨Ϊ��ά�����
	tmip.varPara= varPara;	//�������

	//����SDK���в�ֵ
	if(!EsInterForModel(tmip))
		return;

	//ʹ��IJK��ȡ��ֵ������������ı���ʽ������ڲ鿴
	string strSaveFile = "E:\\001_GridWorld\\004_TestData\\model_for_test\\SDK����ģ��_bachang\\��������\\���������\\oilwater_ijk.dat";
	ofstream fout(strSaveFile, ios::out);
	if(!fout.is_open())
		return;

	for(int k=0; k<tmip.gi.nZNum; k++)
	{
		for(int j=0; j<tmip.gi.nYNum; j++)
		{
			for(int i=0; i<tmip.gi.nXNum; i++)
			{
				double v = EsGetContinuousPropertyByIJK(tmip,i,j,k);
				fout << setw(4) << i << "\t" << setw(4) << j << "\t" << setw(4) << k << "\t" << setw(8) << fixed << setprecision(4) << v << endl;
			}
		}
	}

	fout.close();
}
#endif

/**********��ɢ����ָʾ������ֵ����ӿںͲ�ֵ�����ȡ�ӿڵ���Demo**********/
#ifdef IK3D
void main()
{	
	//׼��OilWater���Բ�ֵ�Ĳ���
	GridInfo gridInfo(45, 49, 1, 99.547226, 99.812538, 2.304501, 676067.647571, 5085799.081054, 1.152251, 0, 0, 0); //������������
	VariogramPara varPara(Exponential, 1, 0.0001, 2000, 1800, 8, 0, 90); //������������ı���
	string strXYZFile = "E:\\001_GridWorld\\004_TestData\\model_for_test\\SDK����ģ��_bachang\\��������\\��ɢָʾ�����\\ylsy_xyz.dat"; //XYZ��ʽ�������ݣ�����ת����IJK���в���

	ModelInterPara tmip; //�����ֵ�����ṹ��
	tmip.strWorkPath = "E:\\001_GridWorld\\004_TestData\\model_for_test\\SDK����ģ��_bachang\\��������\\��ɢָʾ�����\\"; //���ù���Ŀ¼��Ŀ¼����ڣ�
	tmip.nAttrType = Discrete; //����Ϊ����
	tmip.gi = gridInfo; //�����������
	TransXYZtoIJKDiscrete(strXYZFile, gridInfo, tmip.vecCell); //����IJK����

	tmip.nInterMethod = IndicatorKriging; //�����ֵ�㷨Ϊָʾ�����

	//������ɢ���Ա���
	tmip.faciesPara.resize(2);	
	tmip.faciesPara[0].nCode = 1;
	tmip.faciesPara[0].varPara = varPara;
	tmip.faciesPara[1].nCode = 3;
	tmip.faciesPara[1].varPara = varPara;


	//����SDK���в�ֵ
	if(!EsInterForModel(tmip))
		return;

	//ʹ��IJK��ȡ��ֵ������������ı���ʽ������ڲ鿴
	string strSaveFile = "E:\\001_GridWorld\\004_TestData\\model_for_test\\SDK����ģ��_bachang\\��������\\��ɢָʾ�����\\ylsy_ijk.dat";
	ofstream fout(strSaveFile, ios::out);
	if(!fout.is_open())
		return;

	for(int k=0; k<tmip.gi.nZNum; k++)
	{
		for(int j=0; j<tmip.gi.nYNum; j++)
		{
			for(int i=0; i<tmip.gi.nXNum; i++)
			{
				int v = EsGetDiscretePropertyByIJK(tmip,i,j,k);
				fout << setw(4) << i << "\t" << setw(4) << j << "\t" << setw(4) << k << "\t" << setw(8) << fixed << setprecision(4) << v << endl;
			}
		}
	}

	fout.close();
}
#endif

/**********��ɢ�������ָʾģ���ֵ����ӿںͲ�ֵ�����ȡ�ӿڵ���Demo**********/
#ifdef SISIM
void main()
{	
	//׼��OilWater���Բ�ֵ�Ĳ���
	GridInfo gridInfo(45, 49, 1, 99.547226, 99.812538, 2.304501, 676067.647571, 5085799.081054, 1.152251, 0, 0, 0); //������������
	VariogramPara varPara(Exponential, 1, 0.0001, 2000, 1800, 8, 0, 90); //������������ı���
	string strXYZFile = "E:\\001_GridWorld\\004_TestData\\model_for_test\\SDK����ģ��_bachang\\��������\\��ɢ���ָʾ\\ylsy_xyz.dat"; //XYZ��ʽ�������ݣ�����ת����IJK���в���

	ModelInterPara tmip; //�����ֵ�����ṹ��
	tmip.strWorkPath = "E:\\001_GridWorld\\004_TestData\\model_for_test\\SDK����ģ��_bachang\\��������\\��ɢ���ָʾ\\"; //���ù���Ŀ¼��Ŀ¼����ڣ�
	tmip.nAttrType = Discrete; //����Ϊ����
	tmip.gi = gridInfo; //�����������
	TransXYZtoIJKDiscrete(strXYZFile, gridInfo, tmip.vecCell); //����IJK����

	tmip.nInterMethod = SequentialIndicatorSimulation; //�����ֵ�㷨Ϊ���ָʾģ��
	tmip.expertPara.nSeed = 21697; //�������ģ������(����̽һ��)

	//������ɢ���Ա���
	tmip.faciesPara.resize(2);	
	tmip.faciesPara[0].nCode = 1;
	tmip.faciesPara[0].varPara = varPara;
	tmip.faciesPara[1].nCode = 3;
	tmip.faciesPara[1].varPara = varPara;


	//����SDK���в�ֵ
	if(!EsInterForModel(tmip))
		return;

	//ʹ��IJK��ȡ��ֵ������������ı���ʽ������ڲ鿴
	string strSaveFile = "E:\\001_GridWorld\\004_TestData\\model_for_test\\SDK����ģ��_bachang\\��������\\��ɢ���ָʾ\\ylsy_ijk.dat";
	ofstream fout(strSaveFile, ios::out);
	if(!fout.is_open())
		return;

	for(int k=0; k<tmip.gi.nZNum; k++)
	{
		for(int j=0; j<tmip.gi.nYNum; j++)
		{
			for(int i=0; i<tmip.gi.nXNum; i++)
			{
				int v = EsGetDiscretePropertyByIJK(tmip,i,j,k);
				fout << setw(4) << i << "\t" << setw(4) << j << "\t" << setw(4) << k << "\t" << setw(8) << fixed << setprecision(4) << v << endl;
			}
		}
	}

	fout.close();
}
#endif

/**********��Χ�����ӿڵ���Demo**********/
#ifdef ESTIMATE
void main()
{	
	GridInfo gridInfo(45, 49, 1, 99.547226, 99.812538, 2.304501, 676067.647571, 5085799.081054, 1.152251, 0, 0, 0); //������������
	string strXYZFile = "E:\\001_GridWorld\\004_TestData\\model_for_test\\SDK����ģ��_bachang\\��������\\��Χ����\\ylsy_xyz.dat"; //XYZ��ʽ�������ݣ�����ת����IJK���в���

	vector<Cell> vecUpdateData;
	TransXYZtoIJKDiscrete(strXYZFile, gridInfo, vecUpdateData); //�õ�IJK����

	int rangeI = 1; //����I������������ = I����ʵ��������Χ/I�������񲽳�
	int rangeJ = 1; //����J������������ = J����ʵ��������Χ/J�������񲽳�
	int rangeK = 1; //����K������������ = K����ʵ��������Χ/K�������񲽳�

	vector<GridInfo> vecGIforSampleDownload; //�������ط�Χ��
	vector<GridInfo> vecGIforUpdate; //���巶Χ��
	EsEvaluateAreaForUpdate(vecUpdateData, rangeI, rangeJ, rangeK, gridInfo, vecGIforSampleDownload, vecGIforUpdate); //���ýӿڵõ����巶Χ

	if(vecGIforSampleDownload.size() != vecGIforUpdate.size())
		return;
	
#ifdef BLOCK
	string strAreaFile = "E:\\001_GridWorld\\004_TestData\\model_for_test\\SDK����ģ��_bachang\\��������\\��Χ����\\area_block.dat"; //��ֵ��Χ��Ϣ
#else
	string strAreaFile = "E:\\001_GridWorld\\004_TestData\\model_for_test\\SDK����ģ��_bachang\\��������\\��Χ����\\area.dat"; //��ֵ��Χ��Ϣ
#endif

	ofstream fout(strAreaFile, ios::out);
	if(!fout.is_open())
		return;
	
	for(size_t i=0; i<vecGIforUpdate.size(); i++)
	{
		fout << "���巶Χ��" << "\t" << "��ʼI:" << vecGIforUpdate[i].nIOri << "\t" << "��ʼJ:" << vecGIforUpdate[i].nJOri << "\t" << "��ʼK:" << vecGIforUpdate[i].nKOri << "\t"
			<< "I�������:" << vecGIforUpdate[i].nXNum << "\t" << "J�������:" << vecGIforUpdate[i].nYNum << "\t" << "K�������:" << vecGIforUpdate[i].nZNum << "\n";

		fout << "������Χ��" << "\t" << "��ʼI:" << vecGIforSampleDownload[i].nIOri << "\t" << "��ʼJ:" << vecGIforSampleDownload[i].nJOri << "\t" << "��ʼK:" << vecGIforSampleDownload[i].nKOri << "\t"
			<< "I�������:" << vecGIforSampleDownload[i].nXNum << "\t" << "J�������:" << vecGIforSampleDownload[i].nYNum << "\t" << "K�������:" << vecGIforSampleDownload[i].nZNum << "\n";

		fout << "\n";
	}
	fout.close();
}
#endif