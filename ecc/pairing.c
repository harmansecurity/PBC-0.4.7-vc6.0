#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gmp.h>
#include "pbc_field.h"
#include "pbc_darray.h"
#include "pbc_poly.h"
#include "pbc_curve.h"
#include "pbc_fops.h"
#include "pbc_symtab.h"
#include "pbc_parse.h"
#include "pbc_tracker.h"
#include "pbc_pairing.h"
#include "pbc_memory.h"

#include "pbc_a_param.h"
#include "pbc_mnt.h"
#include "pbc_d_param.h"
#include "pbc_e_param.h"
#include "pbc_f_param.h"
#include "pbc_a1_param.h"
#include "pbc_g_param.h"

#include "pbc_utils.h"

int generic_is_almost_coddh(element_ptr a, element_ptr b,
	element_ptr c, element_ptr d, pairing_t pairing)
{
    int res = 0;
    element_t t0, t1;

    element_init(t0, pairing->GT);
    element_init(t1, pairing->GT);
    bilinear_map(t0, a, d, pairing);
    bilinear_map(t1, b, c, pairing);
    if (!element_cmp(t0, t1)) {
	res = 1;
    } else {
	element_mul(t0, t0, t1);
	if (element_is1(t0)) res = 1;
    }
    element_clear(t0);
    element_clear(t1);
    return res;
}

void phi_warning(element_ptr out, element_ptr in, pairing_ptr pairing)
{
    UNUSED_VAR(out);
    UNUSED_VAR(in);
    UNUSED_VAR(pairing);
    printf("Phi() not implemented for this pairing type yet!\n");
}

void default_option_set(struct pairing_s *pairing, char *key, char *value)
{
    UNUSED_VAR(pairing);
    UNUSED_VAR(key);
    UNUSED_VAR(value);
}

void default_pp_init(pairing_pp_t p, element_ptr in1, pairing_t pairing) {
    UNUSED_VAR(pairing);
    p->data = (void *) in1;
}

void default_pp_apply(element_ptr out, element_ptr in2, pairing_pp_t p)
{
    pairing_apply(out, p->data, in2, p->pairing);
}

void default_pp_clear(pairing_pp_t p)
{
    UNUSED_VAR(p);
}

void pairing_init_inp_generic (pairing_t pairing, fetch_ops_t fops, void *ctx)
{
    char *s;
    token_t tok;

    assert (fops);
    assert (ctx);

    pairing->option_set = default_option_set;
    pairing->pp_init = default_pp_init;
    pairing->pp_clear = default_pp_clear;
    pairing->pp_apply = default_pp_apply;
    token_init(tok);
    token_get_generic (tok, fops, ctx);
    if (tok->type != token_word) {
	return;
    }
    //word should be "type"
    token_get_generic (tok, fops, ctx);
    if (tok->type != token_word) {
	return;
    }
    s = pbc_strdup(tok->s);

    pairing->is_almost_coddh = generic_is_almost_coddh;
    pairing->phi = phi_warning;
    //TODO: yuck!
    if (!strcmp(s, "a")) {
	a_param_t ap;

	a_param_init(ap);
	a_param_inp_generic (ap, fops, ctx);
	pairing_init_a_param(pairing, ap);
	a_param_clear(ap);
    } else 
	if (!strcmp(s, "d")) {
	d_param_t cp;

	d_param_init(cp);
	d_param_inp_generic (cp, fops, ctx);
	pairing_init_d_param(pairing, cp);
	d_param_clear(cp);
    } 
#if 0
	else if (!strcmp(s, "e")) {
	e_param_t ep;

	e_param_init(ep);
	e_param_inp_generic (ep, fops, ctx);
	pairing_init_e_param(pairing, ep);
	e_param_clear(ep);
    } 
#endif
	else if (!strcmp(s, "f")) {
	f_param_t fp;

	f_param_init(fp);
	f_param_inp_generic (fp, fops, ctx);
	pairing_init_f_param(pairing, fp);
	f_param_clear(fp);
    }
#if 0	
	else if (!strcmp(s, "a1")) {
	a1_param_t a1p;

	a1_param_init(a1p);
	a1_param_inp_generic (a1p, fops, ctx);
	pairing_init_a1_param(pairing, a1p);
	a1_param_clear(a1p);
    } else if (!strcmp(s, "g")) {
	g_param_t gp;

	g_param_init(gp);
	g_param_inp_generic (gp, fops, ctx);
	pairing_init_g_param(pairing, gp);
	g_param_clear(gp);
    }
#endif
	else {
	fprintf(stderr, "unknown pairing type!\n");
	exit(1);
    }
    token_clear(tok);
    pbc_free(s);
}

void pairing_init_inp_buf (pairing_t pairing, const char *buf, size_t len)
{
    tracker_t t;

    assert (buf);
    tracker_init (&t, buf, len);
    pairing_init_inp_generic (pairing, &fops_buf, &t);
}

void pairing_init_inp_str(pairing_t pairing, FILE *stream)
{
    assert (stream);
    pairing_init_inp_generic (pairing, &fops_str, stream);
}

void pairing_clear(pairing_t pairing)
{
    pairing->clear_func(pairing);
}
