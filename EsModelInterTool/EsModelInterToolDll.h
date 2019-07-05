/*************************************************************************
* Copyright (c) 2018,�������������������ɷ����޹�˾
* All rights reserved.
* 
* �ļ����ƣ�EsModelInterToolDll.h
* ժ    Ҫ��ģ���Զ����²�ֵ����SDK
* 
* ��    �ߣ��ο�ǿ
* ������ڣ�2018��11��22��
*************************************************************************/

#include <vector>
#include <string>
#include <map>

using std::vector;
using std::string;
using std::map;


namespace gridworld
{
	//���β�ֵ������������(��ʱ��Ϊ1ǧ��)
	const int gLimit = 1e7;

	//�������ͼ���
	enum AttrType
	{
		Continuous, //����
		Discrete //��ɢ
	};

	//��������
	enum VariogramType
	{
		Spherical = 1, //����
		Exponential, //ָ����
		Gaussian //��˹��	
	};

	//��ֵ�㷨
	enum InterMethod
	{
		SequentialGaussianSimulation, //����������˹ģ��
		Kriging, //������������ֵ

		SequentialIndicatorSimulation, //��ɢ�����ָʾģ��
		IndicatorKriging //��ɢ��ָʾ�����
	};

	//���������
	enum KrigingType
	{
		Kriging_SK, //�򵥿����
		Kriging_OK //��ͨ����� 
	};

	//����
	struct VariogramPara
	{
		VariogramType varType; //�������ͣ����룩
		double dSill; //��ֵ̨�����룩
		double dNugget; //���ֵ�����룩
		double dMajorRange; //����������Χ�����룩
		double dMinorRange; //����������Χ�����룩
		double dVerticalRange; //��ֱ������Χ�����룩
		double dAzimuth; //�����Ƕȣ����룩
		double dDip; //��ǣ����룩

		//��ʼ��
		VariogramPara() :varType(Exponential), dSill(1.0), dNugget(0.0001), dMajorRange(2000), dMinorRange(1800), dVerticalRange(8), dAzimuth(0), dDip(90) {}
		VariogramPara(VariogramType type, double sill, double nugget, double mRange, double nRange, double vRange, double azimuth, double dip) :
			varType(type), dSill(sill), dNugget(nugget), dMajorRange(mRange), dMinorRange(nRange), dVerticalRange(vRange), dAzimuth(azimuth), dDip(dip) {}
	};

	//��ɢ�����
	struct CodeParaSet
	{
		int nCode; //����루��ɢ���Բ�ֵʱ-���룩
		VariogramPara varPara; //���������ɢ���Բ�ֵʱ-���룩
	};

	//��ֵ��������
	struct ExpertPara
	{
		KrigingType kType; //��������ͣ���ѡ��
		double dSearchMulti; //������Χ�ĳ�������ѡ��
		int nMinNumNodesUse; //ʹ������Դ����С��������ѡ��
		int nMaxNumNodesUse; //ʹ������Դ�������������ѡ��

		bool bUseOctant; //�Ƿ�ʹ��������������ѡ��
		int nNumOctant; //��ʹ����������ʱÿ�����޵����������������ѡ��

		int nSimulatedNodesUse; //ʹ����������������������������ѡ��

		bool bIsAssignToDataNodes; //�Ƿ���������ƥ�䵽���񣨿�ѡ��

		bool bIsUseMulti; //�Ƿ�ʹ�ö�����������������ѡ��
		int nGridMultiStep; //������������������ѡ��

		int  nSeed; //���ģ�����ӣ���ѡ������˹�����ָʾ��Ҫ�����û���������SDK���Զ����ɣ�

