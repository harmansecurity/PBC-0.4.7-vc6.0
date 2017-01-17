#include <pbc.h>
#include <pbc_time.h>
#include <string.h>

int main(void){
	pairing_t pairing;
	element_t P,P1,P2,u,ui,sP2,temp,S1,S2,C2,C3;
	element_t s,r,t;
	element_t m,C1,T1,T2,m1;
	//定义ptr为指针变量
	element_ptr ptr;
	//声明L为动态数组变量
	darray_t L;
	int i,n,count;
	int byte1,byte2,byte3;
	double time1,time2;
	//声明一个长度为50的字符串数组
	char string[51];
	a_param_input(pairing);
	//初始化动态数组变量L
	darray_init(L);
	//将变量初始化为Zr中的元素
	element_init_Zr(s,pairing);
	element_init_Zr(r,pairing);
	element_init_Zr(t,pairing);
	//将变量初始化为G1中的元素
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
	//将变量初始化为GT中的元素
	element_init_GT(T1,pairing);
	element_init_GT(T2,pairing);
	element_init_GT(m,pairing);
	element_init_GT(C1,pairing);
	element_init_GT(m1,pairing);
	//判断所用的配对是否为对称配对，不是则输出错误提示信息
	if(!pairing_is_symmetric(pairing)){
		fprintf(stderr,"--必须要是对称配对才能工作！--");
		exit(1);
	}

	printf("***************************进入waters模式*******************************\n");

	//系统建立阶段
	printf("------------------------系统建立阶段------------------------------\n");
	time1=get_time();
	//生成PKG的秘密值s
	element_random(s);
	//生成G1的生成元P
	element_random(P);
	//计算P1=sP
	element_mul_zn(P1,P,s);
	//生成PKG随机选择的P2和u
	element_random(P2);
	element_random(u);
	//生成PKG选择的矢量U，存储在动态数组L中，这里假设长度为50
	for(i=0;i<50;i++){
		element_random(ui);
		darray_append(L,ui);//向动态数组中追加元素变量ui
	}
	n=darray_count(L);//计算动态数组中元素的个数
	element_mul_zn(sP2,P2,s);
	time2=get_time();
	element_printf("P=%B\n",P);
	element_printf("P1=%B\n",P1);
	element_printf("u=%B\n",u);
	element_printf("主密钥sP2=%B\n",sP2);
	element_printf("这个U的长度=%d\n",n);
	//输出系统建立阶段所用的时间
	printf("系统建立阶段所用的时间=%fs\n",time2-time1);

	//密钥提取阶段
	printf("------------------------密钥提取阶段------------------------------\n");
	printf("输入用户的身份信息，用二进制表示，少于50比特：\n");
	scanf("%s",&string);
	time1=get_time();
	//求身份的二进制长度
	count=strlen(string);
	printf("count=%d\n",count);
	//求身份中第一个二进制位为1的位置i,并将矢量U中的ui赋给指针ptr
	for(i=0;i<=count;i++){
		if(string[i]='1'){
			ptr=L->item[i]; //https://zhidao.baidu.com/question/143562807.html
			break;
		}
	}
	i=i+1;
	for(i;i<=count;i++){//查找身份中其他比特位为1的位置
		if(string[i]=='i')
			element_add(ptr,ptr,L->item[i]);//执行求和运算
	}
	//计算temp=u+ui
	element_add(temp,u,ptr);
	element_random(r);//生成PKG选取的随机数r
	element_mul_zn(S1,temp,r);
	element_add(S1,S1,sP2);
	element_mul_zn(S2,P,r);
	time2=get_time();
	element_printf("r=%B\n",r);
	//输出私钥
	printf("用户身份的私钥 %s is:\n",string);
	element_printf("S1=%B\n",S1);
	element_printf("S2=%B\n",S2);
	//输出密钥提取阶段所用的时间
	printf("密钥提取阶段所用的时间=%fs\n",time2-time1);

	//加密阶段
	printf("------------------------加密阶段------------------------------\n");
	time1=get_time();
	//生成要加密的消息m
	element_random(m);
	//生成加密使用的随机数
	element_random(t);
	pairing_apply(C1,P1,P2,pairing);
	element_pow_zn(C1,C1,t);
	element_mul(C1,C1,m);
	element_mul_zn(C2,P,t);
	element_mul_zn(C3,temp,t);
	time2=get_time();
	element_printf("这个消息m是 %B\n",m);
	printf("这个m的密文是：\n");
	element_printf("C1=%B\n",C1);
	element_printf("C2=%B\n",C2);
	element_printf("C3=%B\n",C3);
	//输出加密阶段所用的时间
	printf("加密阶段所用的时间=%fs\n",time2-time1);

	//解密阶段
	printf("------------------------解密阶段------------------------------\n");
	time1=get_time();
	pairing_apply(T1,C3,S2,pairing);
	pairing_apply(T2,C2,S1,pairing);
	//求T2的逆元
	element_invert(T2,T2);
	element_mul(T2,T2,T1);
	element_mul(m1,C1,T2);
	time2=get_time();
	printf("这个恢复的消息是：\n");
	element_printf("m1=%B\n",m1);
	//输出解密阶段所用的时间
	printf("解密阶段所用的时间=%fs\n",time2-time1);
	byte1=element_length_in_bytes(C1);
	byte2=element_length_in_bytes(C2);
	byte3=element_length_in_bytes(C3);
	//输出密文C字节的长度
	printf("密文的字节长度=%d\n",byte1+byte2+byte3);

	//清除所定义的元素变量、动态数组变量和配对变量，释放资源
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