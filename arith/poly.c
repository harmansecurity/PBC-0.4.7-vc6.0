#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <gmp.h>
#include "pbc_field.h"
#include "pbc_darray.h"
#include "pbc_poly.h"
#include "pbc_utils.h"
#include "pbc_memory.h"

//implements R[x] for a given ring R
//also R[x]_{f(x)}

void poly_alloc(element_ptr e, int n)
{
    poly_field_data_ptr pdp = e->field->data;
    poly_element_ptr p = e->data;
    element_ptr e0;
    int k;
    k = p->coeff->count;
    while (k < n) {
	e0 = pbc_malloc(sizeof(element_t));
	element_init(e0, pdp->field);
	darray_append(p->coeff, e0);
	k++;
    }
    while (k > n) {
	k--;
	e0 = darray_at(p->coeff, k);
	element_clear(e0);
	pbc_free(e0);
	darray_remove_last(p->coeff);
    }
}

static void poly_init(element_ptr e)
{
    poly_element_ptr p;
    e->data = pbc_malloc(sizeof(poly_element_t));
    p = e->data;
    darray_init(p->coeff);
}

static void poly_clear(element_ptr e)
{
    poly_element_ptr p = e->data;

    poly_alloc(e, 0);
    darray_clear(p->coeff);
    pbc_free(e->data);
}

void poly_remove_leading_zeroes(element_ptr e)
{
    poly_element_ptr p = e->data;
    int n = p->coeff->count - 1;
    while (n >= 0) {
	element_ptr e0 = p->coeff->item[n];
	if (!element_is0(e0)) return;
	element_clear(e0);
	pbc_free(e0);
	darray_remove_last(p->coeff);
	n--;
    }
}

static void poly_set_si(element_ptr e, signed long int op)
{
    poly_element_ptr p = e->data;
    element_ptr e0;

    poly_alloc(e, 1);
    e0 = p->coeff->item[0];
    element_set_si(e0, op);
    poly_remove_leading_zeroes(e);
}

static void poly_set_mpz(element_ptr e, mpz_ptr op)
{
    poly_element_ptr p = e->data;
    element_ptr e0;

    poly_alloc(e, 1);
    e0 = p->coeff->item[0];
    element_set_mpz(e0, op);
    poly_remove_leading_zeroes(e);
}

void poly_set_coeff(element_ptr e, element_ptr a, int n)
{
    poly_element_ptr p = e->data;
    element_ptr e0;
    if (p->coeff->count < n + 1) {
	poly_alloc(e, n + 1);
    }
    e0 = p->coeff->item[n];
    element_set(e0, a);
}

void poly_setx(element_ptr f)
{
    poly_alloc(f, 2);
    element_set0(poly_coeff(f, 0));
    element_set1(poly_coeff(f, 1));
}

static void poly_set(element_ptr dst, element_ptr src)
{
    poly_element_ptr psrc = src->data;
    poly_element_ptr pdst = dst->data;
    int i;

    poly_alloc(dst, psrc->coeff->count);
    for (i=0; i<psrc->coeff->count; i++) {
	element_set(pdst->coeff->item[i], psrc->coeff->item[i]);
    }
}

static int poly_sgn(element_ptr f)
{
    int res = 0;
    int i;
    int n = poly_coeff_count(f);
    for (i=0; i<n; i++) {
	res = element_sgn(poly_coeff(f, i));
	if (res) break;
    }
    return res;
}

static void poly_add(element_ptr sum, element_ptr f, element_ptr g)
{
    int i, n, n1;
    element_ptr big;

    n = poly_coeff_count(f);
    n1 = poly_coeff_count(g);
    if (n > n1) {
	big = f;
	n = n1;
	n1 = poly_coeff_count(f);
    } else {
	big = g;
    }

    poly_alloc(sum, n1);
    for (i=0; i<n; i++) {
	element_add(poly_coeff(sum, i), poly_coeff(f, i), poly_coeff(g, i));
    }
    for (; i<n1; i++) {
	element_set(poly_coeff(sum, i), poly_coeff(big, i));
    }
    poly_remove_leading_zeroes(sum);
}

static void poly_sub(element_ptr diff, element_ptr f, element_ptr g)
{
    int i, n, n1;
    element_ptr big;

    n = poly_coeff_count(f);
    n1 = poly_coeff_count(g);
    if (n > n1) {
	big = f;
	n = n1;
	n1 = poly_coeff_count(f);
    } else {
	big = g;
    }

    poly_alloc(diff, n1);
    for (i=0; i<n; i++) {
	element_sub(poly_coeff(diff, i), poly_coeff(f, i), poly_coeff(g, i));
    }
    for (; i<n1; i++) {
	if (big == f) {
	    element_set(poly_coeff(diff, i), poly_coeff(big, i));
	} else {
	    element_neg(poly_coeff(diff, i), poly_coeff(big, i));
	}
    }
    poly_remove_leading_zeroes(diff);
}

static void poly_neg(element_ptr f, element_ptr g)
{
    poly_element_ptr pf = f->data;
    poly_element_ptr pg = g->data;
    int i, n;

    n = pg->coeff->count;
    poly_alloc(f, n);
    for (i=0; i<n; i++) {
	element_neg(pf->coeff->item[i], pg->coeff->item[i]);
    }
}

static void poly_double(element_ptr f, element_ptr g)
{
    poly_element_ptr pf = f->data;
    poly_element_ptr pg = g->data;
    int i, n;

    n = pg->coeff->count;
    poly_alloc(f, n);
    for (i=0; i<n; i++) {
	element_double(pf->coeff->item[i], pg->coeff->item[i]);
    }
}

static void poly_mul_mpz(element_ptr f, element_ptr g, mpz_ptr z)
{
    poly_element_ptr pf = f->data;
    poly_element_ptr pg = g->data;
    int i, n;

    n = pg->coeff->count;
    poly_alloc(f, n);
    for (i=0; i<n; i++) {
	element_mul_mpz(pf->coeff->item[i], pg->coeff->item[i], z);
    }
}

static void poly_mul_si(element_ptr f, element_ptr g, signed long int z)
{
    poly_element_ptr pf = f->data;
    poly_element_ptr pg = g->data;
    int i, n;

    n = pg->coeff->count;
    poly_alloc(f, n);
    for (i=0; i<n; i++) {
	element_mul_si(pf->coeff->item[i], pg->coeff->item[i], z);
    }
}

