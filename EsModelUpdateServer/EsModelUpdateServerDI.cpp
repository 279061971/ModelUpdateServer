#include <fstream>
#include "EsModelUpdateServerDI.h"
#include "EsGwTruncatedGridFile.h"

using std::ifstream;
using std::ofstream;
using std::ios;

gridworld::CModelUpdateServerDI::CModelUpdateServerDI()
{

}

gridworld::CModelUpdateServerDI::~CModelUpdateServerDI()
{

}

bool gridworld::CModelUpdateServerDI::EsGetUpdatedSampleData(const AttrType& nAttrType, const GridInfo& gi, const string& strUpdatedDataFile, vector<Cell>& vecCell)
{
	switch (nAttrType)
	{
	case Continuous:
		if (!TransXYZtoIJKContinuous(strUpdatedDataFile, gi, vecCell))
			return false;
		break;
	case Discrete:
		if (!TransXYZtoIJKDiscrete(strUpdatedDataFile, gi, vecCell))
			return false;
		break;
	default:
		return false;
		break;
	}
	
	return true;
}

bool gridworld::CModelUpdateServerDI::EsDownLoadData(const AttrType& nAttrType, const GridInfo& vecGIforSampleDownload, const string& strSampleFile, vector<Cell>& vecCell)
{
	//按属性类型分别获取
	if (nAttrType == Continuous)
	{
		vector<Cell> vecCellAll;
		if (!TransXYZtoIJKContinuous(strSampleFile, vecGIforSampleDownload, vecCellAll))
			return false;

		//获取样本下载范围内的原始样本
		vecCell.clear();
		for (auto cell : vecCellAll)
		{
			if(cell.nI >= vecGIforSampleDownload.nIOri && cell.nI <= (vecGIforSampleDownload.nIOri + vecGIforSampleDownload.nXNum - 1)
				&& cell.nJ >= vecGIforSampleDownload.nJOri && cell.nJ <= (vecGIforSampleDownload.nJOri + vecGIforSampleDownload.nYNum - 1)
				&& cell.nK >= vecGIforSampleDownload.nKOri && cell.nK <= (vecGIforSampleDownload.nKOri + vecGIforSampleDownload.nZNum - 1))
			{
				vecCell.push_back(cell);
			}
		}
	}
	else
	{
		vector<Cell> vecCellAll;
		if (!TransXYZtoIJKDiscrete(strSampleFile, vecGIforSampleDownload, vecCellAll))
			return false;

		//获取样本下载范围内的原始样本
		vecCell.clear();
		for (auto cell : vecCellAll)
		{
			if (cell.nI >= vecGIforSampleDownload.nIOri && cell.nI <= (vecGIforSampleDownload.nIOri + vecGIforSampleDownload.nXNum - 1)
				&& cell.nJ >= vecGIforSampleDownload.nJOri && cell.nJ <= (vecGIforSampleDownload.nJOri + vecGIforSampleDownload.nYNum - 1)
				&& cell.nK >= vecGIforSampleDownload.nKOri && cell.nK <= (vecGIforSampleDownload.nKOri + vecGIforSampleDownload.nZNum - 1))
			{
				vecCell.push_back(cell);
			}
		}
	}

	return true;
}

