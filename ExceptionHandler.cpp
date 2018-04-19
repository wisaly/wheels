#include <QDateTime>
#include "ExceptionHandler.h"
#include <eh.h>
#include "CallStack.h"
#include <QDebug>
#include <QFile>
#include <QScopedArrayPointer>

#include <QCoreApplication>
#include <DbgHelp.h>


void ExceptionHandler::install()
{
    CallStack::initialize();
    SetUnhandledExceptionFilter(filter);
}

LONG WINAPI ExceptionHandler::filter(LPEXCEPTION_POINTERS lpExceptionInfo)
{
    QString dateText = QDateTime::currentDateTime().toString("yyyyMMddhhmmsszzz");
    QString logFileName = qApp->applicationDirPath() + QString("/ue%1.log").arg(dateText);
    QString dmpFileName = qApp->applicationDirPath() + QString("/ue%1.dmp").arg(dateText);
    writeLog(logFileName, lpExceptionInfo);
    minidump(dmpFileName, lpExceptionInfo);

    QString errText = QString::fromLocal8Bit("发生了无法处理的异常，程序即将退出。\r\n日志文件:%1\r\n转存文件:%2")
        .arg(logFileName).arg(dmpFileName);

    QByteArray errTextBa = errText.toLocal8Bit();

    ::MessageBoxA(NULL, errTextBa.constData(), "错误", MB_OK | MB_ICONERROR);

    return EXCEPTION_EXECUTE_HANDLER;
}

void ExceptionHandler::writeLog(QString fileName, LPEXCEPTION_POINTERS pep)
{
    QStringList stack = CallStack::generate(pep->ContextRecord);

    QFile fd(fileName);
    if (fd.open(QFile::WriteOnly | QFile::Append))
    {
        fd.write(QString("Unhandled exception %1(0x%2) at 0x%3\r\nLog at %4\r\n\r\n")
            .arg(exceptionCodeText(pep->ExceptionRecord->ExceptionCode))
            .arg(pep->ExceptionRecord->ExceptionCode,0,16)
            .arg(reinterpret_cast<qulonglong>(pep->ExceptionRecord->ExceptionAddress),0,16)
            .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")).toUtf8());
        fd.write(stack.join("\r\n").toUtf8());

        fd.close();
    }
}

void ExceptionHandler::minidump(QString fileName, LPEXCEPTION_POINTERS pep)
{
    QByteArray fileNameBa = fileName.toLocal8Bit();
    HANDLE file = CreateFileA(fileNameBa.constData(), GENERIC_READ | GENERIC_WRITE,
        0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    if ((file != NULL) && (file != INVALID_HANDLE_VALUE))
    {
        MINIDUMP_EXCEPTION_INFORMATION mdei;
        mdei.ThreadId = GetCurrentThreadId();
        mdei.ExceptionPointers = pep;
        mdei.ClientPointers = FALSE;

        MINIDUMP_TYPE mdt = MiniDumpNormal;
        BOOL rv = MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(),
            file, mdt, (pep != 0) ? &mdei : 0, 0, 0);

        if (!rv)
            qDebug() << "MiniDumpWriteDump failed.";
        CloseHandle(file);
    }
    else
    {
        qDebug() << "create file failed";
    }
}

QString ExceptionHandler::exceptionCodeText(int code)
{
    switch (code) {
        case EXCEPTION_ACCESS_VIOLATION:         return "EXCEPTION_ACCESS_VIOLATION";
        case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:    return "EXCEPTION_ARRAY_BOUNDS_EXCEEDED";
        case EXCEPTION_BREAKPOINT:               return "EXCEPTION_BREAKPOINT";
        case EXCEPTION_DATATYPE_MISALIGNMENT:    return "EXCEPTION_DATATYPE_MISALIGNMENT";
        case EXCEPTION_FLT_DENORMAL_OPERAND:     return "EXCEPTION_FLT_DENORMAL_OPERAND";
        case EXCEPTION_FLT_DIVIDE_BY_ZERO:       return "EXCEPTION_FLT_DIVIDE_BY_ZERO";
        case EXCEPTION_FLT_INEXACT_RESULT:       return "EXCEPTION_FLT_INEXACT_RESULT";
        case EXCEPTION_FLT_INVALID_OPERATION:    return "EXCEPTION_FLT_INVALID_OPERATION";
        case EXCEPTION_FLT_OVERFLOW:             return "EXCEPTION_FLT_OVERFLOW";
        case EXCEPTION_FLT_STACK_CHECK:          return "EXCEPTION_FLT_STACK_CHECK";
        case EXCEPTION_FLT_UNDERFLOW:            return "EXCEPTION_FLT_UNDERFLOW";
        case EXCEPTION_ILLEGAL_INSTRUCTION:      return "EXCEPTION_ILLEGAL_INSTRUCTION";
        case EXCEPTION_IN_PAGE_ERROR:            return "EXCEPTION_IN_PAGE_ERROR";
        case EXCEPTION_INT_DIVIDE_BY_ZERO:       return "EXCEPTION_INT_DIVIDE_BY_ZERO";
        case EXCEPTION_INT_OVERFLOW:             return "EXCEPTION_INT_OVERFLOW";
        case EXCEPTION_INVALID_DISPOSITION:      return "EXCEPTION_INVALID_DISPOSITION";
        case EXCEPTION_NONCONTINUABLE_EXCEPTION: return "EXCEPTION_NONCONTINUABLE_EXCEPTION";
        case EXCEPTION_PRIV_INSTRUCTION:         return "EXCEPTION_PRIV_INSTRUCTION";
        case EXCEPTION_SINGLE_STEP:              return "EXCEPTION_SINGLE_STEP";
        case EXCEPTION_STACK_OVERFLOW:           return "EXCEPTION_STACK_OVERFLOW";
        default: return "UNKNOWN_EXCEPTION";
    }
}