static void poly_mul(element_ptr r, element_ptr f, element_ptr g)
{
    poly_element_ptr pprod;
    poly_element_ptr pf = f->data;
    poly_element_ptr pg = g->data;
    poly_field_data_ptr pdp = r->field->data;
    int fcount = pf->coeff->count;
    int gcount = pg->coeff->count;
    int i, j, n;
    element_t prod;
    element_t e0;

    if (!fcount || !gcount) {
	element_set0(r);
	return;
    }
    element_init(prod, r->field);
    pprod = prod->data;
    n = fcount + gcount - 1;
    poly_alloc(prod, n);
    element_init(e0, pdp->field);
    for (i=0; i<n; i++) {
	element_ptr x = pprod->coeff->item[i];
	element_set0(x);
	for (j=0; j<=i; j++) {
	    if (j < fcount && i - j < gcount) {
		element_mul(e0, pf->coeff->item[j], pg->coeff->item[i - j]);
		element_add(x, x, e0);
	    }
	}
    }
    poly_remove_leading_zeroes(prod);
    element_set(r, prod);
    element_clear(e0);
    element_clear(prod);
}

void poly_random_monic(element_ptr f, int deg)
{
    int i;
    poly_alloc(f, deg + 1);
    for (i=0; i<deg; i++) {
	element_random(poly_coeff(f, i));
    }
    element_set1(poly_coeff(f, i));
}

element_ptr polymod_coeff(element_ptr e, int i)
{
    element_t *coeff = e->data;

    return coeff[i];
}

inline int polymod_field_degree(field_t f)
{
    polymod_field_data_ptr p = f->data;
    return p->n;
}

static void polymod_random(element_ptr e)
{
    element_t *coeff = e->data;
    int i, n = polymod_field_degree(e->field);

    for (i=0; i<n; i++) {
	element_random(coeff[i]);
    }
}

static void polymod_from_hash(element_ptr e, void *data, int len)
{
    //TODO: improve this
    element_t *coeff = e->data;
    int i, n = polymod_field_degree(e->field);
    for (i=0; i<n; i++) {
	element_from_hash(coeff[i], data, len);
    }
}

void polymod_const_mul(element_ptr res, element_ptr a, element_ptr e)
    //a lies in R, e in R[x]
{
    element_t *coeff = e->data, *dst = res->data;
    int i, n = polymod_field_degree(e->field);

    for (i=0; i<n; i++) {
	element_mul(dst[i], coeff[i], a);
    }
}

static void poly_set0(element_ptr e)
{
    poly_alloc(e, 0);
}

static void poly_set1(element_ptr e)
{
    poly_element_ptr p = e->data;
    element_ptr e0;

    poly_alloc(e, 1);
    e0 = p->coeff->item[0];
    element_set1(e0);
}

static int poly_is0(element_ptr e)
{
    poly_element_ptr p = e->data;
    return !p->coeff->count;
}

static int poly_is1(element_ptr e)
{
    poly_element_ptr p = e->data;
    if (p->coeff->count == 1) {
	return element_is1(p->coeff->item[0]);
    }
    return 0;
}

static size_t poly_out_str(FILE *stream, int base, element_ptr e)
{
    int i;
    int n = poly_coeff_count(e);
    size_t result = 2, status;

    /*
    if (!n) {
	if (EOF == fputs("[0]", stream)) return 0;
	return 3;
    }
    */
    if (EOF == fputc('[', stream)) return 0;
    for (i=0; i<n; i++) {
	if (i) {
	    if (EOF == fputs(", ", stream)) return 0;
	    result += 2;
	}
	status = element_out_str(stream, base, poly_coeff(e, i));
	if (!status) return 0;
	result += status;
    }
    if (EOF == fputc(']', stream)) return 0;
    return result;
}

static int poly_snprint(char *s, size_t size, element_ptr e)
{
    int i;
    int n = poly_coeff_count(e);
    size_t result = 0, left;
    int status;

#ifdef INNER_FUNC
    void clip_sub(void)
    {
	result += status;
	left = result >= size ? 0 : size - result;
    }
#else
#define clip_sub()\
    {\
	result += status;\
	left = result >= size ? 0 : size - result;\
    }
#endif
    status = snprintf(s, size, "[");
    if (status < 0) return status;
    clip_sub();

    for (i=0; i<n; i++) {
	if (i) {
	    status = snprintf(s + result, left, ", ");
	    if (status < 0) return status;
	    clip_sub();
	}
	status = element_snprint(s + result, left, poly_coeff(e, i));
	if (status < 0) return status;
	clip_sub();
    }
    status = snprintf(s + result, left, "]");
    if (status < 0) return status;
    return result + status;
#ifndef INNER_FUNC
#undef clip_sub
#endif
}

void poly_div(element_ptr quot, element_ptr rem,
	element_ptr a, element_ptr b)
{
    poly_element_ptr pq, pr;
    poly_field_data_ptr pdp = a->field->data;
    element_t q, r;
    element_t binv, e0;
    element_ptr qe;
    int m, n;
    int i, k;

    if (element_is0(b)) {
	fprintf(stderr, "BUG! division by zero!\n");
	exit(1);
    }
    n = poly_degree(b);
    m = poly_degree(a);
    if (n > m) {
	element_set(rem, a);
	element_set0(quot);
	return;
    }
    element_init(r, a->field);
    element_init(q, a->field);
    element_init(binv, pdp->field);
    element_init(e0, pdp->field);
    pq = q->data;
    pr = r->data;
    element_set(r, a);
    k = m - n;
    poly_alloc(q, k + 1);
    element_invert(binv, poly_coeff(b, n));
    while (k >= 0) {
	qe = pq->coeff->item[k];
	element_mul(qe, binv, pr->coeff->item[m]);
	for (i=0; i<=n; i++) {
	    element_mul(e0, qe, poly_coeff(b, i));
	    element_sub(pr->coeff->item[i + k], pr->coeff->item[i + k], e0);
	}
	k--;
	m--;
    }
    poly_remove_leading_zeroes(r);
    element_set(quot, q);
    element_set(rem, r);

    element_clear(q);
    element_clear(r);
    element_clear(e0);
    element_clear(binv);
}

