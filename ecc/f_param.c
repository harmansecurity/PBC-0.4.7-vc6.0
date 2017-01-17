#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <gmp.h>
#include "pbc_fops.h"
#include "pbc_symtab.h"
#include "pbc_darray.h"
#include "pbc_field.h"
#include "pbc_fp.h"
#include "pbc_fieldquadratic.h"
#include "pbc_pairing.h"
#include "pbc_f_param.h"
#include "pbc_param.h"
#include "pbc_poly.h"
#include "pbc_curve.h"
#include "pbc_tracker.h"
#include "pbc_memory.h"
#include "pbc_utils.h"

struct f_pairing_data_s {
    field_t Fq, Fq2, Fq2x, Fq12;
    field_t Eq, Etwist;
    element_t negalpha;
    element_t negalphainv;
    mpz_t tateexp;

    //for tate exponentiation speedup:
    //x^{q^k} for various k
    element_t xpowq2, xpowq6, xpowq8;
};
typedef struct f_pairing_data_s f_pairing_data_t[1];
typedef struct f_pairing_data_s *f_pairing_data_ptr;

void f_param_init(f_param_t fp)
{
    mpz_init(fp->q);
    mpz_init(fp->r);
    mpz_init(fp->b);
    mpz_init(fp->beta);
    mpz_init(fp->alpha0);
    mpz_init(fp->alpha1);
}

void f_param_clear(f_param_t fp)
{
    mpz_clear(fp->q);
    mpz_clear(fp->r);
    mpz_clear(fp->b);
    mpz_clear(fp->beta);
    mpz_clear(fp->alpha0);
    mpz_clear(fp->alpha1);
}

void f_param_out_str(FILE *stream, f_param_ptr p)
{
    param_out_type(stream, "f");
    param_out_mpz(stream, "q", p->q);
    param_out_mpz(stream, "r", p->r);
    param_out_mpz(stream, "b", p->b);
    param_out_mpz(stream, "beta", p->beta);
    param_out_mpz(stream, "alpha0", p->alpha0);
    param_out_mpz(stream, "alpha1", p->alpha1);
}

void f_param_inp_generic (f_param_ptr p, fetch_ops_t fops, void *ctx)
{
    symtab_t tab;

    assert (fops);
    assert (ctx);

    symtab_init(tab);
    param_read_generic (tab, fops, ctx);

    lookup_mpz(p->q, tab, "q");
    lookup_mpz(p->r, tab, "r");
    lookup_mpz(p->b, tab, "b");
    lookup_mpz(p->beta, tab, "beta");
    lookup_mpz(p->alpha0, tab, "alpha0");
    lookup_mpz(p->alpha1, tab, "alpha1");

    param_clear_tab(tab);
    symtab_clear(tab);
}

static void tryminusx(mpz_ptr q, mpz_ptr x)
{
    //36x4 - 36x3 + 24x2 - 6x + 1
    //= ((36(x - 1)x + 24)x - 6)x + 1
    mpz_sub_ui(q, x, 1);
    mpz_mul(q, q, x);
    mpz_mul_ui(q, q, 36);
    mpz_add_ui(q, q, 24);
    mpz_mul(q, q, x);
    mpz_sub_ui(q, q, 6);
    mpz_mul(q, q, x);
    mpz_add_ui(q, q, 1);
}

static void tryplusx(mpz_ptr q, mpz_ptr x)
{
    //36x4 + 36x3 + 24x2 + 6x + 1
    //= ((36(x + 1)x + 24)x + 6)x + 1
    mpz_add_ui(q, x, 1);
    mpz_mul(q, q, x);
    mpz_mul_ui(q, q, 36);
    mpz_add_ui(q, q, 24);
    mpz_mul(q, q, x);
    mpz_add_ui(q, q, 6);
    mpz_mul(q, q, x);
    mpz_add_ui(q, q, 1);
}

