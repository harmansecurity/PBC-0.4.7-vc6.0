#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gmp.h>
#include "pbc_field.h"
#include "pbc_darray.h"
#include "pbc_poly.h"
#include "pbc_curve.h"
#include "pbc_memory.h"
#include "pbc_utils.h"
#include "pbc_random.h"


struct point_s {
    int inf_flag;
    element_t x;
    element_t y;
};
typedef struct point_s *point_ptr;
typedef struct point_s point_t[1];

struct curve_data_s {
    field_ptr field;
    element_t a, b;
    element_t gen;
    element_t gen_no_cofac;
    mpz_ptr cofac;
};
typedef struct curve_data_s curve_data_t[1];
typedef struct curve_data_s *curve_data_ptr;

static void curve_init(element_ptr e)
{
	point_ptr p;

    curve_data_ptr cdp = e->field->data;
    e->data = pbc_malloc(sizeof(point_t));
    p = e->data;
    element_init(p->x, cdp->field);
    element_init(p->y, cdp->field);
    p->inf_flag = 1;
}

static void curve_clear(element_ptr e)
{
    point_ptr p = e->data;
    element_clear(p->x);
    element_clear(p->y);
    pbc_free(e->data);
}

static int curve_is_valid_point(element_ptr e)
{
    element_t t0, t1;
    int result;
    curve_data_ptr cdp = e->field->data;
    point_ptr p = e->data;

    if (p->inf_flag) return 1;

    element_init(t0, cdp->field);
    element_init(t1, cdp->field);
    element_square(t0, p->x);
    element_add(t0, t0, cdp->a);
    element_mul(t0, t0, p->x);
    element_add(t0, t0, cdp->b);
    element_square(t1, p->y);
    result = !element_cmp(t0, t1);

    element_clear(t0);
    element_clear(t1);
    return result;
}

static void curve_invert(element_ptr c, element_ptr a)
{
    point_ptr r = c->data, p = a->data;

    if (p->inf_flag) {
	r->inf_flag = 1;
	return;
    }
    r->inf_flag = 0;
    element_set(r->x, p->x);
    element_neg(r->y, p->y);
}

static void curve_set(element_ptr c, element_ptr a)
{
    point_ptr r = c->data, p = a->data;
    if (p->inf_flag) {
	r->inf_flag = 1;
	return;
    }
    r->inf_flag = 0;
    element_set(r->x, p->x);
    element_set(r->y, p->y);
}

static inline void double_no_check(point_ptr r, point_ptr p, element_ptr a)
{
    element_t lambda, e0, e1;
    field_ptr f = r->x->field;

    element_init(lambda, f);
    element_init(e0, f);
    element_init(e1, f);

    //lambda = (3x^2 + a) / 2y
    element_square(lambda, p->x);
    element_mul_si(lambda, lambda, 3);
    element_add(lambda, lambda, a);

    element_double(e0, p->y);

    element_invert(e0, e0);
    element_mul(lambda, lambda, e0);
    //x1 = lambda^2 - 2x
    //element_add(e1, p->x, p->x);
    element_double(e1, p->x);
    element_square(e0, lambda);
    element_sub(e0, e0, e1);
    //y1 = (x - x1)lambda - y
    element_sub(e1, p->x, e0);
    element_mul(e1, e1, lambda);
    element_sub(e1, e1, p->y);

    element_set(r->x, e0);
    element_set(r->y, e1);
    r->inf_flag = 0;

    element_clear(lambda);
    element_clear(e0);
    element_clear(e1);
    return;
}

static void curve_double(element_ptr c, element_ptr a)
{
    curve_data_ptr cdp = a->field->data;
    point_ptr r = c->data, p = a->data;
    if (p->inf_flag) {
	r->inf_flag = 1;
	return;
    }
    if (element_is0(p->y)) {
	r->inf_flag = 1;
	return;
    }
    double_no_check(r, p, cdp->a);
}

