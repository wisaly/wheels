#pragma once
#include <QSharedMemory>
#include <QHash>
#include <QList>
#include <QString>

class GlobalResource;

/*! @brief ȫ����Դ����
 *
 * ����ȡ������ģʽ����init��clean�����������ɺ����
 * ʹ�ù����ڴ棬��ʵ����Ȼ���߳�ʹ���ྲ̬����������Դ
 * ���ڹ����ڴ�ֱ��new�ã����ɿ����ʹ��
 * @author mqm
 */
class Global
{
    Global();
public:
    ~Global();
    // ��ʼ��
    static bool init(QString &errorText, int retryCnt = 0);
    // �����ڴ�
    static void clean();
    // ��ȡ����
    static Global *instance();

    // ��ȡ��Դ
    GlobalResource *resource(QString name);
    // ��Դ�Ƿ����
    bool containsResource(QString name);
    // �����Դ
    GlobalResource *addResource(QString name, GlobalResource *res);
private:
    QSharedMemory sharedMemory_;
    QHash<QString, GlobalResource*> resources_;     // ȫ����Դ������ӳ���
    QList<GlobalResource *> resourceOrder_;         // ȫ����Դ��˳�򱣴棨�����ͷţ�
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

// ��ȡȫ����Դ������TΪ����
template<class T>
T *global()
{
    return reinterpret_cast<T*>(Global::instance()->resource(typeid(T).name())->resource());
}

// ��ʼ��ȫ����Դ
template<class T>
void globalInit()
{
    GlobalResource *res = new GlobalContainer<T>(new T);
    Global::instance()->addResource(typeid(T).name(), res);
}

// ��ʼ��ȫ����Դ�����ڳ�������������ࣩ
template<class T>
void globalInit(T *p)
{
    GlobalResource *res = new GlobalContainer<T>(p);
    Global::instance()->addResource(typeid(T).name(), res);
}