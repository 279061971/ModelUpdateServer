#include <tchar.h>
#include "windows.h"
#include "EsCheckTime.h"
#include "EsModelInterCoreDll.h"


bool gridworld::EsInterFunc(const string& strInterParaFile, int nInterMethod)
{
	DWORD dLastCheckTime = 0;
	static BOOL B = TRUE;
	if(B)
	{
		BOOL bResult = TRUE;
		CHECK_TIME_VALID(bResult, dLastCheckTime);

		if(bResult)
		{
			SHELLEXECUTEINFOA ShExecInfo = {0};
			ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFOA);
			ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
			ShExecInfo.hwnd = NULL;
			ShExecInfo.lpVerb = NULL;

			ShExecInfo.lpParameters = strInterParaFile.c_str(); //参数
			ShExecInfo.lpDirectory = NULL;
			ShExecInfo.nShow = SW_HIDE;
			ShExecInfo.hInstApp = NULL;

			char chpath[MAX_PATH];  
			GetModuleFileNameA(NULL, chpath, sizeof(chpath));
			string strExePath = (string)chpath;
			int pos = (int)strExePath.find_last_of('\\', strExePath.length());
			string strCurrentDir = strExePath.substr(0, pos);

			switch (nInterMethod)
			{
			case 0: //SequentialGaussianSimulation:
				strExePath = strCurrentDir + "\\sgsim.exe";
				ShExecInfo.lpFile = strExePath.c_str(); //exe路径    
				break;
			case 1: //Kriging:
				strExePath = strCurrentDir + "\\kt3d.exe";
				ShExecInfo.lpFile = strExePath.c_str(); //exe路径    
				break;
			case 2: //SequentialIndicatorSimulation:
				strExePath = strCurrentDir + "\\sisim.exe";
				ShExecInfo.lpFile = strExePath.c_str(); //exe路径    
				break;
			case 3: //IndicatorKriging:
				strExePath = strCurrentDir + "\\ik3d.exe";
				ShExecInfo.lpFile = strExePath.c_str(); //exe路径   
				break;
			default:
				return false;
			} 

			ShellExecuteExA(&ShExecInfo);
			WaitForSingleObject(ShExecInfo.hProcess, INFINITE);

			return TRUE;
		}
		else
			B = FALSE;
	}	

	return FALSE;
}
