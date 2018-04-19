#pragma once
#include <QString>
#include <QStringList>
#include <qt_windows.h>
#include <windns.h>

/*! @brief 获取调用堆栈
 *
 * 
 * @author mqm
 */
class CallStack
{
public:
    enum { MAX_STACK_DEPTH = 512, MAX_MODULE_COUNT = 512 };

    // 初始化符号
    static bool initialize();
    // 上一个错误的字符串表示
    static QString lastError();
    // 生成调用堆栈
    static QStringList generate(const void* context = 0);
private:
    static void stackWalk(QWORD *pTrace, DWORD dwMaxDepth, CONTEXT *pContext);
    static QString funcName(QWORD dwFunc);
    static bool initialized_;
};

