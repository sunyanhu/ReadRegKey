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
    // �����
    string displayName;
    // ����汾��
    string displayVersion;
    // �����װĿ¼
    string installLocation;
}U_SOFT_INFO;
class CRegistryKey;
class OCLS_GETOFFICEINFO_API CGetOfficeInfo
{
public:
    CGetOfficeInfo();
    ~CGetOfficeInfo();

    /**
    * ��ȡ������װ��WPS office��Microsoft office���ͼ��汾��
    * @param           officeVersion  office��װ�汾��Ϣ���磺MS_Office_2013_Word, 15.1.0.5115
    * @return          false  ��ȡoffice������officeָMicrosoft office��wps����Ϣʧ�ܻ򱾻�û�а�װoffice
    *                   true  ��ȡoffice��Ϣ�ɹ�
    * @todo                   ��ûʵ��
    */
    bool GetLocalOfficeVersion(map<U_OFFICE_TYPE, string>& officeVersion);

    /**
    * ��ȡ������׺��Ϊ*.dps;*.wps;*.et;*.doc;*.docx;*.xls;*.xlsx;*.ppt;*.pptx;�����ļ�Ĭ�ϴ򿪷�ʽ
    * @param           defaultApp  �洢��׺����Ĭ�ϴ򿪵���������磺*.doc, MS_Office_2013_Word
    * @return          false  ��ȡĬ�ϴ򿪳���ʧ��
    *                   true  ��ȡĬ�ϴ򿪳���ɹ�
    * @todo                   ��ûʵ��
    */
    bool GetDefaultOpenMode(map<string, U_OFFICE_TYPE>& defaultOpenApp);

private:
    //���������װ��Ϣ
    vector<U_SOFT_INFO> mSoftwareInfo;
    //���汾���Ѱ�װ��officeӦ�ó���·�����汾��
    map<string, string> m_InstallInfo;

protected:
    bool SearchRegedit(CRegistryKey regKey, REGSAM regSam);
    bool GetSoftwareInfoForPlatform(bool is64Platform);
    bool SetOfficeVersion(map<U_OFFICE_TYPE, string> &officeVersion);
};
}
#endif