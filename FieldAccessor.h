#pragma once
/*! @brief 字段访问器模板基类
 *
 * 用于实现数据库自动化
 * @author mqm
 */
#include <QVariant>
#include "Common.h"
#include "MemberPointer.h"
#include "ConvertQVariant.h"
#include "ConvertQString.h"

class FieldAccessorBase
{
public:
    FieldAccessorBase(QString name, QString display) : name_(name), display_(display) { placeHolder_ = ':' + name; }
    virtual ~FieldAccessorBase() { }
    virtual QVariant get(const void *record) const { return QVariant(); }
    virtual void set(void *record, const QVariant &o) const { }
    virtual QString getString(const void *record) const { return QString(); }

    QString name() const { return name_; }
    QString placeHolder() const { return placeHolder_; }
    QString display() const { return display_; }
    
    virtual bool isSame(size_t ptr) const { return false; }
    virtual bool greaterThan(void *left, void *right) const { return true; }
    virtual bool lessThan(void *left, void *right) const { return false; }
    
protected:
    QString name_;
    QString placeHolder_;
    QString display_;
};

template<class CLASS, typename TYPE, TYPE CLASS::*MEMBER>
class FieldAccessor : public FieldAccessorBase
{
public:
    FieldAccessor(QString name, QString display = QString()) : FieldAccessorBase(name, display) { }
    QVariant get(const void *record) const override { return ConvertQVariant<TYPE>::toQVariant(static_cast<const CLASS*>(record)->*MEMBER); }
    void set(void *record, const QVariant &o) const override { static_cast<CLASS*>(record)->*MEMBER = ConvertQVariant<TYPE>::fromQVariant(o); }
    QString getString(const void *record) const override { return ConvertQString<TYPE>::toString(static_cast<const CLASS*>(record)->*MEMBER); }
    bool isSame(size_t ptr) const override { return MemberPointer<CLASS, TYPE, MEMBER>().ptr() == ptr; }
    bool greaterThan(void *left, void *right) const override { return static_cast<CLASS*>(left)->*MEMBER > static_cast<CLASS*>(right)->*MEMBER; }
    bool lessThan(void *left, void *right) const override { return static_cast<CLASS*>(left)->*MEMBER < static_cast<CLASS*>(right)->*MEMBER; }
};
