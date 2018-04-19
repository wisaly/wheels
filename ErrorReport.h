#pragma once
#include <QString>
/*! @brief 错误报告基础类
 *
 * 
 * @author mqm
 */
class ErrorReport
{
public:
    ErrorReport(QString reporter = QString());

    // 错误信息
    QString lastError();
protected:
    // 设置错误信息（写日志），总是返回false
    bool setError(QString errorText);
    // 传递错误信息（不写日志），总是返回false
    bool transmitError(ErrorReport &other);
private:
    QString lastError_;
protected:
    QString reporter_;
};