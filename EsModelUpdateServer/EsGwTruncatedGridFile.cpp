#include "EsGwTruncatedGridFile.h"
#include <vector>
#include <stdio.h>
#include <string>
#ifndef WIN32
#include <stdlib.h>
//#define strtok_s strtok_r
#endif

using namespace std;

const static std::string g_szOffset = "offset";
const static std::string g_szDim = "dim";
const static std::string g_szUnits = "units";
const static std::string g_szTypes = "type";
const static std::string g_szBytes = "nbytes";
const static std::string g_szCols = "ncols";
const static std::string g_szEndian = "big";
const static std::string g_szDelimeter = "= \t";
const static std::string g_szLineEnd = "\n";

EsGwTruncatedGridFile::EsGwTruncatedGridFile(char* filePath)
    :m_nOffSet(128), /*m_nCols(1),*/ m_nCols(0), m_bEndian(false), m_nLogicLen(0), m_pBuf(NULL)//@
                , m_szTypes(""), m_szUnits(""), m_nBytes(0)
{
    m_strFilePath = filePath;
}

EsGwTruncatedGridFile::~EsGwTruncatedGridFile()
{
    if (m_fp != NULL)
    {
        //fflush(m_fp);
        //setbuf(m_fp, NULL);

        fclose(m_fp);
        m_fp = NULL;
    }

    if(m_pBuf)
    {
        delete []m_pBuf;
        m_pBuf = NULL;
    }
}

bool EsGwTruncatedGridFile::EsWriteAllArrayData2FileAtOnce(int nLogEleNum, int nCols, int iDataComponentByteSize, void*& pData/*传入空指针，函数内部分配内存*/, __int64& nDataLen)
{
    //判断当前输入参数合理
    __int64 nLen = (__int64)nLogEleNum*nCols*iDataComponentByteSize;
    if (nDataLen != nLen)
        return false;

    if (m_nCols != nCols || m_nLogicLen != nLogEleNum || m_nBytes != iDataComponentByteSize)
        return false;

    fseek(m_fp, 0, SEEK_END);


    #ifdef WIN32
                __int64 nFileLen = _ftelli64(m_fp);
    #else
                __int64 nFileLen = ftell(m_fp);
    #endif


    if (nFileLen != m_nOffSet)
        return false;

    if (m_bEndian && pData)
    {
        _EsInverseBytesOrderofDatas(pData, nLogEleNum*nCols, iDataComponentByteSize);
    }

    if(pData)
    {
        //modified by yxl,20170613
        //当数据文件大于1G时,分次写出
        __int64 nDataStr = 0;
        __int64 nMaxWrite = 1000000000;
        __int64 nRealWrite = 0;
        do {
            if (nDataLen-nDataStr > nMaxWrite)
            {
                nRealWrite = nMaxWrite;
            }
            else
            {
                nRealWrite = nDataLen - nDataStr;
            }

            fwrite((char*)pData+nDataStr, nRealWrite, 1,  m_fp);
            fflush(m_fp);

            nDataStr += nRealWrite;
        } while (nDataLen > nDataStr);
    }

    return true;

}

