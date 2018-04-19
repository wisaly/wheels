#pragma once
#include <QString>
#include <windows.h>

/*! @brief �쳣������
 *
 * 
 * @author mqm
 */
class ExceptionHandler
{
public:
    // ��װ�쳣������
    static void install();

private:
    static LONG WINAPI filter(LPEXCEPTION_POINTERS lpExceptionInfo);
    static void writeLog(QString fileName, LPEXCEPTION_POINTERS pep);
    static void minidump(QString fileName, LPEXCEPTION_POINTERS pep);
    static QString exceptionCodeText(int code);
};

