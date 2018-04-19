#include <QThread>
#include "Global.h"
#include "Common.h"

#define GLOBAL_MEM_KEY "AXYK_SHARED_MEM"

Global::Global() :
sharedMemory_(GLOBAL_MEM_KEY)
{
    bool attachResult = sharedMemory_.attach();
	Q_ASSERT(attachResult);
}


Global::~Global()
{
    for (int i = resourceOrder_.count() - 1; i >= 0; i--)
        delete resourceOrder_[i];

    resourceOrder_.clear();
    resources_.clear();
}

bool Global::init(QString &errorText, int retryCnt)
{
    QSharedMemory sm(GLOBAL_MEM_KEY);
    
    while (!sm.create(sizeof(Global*)))
    {
        if (sm.error() == QSharedMemory::AlreadyExists)
        {
            errorText = ZH("程序已运行");
            if (--retryCnt <= 0)
                return false;
            else
                QThread::msleep(500);
        }
        else
        {
            errorText = sm.errorString();
            return false;
        }
    }

    *static_cast<Global**>(sm.data()) = new Global;
    return true;
}

Global *Global::instance()
{
    QSharedMemory sm(GLOBAL_MEM_KEY);
    bool attachResult = sm.attach(QSharedMemory::ReadOnly);
	Q_ASSERT(attachResult);

    return *static_cast<Global**>(sm.data());
}

void Global::clean()
{
    delete instance();
}

GlobalResource * Global::resource(QString name)
{
    Q_ASSERT(resources_.contains(name));

    return resources_[name];
}

GlobalResource *Global::addResource(QString name, GlobalResource *res)
{
    Q_ASSERT(!resources_.contains(name));

    resources_[name] = res;
    resourceOrder_ << res;
    return res;
}

bool Global::containsResource(QString name)
{
    return resources_.contains(name);
}
