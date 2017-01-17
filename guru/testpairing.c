
#include "pbc.h"

double get_time(void);

int main(int argc, char **argv)
{
    element_t g, h;
    element_t x1, x2;
    element_t zg, zh, z;
    pairing_t pairing;
	double t1,t2;

//    pairing_init_inp_str(pairing, stdin);
//	FILE *pf=fopen("a.param", "r");
//	FILE *pf=fopen("d159.param", "r");
//	FILE *pf=fopen("f.param", "r");

//    pairing_init_inp_str(pairing, stdin);
//    pairing_init_inp_str(pairing, pf);


//	a_param_input(pairing);
//	d_param_input(pairing);
//	f_param_input(pairing);

//	pairing_option_set(pairing, "method", "miller-affine");
//	pairing_option_set(pairing, "method", "shipsey-stange");


	if(argc<2){
		printf("usage: testpairing <a|d|f>");
		return -1;
	}

	switch(argv[1][0]){
	case 'a':
		a_param_input(pairing);
		break;
	case 'd':
		d_param_input(pairing);	
		break;
	case 'f':
		f_param_input(pairing);
		break;
	default:
		printf("usage: testpairing <a|d|f>");
		return -1;
	}

    element_init_G1(g, pairing);
    element_init_G1(zg, pairing);
    element_init_G2(h, pairing);
    element_init_G2(zh, pairing);
    element_init_GT(x1, pairing);
    element_init_GT(x2, pairing);
    element_init_Zr(z, pairing);
	
    element_random(g);
    element_random(h);
    //pairing->phi(g, h, pairing);
    element_printf("g = %B\n", g);
    element_printf("h = %B\n", h);

	t1=get_time();
    pairing_apply(x1, g, h, pairing);
	t2=get_time();
	printf("pairing time %f\n", t2-t1);

    element_printf("f(g, h) = %B\n", x1);

    element_random(z);
    element_printf("z = %B\n", z);

	t1=get_time();
    element_pow_zn(x1, x1, z);
	t2=get_time();
    element_printf("f(g, h)^z = %B\n", x1);
	printf("tate power time %f\n", t2-t1);

	t1=get_time();
    element_pow_zn(zg, g, z);
	t2=get_time();
	printf("point mul time %f\n", t2-t1);
    element_printf("g^z = %B\n", zg);
    pairing_apply(x2, zg, h, pairing);

    element_printf("f(g^z, h) = %B\n", x2);

    if (element_cmp(x1, x2)) {
	printf("BUG! pairing output mismatch\n");
	exit(1);
    }

    element_pow_zn(zh, h, z);
    element_printf("h^z = %B\n", zh);
    pairing_apply(x2, g, zh, pairing);
    element_printf("f(g, h^z) = %B\n", x2);

    if (element_cmp(x1, x2)) {
	printf("BUG! pairing output mismatch\n");
	exit(1);
    }

#if 0
    {
	int i;
	unsigned char *data;
	int len = element_length_in_bytes(h);
	printf("length_in_bytes(h) = %d\n", len);
	data = pbc_malloc(len);
	element_to_bytes(data, h);
	for (i=0; i<len; i++) {
	    printf(" %02X", data[i]);
	    if (15 == (i % 16)) printf("\n");
	}
	printf("\n");
	element_from_bytes(h, data);
	element_printf("from_bytes h = %B\n", h);
    }
#endif

    element_clear(g);
    element_clear(h);
    element_clear(x1);
    element_clear(x2);
    element_clear(zg);
    element_clear(zh);
    element_clear(z);
    pairing_clear(pairing);

//	getchar();
    return 0;
}