bool EsGwTruncatedGridFile::EsExportMetaDatas(bool bSetPtr2SDS )
        {
            //判断文件metadatas总长度小于offset

            //开始输出metadatas
#ifdef WIN32
            _fseeki64(m_fp, 0L, SEEK_SET);
#else
            fseek(m_fp, 0L, SEEK_SET);
#endif
            char *szMeta = new char[m_nOffSet];
            memset(szMeta, 0, m_nOffSet);
            //int nbytes = sizeof(char);
            fwrite(szMeta, sizeof(char)*m_nOffSet, 1, m_fp);


#ifdef WIN32
            _fseeki64(m_fp, 0L, SEEK_SET);
#else
            fseek(m_fp, 0L, SEEK_SET);
#endif
            //offset
#ifdef WIN32
            sprintf_s(szMeta, m_nOffSet, "%s=%d\n", g_szOffset.c_str(), m_nOffSet);
#else
            snprintf(szMeta, m_nOffSet, "%s=%d\n", g_szOffset.c_str(), m_nOffSet);
#endif
            int nDataLen = strlen(szMeta);
            fwrite(szMeta, nDataLen, 1, m_fp);

            //units
            string strUnits = g_szUnits + "=" + m_szUnits + "\n";
            nDataLen =strUnits.length();
            fwrite(strUnits.c_str(), nDataLen, 1, m_fp);


            //@array number 和ncols其实是一个东西。一个数组只有1列！
#ifdef WIN32
            sprintf_s(szMeta, m_nOffSet, "%s=%d\n", g_szCols.c_str(), m_nCols);
#else
            snprintf(szMeta, m_nOffSet, "%s=%d\n", g_szCols.c_str(), m_nCols);
#endif
            nDataLen = strlen(szMeta);
            fwrite(szMeta, nDataLen, 1, m_fp);

            //number of data in each array
#ifdef WIN32
                sprintf_s(szMeta, m_nOffSet, "%s=%d\n", g_szDim.c_str(), m_nLogicLen);
#else
                snprintf(szMeta, m_nOffSet, "%s=%d\n", g_szDim.c_str(), m_nLogicLen);
#endif
            nDataLen = strlen(szMeta);
            fwrite(szMeta, nDataLen, 1, m_fp);

            //type and bytes
            string strType = g_szTypes + "=" + m_szTypes + "\n";
            nDataLen =strType.length();
            fwrite(strType.c_str(), nDataLen, 1, m_fp);


#ifdef WIN32
                sprintf_s(szMeta, m_nOffSet, "%s=%d\n", g_szBytes.c_str(), m_nBytes);
#else
                snprintf(szMeta, m_nOffSet, "%s=%d\n", g_szBytes.c_str(), m_nBytes);
#endif
            nDataLen = strlen(szMeta);
            fwrite(szMeta, nDataLen, 1, m_fp);

            //endian
            int nBig = m_bEndian;
#ifdef WIN32
                sprintf_s(szMeta, m_nOffSet, "%s=%d\n", g_szEndian.c_str(), nBig);
#else
                snprintf(szMeta, m_nOffSet, "%s=%d\n", g_szEndian.c_str(), nBig);
#endif

            nDataLen = strlen(szMeta);
            fwrite(szMeta, nDataLen, 1, m_fp);
/*
            char chSub = 0x1a;
#ifdef WIN32
                sprintf_s(szMeta, m_nOffSet, "<<\n%c", chSub);
#else
                snprintf(szMeta, m_nOffSet, "<<\n%c", chSub);
#endif

            nDataLen = strlen(szMeta);
            fwrite(szMeta, nDataLen, 1, m_fp);
*/
            //free memory
            delete []szMeta, szMeta = NULL;

            //判断当前metadatas长度和offset，超过offset提示错误

            __int64 nLen = _ftelli64(m_fp);


            if(bSetPtr2SDS)//start pos of data section
            {
#ifdef WIN32
                _fseeki64(m_fp, m_nOffSet, SEEK_SET);
#else
                fseek(m_fp, m_nOffSet, SEEK_SET);
#endif
            }

            return nLen <= m_nOffSet;
        }


