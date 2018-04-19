#pragma once
#include <QSqlDatabase>
#include <QLibrary>
#include <QHash>
#include <QThread>
#include <QMutex>
#include "ErrorReport.h"

/*! @brief 数据库连接
 *
 * 
 * @author mqm
 */
class Database : public ErrorReport
{
public:
    class Connection
    {
    public:
        QString id_;
        QString type_;                    // 数据库类型，使用Qt内置插件名
        QString ociPath_;                 // oracle数据库专用oci.dll路径
        QString host_;                    // 数据库host
        int port_ = 0;
        QString name_;
        QString user_;
        QString password_;
    };

    Database();
    ~Database();

    bool init(Connection con);
    bool testOracle(QString ociPath, QString hostName, int port, QString dbName, QString userName, QString password);

    // 自动回滚事务，兼容嵌套事务
    class Transaction
    {
        friend class Database;
        Transaction(Database *db);
    public:
        Transaction(Transaction &other);
        ~Transaction();
        bool commit();
    private:
        Database *database_ = nullptr;
        bool isCommit_ = false; // 默认回滚不提交
    };
    Transaction getTransaction();

    QSqlDatabase &db();
private:
    bool &isInTransaction();
private:
    QLibrary *ociLib_ = nullptr;
    // 复制锁
    static QMutex dbCopyMutex_;
    // 初始线程（原始对象）
    QThread *initThread_ = nullptr;
    // 事务标识
    QHash<QThread*, bool> isInTransaction_;
    // 数据库对象表
    QHash<QThread*, QSqlDatabase> dbl_;
};

