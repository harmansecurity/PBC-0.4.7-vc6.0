#include <pbc.h>
#include <pbc_time.h>
#include <stdio.h>

int main(void){
	pairing_t pairing;
	element_t s,r;
	element_t P,Ppub,Su,Qu,V;
	element_t T1,T2;
	double time1,time2;
	int byte;
	a_param_input(pairing);

	element_init_Zr(s,pairing);
	element_init_Zr(r,pairing);

	element_init_G1(P,pairing);
	element_init_G1(Ppub,pairing);
	element_init_G1(Su,pairing);
	element_init_G1(Qu,pairing);
	element_init_G1(V,pairing);
	
	element_init_GT(T1,pairing);
	element_init_GT(T2,pairing);

	if(!pairing_is_symmetric(pairing)){
		fprintf(stderr,"only works with symmetric pairing\n");
	}
	printf("BasicIdent scheme\n");

	printf("Setup\n");
	time1=get_time();
	element_random(s);
	element_random(P);
	element_mul_zn(Ppub,P,s);
	time2=get_time();
	element_printf("P=%B\n",P);
	element_printf("s=%B\n",s);
	element_printf("Ppub=%B\n",Ppub);

	printf("the time of setup phase = %fs \n",time2-time1);
	printf("Extract\n");
	time1=get_time();
	element_from_hash(Qu,"IDu",3);
	element_mul_zn(Su,Qu,s);
	time2=get_time();
	element_printf("Qu=%B\n",Qu);
	element_printf("Su=%B\n",Su);
	printf("the time of extract phase = % fs \n",time2-time1);

	printf("Encrypt\n");
	 
	time1=get_time();
	element_random(r);
	element_mul_zn(V,P,r);
	pairing_apply(T1,Ppub,Qu,pairing);
	element_pow_zn(T1,T1,r);
	time2=get_time();
	element_printf("r= %B\n",r);
	element_printf("V= %B\n",V);
	element_printf("e(Ppub,Qu)^r = %B\n",T1);
	printf("the time of encrypt phase=%fs\n",time2-time1);

	printf("Decrypt\n");
	time1=get_time();
	pairing_apply(T2,V,Su,pairing);
	time2=get_time();
	element_printf("e(V,Su)=%B\n",T2);
	printf("the time of decrypt phase = %fs \n",time2-time1);

	byte=element_length_in_bytes(V);

	printf("the bytes of ciphertext = %d\n",byte+128);

	element_clear(P);
	element_clear(Ppub);
	element_clear(s);
	element_clear(Su);
	element_clear(Qu);
	element_clear(T2);
	element_clear(T1);
	element_clear(r);
	element_clear(V);
	pairing_clear(pairing);
	return 0;

}