#include<pbc.h>
#include <pbc_time.h>

int main(void){
	pairing_t pairing;
	element_t s,x,r;
	element_t P,Ppub,Qu,Du,Su,Xu,Yu,V,V1;
	element_t T1,T2,alpher;		
	double time1,time2;
	int byte1,byte2;
	//����tΪGMP�е�����
	mpz_t t;
	a_param_input(pairing);//�Զ���������A����ԵĲ���
	mpz_init(t);//��ʼ������t
	//��������ʼ��ΪZr�е�Ԫ��
	element_init_Zr(s,pairing);
	element_init_Zr(x,pairing);
	element_init_Zr(r,pairing);
	//��������ʼ��ΪG1�е�Ԫ��
	element_init_G1(P,pairing);
	element_init_G1(Ppub,pairing);
	element_init_G1(Qu,pairing);
	element_init_G1(Du,pairing);
	element_init_G1(Su,pairing);
	element_init_G1(Xu,pairing);
	element_init_G1(Yu,pairing);
	element_init_G1(V,pairing);
	element_init_G1(V1,pairing);
	//��������ʼ��ΪGT�е�Ԫ��
	element_init_GT(T1,pairing);
	element_init_GT(T2,pairing);
	element_init_GT(alpher,pairing);
	//�ж����õ�����Ƿ�Ϊ�Գ���ԣ����������������ʾ��Ϣ
	if(!pairing_is_symmetric(pairing)){
		fprintf(stderr,"--����Ҫ�ǶԳ���Բ��ܹ�����--\n");
		exit(1);
	}
	
	printf("***************************����FullCL-PKEģʽ*******************************\n");
	
	//ϵͳ�����׶�
	printf("------------------------ϵͳ�����׶�------------------------------\n");
	time1=get_time();
	//����KGC���ѡ�������Կs
	element_random(s);
	//����G1������ԪP
	element_random(P);
	element_mul_zn(Ppub,P,s);//����Ppub=sP
	time2=get_time();
	element_printf("P=%B\n",P);
	element_printf("s=%B\n",s);
	element_printf("Ppub=%B\n",Ppub);
	//���ϵͳ�����׶����õ�ʱ��
	printf("ϵͳ�����׶����õ�ʱ��=%fs\n",time2-time1);
	
	//����˽Կ��ȡ�׶�
	printf("------------------------����˽Կ��ȡ�׶�------------------------------\n");
	time1=get_time();
	element_random(Qu);//ʹ�����ֵģ��IDU��Ӧ��Qu
	element_mul_zn(Du,Qu,s);//���㲿��˽ԿDu=sQu
	time2=get_time();
	//�������˽Կ��ȡ�׶����õ�ʱ��
	printf("����˽Կ��ȡ�׶�������ʱ��=%fs\n",time2-time1);
	
	//��������ֵ�׶�
	printf("------------------------��������ֵ�׶�------------------------------\n");
	time1=get_time();
	element_random(x);
	time2=get_time();
	element_printf("����ֵ��=%B\n",x);
	//�����������ֵ�׶����õ�ʱ��
	printf("��������ֵ�׶�������ʱ��=%fs\n",time2-time1);
	
	//����˽Կֵ�׶�
	printf("------------------------����˽Կֵ�׶�------------------------------\n");
	time1=get_time();
	element_mul_zn(Su,Du,x);//������ȫ˽ԿSu=xDu
	time2=get_time();
	element_printf("��ȫ˽ԿSu��=%B\n",Su);
	//�������˽Կ�׶����õ�ʱ��
	printf("����˽Կ�׶�������ʱ��=%fs\n",time2-time1);
	
	//���ù�Կ�׶�
	printf("------------------------���ù�Կ�׶�------------------------------\n");
	time1=get_time();
	element_mul_zn(Xu,P,x);//����Xu=xP
	element_mul_zn(Yu,Ppub,x);//����Yu=xPpub
	time2=get_time();
	printf("-----------------��Կ�ǣ�---------------\n");
	element_printf("Xu=%B\n",Xu);
	element_printf("Yu=%B\n",Yu);
	//������ù�Կ�׶����õ�ʱ��
	printf("���ù�Կ�׶�������ʱ��=%fs\n",time2-time1);
	
	//���ܽ׶�
	printf("------------------------���ܽ׶�------------------------------\n");
	time1=get_time();
	pairing_apply(T1,Xu,Ppub,pairing);//����T1=e(Xu,Ppub)
	pairing_apply(T2,Yu,P,pairing);
	//�жϹ�Կ�Ƿ���ȷ
	if(!element_cmp(T1,T2)){
		element_from_hash(r,"message",7);//ģ����Ϣ�Ĺ�ϣֵ
		element_random(alpher);//����һ�������alpher
		element_to_mpz(t,alpher);
		element_mul_mpz(r,r,t);//ģ��H3(a,m)=H3(m)*mpz(a)
		element_mul_zn(V,P,r);//����V=rp
		pairing_apply(T1,Yu,Qu,pairing);
		element_pow_zn(T1,T1,r);
		element_printf("r=%B\n",r);
		element_printf("V=%B\n",V);
		element_printf("e(Yu,Qu)^r=%B\n",T1);
	}else{
		printf("���������Կ�ǲ���ȷ�ģ�\n");
		exit(1);
	}
	time2=get_time();
	//������ܽ׶����õ�ʱ��
	printf("���ܽ׶�������ʱ��=%fs\n",time2-time1);

	//���ܽ׶�
	printf("------------------------���ܽ׶�------------------------------\n");
	time1=get_time();
	pairing_apply(T2,V,Su,pairing);//����T2=e(V,Su)
	element_printf("e(V,Su)=%B\n",T2);
	element_from_hash(r,"message",7);
	element_to_mpz(t,alpher);
	element_mul_mpz(r,r,t);//ģ��H3(a,m)=H3(m)*mpz(a)
	element_mul_zn(V1,P,r);//����V1=rP
	if(!element_cmp(V,V1))//�жϽ����Ƿ���ȷ
		printf("-----------------------��������ȷ��!------------------\n");
	else
		printf("------------����!�����ǲ���ȡ��!--------------\n");
	time2=get_time();
	//������ܽ׶������ѵ�ʱ��
	printf("���ܽ׶�������ʱ��=%fs\n",time2-time1);
	//��V���ֽڳ���
	byte1=element_length_in_bytes(V);
	//��a���ֽڳ��ȣ�������Ϣ�ĳ�����ͬ
	byte2=element_length_in_bytes(alpher);
	//�������c���ֽڳ���
	printf("���ĵ��ֽڳ�����=%d\n",byte1+byte2+byte2);
	
	//�ͷ�ռ�õ���Դ
	element_clear(P);
	element_clear(Ppub);
	element_clear(s);
	element_clear(Qu);
	element_clear(Du);
	element_clear(Xu);
	element_clear(Su);
	element_clear(Yu);
	element_clear(T1);
	element_clear(T2);
	element_clear(r);
	element_clear(x);
	element_clear(alpher);

	element_clear(V);
	element_clear(V1);
	pairing_clear(pairing);
	return 0;
}