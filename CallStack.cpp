#include "CallStack.h"
#include <QDebug>
#include <QFileInfo>
#include <WinDNS.h>
#include <DbgHelp.h>
#include <Psapi.h>
#pragma comment(lib, "Dbghelp.lib")
#pragma comment(lib, "Psapi.lib")

bool CallStack::initialized_ = false;

bool CallStack::initialize()
{
    if (initialized_)
        return true;

    // 设置符号引擎
    DWORD symOpts = ::SymGetOptions();
    symOpts |= SYMOPT_LOAD_LINES;
    symOpts |= SYMOPT_DEBUG;
    ::SymSetOptions(symOpts);

    if (FALSE == ::SymInitialize(::GetCurrentProcess(), NULL, TRUE))
    {
        qDebug() << "::SymInitialize failed";
        return false;
    }

    HANDLE processHandle = ::GetCurrentProcess();
    HMODULE modules[MAX_MODULE_COUNT] = {};

    unsigned bytes = 0;

    BOOL result = ::EnumProcessModules(
        processHandle, modules, sizeof(modules), (LPDWORD)&bytes);

    if (!result)
    {
        qDebug() << "::EnumProcessModules failed: " << lastError();
        return false;
    }

    const int iCount = bytes / sizeof(HMODULE);

    for (int i = 0; i < iCount; ++i)
    {
        char szModuleName[MAX_PATH] = { };
        char szImageName[MAX_PATH] = { };
        MODULEINFO Info;

        ::GetModuleInformation(processHandle, modules[i], &Info, sizeof(Info));
        ::GetModuleFileNameExA(processHandle, modules[i], szImageName, MAX_PATH);
        ::GetModuleBaseNameA(processHandle, modules[i], szModuleName, MAX_PATH);

        ::SymLoadModule64(processHandle, modules[i], szImageName, szModuleName, (DWORD64)Info.lpBaseOfDll, (DWORD)Info.SizeOfImage);
    }


    initialized_ = true;
    return true;
}

QString CallStack::lastError()
{
    QString errText;

    DWORD errCode = GetLastError();
    LPSTR msg = NULL;
    ::FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        NULL, errCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        reinterpret_cast<LPSTR>(&msg),
        0,
        NULL
        );

    if (msg == NULL)
    {
        msg = "Unknown";
    }

    errText = QString("Error %1: %2").arg(errCode).arg(QString::fromLocal8Bit(msg));

    ::LocalFree(msg);

    return errText;
}

void CallStack::stackWalk(QWORD *trace, DWORD maxDepth, CONTEXT *context)
{
    STACKFRAME64 stackFrame64;
    HANDLE processHandle = ::GetCurrentProcess();
    HANDLE threadHandle = ::GetCurrentThread();

    DWORD depth = 0;

    ::ZeroMemory(&stackFrame64, sizeof(stackFrame64));

    __try
    {
        stackFrame64.AddrPC.Offset = context->Eip;
        stackFrame64.AddrPC.Mode = AddrModeFlat;
        stackFrame64.AddrStack.Offset = context->Esp;
        stackFrame64.AddrStack.Mode = AddrModeFlat;
        stackFrame64.AddrFrame.Offset = context->Ebp;
        stackFrame64.AddrFrame.Mode = AddrModeFlat;

        bool successed = true;

        while (depth < maxDepth)
        {
            successed = ::StackWalk64(
                IMAGE_FILE_MACHINE_I386,
                processHandle,
                threadHandle,
                &stackFrame64,
                context,
                NULL,
                SymFunctionTableAccess64,
                SymGetModuleBase64,
                NULL
                ) != FALSE;

            trace[depth] = stackFrame64.AddrPC.Offset;
            ++depth;

            if (!successed)
                break;

            if (stackFrame64.AddrFrame.Offset == 0)
                break;
        }
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
    }
}

QString CallStack::funcName(QWORD func)
{
    HANDLE processHandle = ::GetCurrentProcess();
    DWORD64 symbolDisplacement64 = 0;
    SYMBOL_INFO_PACKAGE imgSymbol;
    imgSymbol.si.SizeOfStruct = sizeof(SYMBOL_INFOW);
    imgSymbol.si.MaxNameLen = MAX_SYM_NAME;
    QString symbolName;
    if (!::SymFromAddr(processHandle, func, &symbolDisplacement64, &imgSymbol.si))
        symbolName = "[Unknown]";
    else
        symbolName = QString::fromLocal8Bit(imgSymbol.si.Name);

    DWORD symbolDisplacement = 0;
    IMAGEHLP_LINE64 imgLine;
    imgLine.SizeOfStruct = sizeof(imgLine);

    QString fileName;
    int lineNumber = -1;
    if (::SymGetLineFromAddr64(processHandle, func, &symbolDisplacement, &imgLine))
    {
        fileName = QFileInfo(QString::fromLocal8Bit(imgLine.FileName)).fileName();
        lineNumber = imgLine.LineNumber;
    }

    IMAGEHLP_MODULE imgModule;
    imgModule.SizeOfStruct = sizeof(imgModule);

    QString moduleName;
    if (::SymGetModuleInfo(processHandle, func, &imgModule))
    {
        moduleName = QFileInfo(QString::fromLocal8Bit(imgModule.ImageName)).fileName();
    }

    QString result = QString("%1!%2").arg(moduleName).arg(symbolName);
    if (lineNumber != -1)
        result += QString("@%1#%2").arg(fileName).arg(lineNumber);

    return result;
}

QStringList CallStack::generate(const void* pContext)
{
    QStringList result;
    initialize();

    CONTEXT context;

    if (pContext != NULL)
    {
        ::memcpy_s(&context, sizeof(CONTEXT), pContext, sizeof(CONTEXT));
    }
    else
    {
        ::ZeroMemory(&context, sizeof(context));
        context.ContextFlags = CONTEXT_FULL;

        __asm
        {
            call FakeFuncCall
            FakeFuncCall :

            pop eax;
            mov context.Eip, eax;
            mov context.Ebp, ebp;
            mov context.Esp, esp;
        }
    }

    QWORD stack[MAX_STACK_DEPTH] = { };

    // 由于_stackwalk内部使用SEH 因此不能在其内部使用C++类
    stackWalk(stack, MAX_STACK_DEPTH, &context);

    for (int i = 0; i < MAX_STACK_DEPTH && stack[i] != 0; ++i)
        result << funcName(stack[i]);

    return result;
}