static void curve_mul(element_ptr c, element_ptr a, element_ptr b)
{
    curve_data_ptr cdp = a->field->data;
    point_ptr r = c->data, p = a->data, q = b->data;

    if (p->inf_flag) {
	curve_set(c, b);
	return;
    }
    if (q->inf_flag) {
	curve_set(c, a);
	return;
    }
    if (!element_cmp(p->x, q->x)) {
	if (!element_cmp(p->y, q->y)) {
	    if (element_is0(p->y)) {
		r->inf_flag = 1;
		return;
	    } else {
		double_no_check(r, p, cdp->a);
		return;
	    }
	}
	//points are inverses of each other
	r->inf_flag = 1;
	return;
    } else {
	element_t lambda, e0, e1;

	element_init(lambda, cdp->field);
	element_init(e0, cdp->field);
	element_init(e1, cdp->field);

	//lambda = (y2-y1)/(x2-x1)
	element_sub(e0, q->x, p->x);
	element_invert(e0, e0);
	element_sub(lambda, q->y, p->y);
	element_mul(lambda, lambda, e0);
	//x3 = lambda^2 - x1 - x2
	element_square(e0, lambda);
	element_sub(e0, e0, p->x);
	element_sub(e0, e0, q->x);
	//y3 = (x1-x3)lambda - y1
	element_sub(e1, p->x, e0);
	element_mul(e1, e1, lambda);
	element_sub(e1, e1, p->y);

	element_set(r->x, e0);
	element_set(r->y, e1);
        r->inf_flag = 0;

	element_clear(lambda);
	element_clear(e0);
	element_clear(e1);
    }
}

static int curve_cmp(element_ptr a, element_ptr b)
{
    if (a == b) {
	return 0;
    } else {
	point_ptr p = a->data;
	point_ptr q = b->data;
	if (p->inf_flag) {
	    return q->inf_flag;
	}
	return element_cmp(p->x, q->x) || element_cmp(p->y, q->y);
    }
}

static void curve_set1(element_ptr x)
{
    point_ptr p = x->data;
    p->inf_flag = 1;
}

static int curve_is1(element_ptr x)
{
    point_ptr p = x->data;
    return p->inf_flag;
}

static void curve_random_no_cofac_solvefory(element_ptr a)
{
    //TODO: with 0.5 probability negate y-coord
    curve_data_ptr cdp = a->field->data;
    point_ptr p = a->data;
    element_t t;

    element_init(t, cdp->field);
    p->inf_flag = 0;
    do {
	element_random(p->x);
	element_square(t, p->x);		//t=x^2
	element_add(t, t, cdp->a);		//t=x^2+a
	element_mul(t, t, p->x);		//t=x^3+ax
	element_add(t, t, cdp->b);		//t=x^3+ax+b
    } while (!element_is_sqr(t));	//is t a square 
    element_sqrt(p->y, t);			//y=sqrt(t)
    element_clear(t);
}

static void curve_random_solvefory(element_ptr a)
{
    curve_data_ptr cdp = a->field->data;
    curve_random_no_cofac_solvefory(a);
    if (cdp->cofac) element_mul_mpz(a, a, cdp->cofac);
}

static void curve_random_pointmul(element_ptr a)
{
    curve_data_ptr cdp = a->field->data;
    mpz_t x;
    mpz_init(x);

    pbc_mpz_random(x, a->field->order);
    element_mul_mpz(a, cdp->gen, x);
    mpz_clear(x);
}

void field_curve_use_random_solvefory(field_ptr f)
{
    f->random = curve_random_solvefory;
}

void curve_set_gen_no_cofac(element_ptr a)
{
    curve_data_ptr cdp = a->field->data;
    element_set(a, cdp->gen_no_cofac);
}

static int curve_sign(element_ptr e)
{
    point_ptr p = e->data;
    if (p->inf_flag) return 0;
    return element_sign(p->y);
}

static void curve_from_hash(element_t a, void *data, int len)
{
    //TODO: don't find a hash by the 255th try = freeze!
    void *datacopy;
    element_t t, t1;
    point_ptr p = a->data;
    curve_data_ptr cdp = a->field->data;

    datacopy = pbc_malloc(len);
    memcpy(datacopy, data, len);

    element_init(t, cdp->field);
    element_init(t1, cdp->field);
    p->inf_flag = 0;
    for(;;) {
	element_from_hash(p->x, datacopy, len);
	element_square(t, p->x);
	element_add(t, t, cdp->a);
	element_mul(t, t, p->x);
	element_add(t, t, cdp->b);
	if (element_is_sqr(t)) break;
	((char *) datacopy)[0]++;
    }
    element_sqrt(p->y, t);
    if (element_sgn(p->y) < 0) element_neg(p->y, p->y);

    if (cdp->cofac) element_mul_mpz(a, a, cdp->cofac);

    element_clear(t);
    element_clear(t1);
    pbc_free(datacopy);
}

