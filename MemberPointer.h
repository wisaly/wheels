#pragma once
/*! @brief 成员函数指针转换为基础类型
 *
 * 
 * @author mqm
 */

template <class CLASS, typename TYPE, TYPE CLASS::*MEMBER>
class MemberPointer
{
public:
    MemberPointer() { u.m = MEMBER; }
    ~MemberPointer() { }

    size_t ptr() { return u.v; }

private:
    union ConvertUnion
    {
    public:
        decltype(MEMBER) m;
        size_t v;
    } u;
};

