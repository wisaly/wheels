#pragma once
#include <QString>
/*! @brief ���󱨸������
 *
 * 
 * @author mqm
 */
class ErrorReport
{
public:
    ErrorReport(QString reporter = QString());

    // ������Ϣ
    QString lastError();
protected:
    // ���ô�����Ϣ��д��־�������Ƿ���false
    bool setError(QString errorText);
    // ���ݴ�����Ϣ����д��־�������Ƿ���false
    bool transmitError(ErrorReport &other);
private:
    QString lastError_;
protected:
    QString reporter_;
};