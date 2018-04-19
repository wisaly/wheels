#pragma once
#include <QVariant>
#include "FixPoint.h"

/*! @brief 类型到QVariant转换
 *
 * 用于解决字段访问器转换类型的问题
 * @author mqm
 */
template<class T>
class ConvertQVariant
{
public:
    static QVariant toQVariant(const T t) { return QVariant::fromValue(t); }
    static T fromQVariant(const QVariant &v) { return v.value<T>(); }
};

template<uint IL, uint DL>
class ConvertQVariant<FixPoint<IL,DL>>
{
public:
    static QVariant toQVariant(const Currency t) { return QVariant::fromValue(t.toString()); }
    static Currency fromQVariant(const QVariant &v) { return Currency(v.toString()); }
};

template<>
class ConvertQVariant<bool>
{
public:
    static QVariant toQVariant(const bool t) { return QVariant(t ? 1 : 0); }
    static bool fromQVariant(const QVariant &v) { return v.toInt() != 0; }
};

#define CONVERT_QVARIANT_ENUM(TYPE_NAME)\
    template<> class ConvertQVariant<TYPE_NAME> { public:\
    static QVariant toQVariant(const TYPE_NAME t) { return static_cast<int>(t); }\
    static TYPE_NAME fromQVariant(const QVariant &v) { return static_cast<TYPE_NAME>(v.toInt()); }};

CONVERT_QVARIANT_ENUM(TaxType)

#define CONVERT_QSTRING_ENUM(TYPE_NAME)\
    template<> class ConvertQString<TYPE_NAME> { public:\
    static QString toString(const TYPE_NAME v) { return QString::number(static_cast<int>(v)); }};
