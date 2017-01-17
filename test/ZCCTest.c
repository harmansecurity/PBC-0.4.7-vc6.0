///////////////////////////////////////////////////////////////
//	文 件 名 :ZCCTest.c
//	文件功能 :基于配对的签名体制：ZCC
//	作    者 : LiangHui
//	创建时间 : 2016年12月10日
//	项目名称 ：PBCVC
//	操作系统 : 
//	备    注 :
//	历史记录： : 
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
	a_param_input(pairing);//自动输入类型A的配对的参数
	//将变量初始化为G1中的元素
	element_init_G1(P,pairing);
	element_init_G1(Y,pairing);
	element_init_G1(W,pairing);
	element_init_G1(temp,pairing);
	//将变量初始化为Zr中的元素
	element_init_Zr(h,pairing);
	element_init_Zr(x,pairing);
	//将变量初始化为GT中的元素
	element_init_GT(T1,pairing);
	element_init_GT(T2,pairing);
	//判断所用的配对是否为对称配对，不是则输出错误提示信息
	if(!pairing_is_symmetric(pairing)){
		fprintf(stderr,"--必须要是对称配对才能工作！--\n");
		exit(1);
	}

	printf("***************************进入ZSS模式*******************************\n");
	
	//密钥生成阶段
	printf("------------------------密钥生成阶段------------------------------\n");
	time1=get_time();
	element_random(x);//生成签名者随机选择的x
	element_random(P);//生成G1的生成元
	element_mul_zn(Y,P,x);//计算Y=xP
	time2=get_time();
	printf("---------------这个G1的生成元是：----------------\n");
	element_printf("P=%B\n",P);
	printf("---------------这个私钥是：--------------\n");
	element_printf("x=%B\n",x);
	printf("---------------这个公钥是:---------------\n");
	element_printf("Y=%B\n",Y);
	//输出密钥生成阶段所用的时间
	printf("密钥生成阶段所用的时间=%fs\n",time2-time1);

	//签名阶段
	printf("------------------------签名阶段------------------------------\n");
	time1=get_time();
	//模拟消息的Hash值，即h=H1(m)
	element_from_hash(h,"messageofsign",13);
	element_add(h,h,x);//计算h=h+x,即h=H1(m)+x
	element_invert(h,h);//计算h=1/h,即h=1/[H1(m)+x]
	element_mul_zn(W,P,h);//计算W=hP,即W={1/[H1(m)+x]}P
	time2=get_time();
	printf("----------------这个签名的消息是：-------------\n");
	element_printf("W=%B\n",W);//输出签名
	//签名阶段所用的时间
	printf("签名阶段所用的时间=%fs\n",time2-time1);

	//验证阶段
	printf("------------------------验证阶段------------------------------\n");
	time1=get_time();
	//模拟消息的Hash值，即h=H1(m)
	element_from_hash(h,"messageofsign",13);
	element_mul_zn(temp,P,h);//计算temp=hP,即temp=H1(m)P
	//计算temp=temp+Y,即temp=H1(m)P+Y
	element_add(temp,temp,Y);
	//计算T1=e(temp,W),即T1=e(H1(m)P+Y,W)
	pairing_apply(T1,temp,W,pairing);
	pairing_apply(T2,P,P,pairing);//计算T2=e(P,P)
	//判断签名是否有效
	if(!element_cmp(T1,T2))
		printf("-------------这个签名是有效的！-----------\n");
	else
		printf("-------------这个签名是无效的！------------\n");
	time2=get_time();
	element_printf("e(P,P)=%B\n",T2);
	element_printf("e(H1(m)P+Y,W)=%B\n",T1);
	//输出验证阶段所用的时间
	printf("输出验证阶段所用的时间=%fs\n",time2-time1);
	byte=element_length_in_bytes(W);//求W的字节长度
	//输出签名的字节长度
	printf("------------签名的字节长度=-----------%d\n",byte);
	//释放所占用的资源
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