static size_t curve_out_str(FILE *stream, int base, element_ptr a)
{
    point_ptr p = a->data;
    size_t result, status;
    if (p->inf_flag) {
	if (EOF == fputc('O', stream)) return 0;
	return 1;
    }
    if (EOF == fputc('[', stream)) return 0;
    result = element_out_str(stream, base, p->x);
    if (!result) return 0;
    if (EOF == fputs(", ", stream)) return 0;
    status = element_out_str(stream, base, p->y);
    if (!status) return 0;
    if (EOF == fputc(']', stream)) return 0;
    return result + status + 4;
}

static int curve_snprint(char *s, size_t n, element_ptr a)
{
    point_ptr p = a->data;
    size_t result = 0, left;
    int status;

#ifdef INNER_FUNC
    void clip_sub(void)
    {
		result += status;
		left = result >= n ? 0 : n - result;
    }
#else
#define clip_sub()	do {\
	result += status;\
	left = result >= n ? 0 : n - result;\
	}while(0)
#endif

    if (p->inf_flag) {
	status = snprintf(s, n, "O");
	if (status < 0) return status;
	return 1;
    }

    status = snprintf(s, n, "[");
    if (status < 0) return status;
    clip_sub();
    status = element_snprint(s + result, left, p->x);
    if (status < 0) return status;
    clip_sub();
    status = snprintf(s + result, left, ", ");
    if (status < 0) return status;
    clip_sub();
    status = element_snprint(s + result, left, p->y);
    if (status < 0) return status;
    clip_sub();
    status = snprintf(s + result, left, "]");
    if (status < 0) return status;
    return result + status;
#ifndef INNER_FUNC
#undef clip_sub
#endif
}

static int curve_set_str(element_ptr e, char *s, int base)
{
    point_ptr p = e->data;
    char *cp = s;
    element_set0(e);
    while (*cp && isspace(*cp)) cp++;
    if (*cp == 'O') {
	return cp - s + 1;
    }
    p->inf_flag = 0;
    if (*cp != '[') return 0;
    cp++;
    cp += element_set_str(p->x, cp, base);
    while (*cp && isspace(*cp)) cp++;
    if (*cp != ',') return 0;
    cp++;
    cp += element_set_str(p->y, cp, base);
    if (*cp != ']') return 0;

    if (!curve_is_valid_point(e)) {
	element_set0(e);
	return 0;
    }
    return cp - s + 1;
}

static void field_clear_curve(field_t f)
{
    curve_data_ptr cdp;
    cdp = f->data;
    element_clear(cdp->gen);
    element_clear(cdp->gen_no_cofac);
    if (cdp->cofac) {
	mpz_clear(cdp->cofac);
	pbc_free(cdp->cofac);
    }
    element_clear(cdp->a);
    element_clear(cdp->b);
    pbc_free(cdp);
}

static int curve_length_in_bytes(element_ptr x)
{
    point_ptr p = x->data;
    return element_length_in_bytes(p->x) + element_length_in_bytes(p->y);
}

static int curve_to_bytes(unsigned char *data, element_t e)
{
    point_ptr P = e->data;
    int len;
    len = element_to_bytes(data, P->x);
    len += element_to_bytes(data + len, P->y);
    return len;
}

static int curve_from_bytes(element_t e, unsigned char *data)
{
    point_ptr P = e->data;
    int len;

    P->inf_flag = 0;
    len = element_from_bytes(P->x, data);
    len += element_from_bytes(P->y, data + len);
    //if point does not lie on curve, set it to O
    if (!curve_is_valid_point(e)) {
	element_set0(e);
    }
    return len;
}

static void curve_out_info(FILE *out, field_t f)
{
    int len;
    fprintf(out, "Group of points on elliptic curve");
    if ((len = f->fixed_length_in_bytes)) {
	fprintf(out, ", bits per coord = %d\n", len * 8 / 2);
    } else {
	fprintf(out, "variable-length representation\n");
    }
}

int odd_curve_is_sqr(element_ptr e)
{
    UNUSED_VAR(e);
    return 1;
}

//TODO: untested
int even_curve_is_sqr(element_ptr e)
{
    mpz_t z;
    element_t e1;
    int result;

    mpz_init(z);
    element_init(e1, e->field);
    mpz_sub_ui(z, e->field->order, 1);
    mpz_fdiv_q_2exp(z, z, 1);
    element_pow_mpz(e1, e, z);
    result = element_is1(e1);

    mpz_clear(z);
    element_clear(e1);
    return result;
}

