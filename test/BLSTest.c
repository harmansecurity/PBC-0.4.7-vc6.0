#include <pbc.h>
#include <pbc_time.h>
int main(void){
	pairing_t pairing;
	element_t P,Y,M,W;
	element_t x;
	element_t T1,T2;
	double time1,time2;
	int byte;
	a_param_input(pairing);//自动输入类型A的配对的参数
	//将变量初始化为G1中的元素
	element_init_G1(P,pairing);
	element_init_G1(Y,pairing);
	element_init_G1(M,pairing);
	element_init_G1(W,pairing);
	//将变量初始化为Zr中的元素
	element_init_Zr(x,pairing);
	//将变量初始化为GT中的元素
	element_init_GT(T1,pairing);
	element_init_GT(T2,pairing);
	//判断所用的配对是否为对称配对，不是则输出错误提示信息
	if(!pairing_is_symmetric(pairing)){
		fprintf(stderr,"--必须要是对称配对才能工作！--\n");
		exit(1);
	}
	
	printf("***************************进入BLS模式*******************************\n");
	
	//系统建立阶段
	printf("------------------------密钥生成阶段------------------------------\n");
	time1=get_time();
	//生成签名者随机选择的x
	element_random(x);
	//生成G1的生成元
	element_random(P);
	element_mul_zn(Y,P,x);//计算Y=xp
	time2=get_time();
	printf("-------------G1的生成元是：-----------\n");
	element_printf("P=%B\n",P);
	printf("-------------这个私钥是：-------------\n");
	element_printf("x=%B\n",x);
	printf("-------------这个公钥是：-------------\n");
	element_printf("Y=%B\n",Y);
	//输出密钥生成阶段所用的时间
	printf("密钥生成阶段所用的时间=%fs\n",time2-time1);
	
	//签名阶段
	printf("------------------------签名阶段------------------------------\n");
	time1=get_time();
	//模拟消息的Hash值，即M=H1(m)
	element_from_hash(M,"messageofsign",13);
	element_mul_zn(W,M,x);//计算W=XM
	time2=get_time();
	//输出消息的Hash值，这里是模拟值
	element_printf("H1(m)=%B\n",M);
	printf("-------------------这个签名的消息是：----------------\n");
	element_printf("W=%B\n",W);//输出签名
	//输出签名阶段所用的时间
	printf("签名阶段所用的时间=%fs\n",time2-time1);

	//验证阶段
	printf("------------------------验证阶段------------------------------\n");
	time1=get_time();
	pairing_apply(T1,P,W,pairing);
	//模拟消息的Hash值，即M=H1(m)
	element_from_hash(M,"messageofsign",13);
	pairing_apply(T2,Y,M,pairing);
	//判断签名是否有效
	if(!element_cmp(T1,T2))
		printf("这个签名是有效的！\n");
	else
		printf("这个签名是无效的！\n");
	time2=get_time();
	element_printf("e(P,W)=%B\n",T1);
	element_printf("e(Y,H1(m))=%B\n",T2);
	//输出验证阶段所用的时间
	printf("验证阶段所用的时间=%fs\n",time2-time1);
	byte=element_length_in_bytes(W);//求W的字节长度
	//输出签名的字节长度
	printf("-------------这个签名的字节长度=-----------------%d\n",byte);
	//释放所占用的资源
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