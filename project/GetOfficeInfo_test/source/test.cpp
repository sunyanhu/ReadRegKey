#include "test.h"
#include <iostream>

int main(void)
{
	//MessageBoxA(NULL, "Test", "Test", 0);
	wcout.imbue(locale(""));
	OCLS::COfficeInfo getVersion;
	map<OCLS::U_OFFICE_TYPE, wstring> mp;
	bool ret = getVersion.GetLocalOfficeVersion(mp);
	wcout << L"程序类型枚举值说明：" << endl << endl;
    wcout << 0 << L"：OFFICEAPP_UNINSTALL(未安装办公软件)" << endl;
    wcout << 1 << L"：MS_OFFICE_2003_WORD" << endl;
    wcout << 2 << L"：MS_OFFICE_2003_EXCEL" << endl;
    wcout << 3 << L"：MS_OFFICE_2003_PPT" << endl;
    wcout << 4 << L"：MS_OFFICE_2007_WORD" << endl;
    wcout << 5 << L"：MS_OFFICE_2007_EXCEL" << endl;
	wcout << 6 << L"：MS_OFFICE_2007_PPT" << endl;
	wcout << 7 << L"：MS_OFFICE_2010_WORD" << endl;
	wcout << 8 << L"：MS_OFFICE_2010_EXCEL" << endl;
	wcout << 9 << L"：MS_OFFICE_2010_PPT" << endl;
	wcout << 10<< L"：MS_OFFICE_2013_WORD" << endl;
	wcout << 11<< L"：MS_OFFICE_2013_EXCEL" << endl;
	wcout << 12<< L"：MS_OFFICE_2013_PPT" << endl;
	wcout << 13<< L"：MS_OFFICE_2016_WORD" << endl;
	wcout << 14<< L"：MS_OFFICE_2016_EXCEL" << endl;
	wcout << 15<< L"：MS_OFFICE_2016_PPT" << endl;

	wcout << 16<< L"：WPS_OFFICE_2009_WPS" << endl;
	wcout << 17<< L"：WPS_OFFICE_2009_ET" << endl;
	wcout << 18<< L"：WPS_OFFICE_2009_DPS" << endl;
	wcout << 19<< L"：WPS_OFFICE_2010_WPS" << endl;
	wcout << 20<< L"：WPS_OFFICE_2010_ET" << endl;
	wcout << 21<< L"：WPS_OFFICE_2010_DPS" << endl;
	wcout << 22<< L"：WPS_OFFICE_2012_WPS" << endl;
	wcout << 23<< L"：WPS_OFFICE_2012_ET" << endl;
	wcout << 24<< L"：WPS_OFFICE_2012_DPS" << endl;
	wcout << 25<< L"：WPS_OFFICE_2013_WPS" << endl;
	wcout << 26<< L"：WPS_OFFICE_2013_ET" << endl;
	wcout << 27<< L"：WPS_OFFICE_2013_DPS" << endl;
	wcout << 28<< L"：WPS_OFFICE_2016_WPS" << endl;
	wcout << 29<< L"：WPS_OFFICE_2016_ET" << endl;
	wcout << 30 << L"：WPS_OFFICE_2016_DPS" << endl << endl;
	wcout << "本机安装的office版本：" << endl << endl;
	for (map<OCLS::U_OFFICE_TYPE, wstring>::iterator it = mp.begin(); it != mp.end(); it++)
	{
		wcout << L"类型：" << it->first << L",   安装版本号：" << it->second.data() << endl;
	}
	map<wstring, OCLS::U_OFFICE_TYPE> defaultOpen;
	bool re = getVersion.GetDefaultOpenMode(defaultOpen);
	wcout << endl << L"根据后缀名获取办公软件默认打开程序：" << endl << endl;
	for (map<wstring, OCLS::U_OFFICE_TYPE>::iterator it = defaultOpen.begin(); it != defaultOpen.end();it++)
	{
		wcout << L"后缀名："<< it->first.data() <<L",   默认打开程序：" << it->second << endl;
	}
	getchar();
	return 0;
}
