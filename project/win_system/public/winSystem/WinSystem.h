#ifndef _WIN_SYSTEM_H_
#define _WIN_SYSTEM_H_
#include "Export.h"

namespace OCLS
{
typedef enum _OPER_SYSTEM_TYPE_
{
    OS_UNKNOWN = 0,
    OS_WIN98,
    OS_WINDOWS_NT,
    OS_WINDOWS_ME,
    OS_WIN2000,
    OS_WINXP,
    OS_WIN2003,
    OS_VISTA,
    OS_WIN2008,
    OS_WIN7,
    OS_WIN8,
    OS_WIN2012,
    OS_WIN10
}OPER_SYSTEM_TYPE;

class OCLS_WINSYSTEM_API CWinSystem
{
public:
    CWinSystem();
    ~CWinSystem();

    /**
     * ��ȡ����ϵͳ�İ汾
     * @param                  ��
     * @return                 ���� OPER_SYSTEM_TYPE ö������
     * @todo                   ��ûʵ��
     */
    static OPER_SYSTEM_TYPE GetVersion();

    /**
     * Ŀǰ���еĲ���ϵͳ�汾�Ƿ���ָ�������еĲ���ϵͳ�汾
     * @param         startVer ָ���汾��Χ�еĵͰ汾
     * @param           endVer ָ���汾��Χ�еĸ߰汾 
     * @return          FALSE  Ŀǰ���еĲ���ϵͳ�汾����ָ���汾֮��
     *                   TRUE  Ŀǰ���еĲ���ϵͳ�汾��ָ���汾֮��
     * @todo                   ��ûʵ��
     */
    static BOOL IsVersionZone(OPER_SYSTEM_TYPE startVer, OPER_SYSTEM_TYPE endVer);

    /**
     * Ŀǰ���еĲ���ϵͳ�汾�Ƿ���ָ������ϵͳ�汾֮��
     * @param           osVer  ָ������ϵͳ�汾
     * @return          FALSE  Ŀǰ���еĲ���ϵͳ�汾����ָ���汾֮��
     *                   TRUE  Ŀǰ���еĲ���ϵͳ�汾��ָ���汾֮��
     * @todo                   ��ûʵ��
     */
    static BOOL IsVersionLater(OPER_SYSTEM_TYPE osVer);

    /**
     * Ŀǰ���еĲ���ϵͳ�汾�Ƿ���ָ������ϵͳ�汾
     * @param           osVer  ָ������ϵͳ�汾
     * @return          FALSE  Ŀǰ���еĲ���ϵͳ�汾��ָ���汾��ͬ
     *                   TRUE  Ŀǰ���еĲ���ϵͳ�汾��ָ���汾��ͬ
     * @todo                   ��ûʵ��
     */
    static BOOL IsVersion(OPER_SYSTEM_TYPE osVer);

    /**
     * Ŀǰ���еĲ���ϵͳ�Ƿ��Ǽ�ͥ��
     * @param                  ��
     * @return          FALSE  ���Ǽ�ͥ��
     *                   TRUE  �Ǽ�ͥ��
     * @todo                   ��ûʵ��
     */
    static BOOL IsHomeEdition();

    /**
     * �Ƿ���64λ����ϵͳ
     * @param                  ��
     * @return          FALSE  32λ����ϵͳ
     *                   TRUE  64λ����ϵͳ
     */
    static BOOL IsX64Platform();

private:

    /**
    * ��ȡ%windir%\\system32\\ntdll.dll�ļ������汾�ʹΰ汾�ţ����Ի�ȡ����ϵͳ�İ汾
    * @param    majorVersion  ���汾��
    * @param    minorVersion  ��Ҫ�汾��
    * @return          FALSE  ִ��ʧ��
    *                   TRUE  ִ�гɹ�
    * @todo                   ��ûʵ��
    */
    static BOOL GetCurrentVersionByFile(DWORD &majorVersion, DWORD &minorVersion);
};
}
#endif