void poly_const_mul(element_ptr res, element_ptr a, element_ptr poly)
{
    int i, n = poly_coeff_count(poly);
    poly_alloc(res, n);
    for (i=0; i<n; i++) {
	element_mul(poly_coeff(res, i), a, poly_coeff(poly, i));
    }
    poly_remove_leading_zeroes(res);
}

void poly_invert(element_ptr res, element_ptr f, element_ptr m)
{
    element_t q, r0, r1, r2;
    element_t b0, b1, b2;
    element_t inv;

    element_init(b0, res->field);
    element_init(b1, res->field);
    element_init(b2, res->field);
    element_init(q, res->field);
    element_init(r0, res->field);
    element_init(r1, res->field);
    element_init(r2, res->field);
    element_init(inv, poly_base_field(res));
    element_set0(b0);
    element_set1(b1);
    element_set(r0, m);
    element_set(r1, f);

    for (;;) {
	poly_div(q, r2, r0, r1);
	if (element_is0(r2)) break;
	element_mul(b2, b1, q);
	element_sub(b2, b0, b2);
	element_set(b0, b1);
	element_set(b1, b2);
	element_set(r0, r1);
	element_set(r1, r2);
    }
    element_invert(inv, poly_coeff(r1, 0));
    poly_const_mul(res, inv, b1);
    element_clear(inv);
    element_clear(q);
    element_clear(r0);
    element_clear(r1);
    element_clear(r2);
    element_clear(b0);
    element_clear(b1);
    element_clear(b2);
}

void element_polymod_to_poly(element_ptr f, element_ptr e)
{
    polymod_field_data_ptr p = e->field->data;
    element_t *coeff = e->data;
    int i, n = p->n;
    poly_alloc(f, n);
    for (i=0; i<n; i++) {
	element_set(poly_coeff(f, i), coeff[i]);
    }
    poly_remove_leading_zeroes(f);
}

void element_poly_to_polymod_truncate(element_ptr e, element_ptr f)
{
    polymod_field_data_ptr p = e->field->data;
    element_t *coeff = e->data;
    int i;
    int n;
    n = poly_coeff_count(f);
    if (n > p->n) n = p->n;

    for (i=0; i<n; i++) {
	element_set(coeff[i], poly_coeff(f, i));
    }
    for (; i<p->n; i++) {
	element_set0(coeff[i]);
    }
}

static void polymod_invert(element_ptr r, element_ptr e)
{
    polymod_field_data_ptr p = r->field->data;
    element_ptr minpoly = p->poly;
    element_t f, r1;

    element_init(f, minpoly->field);
    element_init(r1, minpoly->field);
    element_polymod_to_poly(f, e);

    poly_invert(r1, f, p->poly);

    element_poly_to_polymod_truncate(r, r1);

    element_clear(f);
    element_clear(r1);
}

void element_field_to_poly(element_ptr f, element_ptr g)
{
    poly_alloc(f, 1);
    element_set(poly_coeff(f, 0), g);
    poly_remove_leading_zeroes(f);
}

void element_field_to_polymod(element_ptr f, element_ptr g)
{
    polymod_field_data_ptr p = f->field->data;
    element_t *coeff = f->data;
    int i, n = p->n;
    element_set(coeff[0], g);
    for (i=1; i<n; i++) {
	element_set0(coeff[i]);
    }
}

static int poly_cmp(element_ptr f, element_ptr g)
{
    int i;
    int n = poly_coeff_count(f);
    int n1 = poly_coeff_count(g);
    if (n != n1) return 1;
    for (i=0; i<n; i++) {
	if (element_cmp(poly_coeff(f, i), poly_coeff(g, i))) return 1;
    }
    return 0;
}

static void field_clear_poly(field_ptr f)
{
    poly_field_data_ptr p = f->data;
    pbc_free(p);
}

//2 bytes hold the number of terms
//then the terms follow
//bad for sparse polynomials

static int poly_length_in_bytes(element_t p)
{
    int count = poly_coeff_count(p);
    int result = 2;
    int i;
    for (i=0; i<count; i++) {
	result += element_length_in_bytes(poly_coeff(p, i));
    }
    return result;
}

static int poly_to_bytes(unsigned char *buf, element_t p)
{
    int count = poly_coeff_count(p);
    int result = 2;
    int i;
    buf[0] = (unsigned char) count;
    buf[1] = (unsigned char) (count >> 8);
    for (i=0; i<count; i++) {
	result += element_to_bytes(&buf[result], poly_coeff(p, i));
    }
    return result;
}

static int poly_from_bytes(element_t p, unsigned char *buf)
{
    int result = 2;
    int count = buf[0] + buf[1] * 256;
    int i;
    poly_alloc(p, count);
    for (i=0; i<count; i++) {
	result += element_from_bytes(poly_coeff(p, i), &buf[result]);
    }
    return result;
}

//Contrived? This returns to_mpz(constant term)
static void poly_to_mpz(mpz_t z, element_ptr e)
{
    if (!poly_coeff_count(e)) {
	mpz_set_ui(z, 0);
    } else {
	element_to_mpz(z, poly_coeff(e, 0));
    }
}

void poly_out_info(FILE *str, field_ptr f)
{
    poly_field_data_ptr p = f->data;
    fprintf(str, "Polynomial ring. Base field:\n");
    field_out_info(str, p->field);
}

void field_init_poly(field_ptr f, field_ptr base_field)
{
    poly_field_data_ptr p;

    field_init(f);
    f->data = pbc_malloc(sizeof(poly_field_data_t));
    p = f->data;
    p->field = base_field;
    p->mapbase = element_field_to_poly;
    f->field_clear = field_clear_poly;
    f->init = poly_init;
    f->clear = poly_clear;
    f->set_si = poly_set_si;
    f->set_mpz = poly_set_mpz;
    f->to_mpz = poly_to_mpz;
    f->out_str = poly_out_str;
    f->snprint = poly_snprint;
    f->set = poly_set;
    f->sign = poly_sgn;
    f->add = poly_add;
    f->doub = poly_double;
    f->is0 = poly_is0;
    f->is1 = poly_is1;
    f->set0 = poly_set0;
    f->set1 = poly_set1;
    f->sub = poly_sub;
    f->neg = poly_neg;
    f->mul = poly_mul;
    f->mul_mpz = poly_mul_mpz;
    f->mul_si = poly_mul_si;
    f->cmp = poly_cmp;
    f->out_info = poly_out_info;

    f->to_bytes = poly_to_bytes;
    f->from_bytes = poly_from_bytes;
    f->fixed_length_in_bytes = -1;
    f->length_in_bytes = poly_length_in_bytes;
}

