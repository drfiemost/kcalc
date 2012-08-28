
#ifndef KNUMBER_INTEGER_H_
#define KNUMBER_INTEGER_H_

#include "knumber_base.h"

class KNumber;

namespace detail {

class knumber_integer : public knumber_base {
	friend class ::KNumber;
	friend class knumber_error;
	friend class knumber_fraction;
	friend class knumber_float;

public:
	explicit knumber_integer(const QString &s);
	explicit knumber_integer(qint32 value);
	explicit knumber_integer(qint64 value);
	explicit knumber_integer(quint32 value);
	explicit knumber_integer(quint64 value);
	explicit knumber_integer(mpz_t mpz);
	virtual ~knumber_integer();
	
public:
	virtual knumber_base *clone();

public:
	virtual QString toString(int precision) const;
	virtual quint64 toUint64() const;
	virtual qint64 toInt64() const;
	
public:
	virtual bool is_even() const;
	virtual bool is_odd() const;
	virtual bool is_integer() const;
	virtual bool is_zero() const;
	virtual int sign() const;

public:
	virtual knumber_base *add(knumber_base *rhs);
	virtual knumber_base *sub(knumber_base *rhs);
	virtual knumber_base *mul(knumber_base *rhs);
	virtual knumber_base *div(knumber_base *rhs);
	virtual knumber_base *mod(knumber_base *rhs);
	
public:
	virtual knumber_base *bitwise_and(knumber_base *rhs);
	virtual knumber_base *bitwise_xor(knumber_base *rhs);
	virtual knumber_base *bitwise_or(knumber_base *rhs);
	virtual knumber_base *bitwise_shift(knumber_base *rhs);
	
public:
	virtual knumber_base *pow(knumber_base *rhs);
	virtual knumber_base *neg();
	virtual knumber_base *cmp();
	virtual knumber_base *abs();
	virtual knumber_base *sqrt();
	virtual knumber_base *cbrt();
	virtual knumber_base *factorial();
	virtual knumber_base *reciprocal();
	
public:
	virtual knumber_base *log2();
	virtual knumber_base *log10();
	virtual knumber_base *ln();
	virtual knumber_base *exp2();
	virtual knumber_base *exp10();
	virtual knumber_base *exp();
	virtual knumber_base *bin(knumber_base *rhs);
	
public:
	virtual knumber_base *sin();
	virtual knumber_base *cos();
	virtual knumber_base *tan();
	virtual knumber_base *asin();
	virtual knumber_base *acos();
	virtual knumber_base *atan();
	virtual knumber_base *sinh();
	virtual knumber_base *cosh();
	virtual knumber_base *tanh();
	virtual knumber_base *asinh();
	virtual knumber_base *acosh();
	virtual knumber_base *atanh();
	
public:
	virtual int compare(knumber_base *rhs);

private:
	// conversion constructors
	explicit knumber_integer(const knumber_integer *value);
	explicit knumber_integer(const knumber_fraction *value);
	explicit knumber_integer(const knumber_float *value);
	explicit knumber_integer(const knumber_error *value);

private:
	mpz_t mpz_;
};

}

#endif
