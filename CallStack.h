#pragma once
#include <QString>
#include <QStringList>
#include <qt_windows.h>
#include <windns.h>

/*! @brief ��ȡ���ö�ջ
 *
 * 
 * @author mqm
 */
class CallStack
{
public:
    enum { MAX_STACK_DEPTH = 512, MAX_MODULE_COUNT = 512 };

    // ��ʼ������
    static bool initialize();
    // ��һ��������ַ�����ʾ
    static QString lastError();
    // ���ɵ��ö�ջ
    static QStringList generate(const void* context = 0);
private:
    static void stackWalk(QWORD *pTrace, DWORD dwMaxDepth, CONTEXT *pContext);
    static QString funcName(QWORD dwFunc);
    static bool initialized_;
};

