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
     * 获取操作系统的版本
     * @param                  无
     * @return                 返回 OPER_SYSTEM_TYPE 枚举类型
     * @todo                   暂没实现
     */
    static OPER_SYSTEM_TYPE GetVersion();

    /**
     * 目前运行的操作系统版本是否在指定区域中的操作系统版本
     * @param         startVer 指定版本范围中的低版本
     * @param           endVer 指定版本范围中的高版本 
     * @return          FALSE  目前运行的操作系统版本不在指定版本之中
     *                   TRUE  目前运行的操作系统版本在指定版本之中
     * @todo                   暂没实现
     */
    static BOOL IsVersionZone(OPER_SYSTEM_TYPE startVer, OPER_SYSTEM_TYPE endVer);

    /**
     * 目前运行的操作系统版本是否在指定操作系统版本之后
     * @param           osVer  指定操作系统版本
     * @return          FALSE  目前运行的操作系统版本不在指定版本之后
     *                   TRUE  目前运行的操作系统版本在指定版本之后
     * @todo                   暂没实现
     */
    static BOOL IsVersionLater(OPER_SYSTEM_TYPE osVer);

    /**
     * 目前运行的操作系统版本是否是指定操作系统版本
     * @param           osVer  指定操作系统版本
     * @return          FALSE  目前运行的操作系统版本与指定版本不同
     *                   TRUE  目前运行的操作系统版本与指定版本相同
     * @todo                   暂没实现
     */
    static BOOL IsVersion(OPER_SYSTEM_TYPE osVer);

    /**
     * 目前运行的操作系统是否是家庭版
     * @param                  无
     * @return          FALSE  不是家庭版
     *                   TRUE  是家庭版
     * @todo                   暂没实现
     */
    static BOOL IsHomeEdition();

    /**
     * 是否是64位操作系统
     * @param                  无
     * @return          FALSE  32位操作系统
     *                   TRUE  64位操作系统
     */
    static BOOL IsX64Platform();

private:

    /**
    * 获取%windir%\\system32\\ntdll.dll文件的主版本和次版本号，用以获取操作系统的版本
    * @param    majorVersion  主版本号
    * @param    minorVersion  次要版本号
    * @return          FALSE  执行失败
    *                   TRUE  执行成功
    * @todo                   暂没实现
    */
    static BOOL GetCurrentVersionByFile(DWORD &majorVersion, DWORD &minorVersion);
};
}
#endif
