#include <pbc.h>
#include <pbc_time.h>
int main(void){
	pairing_t pairing;
	element_t P,U,V,W,temp1,temp3;
	element_t x,y,m,r,temp2;
	element_t T1,T2;
	int byte1,byte2;
	double time1,time2;
	a_param_input(pairing);//�Զ���������A����ԵĲ���
	//��������ʼ��ΪG1�е�Ԫ��
	element_init_G1(P,pairing);
	element_init_G1(U,pairing);
	element_init_G1(V,pairing);
	element_init_G1(W,pairing);
	element_init_G1(temp1,pairing);
	element_init_G1(temp3,pairing);
	//��������ʼ��ΪZr�е�Ԫ��
	element_init_Zr(y,pairing);
	element_init_Zr(x,pairing);
	element_init_Zr(m,pairing);
	element_init_Zr(r,pairing);
	element_init_Zr(temp2,pairing);
	//��������ʼ��ΪGT�е�Ԫ��
	element_init_GT(T1,pairing);
	element_init_GT(T2,pairing);
	//�ж����õ�����Ƿ�Ϊ�Գ���ԣ����������������ʾ��Ϣ
	if(!pairing_is_symmetric(pairing)){
		fprintf(stderr,"--����Ҫ�ǶԳ���Բ��ܹ�����--\n");
		exit(1);
	}
	
	printf("***************************����BBģʽ*******************************\n");
	
	//��Կ���ɽ׶�
	printf("------------------------��Կ���ɽ׶�------------------------------\n");
	time1=get_time();
	//����ǩ�������ѡ���˽Կx��y
	element_random(x);
	element_random(y);
	element_random(P);//����G1������ԪP
	element_mul_zn(U,P,x);//���㹫ԿU=xP
	element_mul_zn(V,P,y);//���㹫ԿV=yP
	time2=get_time();
	printf("--------------G1������Ԫ�ǣ�-----------\n");
	element_printf("P=%B\n",P);
	printf("---------------���˽Կ�ǣ�-------------\n");
	element_printf("x=%B\n",x);
	element_printf("y=%B\n",y);
	printf("---------------�����Կ�ǣ�-------------\n");
	element_printf("U=%B\n",U);
	element_printf("V=%B\n",V);
	//�����Կ���ɽ׶����õ�ʱ��
	printf("��Կ���ɽ׶����õ�ʱ��=%fs\n",time2-time1);

	//ǩ���׶�
	printf("------------------------ǩ���׶�------------------------------\n");
	time1=get_time();
	element_random(m);//����Ҫǩ������Ϣm
	element_random(r);//����ǩ����ѡ��������r
	element_mul(temp2,y,r);//����temp2=ry
	//����temp2=temp2+m,��temp2=m+ry
	element_add(temp2,temp2,m);
	//����temp2=temp2+x,��temp2=x+m+ry
	element_add(temp2,temp2,x);
	//��x+m+ry��Ϊ0������x+m+ry����Ԫ1/(x+m+ry)
	if (!element_is0(temp2)){
		element_invert(temp2,temp2);//��temp2=1/(x+m+ry)
		//����W=temp2P
		element_mul_zn(W,P,temp2);
	}else{
		//����ѡ�������r
		printf("ѡ����һ�������r!");
		exit(1);
	}
	time2=get_time();
	element_printf("-----------�����Ϣm=-----------%B\n",m);
	printf("-----------���ǩ������Ϣ�ǣ�-----------\n");
	element_printf("W=%B\n",W);
	element_printf("r=%B\n",r);
	//���ǩ���׶����õ�ʱ��
	printf("ǩ���׶����õ�ʱ��=%fs\n",time2-time1);

	//��֤�׶�
	printf("------------------------��֤�׶�------------------------------\n");
	time1=get_time();
	element_mul_zn(temp1,V,r);//����temp1=rV
	element_mul_zn(temp3,P,m);//����temp3=mP
	//����temp1=temp1+temp3����temp1=mP+rV
	element_add(temp1,temp1,temp3);
	//����temp1=temp1+U,��temp1=U+mP+rV
	element_add(temp1,temp1,U);
	//����T1=e(temp1,W),��T1=e(U+mP+rV,W)
	pairing_apply(T1,temp1,W,pairing);
	pairing_apply(T2,P,P,pairing);//����T2=e(P,P)
	//�ж�ǩ���Ƿ���Ч
	if(!element_cmp(T1,T2))
		printf("--------------���ǩ������Ч�ģ�-------------\n");
	else
		printf("-------------���ǩ��������Ч�ģ�------------\n");
	time2=get_time();
	element_printf("e(P,P)=%B\n",T2);
	element_printf("e(U+mP+rV,w)=%B\n",T1);
	//�����֤�׶����õ�ʱ��
	printf("��֤�׶����õ�ʱ��=%fs\n",time2-time1);
	byte1=element_length_in_bytes(W);//��W���ֽڳ���
	byte2=element_length_in_bytes(r);//��r���ֽڳ���
	//���ǩ�����ֽڳ���
	printf("���ǩ�����ֽڳ���=%d\n",byte1+byte2);
	//�ͷ���ռ�õ���Դ
	element_clear(P);
	element_clear(U);
	element_clear(temp1);
	element_clear(V);
	element_clear(W);
	element_clear(temp3);
	element_clear(x);
	element_clear(y);
	element_clear(m);
	element_clear(r);
	element_clear(temp2);
	element_clear(T1);
	element_clear(T2);
	pairing_clear(pairing);
	return 0;
}