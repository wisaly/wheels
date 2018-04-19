#pragma once
#include <QList>
#include "FieldAccessor.h"

/*!
 * \file DatabaseRecord.h
 *
 * \author mqm
 *
 * 数据库表字段映射宏，用于实现数据库自动化
 */

#define DEFINE_TABLE(RECORD_CLASS, TABLE_NAME)\
class RECORD_CLASS##Defination {\
public:\
    QString tableName_;\
    typedef RECORD_CLASS RecordClass;\
    QList<FieldAccessorBase*> fields_;\
    FieldAccessorBase* idField_ = nullptr;\
    ~RECORD_CLASS##Defination() { for (FieldAccessorBase *p : fields_) delete p; }\
    RECORD_CLASS##Defination(){\
    tableName_ = TABLE_NAME;

#define DEFINE_FIELD(MEMBER, FIELD, DISPLAY)\
    fields_ << new FieldAccessor<RecordClass, decltype(RecordClass::MEMBER), &RecordClass::MEMBER>\
    (FIELD, DISPLAY);

#define END_DEFINE_TABLE(ID_FIELD) if(ID_FIELD != -1) idField_ = fields_[ID_FIELD]; }};

#define DEFINE_SIMPLE_FIELD(FIELD) DEFINE_FIELD(FIELD##_,#FIELD,"")