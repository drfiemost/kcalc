/*
    kCalculator, a scientific calculator for the X window system using the
    Qt widget libraries, available at no cost at http://www.troll.no

    The stack engine contained in this file was take from
    Martin Bartlett's xfrmcalc

    portions:	Copyright (C) 2003-2005 Klaus Niederkrueger

    portions:	Copyright (C) 1996 Bernd Johannes Wuebben
                                   wuebben@math.cornell.edu

    portions: 	Copyright (C) 1995 Martin Bartlett

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/


#if defined(_ISOC99_SOURCE)
	#include <cassert>
	#include <cstdio>
	#include <climits>
	#include <csignal>
	#include <cerrno>
	#include <cstring>
	using namespace std;
#else
	#include <limits.h>
	#include <stdio.h>
	#include <assert.h>	
	#include <signal.h>
	#include <errno.h>
	#include <string.h>
#endif

#include <config.h>
#include "kcalc_core.h"
#include <stdlib.h>
#include <klocale.h>
#include <kmessagebox.h>
#include "kcalctype.h"

#ifndef HAVE_FUNC_ISINF
        #ifdef HAVE_IEEEFP_H
                #include <ieeefp.h>
        #else
                #include <math.h>
        #endif

int isinf(double x) { return !finite(x) && x==x; }
#endif


static void fpe_handler(int fpe_parm)
{
	UNUSED(fpe_parm);
	//	display_error = true;
	//tmp_number = 0L;
}


static bool _error;

static KNumber ExecOr(const KNumber & left_op, const KNumber & right_op)
{
	return (left_op | right_op);
}

static KNumber ExecXor(const KNumber & left_op, const KNumber & right_op)
{
	return (left_op | right_op) - (left_op & right_op);
}

static KNumber ExecAnd(const KNumber & left_op, const KNumber & right_op)
{
	return (left_op & right_op);
}

static KNumber ExecLsh(const KNumber & left_op, const KNumber & right_op)
{
	return left_op << right_op;
}

static KNumber ExecRsh(const KNumber & left_op, const KNumber & right_op)
{
	return left_op >> right_op;
}

static KNumber ExecAdd(const KNumber & left_op, const KNumber & right_op)
{
	return left_op + right_op;
}

static KNumber ExecSubtract(const KNumber & left_op, const KNumber & right_op)
{
	return left_op - right_op;
}

static KNumber ExecMultiply(const KNumber & left_op, const KNumber & right_op)
{
	return left_op * right_op;
}

static KNumber ExecDivide(const KNumber & left_op, const KNumber & right_op)
{
	return left_op / right_op;
}

static KNumber ExecMod(const KNumber & left_op, const KNumber & right_op)
{
	return left_op % right_op;
}

static KNumber ExecIntDiv(const KNumber & left_op, const KNumber & right_op)
{
  	return (left_op / right_op).integerPart();
}

bool isoddint(const KNumber & input)
{
	if (input.type() != KNumber::IntegerType) return false;
	// Routine to check if KNumber is an Odd integer
	return ( (input / KNumber(2)).type() == KNumber::IntegerType);
}

static KNumber ExecPower(const KNumber & left_op, const KNumber & right_op)
{
  return KNumber::Zero;
#if 0
  if (right_op == KNumber::Zero)
	  if (left_op == KNumber::Zero) // 0^0 not defined
		{
			_error = true;		
			return KNumber::Zero;
		}
		else
			return KNumber::One;

	if (left_op < 0 && isoddint(1 / right_op))
		left_op = - POW((-left_op), right_op);
	else
		left_op = POW(left_op, right_op);

	if (errno == EDOM || errno == ERANGE)
	{
		_error = true;
		return KNumber::Zero;
	}
	else
		return left_op;
#endif
}

static KNumber ExecPwrRoot(const KNumber & left_op, const KNumber & right_op)
{
			return KNumber::Zero;
#if 0
	// printf("ExecPwrRoot  %g left_op, %g right_op\n", left_op, right_op);
	if (right_op == 0)
	{
		_error = true;
		return 0L;
	}

	if (left_op < 0 && isoddint(right_op))
		left_op = - POW((-left_op), (1L)/right_op);
	else
		left_op = POW(left_op, (1L)/right_op);

	if (errno == EDOM || errno == ERANGE)
	{
		_error = true;
		return 0;
	}
	else
		return left_op;
#endif
}

static KNumber ExecAddP(const KNumber & left_op, const KNumber & right_op)
{
	return left_op * (KNumber::One + right_op/KNumber(100));
}

static KNumber ExecSubP(const KNumber & left_op, const KNumber & right_op)
{
	return left_op * (KNumber::One - right_op/KNumber(100));
}

static KNumber ExecMultiplyP(const KNumber & left_op, const KNumber & right_op)
{
	return left_op * right_op / KNumber(100);
}

static KNumber ExecDivideP(const KNumber & left_op, const KNumber & right_op)
{
	return left_op * KNumber(100) / right_op;
}


// build precedence list
const struct operator_data CalcEngine::Operator[] = {
  { 0, NULL,     NULL}, // FUNC_EQUAL
  { 0, NULL,     NULL}, // FUNC_PERCENT
  { 0, NULL,     NULL}, // FUNC_BRACKET
  { 1, ExecOr,   NULL}, // FUNC_OR
  { 2, ExecXor,  NULL}, // FUNC_XOR
  { 3, ExecAnd,  NULL}, // FUNC_AND
  { 4, ExecLsh,  NULL}, // FUNC_LSH
  { 4, ExecRsh,  NULL}, // FUNC_RSH
  { 5, ExecAdd,  ExecAddP}, // FUNC_ADD
  { 5, ExecSubtract, ExecSubP}, // FUNC_SUBTRACT
  { 6, ExecMultiply, ExecMultiplyP}, // FUNC_MULTIPLY
  { 6, ExecDivide,   ExecDivideP}, // FUNC_DIVIDE
  { 6, ExecMod,  NULL}, // FUNC_MOD
  { 6, ExecIntDiv, NULL}, // FUNC_INTDIV
  { 7, ExecPower,  NULL}, // FUNC_POWER
  { 7, ExecPwrRoot, NULL} // FUNC_PWR_ROOT
};


CalcEngine::CalcEngine()
  :   _percent_mode(false)
{
	//
	// Basic initialization involves initializing the calcultion
	// stack, and setting up the floating point excetion signal
	// handler to trap the errors that the code can/has not been
	// written to trap.
	//

	struct sigaction fpe_trap;

	sigemptyset(&fpe_trap.sa_mask);
	fpe_trap.sa_handler = &fpe_handler;
#ifdef SA_RESTART
	fpe_trap.sa_flags = SA_RESTART;
#endif
	sigaction(SIGFPE, &fpe_trap, NULL);

	_last_number = KNumber::Zero;
	_error = false;
}

KNumber CalcEngine::lastOutput(bool &error) const
{
	error = _error;
	return _last_number;
}

void CalcEngine::ArcCosDeg(KNumber input)
{
#if 0
	KNumber tmp = ACOS(input);

	_last_number = Rad2Deg(tmp);

	//if (errno == EDOM || errno == ERANGE)
	//	_error = true;
#endif
}

void CalcEngine::ArcCosRad(KNumber input)
{
#if 0
	KNumber tmp = ACOS(input);
	
	_last_number = tmp;

	//if (errno == EDOM || errno == ERANGE)
	//	_error = true;
#endif
}

void CalcEngine::ArcCosGrad(KNumber input)
{
#if 0
	KNumber tmp = ACOS(input);

	_last_number = Rad2Gra(tmp);

	//if (errno == EDOM || errno == ERANGE)
	//	_error = true;
#endif
}

void CalcEngine::ArcSinDeg(KNumber input)
{
#if 0
	KNumber tmp = ASIN(input);
	
	_last_number = Rad2Deg(tmp);

	//if (errno == EDOM || errno == ERANGE)
	//	_error = true;
#endif
}

void CalcEngine::ArcSinRad(KNumber input)
{
#if 0
	KNumber tmp = ASIN(input);

	_last_number = tmp;

	//if (errno == EDOM || errno == ERANGE)
	//	_error = true;
#endif
}

void CalcEngine::ArcSinGrad(KNumber input)
{
#if 0
	KNumber tmp = ASIN(input);

	_last_number = Rad2Gra(tmp);

	//if (errno == EDOM || errno == ERANGE)
	//	_error = true;
#endif
}

void CalcEngine::ArcTangensDeg(KNumber input)
{
#if 0
	KNumber tmp = ATAN(input);
	
	_last_number = Rad2Deg(tmp);

	//if (errno == EDOM || errno == ERANGE)
	//	_error = true;
#endif
}

void CalcEngine::ArcTangensRad(KNumber input)
{
#if 0
	_last_number = ATAN(input);

	//if (errno == EDOM || errno == ERANGE)
	//	_error = true;
#endif
}

void CalcEngine::ArcTangensGrad(KNumber input)
{
#if 0
	KNumber tmp = ATAN(input);

	_last_number = Rad2Gra(tmp);

	//if (errno == EDOM || errno == ERANGE)
	//	_error = true;
#endif
}

void CalcEngine::AreaCosHyp(KNumber input)
{
	CALCAMNT tmp_num = input.toQString().toDouble();
	_last_number = KNumber(double(ACOSH(tmp_num)));
}

void CalcEngine::AreaSinHyp(KNumber input)
{
	CALCAMNT tmp_num = input.toQString().toDouble();
	_last_number = KNumber(double(ASINH(tmp_num)));
}

void CalcEngine::AreaTangensHyp(KNumber input)
{
	CALCAMNT tmp_num = input.toQString().toDouble();
	_last_number = KNumber(double(ATANH(tmp_num)));
}

void CalcEngine::Complement(KNumber input)
{
#if 0
	KNumber boh_work_d;

	MODF(input, &boh_work_d);

	if (boh_work_d.abs() > KCALC_LONG_MAX)
	{
		_error = true;
		return;
	}

	KCALC_LONG boh_work = static_cast<KCALC_LONG>(boh_work_d);

	_last_number = ~boh_work;
#endif
}

void CalcEngine::CosDeg(KNumber input)
{
#if 0
	KNumber tmp = input;
	
	tmp = Deg2Rad(input);

	_last_number = COS(tmp);

	// Now a cheat to help the weird case of COS 90 degrees not being 0!!!
	if (_last_number < POS_ZERO && _last_number > NEG_ZERO)
		_last_number = KNumber::Zero;

	//if (errno == EDOM || errno == ERANGE)
	//	_error = true;
#endif
}

void CalcEngine::CosRad(KNumber input)
{
#if 0
	KNumber tmp = input;

	_last_number = COS(tmp);

	// Now a cheat to help the weird case of COS 90 degrees not being 0!!!
	if (_last_number < POS_ZERO && _last_number > NEG_ZERO)
		_last_number = KNumber::Zero;

	//if (errno == EDOM || errno == ERANGE)
	//	_error = true;
#endif
}

void CalcEngine::CosGrad(KNumber input)
{
#if 0
	KNumber tmp = input;

	tmp = Gra2Rad(input);

	_last_number = COS(tmp);

	// Now a cheat to help the weird case of COS 90 degrees not being 0!!!
	if (_last_number < POS_ZERO && _last_number > NEG_ZERO)
		_last_number = KNumber::Zero;

	//if (errno == EDOM || errno == ERANGE)
	//	_error = true;
#endif
}

void CalcEngine::CosHyp(KNumber input)
{
	CALCAMNT tmp_num = input.toQString().toDouble();
	_last_number = KNumber(double(COSH(tmp_num)));
}

void CalcEngine::Cube(KNumber input)
{
	_last_number = input*input*input;
}

void CalcEngine::CubeRoot(KNumber input)
{
	_last_number = input.cbrt();
}

void CalcEngine::Exp(KNumber input)
{
	CALCAMNT tmp_num = input.toQString().toDouble();
	_last_number = KNumber(double(EXP(tmp_num)));
}

void CalcEngine::Exp10(KNumber input)
{
#if 0
	_last_number = POW(10, input);
#endif
}


static KNumber _factorial(KNumber input)
{
	KNumber tmp_amount = input;

	// don't do recursive factorial,
	// because large numbers lead to
	// stack overflows
	while (tmp_amount > KNumber::One)
	{
		tmp_amount -= KNumber::One;

		input = tmp_amount * input;

	}

	if (tmp_amount < KNumber::One)
		return KNumber::One;
	return input;
}


void CalcEngine::Factorial(KNumber input)
{
	KNumber tmp_amount = input.integerPart();

	if (input < KNumber::Zero)
	{
		_error = true;
		return;
	}

	_last_number = _factorial(tmp_amount);
}

void CalcEngine::InvertSign(KNumber input)
{
	_last_number = -input;
}

void CalcEngine::Ln(KNumber input)
{
	if (input <= KNumber::Zero)
		_error = true;
	else {
		CALCAMNT tmp_num = input.toQString().toDouble();
		_last_number = KNumber(double(LN(tmp_num)));
	}
}

void CalcEngine::Log10(KNumber input)
{
	if (input <= KNumber::Zero)
		_error = true;
	else {
		CALCAMNT tmp_num = input.toQString().toDouble();
		_last_number = KNumber(double(LOG_TEN(tmp_num)));
	}
}

void CalcEngine::ParenClose(KNumber input)
{
 	// evaluate stack until corresponding opening bracket
	while (!_stack.isEmpty())
	{
		_node tmp_node = _stack.pop();
		if (tmp_node.operation == FUNC_BRACKET)
			break;
		input = evalOperation(tmp_node.number, tmp_node.operation,
				      input);
	}
	_last_number = input;
	return;
}

void CalcEngine::ParenOpen(KNumber input)
{
	enterOperation(input, FUNC_BRACKET);
}

void CalcEngine::Reciprocal(KNumber input)
{
	_last_number = KNumber::One/input;
}

void CalcEngine::SinDeg(KNumber input)
{
#if 0
	KNumber tmp = input;

	tmp = Deg2Rad(input);

	_last_number = SIN(tmp);

	// Now a cheat to help the weird case of COS 90 degrees not being 0!!!
	if (_last_number < POS_ZERO && _last_number > NEG_ZERO)
		_last_number = KNumber::Zero;

	//if (errno == EDOM || errno == ERANGE)
	//	_error = true;
#endif
}

void CalcEngine::SinRad(KNumber input)
{
#if 0
	KNumber tmp = input;

	_last_number = SIN(tmp);

	// Now a cheat to help the weird case of COS 90 degrees not being 0!!!
	if (_last_number < POS_ZERO && _last_number > NEG_ZERO)
		_last_number = KNumber::Zero;

	//if (errno == EDOM || errno == ERANGE)
	//	_error = true;
#endif
}

void CalcEngine::SinGrad(KNumber input)
{
#if 0
	KNumber tmp = input;

	tmp = Gra2Rad(input);

	_last_number = SIN(tmp);

	// Now a cheat to help the weird case of COS 90 degrees not being 0!!!
	if (_last_number < POS_ZERO && _last_number > NEG_ZERO)
		_last_number = KNumber::Zero;

	//if (errno == EDOM || errno == ERANGE)
	//	_error = true;
#endif
}

void CalcEngine::SinHyp(KNumber input)
{
	CALCAMNT tmp_num = input.toQString().toDouble();
	_last_number = KNumber(double(SINH(tmp_num)));
}

void CalcEngine::Square(KNumber input)
{
	_last_number = input*input;
}

void CalcEngine::SquareRoot(KNumber input)
{
	_last_number = input.sqrt();
}

void CalcEngine::StatClearAll(KNumber input)
{
	UNUSED(input);
	stats.clearAll();
}

void CalcEngine::StatCount(KNumber input)
{
	UNUSED(input);
	_last_number = KNumber(stats.count());
}

void CalcEngine::StatDataNew(KNumber input)
{
	stats.enterData(input);
	_last_number = KNumber(stats.count());
}

void CalcEngine::StatDataDel(KNumber input)
{
	UNUSED(input);
	stats.clearLast();
	_last_number = KNumber::Zero;
}

void CalcEngine::StatMean(KNumber input)
{
	UNUSED(input);
	_last_number = stats.mean();

	_error = stats.error();
}

void CalcEngine::StatMedian(KNumber input)
{
	UNUSED(input);
	_last_number = stats.median();

	_error = stats.error();
}

void CalcEngine::StatStdDeviation(KNumber input)
{
	UNUSED(input);
	_last_number = stats.std();

	_error = stats.error();
}

void CalcEngine::StatStdSample(KNumber input)
{
	UNUSED(input);
	_last_number = stats.sample_std();

	_error = stats.error();
}

void CalcEngine::StatSum(KNumber input)
{
	UNUSED(input);
	_last_number = stats.sum();
}

void CalcEngine::StatSumSquares(KNumber input)
{
	UNUSED(input);
	_last_number = stats.sum_of_squares();

	_error = stats.error();
}

void CalcEngine::TangensDeg(KNumber input)
{
	KNumber aux, tmp = Deg2Rad(input);
	
	aux = tmp.abs();
	// put aux between 0 and pi
	while (aux > KNumber::Pi) aux -= KNumber::Pi;
	// if were are really close to pi/2 throw an error
	// tan(pi/2) => inf
	// using the 10 factor because without it 270Âº tan still gave a result
#warning is this still necesary
	//	if ( (aux - pi/2 < POS_ZERO * 10) && (aux - pi/2 > NEG_ZERO * 10) )
	//		_error = true;
	//	else
		CALCAMNT tmp_num = tmp.toQString().toDouble();
		_last_number = KNumber(double(TAN(tmp_num)));

	// Now a cheat to help the weird case of TAN 0 degrees not being 0!!!
		//	if (_last_number < POS_ZERO && _last_number > NEG_ZERO)
		//		_last_number = KNumber::Zero;

	//if (errno == EDOM || errno == ERANGE)
	//	_error = true;
}

void CalcEngine::TangensRad(KNumber input)
{
	KNumber aux, tmp = input;
	
	aux = tmp.abs();
	// put aux between 0 and pi
	while (aux > KNumber::Pi) aux -= KNumber::Pi;
	// if were are really close to pi/2 throw an error
	// tan(pi/2) => inf
	// using the 10 factor because without it 270Âº tan still gave a result
#warning is this still necesary
	//if ( (aux - pi/2 < POS_ZERO * 10) && (aux - pi/2 > NEG_ZERO * 10) )
	//		_error = true;
	//	else
		CALCAMNT tmp_num = tmp.toQString().toDouble();
		_last_number = KNumber(double(TAN(tmp_num)));

	// Now a cheat to help the weird case of TAN 0 degrees not being 0!!!
#warning is this still necesary
	//	if (_last_number < POS_ZERO && _last_number > NEG_ZERO)
	//	_last_number = KNumber::Zero;

	//if (errno == EDOM || errno == ERANGE)
	//	_error = true;
}

void CalcEngine::TangensGrad(KNumber input)
{
	KNumber aux, tmp = Gra2Rad(input);
	
	aux = tmp.abs();
	// put aux between 0 and pi
	while (aux > KNumber::Pi) aux -= KNumber::Pi;
	// if were are really close to pi/2 throw an error
	// tan(pi/2) => inf
	// using the 10 factor because without it 270Âº tan still gave a result
	if(false)
#warning is this still necesary
	  //if ( (aux - KNumber::Pi/2 < POS_ZERO * 10) && (aux - KNumber::Pi/2 > NEG_ZERO * 10) )
		_error = true;
	else {
		CALCAMNT tmp_num = tmp.toQString().toDouble();
		_last_number = KNumber(double(TAN(tmp_num)));
	}
	// Now a cheat to help the weird case of TAN 0 degrees not being 0!!!
#warning is this still necesary
	//	if (_last_number < POS_ZERO && _last_number > NEG_ZERO)
	//	_last_number = KNumber::Zero;

	//if (errno == EDOM || errno == ERANGE)
	//	_error = true;
}

void CalcEngine::TangensHyp(KNumber input)
{
	CALCAMNT tmp_num = input.toQString().toDouble();
	_last_number = KNumber(double(TANH(tmp_num)));
}

KNumber CalcEngine::evalOperation(KNumber arg1, Operation operation,
				   KNumber arg2)
{
	if (!_percent_mode || Operator[operation].prcnt_ptr == NULL)
	{
		return (Operator[operation].arith_ptr)(arg1, arg2);
	} else {
		_percent_mode = false;
		return (Operator[operation].prcnt_ptr)(arg1, arg2);
	}
}

void CalcEngine::enterOperation(KNumber number, Operation func)
{
	_node tmp_node;

	if (func == FUNC_BRACKET)
	{
		tmp_node.number = 0;
		tmp_node.operation = FUNC_BRACKET;
	
		_stack.push(tmp_node);

		return;
	}
	
	if (func == FUNC_PERCENT)
	{
		_percent_mode = true;
	}

	tmp_node.number = number;
	tmp_node.operation = func;
	
	_stack.push(tmp_node);

	evalStack();
}

bool CalcEngine::evalStack(void)
{
	// this should never happen
	if (_stack.isEmpty()) KMessageBox::error(0L, i18n("Stack processing error - empty stack"));

	_node tmp_node = _stack.pop();

	while (! _stack.isEmpty())
	{
		_node tmp_node2 = _stack.pop();
		if (Operator[tmp_node.operation].precedence <=
		    Operator[tmp_node2.operation].precedence)
		{
			if (tmp_node2.operation == FUNC_BRACKET) continue;
			KNumber tmp_result =
			  evalOperation(tmp_node2.number, tmp_node2.operation,
					tmp_node.number);
			tmp_node.number = tmp_result;
		}
		else
		{
			_stack.push(tmp_node2);
			break;
		}  
		  
	}

	if(tmp_node.operation != FUNC_EQUAL  &&  tmp_node.operation != FUNC_PERCENT)
		_stack.push(tmp_node);

	_last_number = tmp_node.number;
	return true;
}

void CalcEngine::Reset()
{
	_error = false;
	_last_number = KNumber::Zero;

	_stack.clear();
}


