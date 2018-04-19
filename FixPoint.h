#pragma once
#include <QString>
#include <QDebug>

template<class T> class Compareable;

/*! @brief FixPoint class
 *
 * provide operator override to replace double type.
 * @author mqm
 */
template<uint IL, uint DL>    // integer length, decimal length
class FixPoint : public Compareable<FixPoint<IL,DL>>
{
    static_assert(IL > 0, "IL must > 0"); static_assert(IL > 0, "IL must > 0");
    static_assert(DL > 0, "DL must > 0");
    typedef int T;        // digit value type
public:
    enum { L = IL + DL, IntegerLength = IL, DecimalLength = DL};
    // Default constructor
    FixPoint();
    // Copy constructor
    FixPoint(const FixPoint &other);
    // Move constructor
    FixPoint(FixPoint&& other);
    // Construct with int
    FixPoint(int other);
    // Construct with QString
    FixPoint(QString other);
    // Destruct
    ~FixPoint();

    // Reset value to 0
    void clear();
    // Is value equal 0
    bool isZero() const;
    // Is value positive
    bool isPositive() const;
    // Is overflow
    bool isOverflow() const;

    // Round at specify precition, default 2
    FixPoint<IL, DL> &round(int precition = 2);
    // Convert to string
    QString toString(int precition = -1) const;
    // Convert to int
    int toInt() const;

    // operator override
    FixPoint<IL, DL> &operator=(const FixPoint<IL, DL> &other);
    FixPoint<IL, DL> &operator=(FixPoint<IL, DL>&& other);
    FixPoint<IL, DL> &operator=(int other);
    FixPoint<IL, DL> &operator=(QString other);
    FixPoint<IL, DL> operator-() const;
    FixPoint<IL, DL> operator+(const FixPoint<IL, DL> &other) const;
    FixPoint<IL, DL> operator-(const FixPoint<IL, DL> &other) const;
    FixPoint<IL, DL> operator*(const FixPoint<IL, DL> &other) const;
    FixPoint<IL, DL> operator/(const FixPoint<IL, DL> &other) const;
    FixPoint<IL, DL> &operator+=(FixPoint<IL, DL> other);
    FixPoint<IL, DL> &operator-=(FixPoint<IL, DL> other);

private:
    // construct with array
    FixPoint(T* other);
    // raw compare function return (-1,0,1)
    int compare(T* a1, T* a2, int l = L) const;
    // override virtual function from Compareable
    int compare(const FixPoint<IL, DL> &other) const override;
    // raw zero judgement
    bool isZero(T* a1, int l = L) const;
    // raw round function
    bool round(T* a1, int index) const;
    // raw operator functions, return false if overflow
    bool plus(T* a1, T* a2, T* r) const;
    bool minus(T *a1, T* a2, T* r, int l = L) const;
    bool multiply(T *a1, T a2, T* r) const;

private:
    bool positive_ = true;        // is positive
    bool overflow_ = false;        // is overflow
    T *d_ = nullptr;            // digits

#ifdef _DEBUG
    QString debug_;     // debug display value
#   define PREPARE_DEBUG_VALUE do { debug_ = toString(); } while (0)
#else
#   define PREPARE_DEBUG_VALUE
#endif
};

template<uint IL, uint DL>
bool FixPoint<IL, DL>::isOverflow() const
{
    return overflow_;
}

template<uint IL, uint DL>
FixPoint<IL, DL>::FixPoint()
{
    d_ = new T[L];
    clear();
}

template<uint IL, uint DL>
FixPoint<IL, DL>::FixPoint(const FixPoint &other)
{
    d_ = new T[L];
    operator=(other);
}

template<uint IL, uint DL>
FixPoint<IL, DL>::FixPoint(int other)
{
    d_ = new T[L];
    operator=(other);
}

template<uint IL, uint DL>
FixPoint<IL, DL>::FixPoint(QString other)
{
    if (other.contains('e', Qt::CaseInsensitive))
        other = QString::number(other.toDouble(), 'f', DL);
    
    d_ = new T[L];
    operator=(other);
}

template<uint IL, uint DL>
FixPoint<IL, DL>::FixPoint(FixPoint&& other)
{
    operator=(std::forward<FixPoint>(other));
}