void f_param_gen(f_param_t fp, int bits)
{
    //36 is a 6-bit number
    int xbit = (bits - 6) / 4;
    //TODO: use binary search to find smallest appropriate x
    mpz_t x, t;
    mpz_ptr q = fp->q;
    mpz_ptr r = fp->r;
    mpz_ptr b = fp->b;
    field_t Fq, Fq2, Fq2x;
    element_t e1;
    element_t f;
    field_t c;
    element_t P;

    mpz_init(x);
    mpz_init(t);
    mpz_setbit(x, xbit);
    for (;;) {
	mpz_mul(t, x, x);
	mpz_mul_ui(t, t, 6);
	mpz_add_ui(t, t, 1);
	tryminusx(q, x);
	mpz_sub(r, q, t);
	mpz_add_ui(r, r, 1);
	if (mpz_probab_prime_p(q, 10) && mpz_probab_prime_p(r, 10)) break;

	tryplusx(q, x);
	mpz_sub(r, q, t);
	mpz_add_ui(r, r, 1);
	if (mpz_probab_prime_p(q, 10) && mpz_probab_prime_p(r, 10)) break;

	mpz_add_ui(x, x, 1);
    }

    field_init_fp(Fq, q);
    element_init(e1, Fq);

    for (;;) {
	element_random(e1);
	field_init_curve_b(c, e1, r, NULL);
	element_init(P, c);

	element_random(P);

	element_mul_mpz(P, P, r);
	if (element_is0(P)) break;
	element_clear(P);
	field_clear(c);
    }
    element_to_mpz(b, e1);

    element_clear(e1);

    field_init_quadratic(Fq2, Fq);

    element_to_mpz(fp->beta, field_get_nqr(Fq));

    field_init_poly(Fq2x, Fq2);

    element_init(f, Fq2x);

    //find irreducible polynomial f = x^6 + alpha
    poly_alloc(f, 7);
    element_set1(poly_coeff(f, 6));
    for (;;) {
	element_random(poly_coeff(f, 0));
	if (poly_is_irred(f)) break;
    }

    //extend F_q^2 using f = x^6 + alpha
    //see if sextic twist contains a subgroup of order r
    //if not, it's the wrong twist: replace alpha with alpha^5
    {
	field_t ctest;
	element_t Ptest;
	mpz_t z0, z1;
	mpz_init(z0);
	mpz_init(z1);
	element_init(e1, Fq2);
	element_set_mpz(e1, fp->b);
	element_mul(e1, e1, poly_coeff(f, 0));
	element_neg(e1, e1);

	field_init_curve_b(ctest, e1, r, NULL);
	element_init(Ptest, ctest);
	element_random(Ptest);

	//I'm not sure what the #E'(F_q^2) is, but
	//it definitely divides n_12 = #E(F_q^12). It contains a
	//subgroup of order r if and only if
	//(n_12 / r^2)P != O for some (in fact most) P in E'(F_q^6)
	mpz_pow_ui(z0, q, 12);
	mpz_add_ui(z0, z0, 1);
	compute_trace_n(z1, q, t, 12);
	mpz_sub(z1, z0, z1);
	mpz_mul(z0, r, r);
	mpz_divexact(z1, z1, z0);

	element_mul_mpz(Ptest, Ptest, z1);
	if (element_is0(Ptest)) {
	    mpz_set_ui(z0, 5);
	    element_pow_mpz(poly_coeff(f, 0), poly_coeff(f, 0), z0);
	}
	element_clear(e1);
	element_clear(Ptest);
	field_clear(ctest);
	mpz_clear(z0);
	mpz_clear(z1);
    }

    element_to_mpz(fp->alpha0, fi_re(poly_coeff(f, 0)));
    element_to_mpz(fp->alpha1, fi_im(poly_coeff(f, 0)));

    element_clear(f);

    field_clear(Fq2x);
    field_clear(Fq2);
    field_clear(Fq);

    mpz_clear(t);
    mpz_clear(x);
}

