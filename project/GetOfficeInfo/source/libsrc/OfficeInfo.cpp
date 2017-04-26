#include "GetOfficeInfo\OfficeInfo.h"
#include <io.h>
#include "winSystem/WinSystem.h"
#include "RegistryKey/RegistryKey.h"
#include "RegistryKey/RegKeyIterator.h"
#include <algorithm>
#include <Shlwapi.h>
#include <ShlGuid.h>

#define U_SOFTWARE_WOW6432NODE_KEY  "SOFTWARE\\Wow6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall"
#define U_SOFTWARE_SETUP_KEY  "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall"
#define U_WINWORD_EXE "winword.exe"
#define U_EXCLE_EXE "excel.exe"
#define U_POWERPNT_EXE "powerpnt.exe"
#define U_WPS_EXE "wps.exe"
#define U_ET_EXE "et.exe"
#define U_WPP_EXE "wpp.exe"

static LPCSTR suffix[] = { ".dps", ".wps", ".et", ".doc", ".docx", ".xls", ".xlsx", ".ppt", ".pptx" };
namespace OCLS
{

COfficeInfo::COfficeInfo()
{
}

COfficeInfo::~COfficeInfo()
{
}

bool GetRegItemSTR(CRegistryKey regKey, const LPCSTR& subKey, const string& itemKey, string &itemValue, REGSAM regSam)
{
    bool retCode = false;
    try
    {
        CRegistryKey regSubKey = regKey.OpenKey(subKey, regSam);
        CRegistryValue regValue = regSubKey.QueryValue((LPCSTR)itemKey.c_str());
        if (regValue.m_bufSize != 0)
        {
            itemValue.assign((LPCSTR)regValue);
            retCode = true;
        }
    }
    catch (...)
    {
        return retCode;
    }
    return retCode;
}

bool COfficeInfo::SearchRegedit(CRegistryKey regKey, REGSAM regSam)
{
    bool ret = true;
    for (CRegistryKey::SubkeyIterator it = regKey.BeginSubkeyIteration(); it != regKey.EndSubkeyIteration(); ++it)
    {
        LPCSTR subKey;
        subKey = it.GetName();

        string displayName = "";
        bool istrue = GetRegItemSTR(regKey, subKey, "DisplayName", displayName, regSam);
        if (istrue == true && !displayName.empty() && displayName.find("(KB") == -1 && displayName.find("Microsoft Office") != -1 || displayName.find("WPS Office") != -1)
        {
            bool bfind = false;

            for (vector<U_SOFT_INFO>::iterator fit = mSoftwareInfo.begin(); fit != mSoftwareInfo.end(); fit++)
            {

                if (fit->displayName.find(displayName) != -1)
                {
                    bfind = true;
                    break;
                }
            }

            string sInstallRoot = "InstallLocation";
            if (displayName.find("WPS Office") != -1)
            {
                sInstallRoot = "LocationRoot";
            }

            if (bfind == false)
            {
                U_SOFT_INFO info;
                info.displayName = displayName;
                GetRegItemSTR(regKey, subKey, "DisplayVersion", info.displayVersion, regSam);
                GetRegItemSTR(regKey, subKey, sInstallRoot, info.installLocation, regSam);
                if (!info.displayVersion.empty() && !info.installLocation.empty())
                {
                    mSoftwareInfo.push_back(info);
                }
            }
        }
    }
    return ret;
}

bool COfficeInfo::GetSoftwareInfoForPlatform(bool is64Platform)
{
    bool ret = true;
    try
    {
        REGSAM regSam;
        if (is64Platform)
        {
            regSam = KEY_READ | KEY_WOW64_64KEY;
        }
        else
        {
            regSam = KEY_READ;
        }

        CRegistryKey regKey(HKEY_LOCAL_MACHINE, U_SOFTWARE_SETUP_KEY, regSam);
        SearchRegedit(regKey, regSam);
        CRegistryKey regKey1(HKEY_CURRENT_USER, U_SOFTWARE_SETUP_KEY, regSam);
        SearchRegedit(regKey1, regSam);
        CRegistryKey regKey2(HKEY_LOCAL_MACHINE, U_SOFTWARE_WOW6432NODE_KEY, regSam);
        SearchRegedit(regKey2, regSam);
        CRegistryKey regKey3(HKEY_CURRENT_USER, U_SOFTWARE_WOW6432NODE_KEY, regSam);
        SearchRegedit(regKey3, regSam);
    }
    catch (...)
    {
        ret = false;
    }

    return ret;
}

bool COfficeInfo::SetOfficeVersion(map<U_OFFICE_TYPE, string> &officeVersion)
{
    bool ret = true;
    vector<U_SOFT_INFO>::iterator iter;
    for (iter = mSoftwareInfo.begin(); iter != mSoftwareInfo.end(); iter++)
    {
        if (iter->displayName.find("Microsoft Office") != -1)
        {
            string version = iter->displayVersion;
            int inps = version.find_first_of(".");
            version = version.substr(0, inps);

            switch (atoi(version.c_str()))
            {
            case 11:
            {
                       string exe = "";
                       int len = iter->installLocation.length();
                       exe = iter->installLocation.substr(len - 1, string::npos);
                       if (exe.compare("\\") == 0)
                       {
                           exe = iter->installLocation + "OFFICE11\\";
                       }
                       else
                       {
                           exe = iter->installLocation + "\\OFFICE11\\";
                       }

                       string wordexe = exe + U_WINWORD_EXE;
                       if (_access(wordexe.c_str(), 0) != -1)
                       {
                           officeVersion.insert(pair<U_OFFICE_TYPE, string>(MS_OFFICE_2003_WORD, iter->displayVersion));
                           m_InstallInfo.insert(pair <string, string>(wordexe.c_str(), iter->displayVersion));
                       }

                       string excelexe = exe + U_EXCLE_EXE;
                       if (_access(excelexe.c_str(), 0) != -1)
                       {
                           officeVersion.insert(pair<U_OFFICE_TYPE, string>(MS_OFFICE_2003_EXCEL, iter->displayVersion));
                           m_InstallInfo.insert(pair <string, string>(excelexe.c_str(), iter->displayVersion));
                       }

                       string powerpntexe = exe + U_POWERPNT_EXE;
                       if (_access(powerpntexe.c_str(), 0) != -1)
                       {
                           officeVersion.insert(pair<U_OFFICE_TYPE, string>(MS_OFFICE_2003_PPT, iter->displayVersion));
                           m_InstallInfo.insert(pair <string, string>(powerpntexe.c_str(), iter->displayVersion));
                       }
            }
                break;
            case 12:
            {
                       string exe = "";
                       int len = iter->installLocation.length();
                       exe = iter->installLocation.substr(len - 1, string::npos);
                       if (exe.compare("\\") == 0)
                       {
                           exe = iter->installLocation + "OFFICE12\\";
                       }
                       else
                       {
                           exe = iter->installLocation + "\\OFFICE12\\";
                       }

                       string wordexe = exe + U_WINWORD_EXE;
                       if (_access(wordexe.c_str(), 0) != -1)
                       {
                           officeVersion.insert(pair<U_OFFICE_TYPE, string>(MS_OFFICE_2007_WORD, iter->displayVersion));
                           m_InstallInfo.insert(pair <string, string>(wordexe.c_str(), iter->displayVersion));
                       }

                       string excelexe = exe + U_EXCLE_EXE;
                       if (_access(excelexe.c_str(), 0) != -1)
                       {
                           officeVersion.insert(pair<U_OFFICE_TYPE, string>(MS_OFFICE_2007_EXCEL, iter->displayVersion));
                           m_InstallInfo.insert(pair <string, string>(excelexe.c_str(), iter->displayVersion));
                       }

                       string powerpntexe = exe + U_POWERPNT_EXE;
                       if (_access(powerpntexe.c_str(), 0) != -1)
                       {
                           officeVersion.insert(pair<U_OFFICE_TYPE, string>(MS_OFFICE_2007_PPT, iter->displayVersion));
                           m_InstallInfo.insert(pair <string, string>(powerpntexe.c_str(), iter->displayVersion));
                       }
            }
                break;
            case 14:
            {
                       string exe = "";
                       int len = iter->installLocation.length();
                       exe = iter->installLocation.substr(len - 1, string::npos);
                       if (exe.compare("\\") == 0)
                       {
                           exe = iter->installLocation + "OFFICE14\\";
                       }
                       else
                       {
                           exe = iter->installLocation + "\\OFFICE14\\";
                       }

                       string wordexe = exe + U_WINWORD_EXE;
                       if (_access(wordexe.c_str(), 0) != -1)
                       {
                           officeVersion.insert(pair<U_OFFICE_TYPE, string>(MS_OFFICE_2010_WORD, iter->displayVersion));
                           m_InstallInfo.insert(pair <string, string>(wordexe.c_str(), iter->displayVersion));
                       }

                       string excelexe = exe + U_EXCLE_EXE;
                       if (_access(excelexe.c_str(), 0) != -1)
                       {
                           officeVersion.insert(pair<U_OFFICE_TYPE, string>(MS_OFFICE_2010_EXCEL, iter->displayVersion));
                           m_InstallInfo.insert(pair <string, string>(excelexe.c_str(), iter->displayVersion));
                       }

                       string powerpntexe = exe + U_POWERPNT_EXE;
                       if (_access(powerpntexe.c_str(), 0) != -1)
                       {
                           officeVersion.insert(pair<U_OFFICE_TYPE, string>(MS_OFFICE_2010_PPT, iter->displayVersion));
                           m_InstallInfo.insert(pair <string, string>(powerpntexe.c_str(), iter->displayVersion));
                       }
            }
                break;
            case 15:
            {
                       string exe = "";
                       int len = iter->installLocation.length();
                       exe = iter->installLocation.substr(len - 1, string::npos);
                       if (exe.compare("\\") == 0)
                       {
                           exe = iter->installLocation + "OFFICE15\\";
                       }
                       else
                       {
                           exe = iter->installLocation + "\\OFFICE15\\";
                       }

                       string wordexe = exe + U_WINWORD_EXE;
                       if (_access(wordexe.c_str(), 0) != -1)
                       {
                           officeVersion.insert(pair<U_OFFICE_TYPE, string>(MS_OFFICE_2013_WORD, iter->displayVersion));
                           m_InstallInfo.insert(pair <string, string>(wordexe.c_str(), iter->displayVersion));
                       }

                       string excelexe = exe + U_EXCLE_EXE;
                       if (_access(excelexe.c_str(), 0) != -1)
                       {
                           officeVersion.insert(pair<U_OFFICE_TYPE, string>(MS_OFFICE_2013_EXCEL, iter->displayVersion));
                           m_InstallInfo.insert(pair <string, string>(excelexe.c_str(), iter->displayVersion));
                       }

                       string powerpntexe = exe + U_POWERPNT_EXE;
                       if (_access(powerpntexe.c_str(), 0) != -1)
                       {
                           officeVersion.insert(pair<U_OFFICE_TYPE, string>(MS_OFFICE_2013_PPT, iter->displayVersion));
                           m_InstallInfo.insert(pair <string, string>(powerpntexe.c_str(), iter->displayVersion));
                       }
            }
                break;
            case 16:
            {
                       string exe = "";
                       int len = iter->installLocation.length();
                       exe = iter->installLocation.substr(len - 1, string::npos);
                       if (exe.compare("\\") == 0)
                       {
                           exe = iter->installLocation + "OFFICE16\\";
                       }
                       else
                       {
                           exe = iter->installLocation + "\\OFFICE16\\";
                       }

                       string wordexe = exe + U_WINWORD_EXE;
                       if (_access(wordexe.c_str(), 0) != -1)
                       {
                           officeVersion.insert(pair<U_OFFICE_TYPE, string>(MS_OFFICE_2016_WORD, iter->displayVersion));
                           m_InstallInfo.insert(pair <string, string>(wordexe.c_str(), iter->displayVersion));
                       }

                       string excelexe = exe + U_EXCLE_EXE;
                       if (_access(excelexe.c_str(), 0) != -1)
                       {
                           officeVersion.insert(pair<U_OFFICE_TYPE, string>(MS_OFFICE_2016_EXCEL, iter->displayVersion));
                           m_InstallInfo.insert(pair <string, string>(excelexe.c_str(), iter->displayVersion));
                       }

                       string powerpntexe = exe + U_POWERPNT_EXE;
                       if (_access(powerpntexe.c_str(), 0) != -1)
                       {
                           officeVersion.insert(pair<U_OFFICE_TYPE, string>(MS_OFFICE_2016_PPT, iter->displayVersion));
                           m_InstallInfo.insert(pair <string, string>(powerpntexe.c_str(), iter->displayVersion));
                       }
            }
                break;
            default:
                break;
            }
        }

        if (iter->displayName.find("WPS Office") != -1)
        {
            string version = iter->displayVersion;
            string exe = "";
            int len = iter->installLocation.length();
            exe = iter->installLocation.substr(len - 1, string::npos);
            if (exe.compare("\\") == 0)
            {
                exe = iter->installLocation + "office6\\";
            }
            else
            {
                exe = iter->installLocation + "\\office6\\";
            }

            if (version.compare(0, 3, "6.4") == 0)
            {
                string wordexe = exe + U_WPS_EXE;
                if (_access(wordexe.c_str(), 0) != -1)
                {
                    officeVersion.insert(pair<U_OFFICE_TYPE, string>(WPS_OFFICE_2009_WPS, iter->displayVersion));
                    m_InstallInfo.insert(pair <string, string>(wordexe.c_str(), iter->displayVersion));
                }

                string excelexe = exe + U_ET_EXE;
                if (_access(excelexe.c_str(), 0) != -1)
                {
                    officeVersion.insert(pair<U_OFFICE_TYPE, string>(WPS_OFFICE_2009_ET, iter->displayVersion));
                    m_InstallInfo.insert(pair <string, string>(excelexe.c_str(), iter->displayVersion));
                }

                string powerpntexe = exe + U_WPP_EXE;
                if (_access(powerpntexe.c_str(), 0) != -1)
                {
                    officeVersion.insert(pair<U_OFFICE_TYPE, string>(WPS_OFFICE_2009_DPS, iter->displayVersion));
                    m_InstallInfo.insert(pair <string, string>(powerpntexe.c_str(), iter->displayVersion));
                }
            }

            else if (version.compare(0, 3, "6.6") == 0)
            {
                string wordexe = exe + U_WPS_EXE;
                if (_access(wordexe.c_str(), 0) != -1)
                {
                    officeVersion.insert(pair<U_OFFICE_TYPE, string>(WPS_OFFICE_2010_WPS, iter->displayVersion));
                    m_InstallInfo.insert(pair <string, string>(wordexe.c_str(), iter->displayVersion));
                }

                string excelexe = exe + U_ET_EXE;
                if (_access(excelexe.c_str(), 0) != -1)
                {
                    officeVersion.insert(pair<U_OFFICE_TYPE, string>(WPS_OFFICE_2010_ET, iter->displayVersion));
                    m_InstallInfo.insert(pair <string, string>(excelexe.c_str(), iter->displayVersion));
                }

                string powerpntexe = exe + U_WPP_EXE;
                if (_access(powerpntexe.c_str(), 0) != -1)
                {
                    officeVersion.insert(pair<U_OFFICE_TYPE, string>(WPS_OFFICE_2010_DPS, iter->displayVersion));
                    m_InstallInfo.insert(pair <string, string>(powerpntexe.c_str(), iter->displayVersion));
                }
            }

            else if (version.compare(0, 1, "8") == 0)
            {
                string wordexe = exe + U_WPS_EXE;
                if (_access(wordexe.c_str(), 0) != -1)
                {
                    officeVersion.insert(pair<U_OFFICE_TYPE, string>(WPS_OFFICE_2012_WPS, iter->displayVersion));
                    m_InstallInfo.insert(pair <string, string>(wordexe.c_str(), iter->displayVersion));
                }

                string excelexe = exe + U_ET_EXE;
                if (_access(excelexe.c_str(), 0) != -1)
                {
                    officeVersion.insert(pair<U_OFFICE_TYPE, string>(WPS_OFFICE_2012_ET, iter->displayVersion));
                    m_InstallInfo.insert(pair <string, string>(excelexe.c_str(), iter->displayVersion));
                }

                string powerpntexe = exe + U_WPP_EXE;
                if (_access(powerpntexe.c_str(), 0) != -1)
                {
                    officeVersion.insert(pair<U_OFFICE_TYPE, string>(WPS_OFFICE_2012_DPS, iter->displayVersion));
                    m_InstallInfo.insert(pair <string, string>(powerpntexe.c_str(), iter->displayVersion));
                }
            }

            else if (version.compare(0, 1, "9") == 0)
            {
                string wordexe = exe + U_WPS_EXE;
                if (_access(wordexe.c_str(), 0) != -1)
                {
                    officeVersion.insert(pair<U_OFFICE_TYPE, string>(WPS_OFFICE_2013_WPS, iter->displayVersion));
                    m_InstallInfo.insert(pair <string, string>(wordexe.c_str(), iter->displayVersion));
                }

                string excelexe = exe + U_ET_EXE;
                if (_access(excelexe.c_str(), 0) != -1)
                {
                    officeVersion.insert(pair<U_OFFICE_TYPE, string>(WPS_OFFICE_2013_ET, iter->displayVersion));
                    m_InstallInfo.insert(pair <string, string>(excelexe.c_str(), iter->displayVersion));
                }

                string powerpntexe = exe + U_WPP_EXE;
                if (_access(powerpntexe.c_str(), 0) != -1)
                {
                    officeVersion.insert(pair<U_OFFICE_TYPE, string>(WPS_OFFICE_2013_DPS, iter->displayVersion));
                    m_InstallInfo.insert(pair <string, string>(powerpntexe.c_str(), iter->displayVersion));
                }
            }

            else if (version.compare(0, 2, "10") == 0)
            {
                string wordexe = exe + U_WPS_EXE;
                if (_access(wordexe.c_str(), 0) != -1)
                {
                    officeVersion.insert(pair<U_OFFICE_TYPE, string>(WPS_OFFICE_2016_WPS, iter->displayVersion));
                    m_InstallInfo.insert(pair <string, string>(wordexe.c_str(), iter->displayVersion));
                }

                string excelexe = exe + U_ET_EXE;
                if (_access(excelexe.c_str(), 0) != -1)
                {
                    officeVersion.insert(pair<U_OFFICE_TYPE, string>(WPS_OFFICE_2016_ET, iter->displayVersion));
                    m_InstallInfo.insert(pair <string, string>(excelexe.c_str(), iter->displayVersion));
                }

                string powerpntexe = exe + U_WPP_EXE;
                if (_access(powerpntexe.c_str(), 0) != -1)
                {
                    officeVersion.insert(pair<U_OFFICE_TYPE, string>(WPS_OFFICE_2016_DPS, iter->displayVersion));
                    m_InstallInfo.insert(pair <string, string>(powerpntexe.c_str(), iter->displayVersion));
                }
            }
        }
    }
    return ret;
}

bool COfficeInfo::GetLocalOfficeVersion(map<U_OFFICE_TYPE, string>& officeVersion)
{
    bool ret = true;
    mSoftwareInfo.clear();
    m_InstallInfo.clear();
    officeVersion.clear();
    CWinSystem is64Platform;
    GetSoftwareInfoForPlatform(false);

    if (is64Platform.IsX64Platform() == TRUE)
    {
        GetSoftwareInfoForPlatform(true);
    }

    if (mSoftwareInfo.empty())
    {
        officeVersion.insert(pair<U_OFFICE_TYPE, string>(OFFICEAPP_UNINSTALL, "未检测到Microsoft Office或WPS办公软件"));
        ret = false;
    }

    SetOfficeVersion(officeVersion);
    return ret;
}

bool COfficeInfo::GetDefaultOpenMode(map<string, U_OFFICE_TYPE>& defaultOpenApp)
{
    bool ret = true;
    map<U_OFFICE_TYPE, string> officeV;
    GetLocalOfficeVersion(officeV);
    IQueryAssociations *pAssoc;
    HRESULT hr = AssocCreate(CLSID_QueryAssociations, IID_PPV_ARGS(&pAssoc));
    if (SUCCEEDED(hr))
    {
        char buffer[MAX_PATH] = { 0 };
        for (LPCSTR ext : suffix)
        {
            hr = pAssoc->Init(ASSOCF_INIT_DEFAULTTOSTAR, StringUtil::ansiToUtf16(ext).c_str(), nullptr, nullptr);
            if (SUCCEEDED(hr))
            {
                DWORD size = 255;
                hr = pAssoc->GetString(ASSOCF_NOTRUNCATE, ASSOCSTR_EXECUTABLE, nullptr, (LPWSTR)buffer, &size);

                if (SUCCEEDED(hr))
                {
                    char dst[MAX_PATH] = { 0 };
                    WideCharToMultiByte(CP_ACP, 0, (LPCWCH)buffer, -1, dst, MAX_PATH, "\0 ", 0);
                    string exePath = dst;
                    transform(exePath.begin(), exePath.end(), exePath.begin(), ::tolower);
                    string version;
                    map<string, string>::iterator it;

                    for (it = m_InstallInfo.begin(); it != m_InstallInfo.end(); it++)
                    {
                        string str = it->first;
                        transform(str.begin(), str.end(), str.begin(), ::tolower);
                        if (strcmp(exePath.c_str(), str.c_str()) == 0)
                        {
                            version = it->second;
                            break;
                        }
                    }

                    int hMS = 0, lMS = 0, hLS = 0, lLS = 0;
                    int ihMS = version.find(".");
                    string whatsnew = version.substr(ihMS + 1, version.length() - 1);
                    hMS = atoi((version.substr(0, ihMS)).c_str());
                    int ilMS = whatsnew.find(".");
                    lMS = atoi((whatsnew.substr(0, ilMS)).c_str());

                    if (exePath.find(U_WPS_EXE) != -1 || exePath.find(U_ET_EXE) != -1 || exePath.find(U_WPP_EXE) != -1)
                    {

                        if (hMS == 6 && lMS == 4)
                        {
                            if (exePath.find(U_WPS_EXE) != -1)
                            {
                                defaultOpenApp.insert(pair<string, U_OFFICE_TYPE>(ext, WPS_OFFICE_2009_WPS));
                            }
                            else if (exePath.find(U_ET_EXE) != -1)
                            {
                                defaultOpenApp.insert(pair<string, U_OFFICE_TYPE>(ext, WPS_OFFICE_2009_ET));
                            }
                            else if (exePath.find(U_WPP_EXE) != -1)
                            {
                                defaultOpenApp.insert(pair<string, U_OFFICE_TYPE>(ext, WPS_OFFICE_2009_DPS));
                            }

                        }
                        else if (hMS == 6, lMS == 6)
                        {
                            if (exePath.find(U_WPS_EXE) != -1)
                            {
                                defaultOpenApp.insert(pair<string, U_OFFICE_TYPE>(ext, WPS_OFFICE_2010_WPS));
                            }
                            else if (exePath.find(U_ET_EXE) != -1)
                            {
                                defaultOpenApp.insert(pair<string, U_OFFICE_TYPE>(ext, WPS_OFFICE_2010_ET));
                            }
                            else if (exePath.find(U_WPP_EXE) != -1)
                            {
                                defaultOpenApp.insert(pair<string, U_OFFICE_TYPE>(ext, WPS_OFFICE_2010_DPS));
                            }
                        }

                        else if (hMS == 8)
                        {
                            if (exePath.find(U_WPS_EXE) != -1)
                            {
                                defaultOpenApp.insert(pair<string, U_OFFICE_TYPE>(ext, WPS_OFFICE_2012_WPS));
                            }
                            else if (exePath.find(U_ET_EXE) != -1)
                            {
                                defaultOpenApp.insert(pair<string, U_OFFICE_TYPE>(ext, WPS_OFFICE_2012_ET));
                            }
                            else if (exePath.find(U_WPP_EXE) != -1)
                            {
                                defaultOpenApp.insert(pair<string, U_OFFICE_TYPE>(ext, WPS_OFFICE_2012_DPS));
                            }
                        }
                        else if (hMS == 9)
                        {
                            if (exePath.find(U_WPS_EXE) != -1)
                            {
                                defaultOpenApp.insert(pair<string, U_OFFICE_TYPE>(ext, WPS_OFFICE_2013_WPS));
                            }
                            else if (exePath.find(U_ET_EXE) != -1)
                            {
                                defaultOpenApp.insert(pair<string, U_OFFICE_TYPE>(ext, WPS_OFFICE_2013_ET));
                            }
                            else if (exePath.find(U_WPP_EXE) != -1)
                            {
                                defaultOpenApp.insert(pair<string, U_OFFICE_TYPE>(ext, WPS_OFFICE_2013_DPS));
                            }
                        }
                        else if (hMS == 10)
                        {
                            if (exePath.find(U_WPS_EXE) != -1)
                            {
                                defaultOpenApp.insert(pair<string, U_OFFICE_TYPE>(ext, WPS_OFFICE_2016_WPS));
                            }
                            else if (exePath.find(U_ET_EXE) != -1)
                            {
                                defaultOpenApp.insert(pair<string, U_OFFICE_TYPE>(ext, WPS_OFFICE_2016_ET));
                            }
                            else if (exePath.find(U_WPP_EXE) != -1)
                            {
                                defaultOpenApp.insert(pair<string, U_OFFICE_TYPE>(ext, WPS_OFFICE_2016_DPS));
                            }
                        }
                    }
                    else
                    {
                        switch (hMS)
                        {
                        case 11:
                        {
                            if (exePath.find(U_WINWORD_EXE) != -1)
                            {
                                defaultOpenApp.insert(pair<string, U_OFFICE_TYPE>(ext, MS_OFFICE_2003_WORD));
                            }
                            else if (exePath.find(U_EXCLE_EXE) != -1)
                            {
                                defaultOpenApp.insert(pair<string, U_OFFICE_TYPE>(ext, MS_OFFICE_2003_EXCEL));
                            }
                            else if (exePath.find(U_POWERPNT_EXE) != -1)
                            {
                                defaultOpenApp.insert(pair<string, U_OFFICE_TYPE>(ext, MS_OFFICE_2003_PPT));
                            }
                        }
                        break;
                        case 12:
                        {
                            if (exePath.find(U_WINWORD_EXE) != -1)
                            {
                                defaultOpenApp.insert(pair<string, U_OFFICE_TYPE>(ext, MS_OFFICE_2007_WORD));
                            }
                                   else if (exePath.find(U_EXCLE_EXE) != -1)
                            {
                                defaultOpenApp.insert(pair<string, U_OFFICE_TYPE>(ext, MS_OFFICE_2007_EXCEL));
                            }
                            else if (exePath.find(U_POWERPNT_EXE) != -1)
                            {
                                defaultOpenApp.insert(pair<string, U_OFFICE_TYPE>(ext, MS_OFFICE_2007_PPT));
                            }
                        }
                        break;
                        case 14:
                        {
                            if (exePath.find(U_WINWORD_EXE) != -1)
                            {
                                defaultOpenApp.insert(pair<string, U_OFFICE_TYPE>(ext, MS_OFFICE_2010_WORD));
                            }
                                   else if (exePath.find(U_EXCLE_EXE) != -1)
                            {
                                defaultOpenApp.insert(pair<string, U_OFFICE_TYPE>(ext, MS_OFFICE_2010_EXCEL));
                            }
                            else if (exePath.find(U_POWERPNT_EXE) != -1)
                            {
                                defaultOpenApp.insert(pair<string, U_OFFICE_TYPE>(ext, MS_OFFICE_2010_PPT));
                            }
                        }
                        break;
                        case 15:
                        {
                            if (exePath.find(U_WINWORD_EXE) != -1)
                            {
                                defaultOpenApp.insert(pair<string, U_OFFICE_TYPE>(ext, MS_OFFICE_2013_WORD));
                            }
                            else if (exePath.find(U_EXCLE_EXE) != -1)
                            {
                                defaultOpenApp.insert(pair<string, U_OFFICE_TYPE>(ext, MS_OFFICE_2013_EXCEL));
                            }
                            else if (exePath.find(U_POWERPNT_EXE) != -1)
                            {
                                defaultOpenApp.insert(pair<string, U_OFFICE_TYPE>(ext, MS_OFFICE_2013_PPT));
                            }
                        }
                        break;
                        case 16:
                        {
                            if (exePath.find(U_WINWORD_EXE) != -1)
                            {
                                defaultOpenApp.insert(pair<string, U_OFFICE_TYPE>(ext, MS_OFFICE_2016_WORD));
                            }
                            else if (exePath.find(U_EXCLE_EXE) != -1)
                            {
                                defaultOpenApp.insert(pair<string, U_OFFICE_TYPE>(ext, MS_OFFICE_2016_EXCEL));
                            }
                            else if (exePath.find(U_POWERPNT_EXE) != -1)
                            {
                                defaultOpenApp.insert(pair<string, U_OFFICE_TYPE>(ext, MS_OFFICE_2016_PPT));
                            }
                        }
                        break;
                        }
                    }
                }
            }
        }
        pAssoc->Release();
    }
    return ret;
}
}