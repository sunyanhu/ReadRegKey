#ifndef _OCLS_GETLOCALMACHINEOFFICEVERSION_H
#define _OCLS_GETLOCALMACHINEOFFICEVERSION_H
#include "Export.h"
#include <map>
#include <vector>
#include <windows.h>
#pragma warning(disable: 4251)
using namespace std;

namespace OCLS
{
typedef enum _U_OFFICE_TYPE_
{
    OFFICEAPP_UNINSTALL = 0,
    MS_OFFICE_2003_WORD,
    MS_OFFICE_2003_EXCEL,
    MS_OFFICE_2003_PPT,
    MS_OFFICE_2007_WORD,
    MS_OFFICE_2007_EXCEL,
    MS_OFFICE_2007_PPT,
    MS_OFFICE_2010_WORD,
    MS_OFFICE_2010_EXCEL,
    MS_OFFICE_2010_PPT,
    MS_OFFICE_2013_WORD,
    MS_OFFICE_2013_EXCEL,
    MS_OFFICE_2013_PPT,
    MS_OFFICE_2016_WORD,
    MS_OFFICE_2016_EXCEL,
    MS_OFFICE_2016_PPT,

    WPS_OFFICE_2009_WPS,
    WPS_OFFICE_2009_ET,
    WPS_OFFICE_2009_DPS,
    WPS_OFFICE_2010_WPS,
    WPS_OFFICE_2010_ET,
    WPS_OFFICE_2010_DPS,
    WPS_OFFICE_2012_WPS,
    WPS_OFFICE_2012_ET,
    WPS_OFFICE_2012_DPS,
    WPS_OFFICE_2013_WPS,
    WPS_OFFICE_2013_ET,
    WPS_OFFICE_2013_DPS,
    WPS_OFFICE_2016_WPS,
    WPS_OFFICE_2016_ET,
    WPS_OFFICE_2016_DPS,
}U_OFFICE_TYPE;

typedef struct _U_SOFT_INFO_
{
    // 软件名
    string displayName;
    // 软件版本号
    string displayVersion;
    // 软件安装目录
    string installLocation;
}U_SOFT_INFO;
class CRegistryKey;
class OCLS_GETOFFICEINFO_API CGetOfficeInfo
{
public:
    CGetOfficeInfo();
    ~CGetOfficeInfo();

    /**
    * 获取本机安装的WPS office、Microsoft office类型及版本号
    * @param           officeVersion  office安装版本信息，如：MS_Office_2013_Word, 15.1.0.5115
    * @return          false  获取office（这里office指Microsoft office及wps）信息失败或本机没有安装office
    *                   true  获取office信息成功
    * @todo                   暂没实现
    */
    bool GetLocalOfficeVersion(map<U_OFFICE_TYPE, string>& officeVersion);

    /**
    * 获取本机后缀名为*.dps;*.wps;*.et;*.doc;*.docx;*.xls;*.xlsx;*.ppt;*.pptx;类型文件默认打开方式
    * @param           defaultApp  存储后缀名及默认打开的软件名，如：*.doc, MS_Office_2013_Word
    * @return          false  获取默认打开程序失败
    *                   true  获取默认打开程序成功
    * @todo                   暂没实现
    */
    bool GetDefaultOpenMode(map<string, U_OFFICE_TYPE>& defaultOpenApp);

private:
    //保存软件安装信息
    vector<U_SOFT_INFO> mSoftwareInfo;
    //保存本机已安装的office应用程序路径及版本号
    map<string, string> m_InstallInfo;

protected:
    bool SearchRegedit(CRegistryKey regKey, REGSAM regSam);
    bool GetSoftwareInfoForPlatform(bool is64Platform);
    bool SetOfficeVersion(map<U_OFFICE_TYPE, string> &officeVersion);
};
}
#endif