#include <pbc.h>
#include <pbc_time.h>
#include <string.h>

int main(void){
	pairing_t pairing;
	element_t P,P1,P2,u,ui,sP2,temp,S1,S2,C2,C3;
	element_t s,r,t;
	element_t m,C1,T1,T2,m1;
	//����ptrΪָ�����
	element_ptr ptr;
	//����LΪ��̬�������
	darray_t L;
	int i,n,count;
	int byte1,byte2,byte3;
	double time1,time2;
	//����һ������Ϊ50���ַ�������
	char string[51];
	a_param_input(pairing);
	//��ʼ����̬�������L
	darray_init(L);
	//��������ʼ��ΪZr�е�Ԫ��
	element_init_Zr(s,pairing);
	element_init_Zr(r,pairing);
	element_init_Zr(t,pairing);
	//��������ʼ��ΪG1�е�Ԫ��
	element_init_G1(P,pairing);
	element_init_G1(P1,pairing);
	element_init_G1(P2,pairing);
	element_init_G1(u,pairing);
	element_init_G1(ui,pairing);
	element_init_G1(sP2,pairing);
	element_init_G1(temp,pairing);
	element_init_G1(S1,pairing);
	element_init_G1(S2,pairing);
	element_init_G1(C2,pairing);
	element_init_G1(C3,pairing);
	//��������ʼ��ΪGT�е�Ԫ��
	element_init_GT(T1,pairing);
	element_init_GT(T2,pairing);
	element_init_GT(m,pairing);
	element_init_GT(C1,pairing);
	element_init_GT(m1,pairing);
	//�ж����õ�����Ƿ�Ϊ�Գ���ԣ����������������ʾ��Ϣ
	if(!pairing_is_symmetric(pairing)){
		fprintf(stderr,"--����Ҫ�ǶԳ���Բ��ܹ�����--");
		exit(1);
	}

	printf("***************************����watersģʽ*******************************\n");

	//ϵͳ�����׶�
	printf("------------------------ϵͳ�����׶�------------------------------\n");
	time1=get_time();
	//����PKG������ֵs
	element_random(s);
	//����G1������ԪP
	element_random(P);
	//����P1=sP
	element_mul_zn(P1,P,s);
	//����PKG���ѡ���P2��u
	element_random(P2);
	element_random(u);
	//����PKGѡ���ʸ��U���洢�ڶ�̬����L�У�������賤��Ϊ50
	for(i=0;i<50;i++){
		element_random(ui);
		darray_append(L,ui);//��̬������׷��Ԫ�ر���ui
	}
	n=darray_count(L);//���㶯̬������Ԫ�صĸ���
	element_mul_zn(sP2,P2,s);
	time2=get_time();
	element_printf("P=%B\n",P);
	element_printf("P1=%B\n",P1);
	element_printf("u=%B\n",u);
	element_printf("����ԿsP2=%B\n",sP2);
	element_printf("���U�ĳ���=%d\n",n);
	//���ϵͳ�����׶����õ�ʱ��
	printf("ϵͳ�����׶����õ�ʱ��=%fs\n",time2-time1);

	//��Կ��ȡ�׶�
	printf("------------------------��Կ��ȡ�׶�------------------------------\n");
	printf("�����û��������Ϣ���ö����Ʊ�ʾ������50���أ�\n");
	scanf("%s",&string);
	time1=get_time();
	//����ݵĶ����Ƴ���
	count=strlen(string);
	printf("count=%d\n",count);
	//������е�һ��������λΪ1��λ��i,����ʸ��U�е�ui����ָ��ptr
	for(i=0;i<=count;i++){
		if(string[i]='1'){
			ptr=L->item[i]; //https://zhidao.baidu.com/question/143562807.html
			break;
		}
	}
	i=i+1;
	for(i;i<=count;i++){//�����������������λΪ1��λ��
		if(string[i]=='i')
			element_add(ptr,ptr,L->item[i]);//ִ���������
	}
	//����temp=u+ui
	element_add(temp,u,ptr);
	element_random(r);//����PKGѡȡ�������r
	element_mul_zn(S1,temp,r);
	element_add(S1,S1,sP2);
	element_mul_zn(S2,P,r);
	time2=get_time();
	element_printf("r=%B\n",r);
	//���˽Կ
	printf("�û���ݵ�˽Կ %s is:\n",string);
	element_printf("S1=%B\n",S1);
	element_printf("S2=%B\n",S2);
	//�����Կ��ȡ�׶����õ�ʱ��
	printf("��Կ��ȡ�׶����õ�ʱ��=%fs\n",time2-time1);

	//���ܽ׶�
	printf("------------------------���ܽ׶�------------------------------\n");
	time1=get_time();
	//����Ҫ���ܵ���Ϣm
	element_random(m);
	//���ɼ���ʹ�õ������
	element_random(t);
	pairing_apply(C1,P1,P2,pairing);
	element_pow_zn(C1,C1,t);
	element_mul(C1,C1,m);
	element_mul_zn(C2,P,t);
	element_mul_zn(C3,temp,t);
	time2=get_time();
	element_printf("�����Ϣm�� %B\n",m);
	printf("���m�������ǣ�\n");
	element_printf("C1=%B\n",C1);
	element_printf("C2=%B\n",C2);
	element_printf("C3=%B\n",C3);
	//������ܽ׶����õ�ʱ��
	printf("���ܽ׶����õ�ʱ��=%fs\n",time2-time1);

	//���ܽ׶�
	printf("------------------------���ܽ׶�------------------------------\n");
	time1=get_time();
	pairing_apply(T1,C3,S2,pairing);
	pairing_apply(T2,C2,S1,pairing);
	//��T2����Ԫ
	element_invert(T2,T2);
	element_mul(T2,T2,T1);
	element_mul(m1,C1,T2);
	time2=get_time();
	printf("����ָ�����Ϣ�ǣ�\n");
	element_printf("m1=%B\n",m1);
	//������ܽ׶����õ�ʱ��
	printf("���ܽ׶����õ�ʱ��=%fs\n",time2-time1);
	byte1=element_length_in_bytes(C1);
	byte2=element_length_in_bytes(C2);
	byte3=element_length_in_bytes(C3);
	//�������C�ֽڵĳ���
	printf("���ĵ��ֽڳ���=%d\n",byte1+byte2+byte3);

	//����������Ԫ�ر�������̬�����������Ա������ͷ���Դ
	element_clear(P);
	element_clear(P1);
	element_clear(P2);
	element_clear(u);
	element_clear(ui);
	element_clear(sP2);
	element_clear(temp);
	element_clear(S1);
	element_clear(S2);
	element_clear(C2);
	element_clear(C3);
	element_clear(s);
	element_clear(r);
	element_clear(t);
	element_clear(m);
	element_clear(C1);
	element_clear(T1);
	element_clear(T2);
	element_clear(m1);
	element_clear(pairing);
	return 0;
}