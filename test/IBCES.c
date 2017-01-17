#include <pbc.h>
#include <pbc_time.h>
int main(void){
	pairing_t pairing;
	element_t s,r,h,temp1;
	element_t P,Ppub,Su,Qu,V,X,Z,temp2;
	element_t T1,T2;
	int byte1,byte2;
	double time1,time2;
	a_param_input(pairing);
	//��������ʼ��ΪZr�е�Ԫ��
	element_init_Zr(s,pairing);
	element_init_Zr(r,pairing);
	element_init_Zr(h,pairing);
	element_init_Zr(temp1,pairing);
	//��������ʼ��ΪG1�е�Ԫ��
	element_init_G1(P,pairing);
	element_init_G1(Ppub,pairing);
	element_init_G1(Qu,pairing);
    element_init_G1(Su,pairing);
	element_init_G1(V,pairing);
	element_init_G1(X,pairing);
	element_init_G1(Z,pairing);
	element_init_G1(temp2,pairing);
	//��������ʼ��ΪGT�е�Ԫ��
	element_init_GT(T1,pairing);
	element_init_GT(T2,pairing);
	//�ж����õ�����Ƿ�Ϊ�Գ���ԣ������ǣ�������������ʾ��Ϣ��ֹͣ����
	if(!pairing_is_symmetric(pairing)){
		fprintf(stderr,"ֻ�ܶԶԳƵ���ԲŻ���Ч\n");
		exit(1);
	}
	printf("***************************����IBCESģʽ*******************************\n");
	
	//ϵͳ�����׶�
	printf("------------------------ϵͳ�����׶�------------------------------\n");
	time1=get_time();
	element_random(s);//����PKG���ѡ�������Կs
	element_random(P);//����G1������ԪP
	element_mul_zn(Ppub,P,s);//����Ppub=sP
	time2=get_time();
	element_printf("P=%B\n",P);
	element_printf("s=%B\n",s);
	element_printf("Ppub=%B\n",Ppub);
	//���ϵͳ�����׶����õ�ʱ��
	printf("ϵͳ�����׶����õ�ʱ��=%fs\n",time2-time1);
	
	//��Կ��ȡ�׶�
	printf("------------------------��Կ��ȡ�׶�------------------------------\n");
	time1=get_time();
	//ʹ�����ֵ��ģ��ID��Ӧ��H1(IDU)��Ҳ������element_from_hash(Qu,"IDu",3)
	element_random(Qu);
	element_mul_zn(Su,Qu,s);//����˽ԿSu=sQu
	time2=get_time();
	element_printf("Qu=%B\n",Qu);
	element_printf("Su=%B\n",Su);
	//�����Կ��ȡ�׶����õ�ʱ��
	printf("��Կ��ȡ�׶����õ�ʱ��=%fs\n",time2-time1);

	//���ܽ׶�
	printf("------------------------���ܽ׶�------------------------------\n");
	time1=get_time();
	element_random(r);//����һ�����r��ģ��H3(a,m)��ֵ
	element_mul_zn(V,P,r);//����V=rP
	pairing_apply(T1,Ppub,Qu,pairing);//����T1=e(Ppub,Qu)
	element_pow_zn(T1,T1,r);//T1=e(Ppub,Qu)^r
	time2=get_time();
	element_printf("r=%B\n",r);
	element_printf("V=%B\n",V);
	element_printf("e(Ppub,Qu)^r=%B\n",T1);
	//������ܽ׶����õ�ʱ��
	printf("���ܽ׶����õ�ʱ��=%fs\n",time2-time1);

	//���ܽ׶�
	printf("------------------------���ܽ׶�------------------------------\n");
	time1=get_time();
	pairing_apply(T2,V,Su,pairing);//����T2=e(V,Su)
	element_mul_zn(temp2,P,r);//����temp2=rP
	if(!element_cmp(V,temp2))//�жϽ����Ƿ�ɹ�
		printf("------------�����������ȷ�ģ�---------------\n");
	else
		printf("----------������������ǲ���ȷ�ģ�-----------\n");
	time2=get_time();
	//������ܽ׶����õ�ʱ��
	printf("���ܽ׶����õ�ʱ��=%fs\n",time2-time1);

	//ǩ���׶�
	printf("------------------------ǩ���׶�------------------------------\n");
	time1=get_time();
	element_mul_zn(X,Qu,r);//����X=rQu
	element_random(h);
	element_add(temp1,r,h);
	element_mul_zn(Z,Su,temp1);
	time2=get_time();
	printf("--------���ǩ������Ϣ�ǣ�---------\n");
	element_printf("X=%B\n",X);
	element_printf("Z=%B\n",Z);
	//���ǩ���׶����õ�ʱ��
	printf("ǩ���׶����õ�ʱ��=%fs\n",time2-time1);

	//��֤�׶�
	printf("------------------------��֤�׶�------------------------------\n");
	time1=get_time();
	pairing_apply(T1,P,Z,pairing);
	element_mul_zn(temp2,Qu,h);//����temp2=hQu
	element_add(temp2,temp2,X);
	pairing_apply(T2,Ppub,temp2,pairing);
	//�ж�ǩ���Ƿ���Ч
	if(element_cmp(T1,T2))
		printf("-----------���ǩ������Ч��!---------\n");
	else
		printf("-----------���ǩ������Ч��!---------\n");
	time2=get_time();
	element_printf("e(P,Z)=%B\n",T1);
	element_printf("e(Ppub,X+hQu)=%B\n",T2);
	//�����֤�׶����õ�ʱ��
	printf("��֤�׶����õ�ʱ��=%fs\n",time2-time1);
	byte1=element_length_in_bytes(V);
	printf("���ĵ��ֽڳ���=%d\n",byte1+128+128);
	byte1=element_length_in_bytes(X);
	byte2=element_length_in_bytes(Z);
	//���ǩ�����ֽڳ���
	printf("ǩ�����ֽڳ���=%d\n",byte1+byte2);
	//�ͷ�ռ�õ���Դ
	element_clear(P);
	element_clear(Su);
	element_clear(Ppub);
	element_clear(Qu);
	element_clear(V);
	element_clear(temp1);
	element_clear(temp2);
	element_clear(X);
	element_clear(Z);
	element_clear(s);
	element_clear(r);
	element_clear(h);
	element_clear(T1);
	element_clear(T2);
	pairing_clear(pairing);
	return 0;

}