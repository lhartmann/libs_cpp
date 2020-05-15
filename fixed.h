/***************************************************************************
 *   Copyright (C) 2015 by Lucas V. Hartmann <lucas.hartmann@gmail.com>    *
 *                                                                         *
 *   LICENSE: GNU LGPL                                                     *
 *                                                                         *
 ***************************************************************************/

#ifndef FIXED_H
#define FIXED_H
#include <stdint.h>

template <unsigned q> struct fixed32;
template <unsigned q> struct fixed64;

template <unsigned _q> struct fixed64 {
	enum { q = _q };

    int64_t val;
    fixed64(int64_t v=0) : val(v) {}
    template <unsigned qr> fixed64(const fixed64<qr> &r);
    template <unsigned qr> fixed64(const fixed32<qr> &r);
    int64_t int_part() const {
        return val >> q;
    }
    uint64_t frac_part() const {
        return val << (64-q);
    }
};

template <unsigned _q> struct fixed32 {
	enum { q = _q };

    int32_t val;
    fixed32(int32_t v=0) : val(v) {}
    template <unsigned qr> fixed32(const fixed32<qr> &r);
    template <unsigned qr> fixed32(const fixed64<qr> &r);

    fixed32(double d) : val(d * (1<<q) + 0.5) {}
    double to_double() const {
        return double(val) / (1<<q);
    }

    int32_t int_part() const {
        return val >> q;
    }
    uint32_t frac_part() const {
        return val << (32-q);
    }

    template <unsigned qr> fixed32 &operator=(const fixed32<qr> &r);
    template <unsigned qr> fixed32 &operator=(const fixed64<qr> &r);

    template <unsigned qr> fixed32<_q> operator+=(const fixed32<qr> &r);
    template <unsigned qr> fixed32<_q> operator-=(const fixed32<qr> &r);
    template <unsigned qr> fixed32<_q> operator*=(const fixed32<qr> &r);
    template <unsigned qr> fixed32<_q> operator/=(const fixed32<qr> &r);

    template <unsigned qr> fixed32< (_q>qr)?_q:qr > operator+(const fixed32<qr> &r) const;
    template <unsigned qr> fixed32< (_q>qr)?_q:qr > operator-(const fixed32<qr> &r) const;
    template <unsigned qr> fixed64<_q+qr> operator*(const fixed32<qr> &r) const;
    template <unsigned qr> fixed64<_q+32-qr> operator/(const fixed32<qr> &r) const;
    fixed32<_q> operator >> (int n);
    fixed32<_q> operator << (int n);

    template <unsigned qr> bool operator == (const fixed32<qr> &r) const;
    template <unsigned qr> bool operator != (const fixed32<qr> &r) const;
    template <unsigned qr> bool operator >  (const fixed32<qr> &r) const;
    template <unsigned qr> bool operator >= (const fixed32<qr> &r) const;
    template <unsigned qr> bool operator <  (const fixed32<qr> &r) const;
    template <unsigned qr> bool operator <= (const fixed32<qr> &r) const;
};

// Fixed64 methods
template<unsigned q> template<unsigned qr>
fixed64<q>::fixed64(const fixed64<qr> &r) {
    if (q > qr) val = r.val << (q-qr);
    else        val = r.val >> (qr-q);
}

template<unsigned q> template <unsigned qr>
fixed64<q>::fixed64(const fixed32<qr> &r) {
    if (q > qr) val = r.val << (q-qr);
    else        val = r.val >> (qr-q);
}

// If qa!=qb then prefer range over precision
template<unsigned qa, unsigned qb>
inline fixed64< (qa<qb)?qa:qb > operator+(const fixed64<qa> &a, const fixed64<qb> &b) {
    return fixed64<(qa<qb)?qa:qb>(
        fixed64<(qa<qb)?qa:qb>( a ).val +
        fixed64<(qa<qb)?qa:qb>( b ).val
    );
}

// Fixed32 methods
template<unsigned q> template <unsigned qr>
fixed32<q>::fixed32(const fixed32<qr> &r) {
    if (q > qr) val = r.val << (q-qr);
    else        val = r.val >> (qr-q);
}

template<unsigned q> template <unsigned qr>
fixed32<q>::fixed32(const fixed64<qr> &r) {
    if (q > qr) val = r.val << (q-qr);
    else        val = r.val >> (qr-q);
}

template<unsigned q> template <unsigned qr>
fixed32<q> &fixed32<q>::operator=(const fixed32<qr> &r) {
    if (q > qr) val = r.val << (q-qr);
    else        val = r.val >> (qr-q);
}

template<unsigned q> template <unsigned qr>
fixed32<q> &fixed32<q>::operator=(const fixed64<qr> &r) {
    if (q > qr) val = r.val << (q-qr);
    else        val = r.val >> (qr-q);
    return *this;
}

template<unsigned q> template <unsigned qr>
fixed32<q> fixed32<q>::operator+=(const fixed32<qr> &r) {
	return *this = *this + r;
}

template<unsigned q> template <unsigned qr>
fixed32<q> fixed32<q>::operator-=(const fixed32<qr> &r) {
	return *this = *this - r;
}

