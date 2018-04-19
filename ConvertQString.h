#pragma once
/*! @brief ���͵�QString��ת��
 *
 * ���ڽ�����ݿ�ͨ��ģ����ʾ����
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
        case TaxType::INVALID: return ZH("��Ч����");
        case TaxType::ZZSPTFP: return ZH("��ֵ˰��ͨ��Ʊ");
        case TaxType::ZZSZYFP: return ZH("��ֵ˰ר�÷�Ʊ");
        case TaxType::ZZSPTFPJS: return ZH("��ֵ˰��ͨ��Ʊ����ʽ��");
		case TaxType::DZFP:return ZH("���ӷ�Ʊ");
        }
        return QString();
    }
};