void field_init_curve_ab(field_ptr f, element_ptr a, element_ptr b, mpz_t order, mpz_t cofac)
    /*
    if (element_is0(a)) {
	c->double_nocheck = cc_double_no_check_ais0;
    } else {
	c->double_nocheck = cc_double_no_check;
    }
    */
{
    curve_data_ptr cdp;
    field_init(f);
    mpz_set(f->order, order);
    cdp = f->data = pbc_malloc(sizeof(curve_data_t));
    cdp->field = a->field;
    element_init(cdp->a, cdp->field);
    element_init(cdp->b, cdp->field);
    element_set(cdp->a, a);
    element_set(cdp->b, b);

    f->init = curve_init;
    f->clear = curve_clear;
    f->neg = f->invert = curve_invert;
    f->square = f->doub = curve_double;
    f->add = f->mul = curve_mul;
    f->mul_mpz = element_pow_mpz;
    f->cmp = curve_cmp;
    f->set0 = f->set1 = curve_set1;
    f->is0 = f->is1 = curve_is1;
    f->sign = curve_sign;
    f->set = curve_set;
    f->random = curve_random_pointmul;
    //f->random = curve_random_solvefory;
    f->from_hash = curve_from_hash;
    f->out_str = curve_out_str;
    f->snprint = curve_snprint;
    f->set_str = curve_set_str;
    f->field_clear = field_clear_curve;
    if (cdp->field->fixed_length_in_bytes < 0) {
	f->length_in_bytes = curve_length_in_bytes;
    } else {
	f->fixed_length_in_bytes = 2 * cdp->field->fixed_length_in_bytes;
    }
    f->to_bytes = curve_to_bytes;
    f->from_bytes = curve_from_bytes;
    f->out_info = curve_out_info;

    if (mpz_odd_p(order)) {
	f->is_sqr = odd_curve_is_sqr;
    } else {
	f->is_sqr = even_curve_is_sqr;
    }

    element_init(cdp->gen_no_cofac, f);
    element_init(cdp->gen, f);
    curve_random_no_cofac_solvefory(cdp->gen_no_cofac);
    if (cofac) {
	cdp->cofac = pbc_malloc(sizeof(mpz_t));
	mpz_init(cdp->cofac);
	mpz_set(cdp->cofac, cofac);
	element_mul_mpz(cdp->gen, cdp->gen_no_cofac, cofac);
    } else{
	cdp->cofac = NULL;
	element_set(cdp->gen, cdp->gen_no_cofac);
    }
}

int element_to_bytes_compressed(unsigned char *data, element_ptr e)
    //e must be a point on an elliptic curve
{
    point_ptr P = e->data;
    int len;
    len = element_to_bytes(data, P->x);
    if (element_sign(P->y) > 0) {
	data[len] = 1;
    } else {
	data[len] = 0;
    }
    len++;
    return len;
}

static void point_from_x(point_ptr p, element_t x, element_t a, element_t b)
    //assumes there exists a point with given x coordinate
{
    element_t t;

    element_init(t, x->field);
    p->inf_flag = 0;
    element_square(t, x);
    element_add(t, t, a);
    element_mul(t, t, x);
    element_add(t, t, b);
    element_sqrt(p->y, t);
    element_set(p->x, x);

    element_clear(t);
}

void curve_from_x(element_ptr e, element_t x)
{
    curve_data_ptr cdp = e->field->data;
    point_from_x(e->data, x, cdp->a, cdp->b);
}

int element_from_bytes_compressed(element_ptr e, unsigned char *data)
    //e must be a point on an elliptic curve
{
    curve_data_ptr cdp = e->field->data;
    point_ptr P = e->data;
    int len;
    len = element_from_bytes(P->x, data);
    point_from_x(P, P->x, cdp->a, cdp->b);

    if (data[len]) {
	if (element_sign(P->y) < 0) element_neg(P->y, P->y);
    } else if (element_sign(P->y) > 0) {
	element_neg(P->y, P->y);
    }
    len++;
    return len;
}

int element_length_in_bytes_compressed(element_ptr e)
{
    point_ptr P = e->data;
    return element_length_in_bytes(P->x) + 1;
}

int element_to_bytes_x_only(unsigned char *data, element_ptr e)
    //e must be a point on an elliptic curve
{
    point_ptr P = e->data;
    int len;
    len = element_to_bytes(data, P->x);
    return len;
}

