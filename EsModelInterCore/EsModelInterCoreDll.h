/*************************************************************************
* Copyright (c) 2018,�������������������ɷ����޹�˾
* All rights reserved.
* 
* �ļ����ƣ�EsModelInterCoreDll.h
* ժ    Ҫ��ģ���Զ����²�ֵ����SDK
* 
* ��    �ߣ��ο�ǿ
* ������ڣ�2018��11��22��
*************************************************************************/

#include <string>
using std::string;

namespace gridworld
{
	/**
	* @Function:    ����ֵ�����ļ����͸�gslib������в�ֵ����
	* @Param [in]:  strInterParaFile               ��ֵ�����ļ�
	* @Param [in]:  nInterMethod                   ��ֵ����
	* @return:      true-��ֵ����ɹ�;false-��ֵ����ʧ��
	*/
	extern "C" __declspec(dllexport) bool EsInterFunc(const string& strInterParaFile, int nInterMethod); //��ֵ���ļ���

}