template<uint IL, uint DL>
FixPoint<IL, DL>::~FixPoint()
{
    if (d_)
        delete[] d_;
}

template<uint IL, uint DL>
FixPoint<IL, DL> &FixPoint<IL, DL>::operator=(const FixPoint<IL, DL> &other)
{
    overflow_ = other.overflow_;
    positive_ = other.positive_;
    std::memcpy(d_, other.d_, sizeof(T) * L);

    PREPARE_DEBUG_VALUE;
    return *this;
}

template<uint IL, uint DL>
FixPoint<IL, DL> &FixPoint<IL, DL>::operator=(FixPoint<IL, DL>&& other)
{
    positive_ = other.positive_;
    overflow_ = other.overflow_;
    std::swap(d_, other.d_);

    PREPARE_DEBUG_VALUE;

    return *this;
}

template<uint IL, uint DL>
FixPoint<IL, DL> &FixPoint<IL, DL>::operator=(int other)
{
    clear();
    if (other < 0)
    {
        other = -other;
        positive_ = false;
    }

    for (int i = IL - 1; i >= 0 && other != 0; i--)
    {
        d_[i] = other % 10;
        other /= 10;
    }

    if (other != 0)
        overflow_ = true;

    PREPARE_DEBUG_VALUE;

    return *this;
}

template<uint IL, uint DL>
FixPoint<IL, DL> & FixPoint<IL, DL>::operator=(QString other)
{
    clear();

    int startIndex = 0;
    if (other[0] == '-')
    {
        positive_ = false;
        startIndex = 1;
    }
    else if (other[0] == '+')
    {
        startIndex = 1;
    }

    int pointIndex = other.indexOf('.', startIndex);
    if (pointIndex == -1)
        pointIndex = other.length();

    if (pointIndex - startIndex > IL)
    {
        overflow_ = true;
        return *this;
    }

    // integer part
    for (int i = pointIndex - 1; i >= startIndex; i--)
    {
        T v = other[i].digitValue();
        if (v == -1)
        {
            overflow_ = true;
            return *this;
        }
        d_[IL - pointIndex + i] = v;
    }

    // decimal part
    int endIndex = std::min<int>(other.length(), pointIndex + DL + 1);
    for (int i = pointIndex + 1; i < endIndex; i++)
    {
        T v = other[i].digitValue();
        if (v == -1)
        {
            overflow_ = true;
            return *this;
        }
        d_[IL + i - pointIndex - 1] = v;
    }

    PREPARE_DEBUG_VALUE;

    return *this;
}

template<uint IL, uint DL>
void FixPoint<IL, DL>::clear()
{
    positive_ = true;
    overflow_ = false;
    std::memset(d_, 0, sizeof(T)* L);

    PREPARE_DEBUG_VALUE;
}

template<uint IL, uint DL>
bool FixPoint<IL, DL>::isZero() const
{
    return isZero(d_);
}

template<uint IL, uint DL>
bool FixPoint<IL, DL>::isPositive() const
{
    return positive_;
}

template<uint IL, uint DL>
QString FixPoint<IL, DL>::toString(int precition) const
{
    QString result;
    T cache[L];
    std::memcpy(cache, d_, sizeof(cache));

    if (overflow_)
        return "OF";

    if (!positive_)
        result.append('-');

    // round
    if (precition >= 0)
        if (round(cache, IL + precition))
            return "OF";

    // integer begin position
    int left = 0;
    for (; left < IL; left++)
        if (cache[left] != 0)
            break;

    // decimal end position
    int right = L - 1;
    if (precition >= 0)
        right = IL + precition - 1;
    else
        for (; right >= IL; right--)
            if (cache[right] != 0)
                break;

    if (left == IL)
        result.append('0');
    else
        for (int i = left; i < IL; i++)
            result.append('0' + cache[i]);


    if (right < IL)
        return result;

    result.append('.');

    for (int i = IL; i <= right; i++)
        result.append('0' + cache[i]);

    return result;
}

template<uint IL, uint DL>
int FixPoint<IL, DL>::toInt() const
{
    int result = 0;
    // integer begin position
    int left = 0;
    for (; left < IL; left++)
        if (d_[left] != 0)
            break;

    for (int i = IL - 1; i >= left; i--)
        result += result * 10 + d_[i];

    return result;
}

