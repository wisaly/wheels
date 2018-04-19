#pragma once
/*! @brief 类型到QString的转换
 *
 * 用于解决数据库通用模型显示问题
 * @author mqm
 */

#include <QString>
#include <QDateTime>
#include "Common.h"

template<class T>
class ConvertQString
{
public:
    static QString toString(T v) { return v.toString(); }
};

template<>
class ConvertQString<QString>
{
public:
    static QString toString(QString v) { return v; }
};

template<>
class ConvertQString<int>
{
public:
    static QString toString(int v) { return QString::number(v); }
};

template<>
class ConvertQString<bool>
{
public:
    static QString toString(bool v) { return v ? "true" : "false"; }
};

template<>
class ConvertQString<QDateTime>
{
public:
    static QString toString(QDateTime v) { return v.toString("yyyy-MM-dd hh:mm:ss"); }
};

template<>
class ConvertQString<QDate>
{
public:
	static QString toString(QDate v) { return v.toString(DATE_FORMAT); }
};

template<>
class ConvertQString<TaxType>
{
public:
    static QString toString(TaxType v)
    {
        switch (v)
        {
        case TaxType::INVALID: return ZH("无效类型");
        case TaxType::ZZSPTFP: return ZH("增值税普通发票");
        case TaxType::ZZSZYFP: return ZH("增值税专用发票");
        case TaxType::ZZSPTFPJS: return ZH("增值税普通发票（卷式）");
		case TaxType::DZFP:return ZH("电子发票");
        }
        return QString();
    }
};


