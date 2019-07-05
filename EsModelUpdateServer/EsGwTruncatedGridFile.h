#ifndef ESREADSIMMODELDATA_H
#define ESREADSIMMODELDATA_H

#include <fstream>

class EsGwTruncatedGridFile
{
public:
    EsGwTruncatedGridFile(char* filePath);
    ~EsGwTruncatedGridFile();

    //@该函数用于一次性写出全部数组的全部数据，全部数据存储在pData中，interleave存储，且各个数组的元素类型相同
    bool EsWriteAllArrayData2FileAtOnce(int nLogEleNum, int nCols, int iDataComponentByteSize, void*& pData/*传入空指针，函数内部分配内存*/, __int64& nDataLen);
    bool EsExportMetaDatas(bool bSetPtr2SDS = false);

    //@一次读取文件中的多个数组的数据，每个数组只有1列元素，且元素类型相同
    bool EsReadAllArrayDataAtOnce(int& nLogEleNum, int& nCols, int& iDataComponentByteSize, void*& pData/*传入空指针，函数内部分配内存*/, __int64& nDataLen);


    //set
    void EsSetFilePath(std::string strFilePath) { m_strFilePath = strFilePath;}
    void EsSetFilePtr(FILE* fp, int nCols);

    void EsSetBigEndian(bool bBig);
    void EsSetnCols(int nCols);
    void EsSetLogicLen(int nLogicLen);
    void EsSetType(std::string sztype);//@理论上不变的元数据应该预先统一设置，不用每次追加时检测和更改
    void EsSetBytes(int nBytes);
    void EsSetUnits(std::string szUnits);


    //get
    int EsGetOffSet() { return m_nOffSet; }

    bool EsGetBigEndian() { return m_bEndian; }
    int EsGetnCols() {return m_nCols; }
    int EsGetLogicLen() { return m_nLogicLen; }
    void EsGetTypes(std::string &szType);
    void EsGetBytes(int& nBytes);
    void EsGetUnits(std::string &szUnits);


protected:

    bool EsReadMetaDatas(FILE* m_fp);
    bool _EsInverseBytesOrderofDatas(void* pData, int numDataUnit, int iEleByteSize);
    void _EsInverseByteOrder(char* pData, int count);

    int EsEstimateMetaDatasLength(int nCols);


private:
    std::string m_strFilePath;  //@输出或读入数据的文件路径

    int m_nOffSet;
    int m_nLogicLen;
    int m_nBytes;
    int m_nCols;
    bool m_bEndian;
    std::string m_szUnits;
    std::string m_szTypes;

    FILE* m_fp;
    char* m_pBuf;//文件缓冲区
};

#endif // ESREADSIMMODELDATA_H
