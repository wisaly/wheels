#include "Log.h"

#include <QFile>
#include <QDateTime>
#include <QDebug>

Log::Log()
{
}

Log::~Log()
{
}

void Log::append(QString log)
{
    QString logPath = QString(logDir_ + "/" + partten_).arg(QDateTime::currentDateTime().toString(dtFormat_));
	QFile fd(logPath);
	if (fd.open(QFile::WriteOnly | QFile::Append))
	{
		QString logtext = QString("[%1]: %2\r\n")
			.arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"))
			.arg(log);

        qDebug() << "Log: " << logtext;

		fd.write(logtext.toUtf8());
		fd.close();
	}
}

void Log::setPath(QString dirPath, QString partten /*= "/log%1.txt"*/, QString dtFormat /*= "yyyyMMdd"*/)
{
    logDir_ = dirPath;
    partten_ = partten;
    dtFormat_ = dtFormat;
}

void Log::debug(QString log)
{
    append(log);
}