static void field_clear_polymod(field_ptr f)
{
    polymod_field_data_ptr p = f->data;
    int i, n = p->n;

    for (i=0; i<n; i++) {
	element_clear(p->xpwr[i]);
    }
    pbc_free(p->xpwr);

    element_clear(p->poly);
    pbc_free(f->data);
}

static int polymod_is_sqr(element_ptr e)
{
    int res;
    mpz_t z;
    element_t e0;

    element_init(e0, e->field);
    mpz_init(z);
    mpz_sub_ui(z, e->field->order, 1);
    mpz_divexact_ui(z, z, 2);

    element_pow_mpz(e0, e, z);
    res = element_is1(e0);
    element_clear(e0);
    mpz_clear(z);
    return res;
}

static void polymod_sqrt(element_ptr res, element_ptr a)
    //use Cantor-Zassenhaus aka Legendre's method
    //TODO: use another, faster method?
{
    field_t kx;
    element_t f;
    element_t r, s;
    element_t e0;
    mpz_t z;

    field_init_poly(kx, a->field);
    mpz_init(z);
    element_init(f, kx);
    element_init(r, kx);
    element_init(s, kx);
    element_init(e0, a->field);

    poly_alloc(f, 3);
    element_set1(poly_coeff(f, 2));
    element_neg(poly_coeff(f, 0), a);

    mpz_sub_ui(z, a->field->order, 1);
    mpz_divexact_ui(z, z, 2);
    for (;;) {
	int i;
	element_ptr x;
	element_ptr e1, e2;

	poly_alloc(r, 2);
	element_set1(poly_coeff(r, 1));
	x = poly_coeff(r, 0);
	element_random(x);
	element_mul(e0, x, x);
	if (!element_cmp(e0, a)) {
	    element_set(res, x);
	    break;
	}
	element_set1(s);
	//TODO: this can be optimized greatly
	//since we know r has the form ax + b
	for (i = mpz_sizeinbase(z, 2) - 1; i >=0; i--) {
	    element_mul(s, s, s);
	    if (poly_degree(s) == 2) {
		e1 = poly_coeff(s, 0);
		e2 = poly_coeff(s, 2);
		element_mul(e0, e2, a);
		element_add(e1, e1, e0);
		poly_alloc(s, 2);
		poly_remove_leading_zeroes(s);
	    }
	    if (mpz_tstbit(z, i)) {
		element_mul(s, s, r);
		if (poly_degree(s) == 2) {
		    e1 = poly_coeff(s, 0);
		    e2 = poly_coeff(s, 2);
		    element_mul(e0, e2, a);
		    element_add(e1, e1, e0);
		    poly_alloc(s, 2);
		    poly_remove_leading_zeroes(s);
		}
	    }
	}
	if (poly_degree(s) < 1) continue;
	element_set1(e0);
	e1 = poly_coeff(s, 0);
	e2 = poly_coeff(s, 1);
	element_add(e1, e1, e0);
	element_invert(e0, e2);
	element_mul(e0, e0, e1);
	element_mul(e2, e0, e0);
	if (!element_cmp(e2, a)) {
	    element_set(res, e0);
	    break;
	}
    }

    mpz_clear(z);
    element_clear(f);
    element_clear(r);
    element_clear(s);
    element_clear(e0);
    field_clear(kx);
}

void poly_make_monic(element_t f, element_t g)
{
    int n = poly_coeff_count(g);
    int i;
    element_ptr e0;
    poly_alloc(f, n);
    if (!n) return;

    e0 = poly_coeff(f, n - 1);
    element_invert(e0, poly_coeff(g, n - 1));
    for (i=0; i<n-1; i++) {
	element_mul(poly_coeff(f, i), poly_coeff(g, i), e0);
    }
    element_set1(e0);
}

static int polymod_to_bytes(unsigned char *data, element_t f)
{
    polymod_field_data_ptr p = f->field->data;
    element_t *coeff = f->data;
    int i, n = p->n;
    int len = 0;
    for (i=0; i<n; i++) {
	len += element_to_bytes(data + len, coeff[i]);
    }
    return len;
}

static int polymod_length_in_bytes(element_t f)
{
    polymod_field_data_ptr p = f->field->data;
    element_t *coeff = f->data;
    int i, n = p->n;
    int res = 0;

    for (i=0; i<n; i++) {
	res += element_length_in_bytes(coeff[i]);
    }

    return res;
}

static int polymod_from_bytes(element_t f, unsigned char *data)
{
    polymod_field_data_ptr p = f->field->data;
    element_t *coeff = f->data;
    int i, n = p->n;
    int len = 0;

    for (i=0; i<n; i++) {
	len += element_from_bytes(coeff[i], data + len);
    }
    return len;
}

static void polymod_init(element_t e)
{
    int i;
    polymod_field_data_ptr p = e->field->data;
    int n = p->n;
    element_t *coeff;
    coeff = e->data = pbc_malloc(sizeof(element_t) * n);

    for (i=0; i<n; i++) {
	element_init(coeff[i], p->field);
    }
}

static void polymod_clear(element_t e)
{
    polymod_field_data_ptr p = e->field->data;
    element_t *coeff = e->data;
    int i, n = p->n;
    for (i=0; i<n; i++) {
	element_clear(coeff[i]);
    }
    pbc_free(e->data);
}

static void polymod_set_si(element_t e, signed long int x)
{
    polymod_field_data_ptr p = e->field->data;
    element_t *coeff = e->data;
    int i, n = p->n;
    element_set_si(coeff[0], x);
    for (i=1; i<n; i++) {
	element_set0(coeff[i]);
    }
}

static void polymod_set_mpz(element_t e, mpz_t z)
{
    polymod_field_data_ptr p = e->field->data;
    element_t *coeff = e->data;
    int i, n = p->n;
    element_set_mpz(coeff[0], z);
    for (i=1; i<n; i++) {
	element_set0(coeff[i]);
    }
}

