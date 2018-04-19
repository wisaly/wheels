#pragma once
#include <QString>
#include <windows.h>

/*! @brief 异常处理器
 *
 * 
 * @author mqm
 */
class ExceptionHandler
{
public:
    // 安装异常处理器
    static void install();

private:
    static LONG WINAPI filter(LPEXCEPTION_POINTERS lpExceptionInfo);
    static void writeLog(QString fileName, LPEXCEPTION_POINTERS pep);
    static void minidump(QString fileName, LPEXCEPTION_POINTERS pep);
    static QString exceptionCodeText(int code);
};