template<unsigned q> template <unsigned qr>
fixed32<q> fixed32<q>::operator*=(const fixed32<qr> &r) {
	return *this = *this * r;
}

template<unsigned q> template <unsigned qr>
fixed32<q> fixed32<q>::operator/=(const fixed32<qr> &r) {
	return *this = *this / r;
}

template<unsigned q> template<unsigned qr>
fixed32< (q>qr)?q:qr > fixed32<q>::operator+(const fixed32<qr> &r) const {
    return fixed32<(q>qr)?q:qr>(
        fixed32<(q>qr)?q:qr>( *this ).val +
        fixed32<(q>qr)?q:qr>( r ).val
    );
}

template<unsigned q> template<unsigned qr>
fixed32< (q>qr)?q:qr > fixed32<q>::operator-(const fixed32<qr> &r) const {
    return fixed32<(q>qr)?q:qr>(
        fixed32<(q>qr)?q:qr>( *this ).val -
        fixed32<(q>qr)?q:qr>( r ).val
    );
}

template<unsigned q> template<unsigned qr>
bool fixed32<q>::operator == (const fixed32<qr> &r) const {
    if (q > qr) {
        if (val != r.val << (q-qr)) return false;
        if (val >> (q-qr) != r.val) return false;
        return true;
    }
    if (q < qr) {
        if (val != r.val >> (qr-q)) return false;
        if (val << (qr-q) != r.val) return false;
        return true;
    }
    return val == r.val;
}

template<unsigned q> template<unsigned qr>
bool fixed32<q>::operator != (const fixed32<qr> &r) const {
    return !(*this == r);
}

template<unsigned q> template<unsigned qr>
bool fixed32<q>::operator >  (const fixed32<qr> &r) const {
    if (q > qr) {
        return r < *this;
    }
    if (q < qr) {
        return val > r.val >> (qr-q);
    }
    return val > r.val;
}

template<unsigned q> template<unsigned qr>
bool fixed32<q>::operator >= (const fixed32<qr> &r) const {
    return !(r > *this);
}

template<unsigned q> template<unsigned qr>
bool fixed32<q>::operator <  (const fixed32<qr> &r) const {
    if (q > qr) {
        return val >> (q-qr) < r.val;
    }
    if (q < qr) {
        return r > *this;
    }
    return val < r.val;
}

template<unsigned q> template<unsigned qr>
bool fixed32<q>::operator <= (const fixed32<qr> &r) const {
    return r > *this;
}


template<unsigned q> template <unsigned qr>
fixed64<q+qr> fixed32<q>::operator*(const fixed32<qr> &r) const {
    return fixed64<q+qr>(int64_t(val)*r.val);
}

template <unsigned q> template <unsigned qr>
fixed64<q+32-qr> fixed32<q>::operator/(const fixed32<qr> &r) const {
    // Q(X+32-Y) = Q(X+32) / Q(Y)
    return fixed64<q+32-qr>((int64_t(val)<<32)/r.val);
}

template <unsigned q>
fixed32<q> fixed32<q>::operator >> (int n) {
	return fixed32<q>(val >> n);
}

template <unsigned q>
fixed32<q> fixed32<q>::operator << (int n) {
	return fixed32<q>(val << n);
}


template <unsigned q>
fixed32<q> floor(fixed32<q> x) {
    int32_t m = 0x7FFFFFFF >> (31-q);
    return fixed32<q>(x.val & ~m);
}

template <unsigned q>
fixed32<q> ceil(fixed32<q> x) {
    int32_t m = 0x7FFFFFFF >> (31-q);
    return fixed32<q>((x.val+m) & ~m);
}

// Shortcut typenames
#ifdef FIXED_SHORT_NAMES
typedef fixed32< 0> f32q0;
typedef fixed32< 1> f32q1;
typedef fixed32< 2> f32q2;
typedef fixed32< 3> f32q3;
typedef fixed32< 4> f32q4;
typedef fixed32< 5> f32q5;
typedef fixed32< 6> f32q6;
typedef fixed32< 7> f32q7;
typedef fixed32< 8> f32q8;
typedef fixed32< 9> f32q9;
typedef fixed32<10> f32q10;
typedef fixed32<11> f32q11;
typedef fixed32<12> f32q12;
typedef fixed32<13> f32q13;
typedef fixed32<14> f32q14;
typedef fixed32<15> f32q15;
typedef fixed32<16> f32q16;
typedef fixed32<17> f32q17;
typedef fixed32<18> f32q18;
typedef fixed32<19> f32q19;
typedef fixed32<20> f32q20;
typedef fixed32<21> f32q21;
typedef fixed32<22> f32q22;
typedef fixed32<23> f32q23;
typedef fixed32<24> f32q24;
typedef fixed32<25> f32q25;
typedef fixed32<26> f32q26;
typedef fixed32<27> f32q27;
typedef fixed32<28> f32q28;
typedef fixed32<29> f32q29;
typedef fixed32<30> f32q30;
#endif

#endif // FIXED_H