static void polymod_set(element_t e, element_t f)
{
    polymod_field_data_ptr p = e->field->data;
    element_t *dst = e->data, *src = f->data;
    int i, n = p->n;
    for (i=0; i<n; i++) {
	element_set(dst[i], src[i]);
    }
}

static void polymod_neg(element_t e, element_t f)
{
    polymod_field_data_ptr p = e->field->data;
    element_t *dst = e->data, *src = f->data;
    int i, n = p->n;
    for (i=0; i<n; i++) {
	element_neg(dst[i], src[i]);
    }
}

static int polymod_cmp(element_ptr f, element_ptr g)
{
    polymod_field_data_ptr p = f->field->data;
    element_t *c1 = f->data, *c2 = g->data;
    int i, n = p->n;
    for (i=0; i<n; i++) {
	if (element_cmp(c1[i], c2[i])) return 1;
    }
    return 0;
}

static void polymod_add(element_t r, element_t e, element_t f)
{
    polymod_field_data_ptr p = r->field->data;
    element_t *dst = r->data, *s1 = e->data, *s2 = f->data;
    int i, n = p->n;
    for (i=0; i<n; i++) {
	element_add(dst[i], s1[i], s2[i]);
    }
}

static void polymod_double(element_t r, element_t f)
{
    polymod_field_data_ptr p = r->field->data;
    element_t *dst = r->data, *s1 = f->data;
    int i, n = p->n;
    for (i=0; i<n; i++) {
	element_double(dst[i], s1[i]);
    }
}

static void polymod_sub(element_t r, element_t e, element_t f)
{
    polymod_field_data_ptr p = r->field->data;
    element_t *dst = r->data, *s1 = e->data, *s2 = f->data;
    int i, n = p->n;
    for (i=0; i<n; i++) {
	element_sub(dst[i], s1[i], s2[i]);
    }
}

static void polymod_mul_mpz(element_t e, element_t f, mpz_ptr z)
{
    polymod_field_data_ptr p = e->field->data;
    element_t *dst = e->data, *src = f->data;
    int i, n = p->n;
    for (i=0; i<n; i++) {
	element_mul_mpz(dst[i], src[i], z);
    }
}

static void polymod_mul_si(element_t e, element_t f, signed long int z)
{
    polymod_field_data_ptr p = e->field->data;
    element_t *dst = e->data, *src = f->data;
    int i, n = p->n;
    for (i=0; i<n; i++) {
	element_mul_si(dst[i], src[i], z);
    }
}

//Karatsuba for degree 2 polynomials
static void kar_poly_2(element_t *dst, element_t c3, element_t c4, element_t *s1, element_t *s2, element_t *scratch)
{
    element_ptr c01, c02, c12;

    c12 = scratch[0];
    c02 = scratch[1];
    c01 = scratch[2];

    element_add(c3, s1[0], s1[1]);
    element_add(c4, s2[0], s2[1]);
    element_mul(c01, c3, c4);
    element_add(c3, s1[0], s1[2]);
    element_add(c4, s2[0], s2[2]);
    element_mul(c02, c3, c4);
    element_add(c3, s1[1], s1[2]);
    element_add(c4, s2[1], s2[2]);
    element_mul(c12, c3, c4);

    element_mul(dst[1], s1[1], s2[1]);

    //constant term
    element_mul(dst[0], s1[0], s2[0]);

    //coefficient of x^4
    element_mul(c4, s1[2], s2[2]);

    //coefficient of x^3
    element_add(c3, dst[1], c4);
    element_sub(c3, c12, c3);

    //coefficient of x^2
    element_add(dst[2], c4, dst[0]);
    element_sub(c02, c02, dst[2]);
    element_add(dst[2], dst[1], c02);

    //coefficient of x
    element_sub(c01, c01, dst[0]);
    element_sub(dst[1], c01, dst[1]);
}

static void polymod_mul_degree3(element_ptr res, element_ptr e, element_ptr f)
{
    polymod_field_data_ptr p = res->field->data;
    element_t *dst = res->data, *s1 = e->data, *s2 = f->data;
    element_t c3, c4;
    element_t p0;

    element_init(p0, res->field);
    element_init(c3, p->field);
    element_init(c4, p->field);

    kar_poly_2(dst, c3, c4, s1, s2, p0->data);

    polymod_const_mul(p0, c3, p->xpwr[0]);
    element_add(res, res, p0);
    polymod_const_mul(p0, c4, p->xpwr[1]);
    element_add(res, res, p0);

    element_clear(p0);
    element_clear(c3);
    element_clear(c4);
}

static void polymod_mul_degree6(element_ptr res, element_ptr e, element_ptr f)
{
    polymod_field_data_ptr p = res->field->data;
    element_t *dst = res->data, *s0, *s1 = e->data, *s2 = f->data;
    element_t *a0, *a1, *b0, *b1;
    element_t p0, p1, p2, p3;

    a0 = s1;
    a1 = &s1[3];
    b0 = s2;
    b1 = &s2[3];

    element_init(p0, res->field);
    element_init(p1, res->field);
    element_init(p2, res->field);
    element_init(p3, res->field);

    s0 = p0->data;
    s1 = p1->data;
    s2 = p2->data;
    element_add(s0[0], a0[0], a1[0]);
    element_add(s0[1], a0[1], a1[1]);
    element_add(s0[2], a0[2], a1[2]);

    element_add(s1[0], b0[0], b1[0]);
    element_add(s1[1], b0[1], b1[1]);
    element_add(s1[2], b0[2], b1[2]);

    kar_poly_2(s2, s2[3], s2[4], s0, s1, p3->data);
    kar_poly_2(s0, s0[3], s0[4], a0, b0, p3->data);
    kar_poly_2(s1, s1[3], s1[4], a1, b1, p3->data);

    element_set(dst[0], s0[0]);
    element_set(dst[1], s0[1]);
    element_set(dst[2], s0[2]);

    element_sub(dst[3], s0[3], s0[0]);
    element_sub(dst[3], dst[3], s1[0]);
    element_add(dst[3], dst[3], s2[0]);

    element_sub(dst[4], s0[4], s0[1]);
    element_sub(dst[4], dst[4], s1[1]);
    element_add(dst[4], dst[4], s2[1]);

    element_sub(dst[5], s2[2], s0[2]);
    element_sub(dst[5], dst[5], s1[2]);

    //start reusing part of s0 as scratch(!)
    element_sub(s0[0], s2[3], s0[3]);
    element_sub(s0[0], s0[0], s1[3]);
    element_add(s0[0], s0[0], s1[0]);

    element_sub(s0[1], s2[4], s0[4]);
    element_sub(s0[1], s0[1], s1[4]);
    element_add(s0[1], s0[1], s1[1]);

    polymod_const_mul(p3, s0[0], p->xpwr[0]);
    element_add(res, res, p3);
    polymod_const_mul(p3, s0[1], p->xpwr[1]);
    element_add(res, res, p3);
    polymod_const_mul(p3, s1[2], p->xpwr[2]);
    element_add(res, res, p3);
    polymod_const_mul(p3, s1[3], p->xpwr[3]);
    element_add(res, res, p3);
    polymod_const_mul(p3, s1[4], p->xpwr[4]);
    element_add(res, res, p3);

    element_clear(p0);
    element_clear(p1);
    element_clear(p2);
    element_clear(p3);
}