template<uint IL, uint DL>
FixPoint<IL, DL>::FixPoint(T* other)
{
    d_ = new T[L];
    std::memcpy(d_, other, sizeof(T)* L);

    PREPARE_DEBUG_VALUE;
}

template<uint IL, uint DL>
int FixPoint<IL, DL>::compare(T* a1, T* a2, int l) const
{
    for (int i = 0; i < l; i++)
        if (a1[i] > a2[i])
            return 1;
        else if (a1[i] < a2[i])
            return -1;
    
    return 0;
}

template<uint IL, uint DL>
bool FixPoint<IL, DL>::isZero(T* a1, int l) const
{
    for (int i = 0; i < l; i++)
        if (a1[i] != 0)
            return false;

    return true;
}

template<uint IL, uint DL>
bool FixPoint<IL, DL>::plus(T* a1, T* a2, T* r) const
{
    bool carry = false;
    for (int i = L - 1; i >= 0; i--)
    {
        r[i] = a1[i] + a2[i];

        if (carry)
        {
            r[i]++;
            carry = false;
        }
        if (r[i] >= 10)
        {
            carry = true;
            r[i] -= 10;
        }
    }

    return carry;
}

template<uint IL, uint DL>
bool FixPoint<IL, DL>::minus(T *a1, T* a2, T* r, int l) const
{
    bool borrow = false;
    for (int i = l - 1; i >= 0; i--)
    {
        int v1 = a1[i];
        int v2 = a2[i];

        if (borrow)
        {
            v1--;
            borrow = false;
        }
        if (v1 < v2)
        {
            borrow = true;
            v1 += 10;
        }

        r[i] = v1 - v2;
    }

    return borrow;
}

template<uint IL, uint DL>
bool FixPoint<IL, DL>::multiply(T *a1, T a2, T* r) const
{
    int carry = 0;
    for (int i = L - 1; i >= 0; i--)
    {
        r[i] = a1[i] * a2;

        if (carry > 0)
        {
            r[i] += carry;
            carry = 0;
        }
        if (r[i] >= 10)
        {
            carry = r[i] / 10;
            r[i] %= 10;
        }
    }

    return carry > 0;
}

template<uint IL, uint DL>
bool FixPoint<IL, DL>::round(T* a, int index) const
{
    if (a[index] > 4)
    {
        a[--index] += 1;
        // loop carry
        while (a[index] > 9 && index > 0)
        {
            a[index - 1] += 1;
            a[index] -= 10;
            index--;
        }

        if (a[0] > 9)
        {
            a[0] -= 10;
            return true;
        }
    }

    return false;
}

template<uint IL, uint DL>
FixPoint<IL, DL> FixPoint<IL, DL>::operator-() const
{
    FixPoint<IL, DL> result(*this);
    result.positive_ = !result.positive_;

    return result;
}

template<uint IL, uint DL>
FixPoint<IL, DL> FixPoint<IL, DL>::operator+(const FixPoint<IL, DL> &other) const
{
    FixPoint<IL, DL> result;
    
    if (positive_ == other.positive_)
    {
        // [+] + [+] || [-] + [-]
        result.overflow_ = plus(d_, other.d_, result.d_);
        result.positive_ = positive_;
    }
    else
    {
        // [-] + [+] || [+] + [-]
        int relation = compare(d_, other.d_);
        if (relation > 0)
        {
            result.overflow_ = minus(d_, other.d_, result.d_);
            result.positive_ = positive_;
        }
        else if (relation < 0)
        {
            result.overflow_ = minus(other.d_, d_, result.d_);
            result.positive_ = !positive_;
        }
    }

    return result;
}

template<uint IL, uint DL>
FixPoint<IL, DL> FixPoint<IL, DL>::operator-(const FixPoint<IL, DL> &other) const
{
    FixPoint<IL, DL> result;
    
    if (positive_ != other.positive_)
    {
        // [-] - [+] || [+] - [-]
        result.overflow_ = plus(d_, other.d_, result.d_);
        result.positive_ = positive_;
    }
    else
    {
        // [+] - [+] || [-] - [-]
        int relation = compare(d_, other.d_);
        if (relation > 0)
        {
            result.overflow_ = minus(d_, other.d_, result.d_);
            result.positive_ = positive_;
        }
        else if (relation < 0)
        {
            result.overflow_ = minus(other.d_, d_, result.d_);
            result.positive_ = !positive_;
        }
    }

    return result;
}