static void cc_miller_no_denom(element_t res, mpz_t q, element_t P,
	element_ptr Qx, element_ptr Qy, element_t negalpha)
{
    int m;
    element_t v;
    element_t Z;
    element_t a, b, c;
    element_t t0;
    element_t e0, e1;
    element_ptr Zx, Zy;
    const element_ptr Px = curve_x_coord(P);
    const element_ptr Py = curve_y_coord(P);

#ifdef INNER_FUNC
    void f_miller_evalfn(void)
    {
	// a, b, c lie in Fq
	// Qx, Qy lie in Fq^2
	// Qx is coefficient of x^4
	// Qy is coefficient of x^3
	//
	// computes v *= (a Qx x^4 + b Qy x^3 + c)
	//
	// recall x^6 = -alpha thus
	// x^4 (u0 + u1 x^1 + ... + u5 x^5) =
	// u0 x^4 + u1 x^5
	// - alpha u2 - alpha u3 x - alpha u4 x^2 - alpha u5 x^3
	// and
	// x^4 (u0 + u1 x^1 + ... + u5 x^5) =
	// u0 x^3 + u1 x^4 + u2 x^5
	// - alpha u3 - alpha u4 x - alpha u5 x^2
	element_ptr e2;
	inline void do_term(int i, int j, int k, int flag)
	{
	    e2 = polymod_coeff(e0, i);
	    element_mul(e1, polymod_coeff(v, j), Qx);
	    if (flag == 1) element_mul(e1, e1, negalpha);
	    element_mul(fi_re(e1), fi_re(e1), a);
	    element_mul(fi_im(e1), fi_im(e1), a);
	    element_mul(e2, polymod_coeff(v, k), Qy);
	    element_mul(fi_re(e2), fi_re(e2), b);
	    element_mul(fi_im(e2), fi_im(e2), b);
	    element_add(e2, e2, e1);
	    if (flag == 2) element_mul(e2, e2, negalpha);
	    element_mul(fi_re(e1), fi_re(polymod_coeff(v, i)), c);
	    element_mul(fi_im(e1), fi_im(polymod_coeff(v, i)), c);
	    element_add(e2, e2, e1);
	}
	do_term(0, 2, 3, 2);
	do_term(1, 3, 4, 2);
	do_term(2, 4, 5, 2);
	do_term(3, 5, 0, 1);
	do_term(4, 0, 1, 0);
	do_term(5, 1, 2, 0);

	element_set(v, e0);

	/*
	element_ptr e1;

	e1 = polymod_coeff(e0, 4);

	element_mul(fi_re(e1), fi_re(Qx), a);
	element_mul(fi_im(e1), fi_im(Qx), a);

	e1 = polymod_coeff(e0, 3);

	element_mul(fi_re(e1), fi_re(Qy), b);
	element_mul(fi_im(e1), fi_im(Qy), b);

	element_set(fi_re(polymod_coeff(e0, 0)), c);

	element_mul(v, v, e0);
	*/
    }

    void do_tangent(void)
    {
	//a = -3 Zx^2 since cc->a is 0 for D = 3
	//b = 2 * Zy
	//c = -(2 Zy^2 + a Zx);
	element_square(a, Zx);
	element_mul_si(a, a, 3);
	element_neg(a, a);

	element_add(b, Zy, Zy);

	element_mul(t0, b, Zy);
	element_mul(c, a, Zx);
	element_add(c, c, t0);
	element_neg(c, c);

	f_miller_evalfn();
    }

    void do_line(void)
    {
	//a = -(B.y - A.y) / (B.x - A.x);
	//b = 1;
	//c = -(A.y + a * A.x);
	//but we'll multiply by B.x - A.x to avoid division

	element_sub(b, Px, Zx);
	element_sub(a, Zy, Py);
	element_mul(t0, b, Zy);
	element_mul(c, a, Zx);
	element_add(c, c, t0);
	element_neg(c, c);

	f_miller_evalfn();
    }

#else
#define do_term( i, j,  k, flag)\
	{\
	    e2 = polymod_coeff(e0, i);\
	    element_mul(e1, polymod_coeff(v, j), Qx);\
	    if (flag == 1) element_mul(e1, e1, negalpha);\
	    element_mul(fi_re(e1), fi_re(e1), a);\
	    element_mul(fi_im(e1), fi_im(e1), a);\
	    element_mul(e2, polymod_coeff(v, k), Qy);\
	    element_mul(fi_re(e2), fi_re(e2), b);\
	    element_mul(fi_im(e2), fi_im(e2), b);\
	    element_add(e2, e2, e1);\
	    if (flag == 2) element_mul(e2, e2, negalpha);\
	    element_mul(fi_re(e1), fi_re(polymod_coeff(v, i)), c);\
	    element_mul(fi_im(e1), fi_im(polymod_coeff(v, i)), c);\
	    element_add(e2, e2, e1);\
	}

#define f_miller_evalfn()\
    {\
	element_ptr e2;\
	do_term(0, 2, 3, 2);\
	do_term(1, 3, 4, 2);\
	do_term(2, 4, 5, 2);\
	do_term(3, 5, 0, 1);\
	do_term(4, 0, 1, 0);\
	do_term(5, 1, 2, 0);\
	element_set(v, e0);\
    }

#define do_tangent()\
    {\
	element_square(a, Zx);\
	element_mul_si(a, a, 3);\
	element_neg(a, a);\
	element_add(b, Zy, Zy);\
	element_mul(t0, b, Zy);\
	element_mul(c, a, Zx);\
	element_add(c, c, t0);\
	element_neg(c, c);\
	f_miller_evalfn();\
    }

#define do_line()\
    {\
	element_sub(b, Px, Zx);\
	element_sub(a, Zy, Py);\
	element_mul(t0, b, Zy);\
	element_mul(c, a, Zx);\
	element_add(c, c, t0);\
	element_neg(c, c);\
	f_miller_evalfn();\
    }
#endif

    element_init(a, Px->field);
    element_init(b, a->field);
    element_init(c, a->field);
    element_init(t0, a->field);
    element_init(e0, res->field);
    element_init(e1, Qx->field);

    element_init(v, res->field);
    element_init(Z, P->field);

    element_set(Z, P);
    Zx = curve_x_coord(Z);
    Zy = curve_y_coord(Z);

    element_set1(v);
    m = mpz_sizeinbase(q, 2) - 2;

    //TODO: sliding NAF
    for(;;) {
	do_tangent();

	if (!m) break;

	element_double(Z, Z);
	if (mpz_tstbit(q, m)) {
	    do_line();
	    element_add(Z, Z, P);
	}
	m--;
	element_square(v, v);
    }

    element_set(res, v);

    element_clear(v);
    element_clear(Z);
    element_clear(a);
    element_clear(b);
    element_clear(c);
    element_clear(t0);
    element_clear(e0);
    element_clear(e1);
#ifndef INNER_FUNC
#undef do_term
#undef f_miller_evalfn
#undef do_tangent
#undef do_line
#endif
}