/*
void polymod_mul_degree3(element_ptr res, element_ptr e, element_ptr f)
{
    polymod_field_data_ptr p = res->field->data;
    element_t *dst = res->data, *s1 = e->data, *s2 = f->data;
    element_t *p0data;
    element_ptr c01, c02, c12;
    element_t c3, c4;
    element_t p0;

    element_init(p0, res->field);
    element_init(c3, p->field);
    element_init(c4, p->field);
    p0data = p0->data;

    c12 = p0data[0];
    c02 = p0data[1];
    c01 = p0data[2];

    element_add(c3, s1[0], s1[1]);
    element_add(c4, s2[0], s2[1]);
    element_mul(c01, c3, c4);
    element_add(c3, s1[0], s1[2]);
    element_add(c4, s2[0], s2[2]);
    element_mul(c02, c3, c4);
    element_add(c3, s1[1], s1[2]);
    element_add(c4, s2[1], s2[2]);
    element_mul(c12, c3, c4);

    element_mul(dst[1], s1[1], s2[1]);

    //constant term
    element_mul(dst[0], s1[0], s2[0]);

    //coefficient of x^4
    element_mul(c4, s1[2], s2[2]);

    //coefficient of x^3
    element_add(c3, dst[1], c4);
    element_sub(c3, c12, c3);

    //coefficient of x^2
    element_add(dst[2], c4, dst[0]);
    element_sub(c02, c02, dst[2]);
    element_add(dst[2], dst[1], c02);

    //coefficient of x
    element_sub(c01, c01, dst[0]);
    element_sub(dst[1], c01, dst[1]);

    polymod_const_mul(p0, c3, p->xpwr[0]);
    element_add(res, res, p0);
    polymod_const_mul(p0, c4, p->xpwr[1]);
    element_add(res, res, p0);

    element_clear(p0);
    element_clear(c3);
    element_clear(c4);
}
*/

static void polymod_mul(element_ptr res, element_ptr e, element_ptr f)
//NOTE: cases n = 3, 6 have dedicated routines
{
    polymod_field_data_ptr p = res->field->data;
    int n = p->n;
    element_t *dst;
    element_t *s1 = e->data, *s2 = f->data;
    element_t prod, p0, c0;
    int i, j;
    element_t *high; //coefficients of x^n,...,x^{2n-2}

    high = pbc_malloc(sizeof(element_t) * (n - 1));
    for (i=0; i<n-1; i++) {
	element_init(high[i], p->field);
	element_set0(high[i]);
    }
    element_init(prod, res->field);
    dst = prod->data;
    element_init(p0, res->field);
    element_init(c0, p->field);

    for (i=0; i<n; i++) {
	int ni = n - i;
	for (j=0; j<ni; j++) {
	    element_mul(c0, s1[i], s2[j]);
	    element_add(dst[i + j], dst[i + j], c0);
	}
	for (;j<n; j++) {
	    element_mul(c0, s1[i], s2[j]);
	    element_add(high[j - ni], high[j - ni], c0);
	}
    }

    for (i=0; i<n-1; i++) {
	polymod_const_mul(p0, high[i], p->xpwr[i]);
	element_add(prod, prod, p0);
	element_clear(high[i]);
    }
    pbc_free(high);

    element_set(res, prod);
    element_clear(prod);
    element_clear(p0);
    element_clear(c0);
}

static void polymod_square_degree3(element_ptr res, element_ptr e)
//TODO: investigate if squaring is significantly cheaper
//than multiplication. If so convert to Karatsuba.
{
    element_t *dst = res->data;
    element_t *src = e->data;
    polymod_field_data_ptr p = res->field->data;
    element_t p0;
    element_t c0, c2;
    element_ptr c1, c3;

    element_init(p0, res->field);
    element_init(c0, p->field);
    element_init(c2, p->field);

    c3 = p0->data;
    c1 = c3 + 1;

    element_mul(c3, src[0], src[1]);
    element_mul(c1, src[0], src[2]);
    element_square(dst[0], src[0]);

    element_mul(c2, src[1], src[2]);
    element_square(c0, src[2]);
    element_square(dst[2], src[1]);

    element_add(dst[1], c3, c3);

    element_add(c1, c1, c1);
    element_add(dst[2], dst[2], c1);

    polymod_const_mul(p0, c0, p->xpwr[1]);
    element_add(res, res, p0);

    element_add(c2, c2, c2);
    polymod_const_mul(p0, c2, p->xpwr[0]);
    element_add(res, res, p0);

    element_clear(p0);
    element_clear(c0);
    element_clear(c2);
}

