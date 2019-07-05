
#if !defined(ES_CHECK_TIME)
#define ES_CHECK_TIME

//#include "EsMath/EsArchiveBase.h"
//#include "EsPlatform/EsSystemUtil.h"
#include <time.h>
 

//extern DWORD dLastCheckTime;
#define  _TIME_YEAR 2020
#define _TIME_MONTH 6
#define  _TIME_CHECK_INTERVAL 1000*60*5   

//extern DWORD dLastCheckTime;
//BOOL bResult; DWORD dLastCheckTime;                
//#define  CHECK_TIME_VALID( bResult, dLastCheckTime)  
//versionInfo.dwMajorVersion>=5  win2k以后windows

//Operating system            			Version number dwMajorVersion dwMinorVersion        Other 
//Windows Server 2008         			   	 6.0            6                  0           OSVERSIONINFOEX.wProductType != VER_NT_WORKSTATION 
//Windows Vista               			   	 6.0            6                  0           OSVERSIONINFOEX.wProductType == VER_NT_WORKSTATION 
//Windows Server 2003 R2      			   	 5.2            5                  2           GetSystemMetrics(SM_SERVERR2) != 0 
//Windows Home Server         			   	 5.2            5                  2           OSVERSIONINFOEX.wSuiteMask == VER_SUITE_WH_SERVER 
//Windows Server 2003         			   	 5.2            5                  2           GetSystemMetrics(SM_SERVERR2) == 0 
//Windows XP Professional x64 Edition        5.2            5                  2           (OSVERSIONINFOEX.wProductType == VER_NT_WORKSTATION) && (SYSTEM_INFO.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_AMD64) 
//Windows XP                                 5.1            5                  1           Not applicable 
//Windows 2000                               5.0            5                  0           Not applicable 

//以下主版本号为5时主要考虑xp 和主版本号为6是主要考虑vista系统。
//void _EXPESMATH CHECK_TIME_VALID( BOOL& bResult, DWORD& dLastCheckTime); 

//
#define USING_FILESYTEM_TIME
#ifdef USING_FILESYTEM_TIME
#include <ShlObj.h>

#define CHECK_TIME_VALID(bResult,dLastCheckTime)                                                              \
{                                                                                                             \
    bResult=TRUE;                                                                                             \
    DWORD dNow = GetTickCount();                                                                              \
    if (fabs((double)dNow - dLastCheckTime) > _TIME_CHECK_INTERVAL)                                           \
    {                                                                                                         \
        OSVERSIONINFOEX versionInfo;                                                                          \
        versionInfo.dwOSVersionInfoSize = sizeof (OSVERSIONINFOEX);                                           \
        BOOL bGetOSInfo =::GetVersionEx ((OSVERSIONINFO*)&versionInfo);                                       \
        if (bGetOSInfo&&versionInfo.dwMajorVersion>=5                                                         \
            &&(versionInfo.wProductType ==VER_NT_WORKSTATION ||versionInfo.wProductType ==VER_NT_SERVER ))    \
        {                                                                                                     \
            dLastCheckTime = dNow;                                                                            \
            TCHAR szPath[1024];                                                                               \
            BOOL bFilePathValid=FALSE;                                                                        \
            if(versionInfo.dwMajorVersion == 5)                                                               \
            {                                                                                                 \
                if (S_OK == SHGetFolderPath( NULL,CSIDL_SENDTO,NULL,SHGFP_TYPE_DEFAULT,szPath))               \
                {                                                                                             \
                    int locat = lstrlen(szPath);                                                              \
                    TCHAR* p = szPath + locat - 6;                                                            \
                    _tcscpy(p,_T("ntuser.dat.LOG"));                                                          \
                    bFilePathValid=TRUE;                                                                      \
                }                                                                                             \
            }                                                                                                 \
            else                                                                                              \
            {                                                                                                 \
                if (S_OK == SHGetFolderPath( NULL,CSIDL_PROFILE,NULL,SHGFP_TYPE_DEFAULT,szPath))              \
                {                                                                                             \
                    int locat = lstrlen(szPath);                                                              \
                    TCHAR* p = szPath + locat ;                                                               \
                    _tcscpy(p,_T("\\NTUSER.DAT"));                                                            \
                    bFilePathValid=TRUE;                                                                      \
                }                                                                                             \
            }                                                                                                 \
            WIN32_FILE_ATTRIBUTE_DATA data;                                                                   \
            if (bFilePathValid&&GetFileAttributesEx(szPath , GetFileExInfoStandard, &data))                   \
            {                                                                                                 \
                SYSTEMTIME sysTime1, sysTime2;                                                                \
                GetLocalTime( &sysTime1 );                                                                    \
                FileTimeToSystemTime(&data.ftLastWriteTime ,&sysTime2);                                       \
                                                                                                              \
                WORD y = sysTime1.wYear>sysTime2.wYear ? sysTime1.wYear : sysTime2.wYear;                     \
                WORD m = sysTime1.wYear>sysTime2.wYear ? sysTime1.wMonth : sysTime2.wMonth;                   \
                                                                                                              \
                if (y > _TIME_YEAR)                                                                           \
                {                                                                                             \
                    bResult = FALSE;                                                                          \
                }                                                                                             \
                else                                                                                          \
                {                                                                                             \
                    if(y==_TIME_YEAR && m>_TIME_MONTH)                                                        \
                        bResult = FALSE;                                                                      \
                }                                                                                             \
            }                                                                                                 \
            else                                                                                              \
                bResult = FALSE;                                                                              \
        }                                                                                                     \
        else                                                                                                  \
            bResult=FALSE;                                                                                    \
    }                                                                                                         \
}                                                                                                             
#else
#define CHECK_TIME_VALID(bResult,dLastCheckTime)            \
{          \
    bResult=TRUE;\
    DWORD dNow = EsGetTickCount();    \
    if(fabs((double)dNow - dLastCheckTime) > _TIME_CHECK_INTERVAL)    \
    {\
        time_t timep;\
        time(&timep);\
        struct tm *p;\
        p = gmtime(&timep);\
        if(p->tm_year > _TIME_YEAR)     \
            bResult = FALSE;                    \
        if(p->tm_year == _TIME_YEAR && p->tm_mon > _TIME_MONTH)   \
            bResult = FALSE;    \
    }\
 }
#endif  


#endif
