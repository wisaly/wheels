#pragma once
#include <QSharedMemory>
#include <QHash>
#include <QList>
#include <QString>

class GlobalResource;

/*! @brief 全局资源管理
 *
 * 用于取代单例模式，除init和clean，其它函数由宏调用
 * 使用共享内存，可实现自然跨线程使用类静态函数访问资源
 * 由于共享内存直接new得，不可跨进程使用
 * @author mqm
 */
class Global
{
    Global();
public:
    ~Global();
    // 初始化
    static bool init(QString &errorText, int retryCnt = 0);
    // 清理内存
    static void clean();
    // 获取单例
    static Global *instance();

    // 获取资源
    GlobalResource *resource(QString name);
    // 资源是否存在
    bool containsResource(QString name);
    // 添加资源
    GlobalResource *addResource(QString name, GlobalResource *res);
private:
    QSharedMemory sharedMemory_;
    QHash<QString, GlobalResource*> resources_;     // 全局资源与名称映射表
    QList<GlobalResource *> resourceOrder_;         // 全局资源按顺序保存（用于释放）
};

class GlobalResource
{
public:
    GlobalResource(){}
    virtual ~GlobalResource(){}
    virtual void *resource() = 0;
};


template<class T>
class GlobalContainer : public GlobalResource
{
public:
    GlobalContainer(T *res) : res_(res) { }
    ~GlobalContainer() override { delete res_; }
    void *resource() override { return reinterpret_cast<void*>(res_); }

private:
    T *res_ = nullptr;
};

// 获取全局资源，参数T为类名
template<class T>
T *global()
{
    return reinterpret_cast<T*>(Global::instance()->resource(typeid(T).name())->resource());
}

// 初始化全局资源
template<class T>
void globalInit()
{
    GlobalResource *res = new GlobalContainer<T>(new T);
    Global::instance()->addResource(typeid(T).name(), res);
}

// 初始化全局资源（用于抽象类或其它父类）
template<class T>
void globalInit(T *p)
{
    GlobalResource *res = new GlobalContainer<T>(p);
    Global::instance()->addResource(typeid(T).name(), res);
}