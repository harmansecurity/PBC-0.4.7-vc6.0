#include <pbc.h>
#include <pbc_time.h>
int main(void){
	pairing_t pairing;
	element_t P,Y,M,W;
	element_t x;
	element_t T1,T2;
	double time1,time2;
	int byte;
	a_param_input(pairing);//�Զ���������A����ԵĲ���
	//��������ʼ��ΪG1�е�Ԫ��
	element_init_G1(P,pairing);
	element_init_G1(Y,pairing);
	element_init_G1(M,pairing);
	element_init_G1(W,pairing);
	//��������ʼ��ΪZr�е�Ԫ��
	element_init_Zr(x,pairing);
	//��������ʼ��ΪGT�е�Ԫ��
	element_init_GT(T1,pairing);
	element_init_GT(T2,pairing);
	//�ж����õ�����Ƿ�Ϊ�Գ���ԣ����������������ʾ��Ϣ
	if(!pairing_is_symmetric(pairing)){
		fprintf(stderr,"--����Ҫ�ǶԳ���Բ��ܹ�����--\n");
		exit(1);
	}
	
	printf("***************************����BLSģʽ*******************************\n");
	
	//ϵͳ�����׶�
	printf("------------------------��Կ���ɽ׶�------------------------------\n");
	time1=get_time();
	//����ǩ�������ѡ���x
	element_random(x);
	//����G1������Ԫ
	element_random(P);
	element_mul_zn(Y,P,x);//����Y=xp
	time2=get_time();
	printf("-------------G1������Ԫ�ǣ�-----------\n");
	element_printf("P=%B\n",P);
	printf("-------------���˽Կ�ǣ�-------------\n");
	element_printf("x=%B\n",x);
	printf("-------------�����Կ�ǣ�-------------\n");
	element_printf("Y=%B\n",Y);
	//�����Կ���ɽ׶����õ�ʱ��
	printf("��Կ���ɽ׶����õ�ʱ��=%fs\n",time2-time1);
	
	//ǩ���׶�
	printf("------------------------ǩ���׶�------------------------------\n");
	time1=get_time();
	//ģ����Ϣ��Hashֵ����M=H1(m)
	element_from_hash(M,"messageofsign",13);
	element_mul_zn(W,M,x);//����W=XM
	time2=get_time();
	//�����Ϣ��Hashֵ��������ģ��ֵ
	element_printf("H1(m)=%B\n",M);
	printf("-------------------���ǩ������Ϣ�ǣ�----------------\n");
	element_printf("W=%B\n",W);//���ǩ��
	//���ǩ���׶����õ�ʱ��
	printf("ǩ���׶����õ�ʱ��=%fs\n",time2-time1);

	//��֤�׶�
	printf("------------------------��֤�׶�------------------------------\n");
	time1=get_time();
	pairing_apply(T1,P,W,pairing);
	//ģ����Ϣ��Hashֵ����M=H1(m)
	element_from_hash(M,"messageofsign",13);
	pairing_apply(T2,Y,M,pairing);
	//�ж�ǩ���Ƿ���Ч
	if(!element_cmp(T1,T2))
		printf("���ǩ������Ч�ģ�\n");
	else
		printf("���ǩ������Ч�ģ�\n");
	time2=get_time();
	element_printf("e(P,W)=%B\n",T1);
	element_printf("e(Y,H1(m))=%B\n",T2);
	//�����֤�׶����õ�ʱ��
	printf("��֤�׶����õ�ʱ��=%fs\n",time2-time1);
	byte=element_length_in_bytes(W);//��W���ֽڳ���
	//���ǩ�����ֽڳ���
	printf("-------------���ǩ�����ֽڳ���=-----------------%d\n",byte);
	//�ͷ���ռ�õ���Դ
	element_clear(P);
	element_clear(Y);
	element_clear(M);
	element_clear(W);
	element_clear(x);
	element_clear(T1);
	element_clear(T2);
	pairing_clear(pairing);
	return 0;
}