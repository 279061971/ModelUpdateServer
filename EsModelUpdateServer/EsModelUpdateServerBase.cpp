#include <fstream>
#include "EsModelUpdateServerBase.h"

gridworld::CModelUpdateServerBase::CModelUpdateServerBase()
{

}

gridworld::CModelUpdateServerBase::~CModelUpdateServerBase()
{

}

void gridworld::CModelUpdateServerBase::EsStartServer()
{

}

bool gridworld::CModelUpdateServerBase::EsCheckIsUpdated()
{
	return true;
}

bool gridworld::CModelUpdateServerBase::EsGetUpdatedSampleData()
{
	return true;
}

bool gridworld::CModelUpdateServerBase::EsEvaluate(vector<Cell>& vecUpdateData, const int rangeI, const int rangeJ, const int rangeK,
	const GridInfo& globalGI, vector<GridInfo>& vecGIforSampleDownload, vector<GridInfo>& vecGIforUpdate)
{
	if (!EsEvaluateAreaForUpdate(vecUpdateData, rangeI, rangeJ, rangeK, globalGI, vecGIforSampleDownload, vecGIforUpdate))
		return false;

	return true;
}

bool gridworld::CModelUpdateServerBase::EsDownLoadData()
{
	return true;
}

bool gridworld::CModelUpdateServerBase::EsUpdateData(ModelInterPara& tmip)
{
	if (!EsInterForModel(tmip))
		return false;

	return true;
}

bool gridworld::CModelUpdateServerBase::EsUpLoadData()
{
	return true;
}

void gridworld::CModelUpdateServerBase::EsWriteToLog(const string& strLogFile, const string& strMsg)
{
	std::ofstream fout(strLogFile, std::ios::app);
	if (!fout.is_open())
		return;
	
	fout << EsGetTime() << " " << strMsg << "\n";
	fout.close();
}

string gridworld::CModelUpdateServerBase::EsGetTime()
{
	SYSTEMTIME sys;
	GetLocalTime(&sys);
	string strTime = std::to_string(sys.wYear) + "/" + std::to_string(sys.wMonth) + "/" + std::to_string(sys.wDay) + " " + std::to_string(sys.wHour) + ":" + 
		std::to_string(sys.wMinute) + ":" + std::to_string(sys.wSecond) + "." + std::to_string(sys.wMilliseconds) + " ";

	return strTime;
}