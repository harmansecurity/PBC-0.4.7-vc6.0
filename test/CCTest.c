#include <pbc.h>
#include <pbc_time.h>

int main(void){
	pairing_t pairing;
	element_t P,Ppub,Qu,Su,V,W,temp;
	element_t s,r,h,temp1;
	element_t T1,T2;
	double time1,time2;
	int byte1,byte2;
	a_param_input(pairing);//�Զ���������ΪA����ԵĲ���
	//��������ʼ��ΪG1�е�Ԫ��
	element_init_G1(P,pairing);
	element_init_G1(Ppub,pairing);
	element_init_G1(Qu,pairing);
	element_init_G1(Su,pairing);
	element_init_G1(V,pairing);
	element_init_G1(W,pairing);
	element_init_G1(temp,pairing);
	//��������ʼ��ΪZr�е�Ԫ��
	element_init_Zr(s,pairing);
	element_init_Zr(r,pairing);
	element_init_Zr(h,pairing);
	element_init_Zr(temp1,pairing);
	//��������ʼ��ΪGT�е�Ԫ��
	element_init_GT(T1,pairing);
	element_init_GT(T2,pairing);
	//�ж����õ�����Ƿ�Ϊ�Գ���ԣ����������������ʾ��Ϣ
	if(!pairing_is_symmetric(pairing)){
		fprintf(stderr,"--����Ҫ�ǶԳ���Բ��ܹ�����--\n");
		exit(1);
	}
	
	printf("***************************����CCģʽ*******************************\n");
	
	//ϵͳ�����׶�
	printf("------------------------ϵͳ�����׶�------------------------------\n");
	time1=get_time();
	element_random(s);//�������ѡ�������Կ
	element_random(P);//����G1������ԪP
	element_mul_zn(Ppub,P,s);//����Ppub=sP
	time2=get_time();
	element_printf("P=%B\n",P);
	element_printf("Ppub=%B\n",Ppub);
	//���ϵͳ�����׶����õ�ʱ��
	printf("ϵͳ�����׶����õ�ʱ��=%fs\n",time2-time1);

	//��Կ��ȡ�׶�
	printf("------------------------��Կ��ȡ�׶�------------------------------\n");
	time1=get_time();
	//ģ��Qu=H1(IDu),�˴�������������Hashֵ
	element_from_hash(Qu,"IDu",3);
	element_mul_zn(Su,Qu,s);//����˽ԿSu=sQu
	time2=get_time();
	element_printf("Qu=%B\n",Qu);
	element_printf("Su=%B\n",Su);
	//�����Կ��ȡ�׶����õ�ʱ��
	printf("��Կ��ȡ�׶����õ�ʱ��=%fs\n",time2-time1);

	//ǩ���׶�
	printf("------------------------ǩ���׶�------------------------------\n");
	time1=get_time();
	element_random(r);//ѡȡ�����r
	element_mul_zn(V,Qu,r);//����V=rQu
	element_random(h);//ѡȡ���ֵ��ģ��h=H2(m,V)
	element_add(temp1,h,r);//����temp1=r+h
	element_mul_zn(W,Su,temp1);//����W=temp1Su����W=(r+h)Su
	time2=get_time();
	printf("------------���ǩ������Ϣ�ǣ�-----------\n");
	element_printf("V=%B\n",V);
	element_printf("W=%B\n",W);
	//���ǩ���׶����õ�ʱ��
	printf("ǩ���׶����õ�ʱ��=%fs\n",time2-time1);

	//��֤�׶�
	printf("------------------------��֤�׶�------------------------------\n");
	time1=get_time();
	pairing_apply(T1,P,W,pairing);//����T1=e(P,W)
	element_mul_zn(temp,Qu,h);//����temp=hQu
	//����temp=temp+V,��temp=V+hQu
	element_add(temp,temp,V);
	//����T2=e(Ppub,temp),��T2=e(Ppub,V+hQu)
	pairing_apply(T2,Ppub,temp,pairing);
	//�ж�ǩ���Ƿ���Ч
	if(!element_cmp(T1,T2))
		printf("--------------���ǩ������Ч�ģ�-------------\n");
	else
		printf("-------------���ǩ��������Ч�ģ�------------\n");
	time2=get_time();
	element_printf("e(P,W)=%B\n",T1);
	element_printf("e(Ppub,V+hQu)=%B\n",T2);
	//�����֤�׶����õ�ʱ��
	printf("��֤�׶����õ�ʱ��=%fs\n",time2-time1);
	//��W���ֽڳ���
	byte1=element_length_in_bytes(W);
	//��V���ֽڳ���
    byte2=element_length_in_bytes(V);
	//���ǩ�����ֽڳ���
	printf("ǩ�����ֽڳ���=%d\n",byte1+byte2);
	//�ͷ���ռ�õ���Դ
	element_clear(P);
	element_clear(Ppub);                    
	element_clear(Qu);
	element_clear(Su);
	element_clear(W);
	element_clear(V);
	element_clear(temp);
	element_clear(s);
	element_clear(h);
	element_clear(r);
	element_clear(temp1);
	element_clear(T1);
	element_clear(T2);
	pairing_clear(pairing);
	return 0;
}