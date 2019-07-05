#pragma once

#include "EsGlobalDef.h"
#include "EsModelInterToolDll.h"
#include "windows.h"

namespace gridworld
{
	class EXPORT_MODELINTERSERVER CModelInterServer
	{
	public:
		CModelInterServer();
		~CModelInterServer();

	private:
		vector<Record> m_vecWellDataUsedToEvaluate;
		vector<Cell> m_vecCellUsedToEvaluate;
		GridInfo m_GlobalGI;

	};
}