template<uint IL, uint DL>
FixPoint<IL, DL> FixPoint<IL, DL>::operator*(const FixPoint<IL, DL> &other) const
{
    bool overflow = false;
    T resultCache[L + L - 1] = {};
    for (int i = L - 1; i >= 0; i--)
    {
        T roundCache[L] = {};    // hash round result will speed up calculation of long integer multiply
        
        overflow = multiply(d_, other.d_[i], roundCache);
        if (overflow)
            break;

        // shift plus
        overflow = plus(resultCache + i, roundCache, resultCache + i);
        if (overflow)
            break;
    }

    // overflow judgement
    if (!overflow)
        overflow = !isZero(resultCache, IL);

    // round last number
    if (!overflow)
        overflow = round(resultCache + IL, L - 1);

    FixPoint<IL, DL> result(resultCache + IL - 1);
    result.overflow_ = overflow;
    result.positive_ = !(positive_ ^ other.positive_);

    return result;
}

template<uint IL, uint DL>
FixPoint<IL, DL> FixPoint<IL, DL>::operator/(const FixPoint<IL, DL> &other) const
{
    if (other.isZero())
    {
        FixPoint<IL, DL> result;
        result.overflow_ = true;
        return result;
    }

    T c1[L + L + L + DL - 1] = {};
    T c2[L + L] = {};
    T resultCache[L + DL + 1] = {};
    std::memcpy(c1 + L + L - 1, d_, sizeof(T) * L);
    std::memcpy(c2 + L, other.d_, sizeof(T)* L);

    bool overflow = false;

    for (int i = 0; i <= L + DL; i++)
    {
        while (compare(c1 + i, c2, L + L) >= 0)
        {
            minus(c1 + i, c2, c1 + i, L + L);
            resultCache[i]++;
        }
    }

    overflow = round(resultCache, L + DL);

    if (!overflow)
        overflow = !isZero(resultCache, L - IL);

    FixPoint<IL, DL> result(resultCache + L - IL);
    result.overflow_ = overflow;
    result.positive_ = !(positive_ ^ other.positive_);

    return result;
}

template<uint IL, uint DL>
FixPoint<IL, DL>& FixPoint<IL, DL>::round(int precition)
{
    Q_ASSERT(precition < DL);
    round(d_, IL + precition);
    for (int i = IL + precition; i < L; i++)
        d_[i] = 0;

    PREPARE_DEBUG_VALUE;

    return *this;
}

template<uint IL, uint DL>
int FixPoint<IL, DL>::compare(const FixPoint<IL, DL> &other) const
{
    return positive_ != other.positive_ ? (positive_ ? 1 : -1) : compare(d_, other.d_);
}

template<uint IL, uint DL>
QTextStream &operator <<(QTextStream &s, const FixPoint<IL, DL> &fp)
{
    return s << fp.toString();
}

template<uint IL, uint DL>
FixPoint<IL, DL> & FixPoint<IL, DL>::operator+=(FixPoint<IL, DL> other)
{
    *this = std::move(operator+(other));

    PREPARE_DEBUG_VALUE;

    return *this;
}

template<uint IL, uint DL>
FixPoint<IL, DL> & FixPoint<IL, DL>::operator-=(FixPoint<IL, DL> other)
{
    *this = std::move(operator-(other));

    PREPARE_DEBUG_VALUE;

    return *this;
}

template<class T>
class Compareable
{
public:
    bool operator==(const T &other) const { return compare(other) == 0; }
    bool operator>(const T &other) const { return compare(other) > 0; }
    bool operator<(const T &other) const { return compare(other) < 0; }
    bool operator>=(const T &other) const { return compare(other) >= 0; }
    bool operator<=(const T &other) const { return compare(other) <= 0; }
    bool operator!=(const T &other) const { return compare(other) != 0; }
protected:
    virtual int compare(const T &other) const = 0;
};
