// requires
// * stdio.h
// * gmp.h
// * fops.h
// * field.h
#ifndef __PBC_PAIRING_H__
#define __PBC_PAIRING_H__

struct pairing_pp_s {
    struct pairing_s *pairing;
    void *data;
};
typedef struct pairing_pp_s pairing_pp_t[1];
typedef struct pairing_pp_s *pairing_pp_ptr;

struct pairing_s {
    mpz_t r; //order of G1, G2, GT
    field_t Zr; //the field Z_r
    field_ptr G1, G2, GT;

    mpz_t phikonr;
    //Phi_k(q)/r where Phi_k is the kth cyclotomic polynomial,
    //q as in F_q, is the base field

    void (*phi)(element_ptr out, element_ptr in, struct pairing_s *pairing); //isomorphism G2 --> G1
    void (*map)(element_ptr out, element_ptr in1, element_ptr in2,
	    struct pairing_s *p);
    //is_almost coddh returns 1 given (g, g^x, h, h^x) or (g, g^x, h, h^-x)
    //0 otherwise
    //order is important: a, b are from G1, c, d are from G2
    int (*is_almost_coddh)(element_ptr a, element_ptr b,
	    element_ptr c, element_ptr d,
	    struct pairing_s *p);
    void (*clear_func)(struct pairing_s *);
    void (*pp_init)(pairing_pp_t p, element_t in1, struct pairing_s *);
    void (*pp_clear)(pairing_pp_t p);
    void (*pp_apply)(element_t out, element_t in2, pairing_pp_t p);
    void (*option_set)(struct pairing_s *, char *key, char *value);
    void *data;
};

typedef struct pairing_s pairing_t[1];
typedef struct pairing_s *pairing_ptr;

/*@manual pairing_apply
Get ready to perform a pairing whose first input is ''in1'',
and store the results of time-saving precomputation in ''p''.
*/
static inline void pairing_pp_init(pairing_pp_t p, element_t in1, pairing_t pairing)
{
    p->pairing = pairing;
    pairing->pp_init(p, in1, pairing);
}

/*@manual pairing_apply
Clear ''p''. This should be called after ''p'' is no longer needed.
*/
static inline void pairing_pp_clear(pairing_pp_t p)
{
    p->pairing->pp_clear(p);
}

/*@manual pairing_apply
Compute a pairing using ''in2'' and the preprocessed information stored in ''p''
and store the output in ''out''. The inputs to the pairing are the element
previously used to initialize ''p'' and the element ''in2''.
*/
static inline void pairing_pp_apply(element_t out, element_t in2, pairing_pp_t p)
{
    p->pairing->pp_apply(out, in2, p);
}
/*@manual pairing_init
TODO
*/
void pairing_init_inp_generic(pairing_t pairing, fetch_ops_t fops, void *ctx);

/*@manual pairing_init
Read in pairing parameters from array of characters ''buf'' of length ''len''
and use them to initialize ''pairing''.
*/
void pairing_init_inp_buf(pairing_t pairing, const char *buf, size_t len);

/*@manual pairing_init
Read in pairing parameters from ''stream''
and use them to initialize ''pairing''.
*/
void pairing_init_inp_str(pairing_t pairing, FILE *stream);

/*@manual pairing_init
Free the space occupied by ''pairing''. Call
whenever a <type>pairing_t</type> variable is no longer needed.
IMPORTANT: only call this after all elements associated with ''pairing''
have been cleared, as they need information stored in the ''pairing''
structure.
*/
void pairing_clear(pairing_t pairing);

static inline void bilinear_map(element_t out, element_t in1, element_t in2,
    pairing_t pairing) {
    pairing->map(out, in1, in2, pairing);
}

/*@manual pairing_apply
Apply the bilinear map described by ''pairing''. The element ''out''
will be set to the map applied to ''in1'' and ''in2'', that is
''out'' = e(''in1'', ''in2'').
''in1'' must be in the group G1,
''in2'' must be in the group G2, and
''out'' must be in the group GT.
*/
static inline void pairing_apply(element_t out, element_t in1, element_t in2,
    pairing_t pairing)
{
    pairing->map(out, in1, in2, pairing);
}

