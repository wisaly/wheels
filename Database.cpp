#include <QSqlQuery>
#include <QSqlError>
#include <QApplication>
#include "Database.h"
#include "Common.h"


Database::Database() :
ErrorReport(ZH("数据库连接"))
{
}


Database::~Database()
{
    for (QSqlDatabase db : dbl_)
        if (db.isOpen())
            db.close();

    if (ociLib_)
        delete ociLib_;
}

bool Database::init(Connection con)
{
	initThread_ = QThread::currentThread();
    if (con.type_ == "QOCI")
    {
        if (ociLib_)
            delete ociLib_;

        ociLib_ = new QLibrary(con.ociPath_);

        //加载动态库
        if (!ociLib_->load())
            return setError(ZH("加载oci.dll时发生错误"));
    }
    else if (con.type_ == "QSQLITE")
    {
        con.name_ = qApp->applicationDirPath() + "/" + con.name_;
    }

    if (con.id_.isEmpty())
        db() = QSqlDatabase::addDatabase(con.type_);
    else
        db() = QSqlDatabase::addDatabase(con.type_, con.id_);
    db().setPort(con.port_);
    db().setHostName(con.host_);
    db().setDatabaseName(con.name_);
    db().setUserName(con.user_);
    db().setPassword(con.password_);

    if (!db().open())
        return setError(db().lastError().text());
    return true;
}

QSqlDatabase & Database::db()
{
    QThread *th = QThread::currentThread();
    if (!dbl_.contains(th) && th != initThread_)
    {
        QMutexLocker locker(&dbCopyMutex_);
        QSqlDatabase newdb = QSqlDatabase::cloneDatabase(
            dbl_[initThread_], dbl_[initThread_].connectionName() + QString::number(reinterpret_cast<int>(th)));
        if (!newdb.open())
        {
            setError("failed to clone db " + newdb.lastError().text());
        }
        dbl_[th] = newdb;
    }
    return dbl_[th];
}

bool & Database::isInTransaction()
{
    QThread *th = QThread::currentThread();
    if (!isInTransaction_.contains(th))
        isInTransaction_[th] = false;
    return isInTransaction_[th];
}

bool Database::testOracle(QString ociPath, QString hostName, int port, QString dbName, QString userName, QString password)
{
    QLibrary ociLib(ociPath);

    if (!ociLib.load())
    {
        setError(ZH("加载动态库失败"));
        return false;
    }

    const char *testDbName = "TESTDB";
    QSqlDatabase db = QSqlDatabase::addDatabase("QOCI", testDbName);
    db.setPort(port);
    db.setHostName(hostName);
    db.setDatabaseName(dbName);
    db.setUserName(userName);
    db.setPassword(password);

    if (!db.open())
    {
        setError(db.lastError().text());
        return false;
    }

    QSqlDatabase::removeDatabase(testDbName);

    return true;
}

Database::Transaction Database::getTransaction()
{
    return Transaction(this);
}

QMutex Database::dbCopyMutex_;

Database::Transaction::Transaction(Database *db)
{
    if (!db->isInTransaction())
        database_ = db;

    if (database_)
    {
        database_->db().transaction();
        database_->isInTransaction() = true;
    }
}

Database::Transaction::Transaction(Transaction &other)
{
    database_ = other.database_;
}

Database::Transaction::~Transaction()
{
    if (database_ && !isCommit_)
    {
        database_->db().rollback();
        database_->isInTransaction() = false;
    }
}

bool Database::Transaction::commit()
{
    if (database_)
    {
        database_->db().commit();
        isCommit_ = true;
        database_->isInTransaction() = false;
    }
    return true;
}
