#include "EsModelInterToolDll.h"

namespace gridworld
{
/*****************************�ڲ����̽ӿ�**************************************/
	/**
	* @Function:    ��ֵ���������ļ�׼���ӿ�
	* @Param [in]:  tmip                    ��ֵ����
	* @Param [out]: strInterDataFile        ��ֵ���������ļ�
	* @return:      true-׼���ɹ�;false-׼��ʧ��
	*/
	bool EsInterDataFilePrep(ModelInterPara& tmip, string& strInterDataFile);

	/**
	* @Function:    ��ֵ�����ļ�׼���ӿ�
	* @Param [in]:  tmip                    ��ֵ����
	* @Param [in]:  strInterDataFile        ��ֵ���������ļ�
	* @Param [out]: strInterParaFile        ��ֵ�����ļ�
	* @Param [out]: strResultFile           ��ֵ��������ļ�
	* @return:      true-׼���ɹ�;false-׼��ʧ��
	*/
	bool EsInterParaFilePrep(ModelInterPara& tmip, const string& strInterDataFile, string& strInterParaFile, string& strResultFile);

	/**
	* @Function:    ��ֵ����ӿ�
	* @Param [in]:  tmip                                               ��ֵ����
	* @Param [in]:  strInterParaFile                                   ��ֵ�����ļ�
	* @Param [in]:  strResultFile                                      ��ֵ��������ļ�
	* @Param [out]: tmip.vecContinuousProp/tmip.vecDiscreteProp        ��ֵ���
	* @return:      true-��ֵ�ɹ�;false-��ֵʧ��
	*/
	bool EsInterpolation(ModelInterPara& tmip, const string& strInterParaFile, const string& strResultFile);


/*****************************��ͬ��ֵ�㷨�����ļ�׼������**************************************/
	/**
	* @Function:    ׼������˹ģ������ļ�
	* @Param [in]:  tmip                    ��ֵ����
	* @Param [in]:  strInterDataFile        ��ֵ���������ļ�
	* @Param [out]: strInterParaFile        ��ֵ�����ļ�
	* @Param [out]: strResultFile           ��ֵ��������ļ�
	* @return:      true-׼���ɹ�;false-׼��ʧ��
	*/
	bool EsPrepSgsimParaFile(ModelInterPara& tmip, const string& strInterDataFile, string& strInterParaFile, string& strResultFile);

	/**
	* @Function:    ׼����ά���������ļ�
	* @Param [in]:  tmip                    ��ֵ����
	* @Param [in]:  strInterDataFile        ��ֵ���������ļ�
	* @Param [out]: strInterParaFile        ��ֵ�����ļ�
	* @Param [out]: strResultFile           ��ֵ��������ļ�
	* @return:      true-׼���ɹ�;false-׼��ʧ��
	*/
	bool EsPrepKt3dParaFile(ModelInterPara& tmip, const string& strInterDataFile, string& strInterParaFile, string& strResultFile);

	/**
	* @Function:    ׼�����ָʾģ������ļ�
	* @Param [in]:  tmip                    ��ֵ����
	* @Param [in]:  strInterDataFile        ��ֵ���������ļ�
	* @Param [out]: strInterParaFile        ��ֵ�����ļ�
	* @Param [out]: strResultFile           ��ֵ��������ļ�
	* @return:      true-׼���ɹ�;false-׼��ʧ��
	*/
	bool EsPrepSisimParaFile(ModelInterPara& tmip, const string& strInterDataFile, string& strInterParaFile, string& strResultFile);

	/**
	* @Function:    ׼��ָʾ���������ļ�
	* @Param [in]:  tmip                    ��ֵ����
	* @Param [in]:  strInterDataFile        ��ֵ���������ļ�
	* @Param [out]: strInterParaFile        ��ֵ�����ļ�
	* @Param [out]: strResultFile           ��ֵ��������ļ�
	* @return:      true-׼���ɹ�;false-׼��ʧ��
	*/
	bool EsPrepIk3dParaFile(ModelInterPara& tmip, const string& strInterDataFile, string& strInterParaFile, string& strResultFile);


/*****************************��������**************************************/
	/**
	* @Function:    ���������������ѡ��ֵ�㷨�Ƿ�ƥ��
	* @Param [in]:  nAttrType               ��������
	* @Param [in]:  nInterMethod            ��ֵ����
	* @return:      true-ƥ��;false-��ƥ��
	*/
	bool EsCheckInterMethod(const AttrType nAttrType, const InterMethod nInterMethod);