bool gridworld::CModelUpdateServerDI::EsDownLoadData(const AttrType& nAttrType, const GridInfo& gi, const GridInfo& giforSampleDownload, const GridInfo& giforUpdate, const string& strModelDir,
	const string& strPropertyFile, const string& strSampleFile, vector<Cell>& vecCell)
{

	//debug下输出用于插值的样本，用深探查看是否正确
#ifdef _DEBUG
	string strUsdFile = strModelDir + "\\usd.txt";
	ofstream fout(strUsdFile, ios::out);
	if (!fout.is_open())
		return false;

	fout << "Reservoir scaleuped attribute data\n";
	fout << "9\n";
	fout << "Xlocation\nYlocation\nZlocation\nPrimary\nIndex\nWellName\nTopLayer\nBottomLayer\nGlobalIndex\n";
#endif

	//按属性类型分别获取
	if (nAttrType == Continuous)
	{
		vector<int> vecI;
		vector<int> vecJ;
		vector<int> vecK;
		vector<float> vecProContinuous;
		vector<int> vecStartAndEndCellGIdxPerPlanarCell;

		string strIFile = strModelDir + "\\cell\\I.dat";
		string strJFile = strModelDir + "\\cell\\J.dat";
		string strKFile = strModelDir + "\\cell\\K.dat";
		string strLayercolumngrid_startend_per_planarcellPath = strModelDir + "\\cell\\layercolumngrid_startend_per_planarcell.dat";

		readPropertyFile(strIFile, vecI);
		readPropertyFile(strJFile, vecJ);
		readPropertyFile(strKFile, vecK);
		readPropertyFile(strPropertyFile, vecProContinuous);
		readPropertyFile(strLayercolumngrid_startend_per_planarcellPath, vecStartAndEndCellGIdxPerPlanarCell);

		vector<Cell> vecCellAll;
		if (!TransXYZtoIJKContinuous(strSampleFile, giforSampleDownload, vecCellAll))
			return false;

		//首先获取样本下载范围内的原始样本
		vecCell.clear();
		for (auto cell : vecCellAll)
		{
			if (cell.nI >= giforUpdate.nIOri && cell.nI <= (giforUpdate.nIOri + giforUpdate.nXNum - 1)
				&& cell.nJ >= giforUpdate.nJOri && cell.nJ <= (giforUpdate.nJOri + giforUpdate.nYNum - 1)
				&& cell.nK >= giforUpdate.nKOri && cell.nK <= (giforUpdate.nKOri + giforUpdate.nZNum - 1))
			{
				vecCell.push_back(cell);

#ifdef _DEBUG
				int nPlanePos = cell.nJ*gi.nXNum + cell.nI;
				int nCellGIdxStart = vecStartAndEndCellGIdxPerPlanarCell[nPlanePos * 2];
				int nCellGIdxEnd = vecStartAndEndCellGIdxPerPlanarCell[nPlanePos * 2 + 1];

				for (int gidx = nCellGIdxStart; gidx < nCellGIdxEnd; gidx++)
				{
					int k = vecK[gidx];
					if (k >= giforUpdate.nKOri && k <= (giforUpdate.nKOri + giforUpdate.nZNum - 1))
					{
						double x = gi.dXOri + gi.dXStep * (cell.nI - gi.nIOri);
						double y = gi.dYOri + gi.dYStep * (cell.nJ - gi.nJOri);
						double z = gi.dZOri + gi.dZStep * (cell.nK - gi.nKOri);

						fout << x << "\t" << y << "\t" << z << "\t" << cell.dValue << "\t" << cell.nK - gi.nKOri << "\t" << "well" << "\t" << "top" << "\t" << "bot" << "\t" << gidx << "\n";
					}
				}				
#endif

			}
		}

		//然后获取下载范围内，插值范围外的已插值结果
		for (int i = giforSampleDownload.nIOri; i < (giforSampleDownload.nIOri + giforSampleDownload.nXNum); i++)
		{
			for (int j = giforSampleDownload.nJOri; j < (giforSampleDownload.nJOri + giforSampleDownload.nYNum); j++)
			{				
				if (i >= giforUpdate.nIOri && i <= (giforUpdate.nIOri + giforUpdate.nXNum - 1)
					&& j >= giforUpdate.nJOri && j <= (giforUpdate.nJOri + giforUpdate.nYNum - 1))
				{
					continue;
				}

				int nPlanePos = j*gi.nXNum + i;

				int nCellGIdxStart = vecStartAndEndCellGIdxPerPlanarCell[nPlanePos * 2];
				int nCellGIdxEnd = vecStartAndEndCellGIdxPerPlanarCell[nPlanePos * 2 + 1];

				for (int gidx = nCellGIdxStart; gidx < nCellGIdxEnd; gidx++)
				{
					int k = vecK[gidx];
					if (k >= giforSampleDownload.nKOri && k <= (giforSampleDownload.nKOri + giforSampleDownload.nZNum - 1))
					{
						Cell cell;
						cell.nI = i;
						cell.nJ = j;
						cell.nK = k;
						cell.dValue = vecProContinuous[gidx];
						vecCell.push_back(cell);

#ifdef _DEBUG
						double x = gi.dXOri + gi.dXStep * (i - gi.nIOri);
						double y = gi.dYOri + gi.dYStep * (j - gi.nJOri);
						double z = gi.dZOri + gi.dZStep * (k - gi.nKOri);
						fout << x << "\t" << y << "\t" << z << "\t" << vecProContinuous[gidx] << "\t" << k - gi.nKOri << "\t" << "well" << "\t" << "top" << "\t" << "bot" << "\t" << gidx << "\n";
#endif

					}
				}
			}			
		}
	}
	else
	{
		vector<int> vecI;
		vector<int> vecJ;
		vector<int> vecK;
		vector<short> vecProDiscrete;
		vector<int> vecStartAndEndCellGIdxPerPlanarCell;

		string strIFile = strModelDir + "\\cell\\I.dat";
		string strJFile = strModelDir + "\\cell\\J.dat";
		string strKFile = strModelDir + "\\cell\\K.dat";
		string strLayercolumngrid_startend_per_planarcellPath = strModelDir + "\\cell\\layercolumngrid_startend_per_planarcell.dat";

		readPropertyFile(strIFile, vecI);
		readPropertyFile(strJFile, vecJ);
		readPropertyFile(strKFile, vecK);
		readPropertyFile(strPropertyFile, vecProDiscrete);
		readPropertyFile(strLayercolumngrid_startend_per_planarcellPath, vecStartAndEndCellGIdxPerPlanarCell);

		vector<Cell> vecCellAll;
		if (!TransXYZtoIJKDiscrete(strSampleFile, giforSampleDownload, vecCellAll))
			return false;

		//首先获取样本下载范围内的原始样本
		vecCell.clear();
		for (auto cell : vecCellAll)
		{
			if (cell.nI >= giforUpdate.nIOri && cell.nI <= (giforUpdate.nIOri + giforUpdate.nXNum - 1)
				&& cell.nJ >= giforUpdate.nJOri && cell.nJ <= (giforUpdate.nJOri + giforUpdate.nYNum - 1)
				&& cell.nK >= giforUpdate.nKOri && cell.nK <= (giforUpdate.nKOri + giforUpdate.nZNum - 1))
			{
				vecCell.push_back(cell);

#ifdef _DEBUG
				int nPlanePos = cell.nJ*gi.nXNum + cell.nI;
				int nCellGIdxStart = vecStartAndEndCellGIdxPerPlanarCell[nPlanePos * 2];
				int nCellGIdxEnd = vecStartAndEndCellGIdxPerPlanarCell[nPlanePos * 2 + 1];

				for (int gidx = nCellGIdxStart; gidx < nCellGIdxEnd; gidx++)
				{
					int k = vecK[gidx];
					if (k >= giforUpdate.nKOri && k <= (giforUpdate.nKOri + giforUpdate.nZNum - 1))
					{
						double x = gi.dXOri + gi.dXStep * (cell.nI - gi.nIOri);
						double y = gi.dYOri + gi.dYStep * (cell.nJ - gi.nJOri);
						double z = gi.dZOri + gi.dZStep * (cell.nK - gi.nKOri);

						fout << x << "\t" << y << "\t" << z << "\t" << cell.nValue << "\t" << cell.nK - gi.nKOri << "\t" << "well" << "\t" << "top" << "\t" << "bot" << "\t" << gidx << "\n";
					}
				}
#endif

			}
		}

		//然后获取下载范围内，插值范围外的已插值结果
		for (int i = giforSampleDownload.nIOri; i < (giforSampleDownload.nIOri + giforSampleDownload.nXNum); i++)
		{
			for (int j = giforSampleDownload.nJOri; j < (giforSampleDownload.nJOri + giforSampleDownload.nYNum); j++)
			{
				if (i >= giforUpdate.nIOri && i <= (giforUpdate.nIOri + giforUpdate.nXNum - 1)
					&& j >= giforUpdate.nJOri && j <= (giforUpdate.nJOri + giforUpdate.nYNum - 1))
				{
					continue;
				}

				int nPlanePos = j*gi.nXNum + i;

				int nCellGIdxStart = vecStartAndEndCellGIdxPerPlanarCell[nPlanePos * 2];
				int nCellGIdxEnd = vecStartAndEndCellGIdxPerPlanarCell[nPlanePos * 2 + 1];

				for (int gidx = nCellGIdxStart; gidx < nCellGIdxEnd; gidx++)
				{
					int k = vecK[gidx];
					if (k >= giforSampleDownload.nKOri && k <= (giforSampleDownload.nKOri + giforSampleDownload.nZNum - 1))
					{
						Cell cell;
						cell.nI = i;
						cell.nJ = j;
						cell.nK = k;
						cell.nValue = vecProDiscrete[gidx];
						vecCell.push_back(cell);

#ifdef _DEBUG
						double x = gi.dXOri + gi.dXStep * (i - gi.nIOri);
						double y = gi.dYOri + gi.dYStep * (j - gi.nJOri);
						double z = gi.dZOri + gi.dZStep * (k - gi.nKOri);
						fout << x << "\t" << y << "\t" << z << "\t" << vecProDiscrete[gidx] << "\t" << k - gi.nKOri << "\t" << "well" << "\t" << "top" << "\t" << "bot" << "\t" << gidx << "\n";
#endif

					}
				}
			}
		}
	}

#ifdef _DEBUG
	fout.close();
#endif

	return true;
}