static void polymod_square(element_ptr res, element_ptr e)
{
    element_t *dst;
    element_t *src = e->data;
    polymod_field_data_ptr p = res->field->data;
    int n = p->n;

    element_t prod, p0, c0;
    int i, j;

    element_t *high; //coefficients of x^n,...,x^{2n-2}

    high = pbc_malloc(sizeof(element_t) * (n - 1));
    for (i=0; i<n-1; i++) {
	element_init(high[i], p->field);
	element_set0(high[i]);
    }

    element_init(prod, res->field);
    dst = prod->data;
    element_init(p0, res->field);
    element_init(c0, p->field);

    for (i=0; i<n; i++) {
	int twicei = 2 * i;
	element_square(c0, src[i]);
	if (twicei < n) {
	    element_add(dst[twicei], dst[twicei], c0);
	} else {
	    element_add(high[twicei - n], high[twicei - n], c0);
	}

	for (j=i+1; j<n-i; j++) {
	    element_mul(c0, src[i], src[j]);
	    element_add(c0, c0, c0);
	    element_add(dst[i + j], dst[i + j], c0);
	}
	for (;j<n; j++) {
	    element_mul(c0, src[i], src[j]);
	    element_add(c0, c0, c0);
	    element_add(high[i + j - n], high[i + j - n], c0);
	}
    }

    for (i=0; i<n-1; i++) {
	polymod_const_mul(p0, high[i], p->xpwr[i]);
	element_add(prod, prod, p0);
	element_clear(high[i]);
    }
    pbc_free(high);

    element_set(res, prod);
    element_clear(prod);
    element_clear(p0);
    element_clear(c0);
}

static int polymod_is0(element_ptr e)
{
    polymod_field_data_ptr p = e->field->data;
    element_t *coeff = e->data;
    int i, n = p->n;

    for (i=0; i<n; i++) {
	if (!element_is0(coeff[i])) return 0;
    }
    return 1;
}

static int polymod_is1(element_ptr e)
{
    polymod_field_data_ptr p = e->field->data;
    element_t *coeff = e->data;
    int i, n = p->n;

    if (!element_is1(coeff[0])) return 0;
    for (i=1; i<n; i++) {
	if (!element_is0(coeff[i])) return 0;
    }
    return 1;
}

static void polymod_set0(element_ptr e)
{
    polymod_field_data_ptr p = e->field->data;
    element_t *coeff = e->data;
    int i, n = p->n;

    for (i=0; i<n; i++) {
	element_set0(coeff[i]);
    }
}

static void polymod_set1(element_ptr e)
{
    polymod_field_data_ptr p = e->field->data;
    element_t *coeff = e->data;
    int i, n = p->n;

    element_set1(coeff[0]);
    for (i=1; i<n; i++) {
	element_set0(coeff[i]);
    }
}

static int polymod_sgn(element_ptr e)
{
    polymod_field_data_ptr p = e->field->data;
    element_t *coeff = e->data;
    int res = 0;
    int i, n = p->n;
    for (i=0; i<n; i++) {
	res = element_sgn(coeff[i]);
	if (res) break;
    }
    return res;
}

static size_t polymod_out_str(FILE *stream, int base, element_ptr e)
{
    size_t result = 2, status;
    polymod_field_data_ptr p = e->field->data;
    element_t *coeff = e->data;
    int i, n = p->n;

    if (EOF == fputc('[', stream)) return 0;
    for (i=0; i<n; i++) {
	if (i) {
	    if (EOF == fputs(", ", stream)) return 0;
	    result += 2;
	}
	status = element_out_str(stream, base, coeff[i]);
	if (!status) return 0;
	result += status;
    }
    if (EOF == fputc(']', stream)) return 0;
    return result;
}

static int polymod_snprint(char *s, size_t size, element_ptr e)
{
    polymod_field_data_ptr p = e->field->data;
    element_t *coeff = e->data;
    int i, n = p->n;
    size_t result = 0, left;
    int status;

#ifdef INNER_FUNC
    void clip_sub(void)
    {
	result += status;
	left = result >= size ? 0 : size - result;
    }
#else
#define clip_sub()\
    {\
	result += status;\
	left = result >= size ? 0 : size - result;\
    }
#endif
    status = snprintf(s, size, "[");
    if (status < 0) return status;
    clip_sub();

    for (i=0; i<n; i++) {
	if (i) {
	    status = snprintf(s + result, left, ", ");
	    if (status < 0) return status;
	    clip_sub();
	}
	status = element_snprint(s + result, left, coeff[i]);
	if (status < 0) return status;
	clip_sub();
    }
    status = snprintf(s + result, left, "]");
    if (status < 0) return status;
    return result + status;
#ifndef INNER_FUNC
#undef clip_sub
#endif
}

static int polymod_set_str(element_ptr e, char *s, int base)
{
    polymod_field_data_ptr p = e->field->data;
    element_t *coeff = e->data;
    int i, n = p->n;
    char *cp = s;
    element_set0(e);
    while (*cp && isspace(*cp)) cp++;
    if (*cp++ != '[') return 0;
    for (i=0; i<n; i++) {
	cp += element_set_str(coeff[i], cp, base);
	while (*cp && isspace(*cp)) cp++;
	if (i<n-1 && *cp++ != ',') return 0;
    }
    if (*cp++ != ']') return 0;
    return cp - s;
}

//Contrived? This returns to_mpz(constant term)
static void polymod_to_mpz(mpz_t z, element_ptr e)
{
    element_to_mpz(z, polymod_coeff(e, 0));
}

static void compute_x_powers(field_ptr field, element_ptr poly)
//compute x^n,...,x^{2n-2} mod poly
{
    polymod_field_data_ptr p = field->data;
    element_t p0;
    element_ptr pwrn;
    element_t *coeff, *coeff1;
    int i, j;
    int n = p->n;
    element_t *xpwr;
    
    xpwr = p->xpwr;

    element_init(p0, field);
    for (i=0; i<n; i++) {
	element_init(xpwr[i], field);
    }
    pwrn = xpwr[0];
    element_poly_to_polymod_truncate(pwrn, poly);
    element_neg(pwrn, pwrn);

    for (i=1; i<n; i++) {
	coeff = xpwr[i-1]->data;
	coeff1 = xpwr[i]->data;

	element_set0(coeff1[0]);
	for (j=1; j<n; j++) {
	    element_set(coeff1[j], coeff[j - 1]);
	}
	polymod_const_mul(p0, coeff[n - 1], pwrn);
	element_add(xpwr[i], xpwr[i], p0);
    }
    element_clear(p0);
}

void polymod_out_info(FILE *str, field_ptr f)
{
    polymod_field_data_ptr p = f->data;
    element_fprintf(str, "Field extension. Min poly = %B\n", p->poly);
    fprintf(str, "Base field:\n");
    field_out_info(str, p->field);
}