bool EsGwTruncatedGridFile::EsReadMetaDatas(FILE* m_fp)
{
    m_nOffSet = 64;  //元数据缺省长度为64
    m_bEndian = 0;
    m_nCols = 0;
    m_nLogicLen = 0;

    std::string g_szOffset = "offset";
    std::string g_szDim = "dim";
    std::string g_szUnits = "units";
    std::string g_szTypes = "type";
    std::string g_szBytes = "nbytes";
    std::string g_szCols = "ncols";
    std::string g_szEndian = "big";
    //std::string g_szDelimeter = "=";
    //std::string g_szLineEnd = "\n";

    fseek(m_fp, 0, SEEK_SET);

    char szBuf[64];
    int nBufSize = 64;
    memset(&szBuf, 0, nBufSize);

    int nOffset = 0;
    //vector<int> vecBytes;
    //vector<string> vecTypes;
    while (nOffset < m_nOffSet)
    {
        fgets(&szBuf[0], nBufSize, m_fp);
        nOffset = ftell(m_fp);

        const char* pDelim = "= \t\n";
        char* ptr = NULL;
        char* ptrVal = NULL;
        ptrVal = strtok_s(szBuf, pDelim, &ptr);
        std::vector<std::string> vecSzVal;

        while (ptrVal != NULL)
        {
            vecSzVal.push_back(ptrVal);
            ptrVal = strtok_s(NULL, pDelim, &ptr);
        }

        if (vecSzVal.size() < 1)
            break;

        if (vecSzVal[0].compare(g_szOffset.c_str()) == 0)
        {
            m_nOffSet = atoi(vecSzVal[1].c_str());
            //ASSERT(m_nOffSet > 0 && m_nOffSet % 64 == 0);
            continue;
        }

        int nEle = vecSzVal.size();

        if (vecSzVal[0].compare(g_szDim) == 0 && nEle==2)
        {
            m_nLogicLen = atoi(vecSzVal[1].c_str());
            //ASSERT(m_nLogicLen > 0);
            continue;
        }

        if (vecSzVal[0].compare(g_szUnits) == 0  && nEle == 2)
        {
            m_szUnits = vecSzVal[1];
            continue;
        }

        if (vecSzVal[0].compare(g_szTypes) == 0 && nEle== 2)
        {
            m_szTypes = vecSzVal[1];
            continue;
        }

        if (vecSzVal[0].compare(g_szBytes) == 0 && nEle>= 2)
        {
            m_nBytes = atoi(vecSzVal[1].c_str());

            //ASSERT(m_nBytes > 0);
            continue;
        }

        if (vecSzVal[0].compare(g_szCols) == 0  && nEle>= 2)
        {
            m_nCols = atoi(vecSzVal[1].c_str());

            continue;
        }

        if (vecSzVal[0].compare(g_szEndian) == 0  && nEle>= 2)
        {
            m_bEndian = atoi(vecSzVal[1].c_str());

            continue;
        }

        if (vecSzVal[0].compare("<<") == 0)
        {
            break;
        }
    }

    //检验文件参数正确性
    //offset
    if (m_nOffSet <=0 || m_nOffSet % 64 != 0)
    {
        return false;
    }

    //pes and bytes / units
    if (m_nBytes <= 0 || m_szTypes.empty() /*|| m_szUnits.empty()*/)
        return false;

    //nArray
    if (m_nCols < 1)
        return false;


    //data length
    __int64 nEleLen = m_nBytes;
    __int64 nDateLen = nEleLen*m_nCols*m_nLogicLen + m_nOffSet;


    #ifdef WIN32
                _fseeki64(m_fp, 0L, SEEK_END);
                __int64 nFileLen = _ftelli64(m_fp);
    #else
                fseek(m_fp, 0L, SEEK_END);
                __int64 nFileLen = ftell(m_fp);
    #endif

                if (nDateLen != nFileLen)
                    return false;

                return true;
}