		//��ʼ��
		ExpertPara() : kType(Kriging_SK), dSearchMulti(2.0), nMinNumNodesUse(0), nMaxNumNodesUse(12), bUseOctant(false), 
			nNumOctant(2), nSimulatedNodesUse(12), bIsAssignToDataNodes(true), bIsUseMulti(false), nGridMultiStep(3), nSeed(10001) {}
		ExpertPara(KrigingType type, double searchMulti, int minNumNodesUse, int maxNumNodesUse, bool usdOctant, 
			int numOctant, int simulateNodesUsd, bool assignToDataNodes, bool useMulti, int gridMultiStep, int seed) : 
		kType(type), dSearchMulti(searchMulti), nMinNumNodesUse(minNumNodesUse), nMaxNumNodesUse(maxNumNodesUse), bUseOctant(useMulti), nNumOctant(numOctant), 
			nSimulatedNodesUse(simulateNodesUsd), bIsAssignToDataNodes(assignToDataNodes), bIsUseMulti(useMulti), nGridMultiStep(gridMultiStep), nSeed(seed) {}
	};

	//����˹ģ�����ݱ任����
	struct DistributedPara
	{
		bool bIsTrans; //�Ƿ���������ݽ�����̬�任��Ĭ��Ϊ������̬�任��
		double dOutputMin; //���ֵ��Сֵ���ã�Ĭ�ϣ�SDK�Զ����㣩
		double dOutputMax; //���ֵ���ֵ���ã�Ĭ�ϣ�SDK�Զ����㣩

		DistributedPara(): bIsTrans(true), dOutputMin(0), dOutputMax(0){}
	};

	//���巶Χ������Ϣ
	struct GridInfo
	{
		int nXNum; //x�������
		int nYNum; //y�������
		int nZNum; //z�������

		double dXStep; //x���򲽳�
		double dYStep; //y���򲽳�
		double dZStep; //Z����ƽ������ = �ز�ƽ�����/nZNum

		double dXOri; //��ʼ�������ĵ�x���� 
		double dYOri; //��ʼ�������ĵ�y����
		double dZOri; //��ʼ�������ĵ�z���� = zStep/2

		int nIOri; //I����ʼ����ȫ��I
		int nJOri; //J����ʼ����ȫ��J
		int nKOri; //K����ʼ����ȫ��K

		GridInfo() : nXNum(0), nYNum(0), nZNum(0), dXStep(0), dYStep(0), dZStep(0), dXOri(0), dYOri(0), dZOri(0), nIOri(0), nJOri(0), nKOri(0) {}
		GridInfo(int xNum, int yNum, int zNum, double xStep, double yStep, double zStep, double xOri, double yOri, double zOri, int iOri, int jOri, int kOri) : 
			nXNum(xNum), nYNum(yNum), nZNum(zNum), dXStep(xStep), dYStep(yStep), dZStep(zStep), dXOri(xOri), dYOri(yOri), dZOri(zOri), nIOri(iOri), nJOri(jOri), nKOri(kOri) {}
	};

	//��һ����
	struct Cell
	{
		int nI; //I����0��ʼ
		int nJ; //J����0��ʼ
		int nK; //K��,�������£���0����
		double dValue; //��������ֵ
		int nValue; //��ɢ������ID

		Cell() : nI(0), nJ(0), nK(0), dValue(0), nValue(0){}
		Cell(int i, int j, int k, double value) : nI(i), nJ(j), nK(k), dValue(value), nValue(0) {}
		Cell(int i, int j, int k, int value) : nI(i), nJ(j), nK(k), dValue(0), nValue(value) {}
	};

	//һ����׼�¼
	struct Record
	{
		double dX; //x����
		double dY; //y����
		double dZ; //z����
		string strValue; //����ֵ

		Record() : dX(0.0), dY(0.0), dZ(0.0) {}
		Record(double x, double y, double z, string value) : dX(x), dY(y), dZ(z), strValue(value) {}
	};

	//�����ֵ��������
	struct ModelInterPara
	{
		//����Ŀ¼
		string strWorkPath; //�û�ָ����SDK���ڴ�Ŀ¼�´���TEMP�ļ������ڴ�Ų�ֵ��������в�������ʱ�ļ������룩,������TEMP�ļ��лᱻɾ��

