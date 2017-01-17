///////////////////////////////////////////////////////////////
//	�� �� �� :ZCCTest.c
//	�ļ����� :������Ե�ǩ�����ƣ�ZCC
//	��    �� : LiangHui
//	����ʱ�� : 2016��12��10��
//	��Ŀ���� ��PBCVC
//	����ϵͳ : 
//	��    ע :
//	��ʷ��¼�� : 
///////////////////////////////////////////////////////////////
#include <pbc.h>
#include <pbc_time.h>
int main(void){
	pairing_t pairing;
	element_t P,Y,W,temp;
	element_t x,h;
	element_t T1,T2;
	int byte;
	double time1,time2;
	a_param_input(pairing);//�Զ���������A����ԵĲ���
	//��������ʼ��ΪG1�е�Ԫ��
	element_init_G1(P,pairing);
	element_init_G1(Y,pairing);
	element_init_G1(W,pairing);
	element_init_G1(temp,pairing);
	//��������ʼ��ΪZr�е�Ԫ��
	element_init_Zr(h,pairing);
	element_init_Zr(x,pairing);
	//��������ʼ��ΪGT�е�Ԫ��
	element_init_GT(T1,pairing);
	element_init_GT(T2,pairing);
	//�ж����õ�����Ƿ�Ϊ�Գ���ԣ����������������ʾ��Ϣ
	if(!pairing_is_symmetric(pairing)){
		fprintf(stderr,"--����Ҫ�ǶԳ���Բ��ܹ�����--\n");
		exit(1);
	}

	printf("***************************����ZSSģʽ*******************************\n");
	
	//��Կ���ɽ׶�
	printf("------------------------��Կ���ɽ׶�------------------------------\n");
	time1=get_time();
	element_random(x);//����ǩ�������ѡ���x
	element_random(P);//����G1������Ԫ
	element_mul_zn(Y,P,x);//����Y=xP
	time2=get_time();
	printf("---------------���G1������Ԫ�ǣ�----------------\n");
	element_printf("P=%B\n",P);
	printf("---------------���˽Կ�ǣ�--------------\n");
	element_printf("x=%B\n",x);
	printf("---------------�����Կ��:---------------\n");
	element_printf("Y=%B\n",Y);
	//�����Կ���ɽ׶����õ�ʱ��
	printf("��Կ���ɽ׶����õ�ʱ��=%fs\n",time2-time1);

	//ǩ���׶�
	printf("------------------------ǩ���׶�------------------------------\n");
	time1=get_time();
	//ģ����Ϣ��Hashֵ����h=H1(m)
	element_from_hash(h,"messageofsign",13);
	element_add(h,h,x);//����h=h+x,��h=H1(m)+x
	element_invert(h,h);//����h=1/h,��h=1/[H1(m)+x]
	element_mul_zn(W,P,h);//����W=hP,��W={1/[H1(m)+x]}P
	time2=get_time();
	printf("----------------���ǩ������Ϣ�ǣ�-------------\n");
	element_printf("W=%B\n",W);//���ǩ��
	//ǩ���׶����õ�ʱ��
	printf("ǩ���׶����õ�ʱ��=%fs\n",time2-time1);

	//��֤�׶�
	printf("------------------------��֤�׶�------------------------------\n");
	time1=get_time();
	//ģ����Ϣ��Hashֵ����h=H1(m)
	element_from_hash(h,"messageofsign",13);
	element_mul_zn(temp,P,h);//����temp=hP,��temp=H1(m)P
	//����temp=temp+Y,��temp=H1(m)P+Y
	element_add(temp,temp,Y);
	//����T1=e(temp,W),��T1=e(H1(m)P+Y,W)
	pairing_apply(T1,temp,W,pairing);
	pairing_apply(T2,P,P,pairing);//����T2=e(P,P)
	//�ж�ǩ���Ƿ���Ч
	if(!element_cmp(T1,T2))
		printf("-------------���ǩ������Ч�ģ�-----------\n");
	else
		printf("-------------���ǩ������Ч�ģ�------------\n");
	time2=get_time();
	element_printf("e(P,P)=%B\n",T2);
	element_printf("e(H1(m)P+Y,W)=%B\n",T1);
	//�����֤�׶����õ�ʱ��
	printf("�����֤�׶����õ�ʱ��=%fs\n",time2-time1);
	byte=element_length_in_bytes(W);//��W���ֽڳ���
	//���ǩ�����ֽڳ���
	printf("------------ǩ�����ֽڳ���=-----------%d\n",byte);
	//�ͷ���ռ�õ���Դ
	element_clear(P);
	element_clear(Y);
	element_clear(temp);
	element_clear(h);
	element_clear(W);
	element_clear(x);
	element_clear(T1);
	element_clear(T2);
	pairing_clear(pairing);
	return 0;
}