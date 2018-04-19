#include "ErrorReport.h"
#include <QDebug>
#include "Log.h"
#include "Global.h"

ErrorReport::ErrorReport(QString reporter) :
reporter_(reporter)
{
    if (reporter_.isEmpty())
    {
        reporter_ = typeid(this).name();
    }
}

QString ErrorReport::lastError()
{
    return lastError_;
}

bool ErrorReport::setError(QString errorText)
{
    lastError_ = errorText;
    global<Log>()->append(QString("<%1>:{ %2 }").arg(reporter_).arg(lastError_));
    return false;
}

bool ErrorReport::transmitError(ErrorReport &other)
{
    lastError_ = other.lastError();
    return false;
}
