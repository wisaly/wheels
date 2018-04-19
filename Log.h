#pragma once
#include <QString>

/*! @brief 通用日志类
 *
 * 
 * @author mqm
 */
class Log
{
public:
	Log();
	~Log();

	void append(QString log);
    void debug(QString log);
	void setPath(QString dirPath, QString partten = "log%1.txt", QString dtFormat = "yyyyMMdd");
private:
    QString logDir_;
    QString partten_;
    QString dtFormat_;
};

