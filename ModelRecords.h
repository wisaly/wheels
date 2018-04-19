#pragma once
/*! @brief 通用数据库模型
 *
 * 用于实现数据库自动化建设
 * @author mqm
 */
#include <QAbstractTableModel>
#include "FieldAccessor.h"
#include <QDebug>

template<class RECORD, class RECORD_DEFINATION>
class ModelRecords : public QAbstractTableModel
{
public:
    ModelRecords() { }
    ~ModelRecords()
    {
        for (FieldAccessorBase *p : columns_)
            if (typeid(p) == typeid(FieldAccessorBase))
                delete p;
    }

    void loadData(const QList<RECORD> &records)
    {
        beginResetModel();
        records_ = records;
        endResetModel();
    }
    QList<RECORD> &records()
    {
        return records_;
    }
	void recordAt(QModelIndex &index, RECORD &record)
	{
		Q_ASSERT(index.row() >= 0 && index.row() < records_.count());
		record = records_[index.row()];
	}
    RECORD &recordRefAt(QModelIndex &index)
    {
		Q_ASSERT(index.row() >= 0 && index.row() < records_.count());
		return records_[index.row()];
    }
protected:
    int columnCount(const QModelIndex &parent) const override { return columns_.count(); }
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override
    {
        if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
            return columns_[section]->display(); 
        return QVariant();
    }
    int rowCount(const QModelIndex &parent) const override { return records_.count(); }
    QVariant data(const QModelIndex &index, int role) const override
    {
		if (role == Qt::DisplayRole && index.isValid())
            return columns_[index.column()]->getString(&records_[index.row()]);
        return QVariant();
    }

    void sort(int column, Qt::SortOrder order = Qt::AscendingOrder) override
    {
        beginResetModel();
        FieldAccessorBase *f = columns_[column];
        std::sort(records_.begin(), records_.end(),
            [f, order](RECORD &left, RECORD &right)
            {
            return order == Qt::AscendingOrder ?
                f->lessThan(&left, &right) :
                f->greaterThan(&left, &right);
            });
        endResetModel();
    }

protected:
    QList<RECORD> records_;
    typedef RECORD RecordClass;
    RECORD_DEFINATION defination_;
    QList<FieldAccessorBase*> columns_;
};

#define VISIABLE_COLUMN(MEMBER)\
    for (FieldAccessorBase *p : defination_.fields_)\
    if (p->isSame(MemberPointer<RecordClass,decltype(RecordClass::MEMBER),&RecordClass::MEMBER>().ptr()))\
    {columns_ << p;break;}

#define CUSTOM_COLUMN(DISPLAY)\
    columns_ << new FieldAccessorBase("",DISPLAY);