		//��ֵ���������������
		AttrType nAttrType; //�������ͣ����룩
		GridInfo gi; //������Ϣ�����룩
		vector<Cell> vecCell; //�������񼯣����룩

		//��ֵ����
		InterMethod nInterMethod; //��ֵ�㷨�����룩
		ExpertPara expertPara; //���������������������
		DistributedPara distPara; //����������̬�任����������˹-���룬����-Ĭ�ϣ�

		//����
		VariogramPara varPara; //�������Ա�����ݣ�����-��ѡ����ɢ-Ĭ�ϣ�		
		vector<CodeParaSet> faciesPara; //��ɢ����ÿ����ı����������ϣ���ɢ-���룬����-Ĭ�ϣ�

		//��ֵ���
		vector<double> vecContinuousProp; //���ڴ洢ȫ��������������ֵ��Ĭ�ϣ�
		vector<int> vecDiscreteProp; //���ڴ洢ȫ��������ɢ����ֵ��Ĭ�ϣ�
	};

	/**
	* @Function:    ʹ�ò����е���������ֵ����������������е�ÿһ����������ֵ
	* @Param [in]:  tmip                                                �����ֵ��������
	* @Param [out]: tmip.vecContinuousProp/tmip.vecDiscreteProp         �������/��ɢ���Բ�ֵ�����vector��С����������һ��
	* @return:      true-��ֵ�ɹ�;false-��ֵʧ��
	*/
	extern "C" __declspec(dllexport) bool EsInterForModel(ModelInterPara& tmip);

	/**
	* @Function:    ��vecDiscreteProp����ȡ��Ӧ����I,J,K��������ֵ
	* @Param [in]:  tmip.vecDiscreteProp         ��ɢ���Բ�ֵ�����vector��С����������һ��
	* @Param [in]:  i                            ��������I
	* @Param [in]:  j                            ��������J
	* @Param [in]:  k                            ��������K
	* @return:      ��ɢ����ֵ
	*/
	extern "C" __declspec(dllexport) int EsGetDiscretePropertyByIJK(ModelInterPara& tmip, const int i, const int j, const int k);

	/** 
	* @Function:    ��vecContinuousProp����ȡ��Ӧ����I,J,K��������ֵ
	* @Param [in]:  tmip.vecContinuousProp       �������Բ�ֵ�����vector��С����������һ��
	* @Param [in]:  i                            ��������I
	* @Param [in]:  j                            ��������J
	* @Param [in]:  k                            ��������K
	* @return:      ��������ֵ
	*/
	extern "C" __declspec(dllexport) double EsGetContinuousPropertyByIJK(ModelInterPara& tmip, const int i, const int j, const int k);

	/**
	* @Function:    ��������������Ӱ�췶Χ,�������·�Χ������������Χ
	* @Param [in]:  vecUpdateData                  ���µ������㼯
	* @Param [in]:  rangeI                         I����������Χ�����������ƣ�
	* @Param [in]:  rangeJ                         J����������Χ�����������ƣ�
	* @Param [in]:  rangeK                         K����������Χ�����������ƣ�
	* @Param [in]:  globalGI                       ���µ����Χ������Ϣ�������������������鷶Χ��
	* @Param [out]: vecGIforSampleDownload         ����������Χ����
	* @Param [out]: vecGIforSampleDownload         ���巶Χ����
	* @return:      true-�����ɹ�;false-����ʧ��
	*/
	extern "C" __declspec(dllexport) bool EsEvaluateAreaForUpdate(vector<Cell>& vecUpdateData, const int rangeI, const int rangeJ, const int rangeK,
		const GridInfo& globalGI, vector<GridInfo>& vecGIforSampleDownload, vector<GridInfo>& vecGIforUpdate);

}