static void f_pairing(element_ptr out, element_ptr in1, element_ptr in2,
	pairing_t pairing)
{
    element_ptr Qbase = in2;
    element_t x, y;
    f_pairing_data_ptr p = pairing->data;

    element_init(x, p->Fq2);
    element_init(y, p->Fq2);
    //map from twist: (x, y) --> (v^-2 x, v^-3 y)
    //where v is the sixth root used to construct the twist
    //i.e. v^6 = -alpha
    //thus v^-2 = -alpha^-1 v^4
    //and  v^-3 = -alpha^-1 v^3
    element_mul(x, curve_x_coord(Qbase), p->negalphainv);
    element_mul(y, curve_y_coord(Qbase), p->negalphainv);

    cc_miller_no_denom(out, pairing->r, in1, x, y, p->negalpha);

    element_clear(x);
    element_clear(y);
{
    element_t epow;

    //x, y now Fq12 temp variables
    element_init(x, p->Fq12);
    element_init(y, p->Fq12);

#ifdef INNER_FUNC
    void qpower(element_ptr e1, element_ptr e) {
	element_set(polymod_coeff(e1, 0), polymod_coeff(out, 0));
	element_mul(polymod_coeff(e1, 1), polymod_coeff(out, 1), e);
	element_square(epow, e);
	element_mul(polymod_coeff(e1, 2), polymod_coeff(out, 2), epow);
	element_mul(epow, epow, e);
	element_mul(polymod_coeff(e1, 3), polymod_coeff(out, 3), epow);
	element_mul(epow, epow, e);
	element_mul(polymod_coeff(e1, 4), polymod_coeff(out, 4), epow);
	element_mul(epow, epow, e);
	element_mul(polymod_coeff(e1, 5), polymod_coeff(out, 5), epow);
    }
#else
#define qpower(e1, e) {\
	element_set(polymod_coeff(e1, 0), polymod_coeff(out, 0));\
	element_mul(polymod_coeff(e1, 1), polymod_coeff(out, 1), e);\
	element_square(epow, e);\
	element_mul(polymod_coeff(e1, 2), polymod_coeff(out, 2), epow);\
	element_mul(epow, epow, e);\
	element_mul(polymod_coeff(e1, 3), polymod_coeff(out, 3), epow);\
	element_mul(epow, epow, e);\
	element_mul(polymod_coeff(e1, 4), polymod_coeff(out, 4), epow);\
	element_mul(epow, epow, e);\
	element_mul(polymod_coeff(e1, 5), polymod_coeff(out, 5), epow);\
    }
#endif

    element_init(epow, p->Fq2);

    qpower(y, p->xpowq8);
    qpower(x, p->xpowq6);
    element_mul(y, y, x);
    qpower(x, p->xpowq2);
    element_mul(x, x, out);
    element_invert(x, x);
    element_mul(out, y, x);

    element_clear(epow);
    element_clear(x);
    element_clear(y);
#ifndef INNER_FUNC
#undef qpower
#endif
}
    element_pow_mpz(out, out, p->tateexp);
}

