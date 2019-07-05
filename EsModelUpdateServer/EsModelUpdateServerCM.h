///*************************************************************************
//* All rights reserved.
//*
//* 文件名称：EsModelUpdateServerCM.h
//* 摘    要：地调局定制模型自动更新服务配置管理类
//*
//* 作    者：何俊强
//* 完成日期：2019/01/29
//*************************************************************************/
//
//#pragma once
//#include <string>
//#include "EsModelUpdateServerBase.h"
//#include "tinyxml.h"
//
//using std::string;
//
//namespace gridworld
//{
//	const char* strConfigSetting = "setting";
//	const char* strConfigSetting_RootURL = "RootURL";
//	const char* strConfigSetting_WorkPath = "WorkPath";
//
//	const char* strConfigProperty = "property";
//	const char* strConfigProperty_PropName = "PropName";
//	const char* strConfigProperty_AttrType = "AttrType";
//	const char* strConfigProperty_CoverageID = "CoverageID";
//	const char* strConfigProperty_FeatureclassID = "FeatureclassID";
//
//	const char* strConfigZone = "zone";
//	const char* strConfigZone_ZoneID = "ZoneID";
//	const char* strConfigZone_InterpolationMethod = "InterpolationMethod";
//
//	const char* strConfigVariogram = "variogram";
//	const char* strConfigVariogram_FaceCode = "FaceCode";
//	const char* strConfigVariogram_VariogramType = "VariogramType";
//	const char* strConfigVariogram_Sill = "Sill";
//	const char* strConfigVariogram_Nugget = "Nugget";
//	const char* strConfigVariogram_MajorRange = "MajorRange";
//	const char* strConfigVariogram_MinorRange = "MinotRange";
//	const char* strConfigVariogram_VerticalRange = "VerticalRange";
//	const char* strConfigVariogram_Azimuth = "Azimuth";
//	const char* strConfigVariogram_Dip = "Dip";
//
//
//	class EXPORTMODELUPDATESERVER CModelUpdateServerCM
//	{
//	public:
//		CModelUpdateServerCM();
//		~CModelUpdateServerCM();
//
//	public:
//		bool EsReadServerConfigSetting(const string& strConfigPath, string& strRootURL, string& strWorkPath);
//		bool EsReadServerConfigProperty(const string& strConfigPath, const string& strPropName, AttrType& nAttrType, string& strCoverageID, string& strFeatureclassID);
//		bool EsReadServerConfigZone(const string& strConfigPath, const string& strPropName, const string& strZoneID, InterMethod& nInterpolationMethod);
//		bool EsReadServerConfigVariogram(const string& strConfigPath, const string& strPropName, const string& strZoneID, VariogramType& nVariogramType,double& dSill, 
//			double& dNugget, double& dMajorRange, double& dMinorRange, double& dVerticalRange, double& dAzimuth, double& dDip);
//		bool EsReadServerConfigVariogramByFaceCode(const string& strConfigPath, const string& strPropName, const string& strZoneID, const string& strFaceCode, VariogramType& nVariogramType,
//			double& dSill, double& dNugget, double& dMajorRange, double& dMinorRange, double& dVerticalRange, double& dAzimuth, double& dDip);
//		bool EsReadServerConfigZoneID(const string& strConfigPath, const string& strPropName, vector<int>& vecZoneID);
//		bool EsReadServerConfigFaceCode(const string& strConfigPath, const string& strPropName, const string& strZoneID, vector<int>& vecFaceCode);
//	};
//}