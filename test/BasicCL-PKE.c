#include <pbc.h>
#include <pbc_time.h>
int main(void){
	pairing_t pairing;
	element_t s,x,r;
	element_t P,Ppub,Qu,Du,Su,Xu,Yu,V;
	element_t T1,T2;
	double time1,time2;
	int byte;
	a_param_input(pairing);//�Զ�����a������ԵĲ���
	//��������ʼ��ΪZr�е�Ԫ��
	element_init_Zr(s,pairing);
	element_init_Zr(r,pairing);
	element_init_Zr(x,pairing);
	//��������ʼ��ΪG1�е�Ԫ��
	element_init_G1(P,pairing);
	element_init_G1(Ppub,pairing);
	element_init_G1(Qu,pairing);
	element_init_G1(Du,pairing);
	element_init_G1(Su,pairing);
	element_init_G1(Xu,pairing);
	element_init_G1(Yu,pairing);
	element_init_G1(V,pairing);
	//��������ʼ��ΪGT�е�Ԫ��
	element_init_GT(T1,pairing);
	element_init_GT(T2,pairing);
	//�ж����õ�����Ƿ�Ϊ�Գ���ԣ����������������ʾ��Ϣ
	if(!pairing_is_symmetric(pairing)){
		fprintf(stderr,"--����Ҫ�ǶԳ���Բ��ܹ�����--");
		exit(1);
	}
	
	printf("***************************����BasicCL-PKEģʽ*******************************\n");
	
	//ϵͳ�����׶�
	printf("------------------------ϵͳ�����׶�------------------------------\n");
	time1=get_time();
	//����KGC���ѡ�������Կs
	element_random(s);
	//����G1������ԪP
	element_random(P);
	element_mul_zn(Ppub,P,s);
	time2=get_time();
	element_printf("P=%B\n",P);
	element_printf("s=%B\n",s);
	element_printf("Ppub=%B\n",Ppub);
	//���ϵͳ�����׶����õ�ʱ��
	printf("ϵͳ�����׶����õ�ʱ��=%fs\n",time2-time1);

	//����˽Կ��ȡ�׶�
	printf("------------------------����˽Կ��ȡ�׶�------------------------------\n");
	time1=get_time();
	//ʹ�����ֵģ��ID��Ӧ��Qu
	element_random(Qu);
	element_mul_zn(Du,Qu,s);
	time2=get_time();
	element_printf("����˽ԿDu=%B\n",Du);
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
		element_random(r);//ѡȡ�����r
		element_mul_zn(V,P,r);//����V=rP
		pairing_apply(T1,Yu,Qu,pairing);//����T1=e(Yu,QU)
		element_pow_zn(T1,T1,r);//����T1=e(Yu,QU)^r
		element_printf("V=%B\n",V);
		element_printf("e(Yu,QU)^r=%B\n",T1);
	}else{
		printf("---------------���������Կ�ǲ���ȷ�ģ�-----------");
		exit(1);
	}
	time2=get_time();
	//������ܽ׶������ѵ�ʱ��
	printf("���ܽ׶�������ʱ��=%fs\n",time2-time1);

	//���ܽ׶�
	printf("------------------------���ܽ׶�------------------------------\n");
	time1=get_time();
	pairing_apply(T2,V,Su,pairing);//����T2=e(V,Su)
	time2=get_time();
	element_printf("T2=e(V,Su)=%B\n",T2);
	//������ܽ׶������ѵ�ʱ��
	printf("���ܽ׶�������ʱ��=%fs\n",time2-time1);
	//��V���ֽڳ���
	byte=element_length_in_bytes(V);
	//������Ϣ����Ϊ128�ֽ�
	printf("���ĵ��ֽڳ�����=%d\n",byte+128);

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
	element_clear(V);
	pairing_clear(pairing);
	return 0;

}