bool gridworld::CModelUpdateServerDI::EsUpLoadData(const AttrType& nAttrType, const int nCurrentZoneIdx, const string& strModelDir, const string& strPropertyFile, ModelInterPara& tmip)
{
	vector<int> vecI;
	vector<int> vecJ;
	vector<int> vecK;
	vector<int> vecZoneIdx;
	vector<short> vecProDiscrete;
	vector<float> vecProContinuous;

	string strIFile = strModelDir + "\\cell\\I.dat";
	string strJFile = strModelDir + "\\cell\\J.dat";
	string strKFile = strModelDir + "\\cell\\K.dat";
	string strZoneFile = strModelDir + "\\cell\\cell_zoneidx.dat";

	readPropertyFile(strIFile, vecI);
	readPropertyFile(strJFile, vecJ);
	readPropertyFile(strKFile, vecK);
	readPropertyFile(strZoneFile, vecZoneIdx);
	if (nAttrType == Discrete)
		readPropertyFile(strPropertyFile, vecProDiscrete);
	else
		readPropertyFile(strPropertyFile, vecProContinuous);


	vector<int> vecGlobalIdx;
	for (size_t i = 0; i < vecZoneIdx.size(); i++)
	{
		if (vecZoneIdx[i] == nCurrentZoneIdx)
		{
			vecGlobalIdx.push_back((int)i);
		}
	}

	if (nAttrType == Discrete)
	{
		if (vecProDiscrete.size() == 0 || vecI.size() == 0 || vecJ.size() == 0 ||
			vecK.size() == 0 || vecZoneIdx.size() == 0 || vecGlobalIdx.size() == 0)
			return false;

		int minK = INT_MAX;
		for (size_t i = 0; i < vecGlobalIdx.size(); i++)
		{
			if (minK > vecK[vecGlobalIdx[i]])
			{
				minK = vecK[vecGlobalIdx[i]];
			}
		}
		for (size_t i = 0; i < vecGlobalIdx.size(); i++)
		{
			int iIndex = vecI[vecGlobalIdx[i]];
			int jIndex = vecJ[vecGlobalIdx[i]];
			int kIndex = vecK[vecGlobalIdx[i]] - minK;

			if (iIndex >= tmip.gi.nIOri && iIndex <= (tmip.gi.nIOri + tmip.gi.nXNum - 1)
				&& jIndex >= tmip.gi.nJOri && jIndex <= (tmip.gi.nJOri + tmip.gi.nYNum - 1))
			{
				int iPos = kIndex*tmip.gi.nXNum*tmip.gi.nYNum + (jIndex - tmip.gi.nJOri)*tmip.gi.nXNum + (iIndex - tmip.gi.nIOri);

				vecProDiscrete[vecGlobalIdx[i]] = tmip.vecDiscreteProp[iPos];
			}
		}

		writePropertyData(strPropertyFile, vecProDiscrete);
	}
	else if (tmip.nAttrType == Continuous)
	{
		if (tmip.vecContinuousProp.size() == 0 || vecI.size() == 0 || vecJ.size() == 0 ||
			vecK.size() == 0 || vecZoneIdx.size() == 0 || vecGlobalIdx.size() == 0)
			return false;

		int minK = INT_MAX;
		for (size_t i = 0; i < vecGlobalIdx.size(); i++)
		{
			if (minK > vecK[vecGlobalIdx[i]])
			{
				minK = vecK[vecGlobalIdx[i]];
			}
		}
		for (size_t i = 0; i < vecGlobalIdx.size(); i++)
		{
			int iIndex = vecI[vecGlobalIdx[i]];
			int jIndex = vecJ[vecGlobalIdx[i]];
			int kIndex = vecZoneIdx[vecGlobalIdx[i]] - minK;

			if (iIndex >= tmip.gi.nIOri && iIndex <= (tmip.gi.nIOri + tmip.gi.nXNum - 1)
				&& jIndex >= tmip.gi.nJOri && jIndex <= (tmip.gi.nJOri + tmip.gi.nYNum - 1))
			{
				int iPos = kIndex*tmip.gi.nXNum*tmip.gi.nYNum + (jIndex - tmip.gi.nJOri)*tmip.gi.nXNum + (iIndex - tmip.gi.nIOri);

				vecProContinuous[vecGlobalIdx[i]] = tmip.vecContinuousProp[iPos];
			}		
		}

		writePropertyData(strPropertyFile, vecProContinuous);
	}

	return true;
}

