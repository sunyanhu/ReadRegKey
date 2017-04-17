#include "test.h"
#include <iostream>

int main(void)
{
	//MessageBoxA(NULL, "Test", "Test", 0);
	cout.imbue(locale(""));
	OCLS::CGetLocalMachineOfficeVersion getVersion;
	map<OCLS::U_OFFICE_TYPE, string> mp;
	bool ret = getVersion.GetLocalOfficeVersion(mp);
	cout << "程序类型枚举值说明：" << endl << endl;
	cout << 0 << ": OFFICEAPP_UNINSTALL(未安装办公软件)" << endl;
	cout << 1 << ": MS_OFFICE_2003_WORD" << endl;
	cout << 2 << ": MS_OFFICE_2003_EXCEL" << endl;
	cout << 3 << ": MS_OFFICE_2003_PPT" << endl;
	cout << 4 << ": MS_OFFICE_2007_WORD" << endl;
	cout << 5 << ": MS_OFFICE_2007_EXCEL" << endl;
	cout << 6 << "：MS_OFFICE_2007_PPT" << endl;
	cout << 7 << "：MS_OFFICE_2010_WORD" << endl;
	cout << 8 << "：MS_OFFICE_2010_EXCEL" << endl;
	cout << 9 << "：MS_OFFICE_2010_PPT" << endl;
	cout << 10<< "：MS_OFFICE_2013_WORD" << endl;
	cout << 11<< "：MS_OFFICE_2013_EXCEL" << endl;
	cout << 12<< "：MS_OFFICE_2013_PPT" << endl;
	cout << 13<< "：MS_OFFICE_2016_WORD" << endl;
	cout << 14<< "：MS_OFFICE_2016_EXCEL" << endl;
	cout << 15<< "：MS_OFFICE_2016_PPT" << endl;

	cout << 16<< "：WPS_OFFICE_2009_WPS" << endl;
	cout << 17<< "：WPS_OFFICE_2009_ET" << endl;
	cout << 18<< "：WPS_OFFICE_2009_DPS" << endl;
	cout << 19<< "：WPS_OFFICE_2010_WPS" << endl;
	cout << 20<< "：WPS_OFFICE_2010_ET" << endl;
	cout << 21<< "：WPS_OFFICE_2010_DPS" << endl;
	cout << 22<< "：WPS_OFFICE_2012_WPS" << endl;
	cout << 23<< "：WPS_OFFICE_2012_ET" << endl;
	cout << 24<< "：WPS_OFFICE_2012_DPS" << endl;
	cout << 25<< "：WPS_OFFICE_2013_WPS" << endl;
	cout << 26<< "：WPS_OFFICE_2013_ET" << endl;
	cout << 27<< "：WPS_OFFICE_2013_DPS" << endl;
	cout << 28<< "：WPS_OFFICE_2016_WPS" << endl;
	cout << 29<< "：WPS_OFFICE_2016_ET" << endl;
	cout << 30 << "：WPS_OFFICE_2016_DPS" << endl << endl;
	cout << "本机安装的office版本：" << endl << endl;
	for (map<OCLS::U_OFFICE_TYPE, string>::iterator it = mp.begin(); it != mp.end(); it++)
	{
		cout << "类型：" << it->first << ",   安装版本号：" << it->second.data() << endl;
	}
	map<string, OCLS::U_OFFICE_TYPE> defaultOpen;
	bool re = getVersion.GetDefaultOpenMode(defaultOpen);
	cout << endl << "根据后缀名为.doc等获取办公软件默认打开程序：" << endl << endl;
	for (map<string, OCLS::U_OFFICE_TYPE>::iterator it = defaultOpen.begin(); it != defaultOpen.end();it++)
	{
		cout << "后缀名："<< it->first.data() <<",   默认打开程序：" << it->second << endl;
	}
	getchar();
	return 0;
}