	/**
	* @Function:    ��ȡ�������ݵ�������Сֵ
	* @Param [in]:  strInterDataFile        ��ֵ���������ļ�
	* @Param [out]: dMin                    ������Сֵ
	* @Param [out]: dMax                    �������ֵ
	* @return:      true-��ȡ�ɹ�;false-��ȡʧ��
	*/
	bool EsGetSampleDataRange(const string& strInterDataFile, double& dMin, double& dMax);

	/**
	* @Function:    ���㵱ǰ�������������еı���
	* @Param [in]:  strInterDataFile        ��ֵ���������ļ�
	* @Param [out]: nCode                   �����
	* @return:      �������������еı���
	*/
	double EsGetCodePercent(const string& strInterDataFile, const int nCode);

	/**
	* @Function:    ���������������
	* @return:      һ������
	*/
	int EsGetOddRandomNum();

	/**
	* @Function:    �ַ����ָ�
	* @Param [in]:  source                  Դ�ַ���
	* @Param [out]: dest                    �ָ����ַ�������
	*/
	void split(const string& source, vector<string>& dest);

	/**
	* @Function:    ɾ������Ŀ¼�µ���ʱ�ļ�
	* @Param [in]:  strWorkPath             ����Ŀ¼·��
	* @return:      true-ɾ���ɹ�;false-ɾ��ʧ��
	*/
	bool EsClearDir(const string& strWorkPath);

	/**
	* @Function:    ik3d��ֵ�������ֱ�ӿ��õ����ԣ���Ҫ����ת��	
	* @Param [in]:  strResultFile           ik3d��ֵ��������ļ�
	* @Param [out]: strResultFile           ��ֵ���ת���󱣴��ļ�
	* @return:      true-ת���ɹ�;false-ת��ʧ��
	*/
	bool EsTransformIk3dResult(const string& strResultFile);

	/**
	* @Function:    ��̬�任
	* @Param [in]:  strInterDataFile        ��ֵ���������ļ�
	* @Param [in]:  strWorkPath             ����Ŀ¼·��
	* @Param [in]:  dMin                    ������Сֵ
	* @Param [in]:  dMax                    �������ֵ
	* @Param [out]: strOutputFile           ������̬�任��������ļ�
	* @return:      true-ת���ɹ�;false-ת��ʧ��
	*/
	bool EsNormalScore(const string& strOutputFile, const string& strInterDataFile, const string& strWorkPath, double& dMin, double& dMax);

	/**
	* @Function:    ��̬�任
	* @Param [in]:  strResultFile           ��ֵ��������ļ�
	* @Param [in]:  strWorkPath             ����Ŀ¼·��
	* @Param [in]:  dMin                    ��ֵ�����Сֵ
	* @Param [in]:  dMax                    ��ֵ������ֵ
	* @Param [out]: strOutputFile           ��ֵ�������̬�任�Ľ�������ļ�
	* @return:      true-ת���ɹ�;false-ת��ʧ��
	*/
	bool EsRevertNormalScore(const string& strOutputFile, const string& strResultFile, const string& strWorkPath, double& dMin, double& dMax);

	/**
	* @Function:    �ز���
	* @Param [in]:  tmip                                               ��ֵ����
	* @Param [in]:  strResultFile                                      ��ֵ��������ļ�
	* @Param [out]: tmip.vecContinuousProp/tmip.vecDiscreteProp        �ز�������Ĳ�ֵ���
	* @return:      true-�ز����ɹ�;false-�ز���ʧ��
	*/
	bool EsResample(ModelInterPara& tmip, const string& strResultFile); //
}