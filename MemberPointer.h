#pragma once
/*! @brief ��Ա����ָ��ת��Ϊ��������
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