void f_pairing_clear(pairing_t pairing)
{
    f_pairing_data_ptr p = pairing->data;
    element_clear(p->negalpha);
    element_clear(p->negalphainv);
    mpz_clear(p->tateexp);
    element_clear(p->xpowq2);
    element_clear(p->xpowq6);
    element_clear(p->xpowq8);
    field_clear(p->Etwist);
    field_clear(p->Eq);

    field_clear(p->Fq12);
    field_clear(p->Fq2x);
    field_clear(p->Fq2);
    field_clear(p->Fq);
    pbc_free(p);

    mpz_clear(pairing->r);
    field_clear(pairing->Zr);
}

void pairing_init_f_param(pairing_t pairing, f_param_t param)
{
    f_pairing_data_ptr p;
    element_t irred;
    element_t e0, e1, e2;
	mpz_ptr z;
    element_t xpowq;

    p = pairing->data = pbc_malloc(sizeof(f_pairing_data_t));
    mpz_init(pairing->r);
    mpz_set(pairing->r, param->r);
    field_init_fp(pairing->Zr, pairing->r);
    field_init_fp(p->Fq, param->q);
    p->Fq->nqr = pbc_malloc(sizeof(element_t));
    element_init(p->Fq->nqr, p->Fq);
    element_set_mpz(p->Fq->nqr, param->beta);
    field_init_quadratic(p->Fq2, p->Fq);
    field_clear(p->Fq2);
    field_init_quadratic(p->Fq2, p->Fq);
    field_init_poly(p->Fq2x, p->Fq2);
    element_init(irred, p->Fq2x);
    poly_alloc(irred, 7);
    element_set1(poly_coeff(irred, 6));

    element_init(p->negalpha, p->Fq2);
    element_init(p->negalphainv, p->Fq2);
    element_set_mpz(fi_re(p->negalpha), param->alpha0);
    element_set_mpz(fi_im(p->negalpha), param->alpha1);

    element_set(poly_coeff(irred, 0), p->negalpha);
    field_init_polymod(p->Fq12, irred);
    element_neg(p->negalpha, p->negalpha);
    element_invert(p->negalphainv, p->negalpha);
    element_clear(irred);

    element_init(e0, p->Fq);
    element_init(e1, p->Fq);
    element_init(e2, p->Fq2);

    element_set_mpz(e1, param->b);
    field_init_curve_ab(p->Eq, e0, e1, pairing->r, NULL);
    element_set_mpz(e0, param->alpha0);
    element_neg(e0, e0);
    element_mul(fi_re(e2), e0, e1);
    element_set_mpz(e0, param->alpha1);
    element_neg(e0, e0);
    element_mul(fi_im(e2), e0, e1);
    element_clear(e0);
    element_init(e0, p->Fq2);
    field_init_curve_ab(p->Etwist, e0, e2, pairing->r, NULL);
    element_clear(e0);
    element_clear(e1);
    element_clear(e2);

    pairing->G1 = p->Eq;
    pairing->G2 = p->Etwist;
    pairing->GT = p->Fq12;
    pairing->map = f_pairing;
    pairing->clear_func = f_pairing_clear;
    
    mpz_init(p->tateexp);
    /* unoptimized tate exponent
    mpz_pow_ui(p->tateexp, param->q, 12);
    mpz_sub_ui(p->tateexp, p->tateexp, 1);
    mpz_divexact(p->tateexp, p->tateexp, param->r);
    */
    z = p->tateexp;
    mpz_mul(z, param->q, param->q);
    mpz_sub_ui(z, z, 1);
    mpz_mul(z, z, param->q);
    mpz_mul(z, z, param->q);
    mpz_add_ui(z, z, 1);
    mpz_divexact(z, z, param->r);

    element_init(p->xpowq2, p->Fq2);
    element_init(p->xpowq6, p->Fq2);
    element_init(p->xpowq8, p->Fq2);
    element_init(xpowq, p->Fq12);

    //there are smarter ways since we know q = 1 mod 6
    //and that x^6 = -alpha
    //but this is fast enough
    element_set1(polymod_coeff(xpowq, 1));
    element_pow_mpz(xpowq, xpowq, param->q);
    element_pow_mpz(xpowq, xpowq, param->q);
    element_set(p->xpowq2, polymod_coeff(xpowq, 1));

    element_pow_mpz(xpowq, xpowq, param->q);
    element_pow_mpz(xpowq, xpowq, param->q);
    element_pow_mpz(xpowq, xpowq, param->q);
    element_pow_mpz(xpowq, xpowq, param->q);
    element_set(p->xpowq6, polymod_coeff(xpowq, 1));

    element_pow_mpz(xpowq, xpowq, param->q);
    element_pow_mpz(xpowq, xpowq, param->q);
    element_set(p->xpowq8, polymod_coeff(xpowq, 1));

    element_clear(xpowq);
}


