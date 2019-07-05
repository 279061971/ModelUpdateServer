//#include <fstream>
//#include "EsModelUpdateServerCM.h"
//
//using std::ifstream;
//using std::ofstream;
//using std::ios;
//
//gridworld::CModelUpdateServerCM::CModelUpdateServerCM()
//{
//
//}
//
//gridworld::CModelUpdateServerCM::~CModelUpdateServerCM()
//{
//
//}
//
//bool gridworld::CModelUpdateServerCM::EsReadServerConfigSetting(const string& strConfigPath, string& strRootURL, string& strWorkPath)
//{
//	TiXmlDocument doc(strConfigPath.c_str());
//	if (!doc.LoadFile())
//		return false;
//
//	TiXmlHandle hDoc(&doc);
//	TiXmlElement* pRoot = hDoc.FirstChildElement().Element();
//	if (pRoot == NULL)
//		return false;
//
//	TiXmlElement* pElem = pRoot->FirstChild()->ToElement();
//	if (pElem == NULL)
//		return false;
//
//	strRootURL = pElem->Attribute(strConfigSetting_RootURL);
//	strWorkPath = pElem->Attribute(strConfigSetting_WorkPath);
//
//	doc.SaveFile();
//	return true;
//}
//
//bool gridworld::CModelUpdateServerCM::EsReadServerConfigProperty(const string& strConfigPath, const string& strPropName, AttrType& nAttrType, string& strCoverageID, string& strFeatureclassID)
//{
//	TiXmlDocument doc(strConfigPath.c_str());
//	if (!doc.LoadFile())
//		return false;
//
//	TiXmlHandle hDoc(&doc);
//	TiXmlElement* pRoot = hDoc.FirstChildElement().Element();
//	if (pRoot == NULL)
//		return false;
//
//	for (TiXmlNode* pChild = pRoot->FirstChild(); pChild; pChild = pChild->NextSibling())
//	{
//		TiXmlElement* pElem = pChild->ToElement();
//		if (pElem && pElem->ValueTStr() == strConfigProperty && pElem->Attribute(strConfigProperty_PropName) == strPropName)
//		{
//			int nType = -1;
//			pElem->Attribute(strConfigProperty_AttrType, &nType);
//			if (nType == 0)
//				nAttrType = Continuous;
//			else if (nType == 1)
//				nAttrType = Discrete;
//
//			strCoverageID = pElem->Attribute(strConfigProperty_CoverageID);
//			strFeatureclassID = pElem->Attribute(strConfigProperty_FeatureclassID);
//			break;
//		}
//	}
//
//	doc.SaveFile();
//	return true;
//}
//
//bool gridworld::CModelUpdateServerCM::EsReadServerConfigZone(const string& strConfigPath, const string& strPropName, const string& strZoneID, InterMethod& nInterpolationMethod)
//{
//	TiXmlDocument doc(strConfigPath.c_str());
//	if (!doc.LoadFile())
//		return false;
//
//	TiXmlHandle hDoc(&doc);
//	TiXmlElement* pRoot = hDoc.FirstChildElement().Element();
//	if (pRoot == NULL)
//		return false;
//
//	for (TiXmlNode* pChild = pRoot->FirstChild(); pChild; pChild = pChild->NextSibling())
//	{
//		TiXmlElement* pElem = pChild->ToElement();
//		if (pElem && pElem->ValueTStr() == strConfigProperty && pElem->Attribute(strConfigProperty_PropName) == strPropName)
//		{
//			for (TiXmlNode* pSecondChild = pElem->FirstChild(); pSecondChild; pSecondChild = pSecondChild->NextSibling())
//			{
//				TiXmlElement* pSecondElem = pSecondChild->ToElement();
//				if (pSecondElem && pSecondElem->ValueTStr() == strConfigZone && pSecondElem->Attribute(strConfigZone_ZoneID) == strZoneID)
//				{
//					int nMehtod = -1;
//					pSecondElem->Attribute(strConfigZone_InterpolationMethod, &nMehtod);
//
//					switch (nMehtod)
//					{
//					case 0:
//						nInterpolationMethod = SequentialGaussianSimulation;
//						break;
//					case 1:
//						nInterpolationMethod = Kriging;
//						break;
//					case 2:
//						nInterpolationMethod = SequentialIndicatorSimulation;
//						break;
//					case 3:
//						nInterpolationMethod = IndicatorKriging;
//						break;
//					default:
//						doc.SaveFile();
//						return false;
//					}
//					break;
//				}
//			}			
//		}
//	}
//
//	doc.SaveFile();
//	return true;
//}
//
//bool gridworld::CModelUpdateServerCM::EsReadServerConfigVariogram(const string& strConfigPath, const string& strPropName, const string& strZoneID, VariogramType& nVariogramType, double& dSill,
//	double& dNugget, double& dMajorRange, double& dMinorRange, double& dVerticalRange, double& dAzimuth, double& dDip)
//{
//	TiXmlDocument doc(strConfigPath.c_str());
//	if (!doc.LoadFile())
//		return false;
//
//	TiXmlHandle hDoc(&doc);
//	TiXmlElement* pRoot = hDoc.FirstChildElement().Element();
//	if (pRoot == NULL)
//		return false;
//
//	for (TiXmlNode* pChild = pRoot->FirstChild(); pChild; pChild = pChild->NextSibling())
//	{
//		TiXmlElement* pElem = pChild->ToElement();
//		if (pElem && pElem->ValueTStr() == strConfigProperty && pElem->Attribute(strConfigProperty_PropName) == strPropName)
//		{
//			for (TiXmlNode* pSecondChild = pElem->FirstChild(); pSecondChild; pSecondChild = pSecondChild->NextSibling())
//			{
//				TiXmlElement* pSecondElem = pSecondChild->ToElement();
//				if (pSecondElem && pSecondElem->ValueTStr() == strConfigZone && pSecondElem->Attribute(strConfigZone_ZoneID) == strZoneID)
//				{
//					TiXmlElement* pThirdElem = pSecondElem->FirstChild()->ToElement();
//					if (pThirdElem && pThirdElem->ValueTStr() == strConfigVariogram)
//					{
//						int nType = -1;
//						pThirdElem->Attribute(strConfigVariogram_VariogramType, &nType);
//
//						switch (nType)
//						{
//						case 1:
//							nVariogramType = Spherical;
//							break;
//						case 2:
//							nVariogramType = Exponential;
//							break;
//						case 3:
//							nVariogramType = Gaussian;
//							break;
//						default:
//							doc.SaveFile();
//							return false;
//						}
//
//						pThirdElem->Attribute(strConfigVariogram_Sill, &dSill);
//						pThirdElem->Attribute(strConfigVariogram_Nugget, &dNugget);
//						pThirdElem->Attribute(strConfigVariogram_MajorRange, &dMajorRange);
//						pThirdElem->Attribute(strConfigVariogram_MinorRange, &dMinorRange);
//						pThirdElem->Attribute(strConfigVariogram_VerticalRange, &dVerticalRange);
//						pThirdElem->Attribute(strConfigVariogram_Azimuth, &dAzimuth);
//						pThirdElem->Attribute(strConfigVariogram_Dip, &dDip);
//						break;
//					}					
//				}
//			}
//		}
//	}
//
//	doc.SaveFile();
//	return true;
//}
//
//bool gridworld::CModelUpdateServerCM::EsReadServerConfigVariogramByFaceCode(const string& strConfigPath, const string& strPropName, const string& strZoneID, const string& strFaceCode, VariogramType& nVariogramType,
//	double& dSill, double& dNugget, double& dMajorRange, double& dMinorRange, double& dVerticalRange, double& dAzimuth, double& dDip)
//{
//	TiXmlDocument doc(strConfigPath.c_str());
//	if (!doc.LoadFile())
//		return false;
//
//	TiXmlHandle hDoc(&doc);
//	TiXmlElement* pRoot = hDoc.FirstChildElement().Element();
//	if (pRoot == NULL)
//		return false;
//
//	for (TiXmlNode* pChild = pRoot->FirstChild(); pChild; pChild = pChild->NextSibling())
//	{
//		TiXmlElement* pElem = pChild->ToElement();
//		if (pElem && pElem->ValueTStr() == strConfigProperty && pElem->Attribute(strConfigProperty_PropName) == strPropName)
//		{
//			for (TiXmlNode* pSecondChild = pElem->FirstChild(); pSecondChild; pSecondChild = pSecondChild->NextSibling())
//			{
//				TiXmlElement* pSecondElem = pSecondChild->ToElement();
//				if (pSecondElem && pSecondElem->ValueTStr() == strConfigZone && pSecondElem->Attribute(strConfigZone_ZoneID) == strZoneID)
//				{
//					TiXmlElement* pThirdElem = pSecondElem->FirstChild()->ToElement();
//					if (pThirdElem && pThirdElem->ValueTStr() == strConfigVariogram && pThirdElem->Attribute(strConfigVariogram_FaceCode) == strFaceCode)
//					{
//						int nType = -1;
//						pThirdElem->Attribute(strConfigVariogram_VariogramType, &nType);
//
//						switch (nType)
//						{
//						case 1:
//							nVariogramType = Spherical;
//							break;
//						case 2:
//							nVariogramType = Exponential;
//							break;
//						case 3:
//							nVariogramType = Gaussian;
//							break;
//						default:
//							doc.SaveFile();
//							return false;
//						}
//
//						pThirdElem->Attribute(strConfigVariogram_Sill, &dSill);
//						pThirdElem->Attribute(strConfigVariogram_Nugget, &dNugget);
//						pThirdElem->Attribute(strConfigVariogram_MajorRange, &dMajorRange);
//						pThirdElem->Attribute(strConfigVariogram_MinorRange, &dMinorRange);
//						pThirdElem->Attribute(strConfigVariogram_VerticalRange, &dVerticalRange);
//						pThirdElem->Attribute(strConfigVariogram_Azimuth, &dAzimuth);
//						pThirdElem->Attribute(strConfigVariogram_Dip, &dDip);
//						break;
//					}
//				}
//			}
//		}
//	}
//
//	doc.SaveFile();
//	return true;
//}
//
//bool gridworld::CModelUpdateServerCM::EsReadServerConfigZoneID(const string& strConfigPath, const string& strPropName, vector<int>& vecZoneID)
//{
//	TiXmlDocument doc(strConfigPath.c_str());
//	if (!doc.LoadFile())
//		return false;
//
//	TiXmlHandle hDoc(&doc);
//	TiXmlElement* pRoot = hDoc.FirstChildElement().Element();
//	if (pRoot == NULL)
//		return false;
//
//	vecZoneID.clear();
//	for (TiXmlNode* pChild = pRoot->FirstChild(); pChild; pChild = pChild->NextSibling())
//	{
//		TiXmlElement* pElem = pChild->ToElement();
//		if (pElem && pElem->ValueTStr() == strConfigProperty && pElem->Attribute(strConfigProperty_PropName) == strPropName)
//		{
//			for (TiXmlNode* pSecondChild = pElem->FirstChild(); pSecondChild; pSecondChild = pSecondChild->NextSibling())
//			{
//				TiXmlElement* pSecondElem = pSecondChild->ToElement();
//				if (pSecondElem && pSecondElem->ValueTStr() == strConfigZone)
//				{
//					int id = -1;
//					pSecondElem->Attribute(strConfigZone_ZoneID, &id);
//					vecZoneID.push_back(id);
//				}
//			}
//		}
//	}
//
//	doc.SaveFile();
//	return true;
//}
//
//bool gridworld::CModelUpdateServerCM::EsReadServerConfigFaceCode(const string& strConfigPath, const string& strPropName, const string& strZoneID, vector<int>& vecFaceCode)
//{
//	TiXmlDocument doc(strConfigPath.c_str());
//	if (!doc.LoadFile())
//		return false;
//
//	TiXmlHandle hDoc(&doc);
//	TiXmlElement* pRoot = hDoc.FirstChildElement().Element();
//	if (pRoot == NULL)
//		return false;
//
//	vecFaceCode.clear();
//	for (TiXmlNode* pChild = pRoot->FirstChild(); pChild; pChild = pChild->NextSibling())
//	{
//		TiXmlElement* pElem = pChild->ToElement();
//		if (pElem && pElem->ValueTStr() == strConfigProperty && pElem->Attribute(strConfigProperty_PropName) == strPropName)
//		{
//			for (TiXmlNode* pSecondChild = pElem->FirstChild(); pSecondChild; pSecondChild = pSecondChild->NextSibling())
//			{
//				TiXmlElement* pSecondElem = pSecondChild->ToElement();
//				if (pSecondElem && pSecondElem->ValueTStr() == strConfigZone && pSecondElem->Attribute(strConfigZone_ZoneID) == strZoneID)
//				{
//					TiXmlElement* pThirdElem = pSecondElem->FirstChild()->ToElement();
//					if (pThirdElem && pThirdElem->ValueTStr() == strConfigVariogram)
//					{
//						int nCode = -1;
//						pThirdElem->Attribute(strConfigVariogram_FaceCode, &nCode);
//						vecFaceCode.push_back(nCode);
//					}
//				}
//			}
//		}
//	}
//
//	doc.SaveFile();
//	return true;
//}