void gridworld::CModelUpdateServerDI::split(const string& source, vector<string>& dest)
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

bool gridworld::CModelUpdateServerDI::TransXYZtoIJKContinuous(const string& strXYZFile, const GridInfo& gi, vector<Cell>& vecCell)
{
	ifstream fin(strXYZFile, ios::in);
	if (!fin.is_open())
		return false;

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
		int k_local = atoi(vecStrData[4].c_str());

		int i = round_double((x - gi.dXOri) / gi.dXStep) + gi.nIOri;
		int j = round_double((y - gi.dYOri) / gi.dYStep) + gi.nJOri;
		int k = k_local + gi.nKOri;

		Cell c(i, j, k, v);
		vecCell.push_back(c);
	}

	fin.close();

	return true;
}

bool gridworld::CModelUpdateServerDI::TransXYZtoIJKDiscrete(const string& strXYZFile, const GridInfo& gi, vector<Cell>& vecCell)
{
	ifstream fin(strXYZFile, ios::in);
	if (!fin.is_open())
		return false;

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
		int k_local = atoi(vecStrData[4].c_str());

		int i = round_double((x - gi.dXOri) / gi.dXStep + gi.nIOri);
		int j = round_double((y - gi.dYOri) / gi.dYStep + gi.nJOri);
		int k = k_local + gi.nKOri;

		Cell c(i, j, k, v);
		vecCell.push_back(c);
	}

	fin.close();

	return true;
}