void f_param_input(pairing_t pairing)
{
	f_param_t fp;

#if 1	
    pairing->option_set = default_option_set;
    pairing->pp_init = default_pp_init;
    pairing->pp_clear = default_pp_clear;
    pairing->pp_apply = default_pp_apply;
    pairing->is_almost_coddh = generic_is_almost_coddh;
    pairing->phi = phi_warning;
#endif

	f_param_init(fp);
#if 1	//160
	mpz_set_str(fp->q, "205523667896953300194896352429254920972540065223", 0);
	mpz_set_str(fp->r, "205523667896953300194895899082072403858390252929", 0);
	mpz_set_str(fp->b, "40218105156867728698573668525883168222119515413", 0);
	mpz_set_str(fp->beta, "115334401956802802075595682801335644058796914268", 0);
	mpz_set_str(fp->alpha0, "191079354656274778837764015557338301375963168470", 0);
	mpz_set_str(fp->alpha1, "71445317903696340296199556072836940741717506375", 0);
#else
	mpz_set_str(fp->q, "16283262548997601220198008118239886027035269286659395419233331082106632227801", 0);
	mpz_set_str(fp->r, "16283262548997601220198008118239886026907663399064043451383740756301306087801", 0);
	mpz_set_str(fp->b, "2464950960736879059729784382069129042500572946234750106015705139417922297079", 0);
	mpz_set_str(fp->beta, "8082179206684214397418611687602366393455177045360569761915651429984401803144", 0);
	mpz_set_str(fp->alpha0, "5196431423910516116672057682974097184242799445180895555637767613962052744304", 0);
	mpz_set_str(fp->alpha1, "12779458336953126276306937760774138062858098783196148793074350437417342122612", 0);
#endif
	pairing_init_f_param(pairing, fp);
	f_param_clear(fp);

	return;
}