/*@manual pairing_op
Returns 1 if G1 and G2 are the same group, 0 otherwise
*/
static inline int pairing_is_symmetric(pairing_t pairing)
{
    return pairing->G1 == pairing->G2;
}

/*@manual pairing_op
Returns the length in bytes needed to represent an element of G1.
*/
static inline int pairing_length_in_bytes_G1(pairing_t pairing)
{
    return pairing->G1->fixed_length_in_bytes;
}

/*@manual pairing_op
Returns the length in bytes needed to represent the x-coordinate of
an element of G1.
*/
static inline int pairing_length_in_bytes_x_only_G1(pairing_t pairing)
{
    return pairing->G1->fixed_length_in_bytes / 2;
}

/*@manual pairing_op
Returns the length in bytes needed to represent a compressed form of
an element of G1. There is some overhead in decompressing.
*/
static inline int pairing_length_in_bytes_compressed_G1(pairing_t pairing)
{
    return pairing->G1->fixed_length_in_bytes / 2 + 1;
}

/*@manual pairing_op
Returns the length in bytes needed to represent an element of G2.
*/
static inline int pairing_length_in_bytes_G2(pairing_t pairing)
{
    return pairing->G2->fixed_length_in_bytes;
}

/*@manual pairing_op
Returns the length in bytes needed to represent a compressed form of
an element of G2. There is some overhead in decompressing.
*/
static inline int pairing_length_in_bytes_compressed_G2(pairing_t pairing)
{
    return pairing->G2->fixed_length_in_bytes / 2 + 1;
}

/*@manual pairing_op
Returns the length in bytes needed to represent the x-coordinate of
an element of G2.
*/
static inline int pairing_length_in_bytes_x_only_G2(pairing_t pairing)
{
    return pairing->G2->fixed_length_in_bytes / 2;
}

/*@manual pairing_op
Returns the length in bytes needed to represent an element of GT.
*/
static inline int pairing_length_in_bytes_GT(pairing_t pairing)
{
    return pairing->GT->fixed_length_in_bytes;
}

/*@manual pairing_op
Returns the length in bytes needed to represent an element of Zr.
*/
static inline int pairing_length_in_bytes_Zr(pairing_t pairing)
{
    return pairing->Zr->fixed_length_in_bytes;
}

int generic_is_almost_coddh(element_ptr a, element_ptr b,
	element_ptr c, element_ptr d, pairing_t pairing);

static inline int is_almost_coddh(element_t a, element_t b,
	element_t c, element_t d, pairing_t pairing) {
    return pairing->is_almost_coddh(a, b, c, d, pairing);
}

/*@manual einit.1
*/
static inline void element_init_G1(element_t e, pairing_t pairing)
{
    element_init(e, pairing->G1);
}

/*@manual einit.1
*/
static inline void element_init_G2(element_t e, pairing_t pairing)
{
    element_init(e, pairing->G2);
}

/*@manual einit.1
Initialize ''e'' to be an element of the group G1, G2 or GT of ''pairing''.
*/
static inline void element_init_GT(element_t e, pairing_t pairing)
{
    element_init(e, pairing->GT);
}

/*@manual einit.1
Initialize ''e'' to be an element of the ring Z_r of ''pairing''.
r is the order of the groups G1, G2 and GT that are involved in the pairing.
*/
static inline void element_init_Zr(element_t e, pairing_t pairing)
{
    element_init(e, pairing->Zr);
}

static inline void pairing_option_set(pairing_t pairing, char *key, char *value)
{
    pairing->option_set(pairing, key, value);
}

extern int generic_is_almost_coddh(element_ptr a, element_ptr b,
	element_ptr c, element_ptr d, pairing_t pairing);

void phi_warning(element_ptr out, element_ptr in, pairing_ptr pairing);
void default_option_set(struct pairing_s *pairing, char *key, char *value);
void default_pp_init(pairing_pp_t p, element_ptr in1, pairing_t pairing);
void default_pp_apply(element_ptr out, element_ptr in2, pairing_pp_t p);
void default_pp_clear(pairing_pp_t p);

#endif //__PBC_PAIRING_H__