template<class T>
bool gridworld::CModelUpdateServerDI::readPropertyFile(string strFilePath, vector<T>& vecProp_T)
{
	int nLogEleNum;
	int nCols;
	int iDataComponentByteSize;
	void *pData;
	long long nDataLen;


	EsGwTruncatedGridFile simModelFile((char*)strFilePath.data());
	if (!simModelFile.EsReadAllArrayDataAtOnce(nLogEleNum, nCols, iDataComponentByteSize, pData, nDataLen))
		return false;

	T* pDataT = (T*)pData;

	if (nLogEleNum < 1)
		return false;

	vecProp_T.resize(nLogEleNum*nCols);
	for (int i = 0; i < nLogEleNum*nCols; i++)
	{
		vecProp_T[i] = pDataT[i];
	}

	return true;
}

template<class T>
bool gridworld::CModelUpdateServerDI::writePropertyData(string filePath, vector<T> &vecProp_T)
{
	FILE* fp;
	errno_t err;
	err = fopen_s(&fp, filePath.c_str(), "wb");
	if (err != 0)
		return false;

	if (fp)
	{
		int nArray = 1;
		int nEle = vecProp_T.size();
		string strUnit;//?
		string strType = typeid(vecProp_T[0]).name();
		int nBytes = sizeof(T);


		void* pData = &(vecProp_T[0]);
		long long nDataLen = nArray*nEle*nBytes;


		EsGwTruncatedGridFile simModelFile((char*)filePath.data());

		simModelFile.EsSetFilePtr(fp, nArray);
		simModelFile.EsSetLogicLen(nEle);
		simModelFile.EsSetUnits(strUnit);
		simModelFile.EsSetType(strType);
		simModelFile.EsSetBytes(nBytes);


		bool bRet(false);
		if (simModelFile.EsExportMetaDatas(true))
		{
			if (simModelFile.EsWriteAllArrayData2FileAtOnce(nEle, nArray, nBytes, pData, nDataLen))
			{
				bRet = true;
			}
		}

		fclose(fp);
		return bRet;
	}

	return false;

}