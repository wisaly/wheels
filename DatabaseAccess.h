#pragma once
#include <QSqlQuery>
#include <QSqlError>
#include "DatabaseRecord.h"
#include "IDGenerator.h"
#include "ErrorReport.h"
#include "DatabaseManager.h"
#include "Global.h"

/*! @brief ���ݷ��ʲ�ģ����
 *
 * ʵ�����ݷ����Զ���֧��
 * @author mqm
 */
template<class RECORD, class RECORD_DEFINATION>
class DatabaseAccess : public ErrorReport
{
public:
    DatabaseAccess()
    {
        reporter_ = ZH("���ݿ����#") + defination_.tableName_;
        QStringList fields;
        QStringList placeHoders;
        QStringList setStatements;
        for (FieldAccessorBase *f : defination_.fields_)
        {
            if (f->name().isEmpty())
                continue;
            fields << f->name();
            placeHoders << f->placeHolder();
            setStatements << QString("%1=%2").arg(f->name()).arg(f->placeHolder());
        }

        insertSql_ = QString("insert into %1 (%2) values(%3)")
            .arg(defination_.tableName_).arg(fields.join(',')).arg(placeHoders.join(','));

        updateSql_ = QString("update %1 set %2 where %3=%4")
            .arg(defination_.tableName_).arg(setStatements.join(',')).arg(fields.first()).arg(placeHoders.first());

        deleteBySql_ = QString("delete from %1").arg(defination_.tableName_);
        deleteSql_ = QString("%1 where %2=%3")
            .arg(deleteBySql_).arg(fields.first()).arg(placeHoders.first());

        selectSql_ = QString("select %1 from %2")
            .arg(fields.join(',')).arg(defination_.tableName_);

        database_ = global<DatabaseManager>()->defaultDb();
    }

    void setDatabase(Database *database)
    {
        database_ = database;
    }

    // ��record�󶨵�query
    void bind(RECORD *record, QSqlQuery &query)
    {
        for (FieldAccessorBase *f : defination_.fields_)
            if (!f->name().isEmpty())
                query.bindValue(f->placeHolder(), f->get(record));
    }
    // ��query��䵽record
    void fill(RECORD *record, QSqlQuery &query)
    {
        for (FieldAccessorBase *f : defination_.fields_)
            if (!f->name().isEmpty())
                f->set(record, query.value(f->name()));
    }

    // �����¼
    bool insert(RECORD &record)
    {
        if (defination_.idField_)
            defination_.idField_->set(&record, IdGenerator::gen());

        QSqlQuery query(database_->db());
		if (!query.prepare(insertSql_))
			return setError(query.lastError().text());

        bind(&record, query);
        if (!query.exec())
            return setError(query.lastError().text() + queryDetails(query));
        return true;
    }
    // �޸ļ�¼
    bool modify(RECORD &record)
    {
        QSqlQuery query(database_->db());
		if (!query.prepare(updateSql_))
			return setError(query.lastError().text());

        bind(&record, query);
        if (!query.exec())
            return setError(query.lastError().text() + queryDetails(query));

        return true;
    }
    // ɾ����¼
    bool remove(RECORD &record)
    {
        QSqlQuery query(database_->db());
		if (!query.prepare(deleteSql_))
			return setError(query.lastError().text());

        query.bindValue(defination_.idField_->placeHolder(), defination_.idField_->get(&record));
        if (!query.exec())
            return setError(query.lastError().text() + queryDetails(query));

        return true;
    }
    bool removeBy(QString condition)
    {
        QSqlQuery query(database_->db());
        QString sql = deleteBySql_;
        if (!condition.isEmpty())
            sql += " where " + condition;
        if (!query.prepare(sql))
            return setError(query.lastError().text());

        if (!query.exec())
            return setError(query.lastError().text() + queryDetails(query));

        return true;
    }
    bool execute(QString sql)
    {
        QSqlQuery query(database_->db());
        if (!query.prepare(sql))
            return setError(query.lastError().text());

        if (!query.exec())
            return setError(query.lastError().text());

        return true;
    }
    // ѡ�����м�¼
    bool select(QList<RECORD> &records)
    {
        QSqlQuery query(database_->db());
		if (!query.prepare(selectSql_))
			return setError(query.lastError().text());

        if (!query.exec())
            return setError(query.lastError().text() + queryDetails(query));

        while (query.next())
        {
            RECORD record;
            fill(&record, query);
            records << record;
        }
        return true;
    }
    // ��������ѡ���¼
    bool selectByPk(RECORD &record)
    {
        if (!defination_.idField_)
            return setError("table no pk");

        QString sql = selectSql_;
        sql += QString(" where %1=%2")
            .arg(defination_.idField_->name()).arg(defination_.idField_->placeHolder());

        QSqlQuery query(database_->db());
		if (!query.prepare(sql))
			return setError(query.lastError().text());
        query.bindValue(defination_.idField_->placeHolder(), defination_.idField_->get(&record));
        if (!query.exec())
            return setError(query.lastError().text() + queryDetails(query));

        if (!query.next())
            return setError("not exist");

        fill(&record, query);
        return true;
    }
    // ѡ�񲿷ּ�¼
    bool selectBy(QString whereCondition, QList<RECORD> &records)
    {
        QString sql = selectSql_;
        if (!whereCondition.isEmpty())
            sql += " where " + whereCondition;
        QSqlQuery query(sql, database_->db());
        query.setForwardOnly(true);
        if (!query.exec())
            return setError(query.lastError().text() + queryDetails(query));

        while (query.next())
        {
            RECORD record;
            fill(&record, query);
            records << record;
        }
        return true;
    }

    // ��ѯ�������ۺϱ����������������
    bool selectScalar(QString aggregation, QString condition, QVariant &result)
    {
        QString sql = QString("select %1 from %2 where %3").arg(aggregation).arg(defination_.tableName_).arg(condition);
        QSqlQuery query(sql, database_->db());
        if (!query.exec())
            return setError(query.lastError().text() + queryDetails(query));

        if (!query.next())
            return setError("0 row returned");

        result = query.value(0);
        return true;
    }

protected:
    QString queryDetails(QSqlQuery query)
    {
        QStringList pairs;
        QMap<QString, QVariant> bound = query.boundValues();
        for (auto i = bound.constBegin(); i != bound.constEnd(); i++)
            pairs << QString("[%1=%2]").arg(i.key(), i.value().toString());

        return QString("(sql=%1,%2)").arg(query.lastQuery()).arg(pairs.join(","));
    }
protected:
    RECORD_DEFINATION defination_;
    QString insertSql_;
    QString updateSql_;
    QString deleteSql_;
    QString deleteBySql_;
    QString selectSql_;
    Database *database_;
};
