#include <pbc.h>
#include <pbc_time.h>
int main(void){
	pairing_t pairing;
	element_t s,x,r;
	element_t P,Ppub,Qu,Du,Su,Xu,Yu,V;
	element_t T1,T2;
	double time1,time2;
	int byte;
	a_param_input(pairing);//自动输入a类型配对的参数
	//将变量初始化为Zr中的元素
	element_init_Zr(s,pairing);
	element_init_Zr(r,pairing);
	element_init_Zr(x,pairing);
	//将变量初始化为G1中的元素
	element_init_G1(P,pairing);
	element_init_G1(Ppub,pairing);
	element_init_G1(Qu,pairing);
	element_init_G1(Du,pairing);
	element_init_G1(Su,pairing);
	element_init_G1(Xu,pairing);
	element_init_G1(Yu,pairing);
	element_init_G1(V,pairing);
	//将变量初始化为GT中的元素
	element_init_GT(T1,pairing);
	element_init_GT(T2,pairing);
	//判断所用的配对是否为对称配对，不是则输出错误提示信息
	if(!pairing_is_symmetric(pairing)){
		fprintf(stderr,"--必须要是对称配对才能工作！--");
		exit(1);
	}
	
	printf("***************************进入BasicCL-PKE模式*******************************\n");
	
	//系统建立阶段
	printf("------------------------系统建立阶段------------------------------\n");
	time1=get_time();
	//生成KGC随机选择的主密钥s
	element_random(s);
	//生成G1的生成元P
	element_random(P);
	element_mul_zn(Ppub,P,s);
	time2=get_time();
	element_printf("P=%B\n",P);
	element_printf("s=%B\n",s);
	element_printf("Ppub=%B\n",Ppub);
	//输出系统建立阶段所用的时间
	printf("系统建立阶段所用的时间=%fs\n",time2-time1);

	//部分私钥提取阶段
	printf("------------------------部分私钥提取阶段------------------------------\n");
	time1=get_time();
	//使用随机值模拟ID对应的Qu
	element_random(Qu);
	element_mul_zn(Du,Qu,s);
	time2=get_time();
	element_printf("部分私钥Du=%B\n",Du);
	//输出部分私钥提取阶段所用的时间
	printf("部分私钥提取阶段所花的时间=%fs\n",time2-time1);

	//设置秘密值阶段
	printf("------------------------设置秘密值阶段------------------------------\n");
	time1=get_time();
	element_random(x);
	time2=get_time();
	element_printf("秘密值是=%B\n",x);
	//输出设置秘密值阶段所用的时间
	printf("设置秘密值阶段所花的时间=%fs\n",time2-time1);

	//设置私钥值阶段
	printf("------------------------设置私钥值阶段------------------------------\n");
	time1=get_time();
	element_mul_zn(Su,Du,x);//计算完全私钥Su=xDu
	time2=get_time();
	element_printf("完全私钥Su是=%B\n",Su);
	//输出设置私钥阶段所用的时间
	printf("设置私钥阶段所花的时间=%fs\n",time2-time1);

	//设置公钥阶段
	printf("------------------------设置公钥阶段------------------------------\n");
	time1=get_time();
	element_mul_zn(Xu,P,x);//计算Xu=xP
	element_mul_zn(Yu,Ppub,x);//计算Yu=xPpub
	time2=get_time();
	printf("-----------------公钥是：---------------\n");
	element_printf("Xu=%B\n",Xu);
	element_printf("Yu=%B\n",Yu);
	//输出设置公钥阶段所用的时间
	printf("设置公钥阶段所花的时间=%fs\n",time2-time1);

	//加密阶段
	printf("------------------------加密阶段------------------------------\n");
	time1=get_time();
	pairing_apply(T1,Xu,Ppub,pairing);//计算T1=e(Xu,Ppub)
	pairing_apply(T2,Yu,P,pairing);
	//判断公钥是否正确
	if(!element_cmp(T1,T2)){
		element_random(r);//选取随机数r
		element_mul_zn(V,P,r);//计算V=rP
		pairing_apply(T1,Yu,Qu,pairing);//计算T1=e(Yu,QU)
		element_pow_zn(T1,T1,r);//计算T1=e(Yu,QU)^r
		element_printf("V=%B\n",V);
		element_printf("e(Yu,QU)^r=%B\n",T1);
	}else{
		printf("---------------错误！这个公钥是不正确的！-----------");
		exit(1);
	}
	time2=get_time();
	//输出加密阶段所花费的时间
	printf("加密阶段所花的时间=%fs\n",time2-time1);

	//解密阶段
	printf("------------------------解密阶段------------------------------\n");
	time1=get_time();
	pairing_apply(T2,V,Su,pairing);//计算T2=e(V,Su)
	time2=get_time();
	element_printf("T2=e(V,Su)=%B\n",T2);
	//输出解密阶段所花费的时间
	printf("解密阶段所花的时间=%fs\n",time2-time1);
	//求V的字节长度
	byte=element_length_in_bytes(V);
	//假设消息长度为128字节
	printf("密文的字节长度是=%d\n",byte+128);

	//释放占用的资源
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