void field_init_polymod(field_ptr f, element_ptr poly)
    //assumes poly is monic
{
    poly_field_data_ptr pdp = poly->field->data;
    polymod_field_data_ptr p;
    int n;

    field_init(f);
    f->data = pbc_malloc(sizeof(polymod_field_data_t));
    p = f->data;
    p->field = pdp->field;
    p->mapbase = element_field_to_poly;
    element_init(p->poly, poly->field);
    element_set(p->poly, poly);
    n = p->n = poly_degree(p->poly);
    f->field_clear = field_clear_polymod;
    f->init = polymod_init;
    f->clear = polymod_clear;
    f->set_si = polymod_set_si;
    f->set_mpz = polymod_set_mpz;
    f->out_str = polymod_out_str;
    f->snprint = polymod_snprint;
    f->set_str = polymod_set_str;
    f->set = polymod_set;
    f->sign = polymod_sgn;
    f->add = polymod_add;
    f->doub = polymod_double;
    f->sub = polymod_sub;
    f->neg = polymod_neg;
    f->is0 = polymod_is0;
    f->is1 = polymod_is1;
    f->set0 = polymod_set0;
    f->set1 = polymod_set1;
    f->cmp = polymod_cmp;
    f->to_mpz = polymod_to_mpz;
    switch(n) {
	case 3:
	    f->mul = polymod_mul_degree3;
	    f->square = polymod_square_degree3;
	    break;
	case 6:
	    f->mul = polymod_mul_degree6;
	    f->square = polymod_square;
	    break;
	default:
	    f->mul = polymod_mul;
	    f->square = polymod_square;
	    break;
    }

    f->mul_mpz = polymod_mul_mpz;
    f->mul_si = polymod_mul_si;
    f->random = polymod_random;
    f->from_hash = polymod_from_hash;
    f->invert = polymod_invert;
    f->is_sqr = polymod_is_sqr;
    f->sqrt = polymod_sqrt;
    f->to_bytes = polymod_to_bytes;
    f->from_bytes = polymod_from_bytes;
    f->out_info = polymod_out_info;

    if (pdp->field->fixed_length_in_bytes < 0) {
	f->fixed_length_in_bytes = -1;
	f->length_in_bytes = polymod_length_in_bytes;
    } else {
	f->fixed_length_in_bytes = pdp->field->fixed_length_in_bytes * poly_degree(poly);
    }
    mpz_pow_ui(f->order, p->field->order, n);

    p->xpwr = pbc_malloc(sizeof(element_t) * n);
    compute_x_powers(f, poly);
}

void trial_divide(darray_ptr factor, darray_ptr mult, mpz_t n, mpz_ptr limit)
{
    mpz_t p, m;
    mpz_ptr fac;
    unsigned int mul;

    mpz_init(p);
    mpz_init(m);
    mpz_set(m ,n);
    mpz_set_ui(p, 2);

    while (mpz_cmp_ui(m, 1)) {
	if (mpz_probab_prime_p(m, 10)) {
	    mpz_set(p, m);
	}
	if (limit && mpz_cmp(p, limit) > 0) {
	    mpz_set(p, m);
	}
	if (mpz_divisible_p(m, p)) {
	    fac = pbc_malloc(sizeof(mpz_t));
	    mul = 0;
	    mpz_init(fac);
	    mpz_set(fac, p);
	    darray_append(factor, fac);
	    do {
		mpz_divexact(m, m, p);
		mul++;
	    } while (mpz_divisible_p(m, p));
	    darray_append(mult, int_to_voidp(mul));
	}
	mpz_nextprime(p, p);
    }
   
    mpz_clear(m);
    mpz_clear(p);
}

void poly_gcd(element_ptr d, element_ptr f, element_ptr g)
{
    element_t a, b, q, r;
    element_init(a, d->field);
    element_init(b, d->field);
    element_init(q, d->field);
    element_init(r, d->field);

    element_set(a, f);
    element_set(b, g);
    for(;;) {
	//TODO: don't care about q
	poly_div(q, r, a, b);
	if (element_is0(r)) break;
	element_set(a, b);
	element_set(b, r);
    }
    element_set(d, b);
    element_clear(a);
    element_clear(b);
    element_clear(q);
    element_clear(r);
}

int poly_is_irred_degfac(element_ptr f, darray_t factor)
    //called by poly_is_irred
    //needs to be passed a list of the factors of deg f
{
    int res;
    element_t xpow, x, g;
    mpz_t deg, z;
    int i, n;
    field_ptr basef = poly_base_field(f);
    field_t rxmod;

    mpz_init(deg);
    mpz_init(z);

    field_init_polymod(rxmod, f);

    mpz_set_ui(deg, poly_degree(f));
    element_init(xpow, rxmod);
    element_init(x, rxmod);
    element_init(g, f->field);
    n = factor->count;
    //element_set1(((element_t *) x->data)[1]);
    element_set1(polymod_coeff(x, 1));

    res = 0;
    for (i=0; i<n; i++) {
	mpz_divexact(z, deg, factor->item[i]);
	mpz_pow_ui(z, basef->order, mpz_get_ui(z));
	element_pow_mpz(xpow, x, z);
	element_sub(xpow, xpow, x);
	if (element_is0(xpow)) {
	    goto done;
	}
	element_polymod_to_poly(g, xpow);
	poly_gcd(g, f, g);
	if (poly_degree(g) != 0) goto done;
    }

    mpz_pow_ui(z, basef->order, poly_degree(f));
    element_pow_mpz(xpow, x, z);
    element_sub(xpow, xpow, x);
    if (element_is0(xpow)) res = 1;

done:
    element_clear(g);
    element_clear(xpow);
    element_clear(x);
    mpz_clear(deg);
    mpz_clear(z);
    field_clear(rxmod);
    return res;
}

    void clear(void *p) {
	mpz_clear(p);
	pbc_free(p);
    }

int poly_is_irred(element_ptr f)
{
    darray_t fac, mul;
    mpz_t deg;
    int res;

    if (poly_degree(f) <= 1) return 1;

    darray_init(fac);
    darray_init(mul);
    mpz_init(deg);

    mpz_set_ui(deg, poly_degree(f));

    trial_divide(fac, mul, deg, NULL);
    res = poly_is_irred_degfac(f, fac);

    darray_forall(fac, clear);

    darray_clear(fac);
    darray_clear(mul);
    mpz_clear(deg);
    return res;
}
