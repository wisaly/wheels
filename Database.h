#pragma once
#include <QSqlDatabase>
#include <QLibrary>
#include <QHash>
#include <QThread>
#include <QMutex>
#include "ErrorReport.h"

/*! @brief ���ݿ�����
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
        QString type_;                    // ���ݿ����ͣ�ʹ��Qt���ò����
        QString ociPath_;                 // oracle���ݿ�ר��oci.dll·��
        QString host_;                    // ���ݿ�host
        int port_ = 0;
        QString name_;
        QString user_;
        QString password_;
    };

    Database();
    ~Database();

    bool init(Connection con);
    bool testOracle(QString ociPath, QString hostName, int port, QString dbName, QString userName, QString password);

    // �Զ��ع����񣬼���Ƕ������
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
        bool isCommit_ = false; // Ĭ�ϻع����ύ
    };
    Transaction getTransaction();

    QSqlDatabase &db();
private:
    bool &isInTransaction();
private:
    QLibrary *ociLib_ = nullptr;
    // ������
    static QMutex dbCopyMutex_;
    // ��ʼ�̣߳�ԭʼ����
    QThread *initThread_ = nullptr;
    // �����ʶ
    QHash<QThread*, bool> isInTransaction_;
    // ���ݿ�����
    QHash<QThread*, QSqlDatabase> dbl_;
};