int element_from_bytes_x_only(element_ptr e, unsigned char *data)
    //e must be a point on an elliptic curve
{
    curve_data_ptr cdp = e->field->data;
    point_ptr P = e->data;
    int len;
    len = element_from_bytes(P->x, data);
    point_from_x(P, P->x, cdp->a, cdp->b);
    return len;
}

int element_length_in_bytes_x_only(element_ptr e)
{
    point_ptr P = e->data;
    return element_length_in_bytes(P->x);
}

inline element_ptr curve_x_coord(element_t e)
{
    return ((point_ptr) e->data)->x;
}

inline element_ptr curve_y_coord(element_t e)
{
    return ((point_ptr) e->data)->y;
}

inline element_ptr curve_a_coeff(element_t e)
{
    return ((curve_data_ptr) e->field->data)->a;
}

inline element_ptr curve_b_coeff(element_t e)
{
    return ((curve_data_ptr) e->field->data)->b;
}

inline element_ptr curve_field_a_coeff(field_t f)
{
    return ((curve_data_ptr) f->data)->a;
}

inline element_ptr curve_field_b_coeff(field_t f)
{
    return ((curve_data_ptr) f->data)->b;
}

void field_init_curve_ab_map(field_t cnew, field_t c,
	fieldmap map, field_ptr mapdest,
	mpz_t ordernew, mpz_t cofacnew)
{
    element_t a, b;
    curve_data_ptr cdp = c->data;

    element_init(a, mapdest);
    element_init(b, mapdest);

    map(a, cdp->a);
    map(b, cdp->b);

    field_init_curve_ab(cnew, a, b, ordernew, cofacnew);
    element_clear(a);
    element_clear(b);
}

void twist_curve(field_ptr c)
    //WARNING: existing points will no longer lie on c
    //as this modifies c in place
{
    curve_data_ptr cdp = c->data;
    element_ptr nqr = field_get_nqr(cdp->field);
    element_mul(cdp->a, cdp->a, nqr);
    element_mul(cdp->a, cdp->a, nqr);
    element_mul(cdp->b, cdp->b, nqr);
    element_mul(cdp->b, cdp->b, nqr);
    element_mul(cdp->b, cdp->b, nqr);

    //TODO: yuck!
    curve_random_no_cofac_solvefory(cdp->gen_no_cofac);
    if (cdp->cofac) {
	element_mul_mpz(cdp->gen, cdp->gen_no_cofac, cdp->cofac);
    } else{
	element_set(cdp->gen, cdp->gen_no_cofac);
    }
}

void field_init_curve_j(field_ptr f, element_ptr j, mpz_t order, mpz_t cofac)
//assumes j != 0, 1728
{
    element_t a, b;
    element_init(a, j->field);
    element_init(b, j->field);

    element_set_si(a, 1728);
    element_sub(a, a, j);
    element_invert(a, a);
    element_mul(a, a, j);

    //b = 2 j / (1728 - j)
    element_add(b, a, a);
    //a = 3 j / (1728 - j)
    element_add(a, a, b);
    field_init_curve_ab(f, a, b, order, cofac);

    element_clear(a);
    element_clear(b);
}

void field_init_curve_b(field_ptr f, element_ptr b, mpz_t order, mpz_t cofac)
{
    element_t a;
    element_init(a, b->field);
    field_init_curve_ab(f, a, b, order, cofac);

    element_clear(a);
}

void compute_trace_n(mpz_t res, mpz_t q, mpz_t trace, int n)
    //compute trace of Frobenius at q^n given trace at q
    //see p.105 of Blake, Seroussi and Smart
{
    int i;
    mpz_t c0, c1, c2;
    mpz_t t0;

    mpz_init(c0);
    mpz_init(c1);
    mpz_init(c2);
    mpz_init(t0);
    mpz_set_ui(c2, 2);
    mpz_set(c1, trace);
    for (i=2; i<=n; i++) {
	mpz_mul(c0, trace, c1);
	mpz_mul(t0, q, c2);
	mpz_sub(c0, c0, t0);
	mpz_set(c2, c1);
	mpz_set(c1, c0);
    }
    mpz_set(res, c1);
    mpz_clear(t0);
    mpz_clear(c2);
    mpz_clear(c1);
    mpz_clear(c0);
}

void curve_set_si(element_t R, long int x, long int y)
{
    point_ptr p = R->data;
    element_set_si(p->x, x);
    element_set_si(p->y, y);
    p->inf_flag = 0;
}