bool EsGwTruncatedGridFile::EsReadAllArrayDataAtOnce(int& nLogEleNum, int& nCols, int& iDataComponentByteSize, void*& pData/*传入空指针，函数内部分配内存*/, __int64& nDataLen)
{
//    FILE *m_fp;
//    m_fp = NULL;
	errno_t err;
    err = fopen_s(&m_fp, m_strFilePath.data(), "rb");
	if (err != 0)
		return false;

    if(m_fp == NULL)
        return false;

    if (!EsReadMetaDatas(m_fp))
        return false;

    nLogEleNum = m_nLogicLen;
    nCols = m_nCols;
    iDataComponentByteSize = m_nBytes;
    nDataLen = (__int64)nCols*iDataComponentByteSize*nLogEleNum;
    pData = new char[nDataLen];


    fseek(m_fp, m_nOffSet, SEEK_SET);
    if (fread(pData, 1, nDataLen, m_fp) != nDataLen)
        return false;

    if (m_bEndian == true)
    {
        int nTotalEle = m_nCols*m_nLogicLen;
        if (!_EsInverseBytesOrderofDatas(pData, nTotalEle, iDataComponentByteSize))
            return false;
    }

    fclose(m_fp);
    m_fp = NULL;

    return true;
}

bool EsGwTruncatedGridFile::_EsInverseBytesOrderofDatas(void *pData, int numDataUnit, int iEleByteSize)
{
    //@为了效率
    //ASSERT(m_bEndian == true);

    if (pData == NULL)
        return false;

    if(iEleByteSize != 8 && iEleByteSize !=4 && iEleByteSize != 2)
        return false;

    int i ;
    for( i= 0; i < numDataUnit; i++)
    {
        _EsInverseByteOrder((char*)((char*)pData+(__int64)i*iEleByteSize), iEleByteSize);
    }
    return true;
}

void EsGwTruncatedGridFile::_EsInverseByteOrder(char* pData, int count)
{
    char tmp;
    //
    //ASSERT(count > 0);
    int half = count/2;
    for(int i= 0; i < half; i++)
    {
        tmp = pData[i];
        pData[i]=pData[count-1-i];
        pData[count-1-i] = tmp;
    }
}

int EsGwTruncatedGridFile::EsEstimateMetaDatasLength(int nCols)
{
    int nLen = 8 + sizeof(int) +  /*"offset=?\n"*/
        6 + sizeof(int) +  /*"nval=?\n or dim=?\n"*/
        7 + 8  + /*"units=? ? ...?\n", ?为units, 预留8字节*/
        6 + 8  + /*"type=？ ？...?\n", ?为type，平均预留8字节*/
        8 + sizeof(int) +  /*"nbytes=? ?...?\n", ?代表对应的type类型数据需要的字节数 */
        5 + sizeof(int) + /*"big=0 or 1\n"*/
        7 + sizeof(int) /*"ncols=?\n"*/;

    //int nLen = 42+12+(40)*nArray;
    nLen = (nLen / 64 + 1) *64;

    return nLen;
}

void EsGwTruncatedGridFile::EsSetFilePtr(FILE* fp, int nCols)
{
    m_fp = fp;
    m_nCols = nCols;
    m_nOffSet = EsEstimateMetaDatasLength(nCols);
    //
    if(m_pBuf)
    {
        delete []m_pBuf;
        m_pBuf = NULL;
    }
    int iBufferSize = 8388608;//8M
    m_pBuf = new char[ iBufferSize ];
    //setbuf(m_fp, m_pBuf);  //error!
    if(m_fp&&m_pBuf)
        setvbuf ( m_fp , m_pBuf , _IOFBF , iBufferSize );
}

void EsGwTruncatedGridFile::EsSetBigEndian(bool bBigEndian)
{
    m_bEndian = bBigEndian;
}

void EsGwTruncatedGridFile::EsSetnCols(int nCols)
{
    m_nCols = nCols;
}

void EsGwTruncatedGridFile::EsSetLogicLen(int nLogicLen)
{
    m_nLogicLen= nLogicLen;
}

void EsGwTruncatedGridFile::EsSetType(string sztype)//@理论上不变的元数据应该预先统一设置，不用每次追加时检测和更改
{
    m_szTypes = sztype;
}

void EsGwTruncatedGridFile::EsSetBytes(int nBytes)
{
    m_nBytes = nBytes;
}

void EsGwTruncatedGridFile::EsSetUnits(string szUnits)
{
    m_szUnits = szUnits;
}




