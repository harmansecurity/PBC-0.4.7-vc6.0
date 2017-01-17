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
	//将变量初始化为Zr中的元素
	element_init_Zr(s,pairing);
	element_init_Zr(r,pairing);
	element_init_Zr(h,pairing);
	element_init_Zr(temp1,pairing);
	//将变量初始化为G1中的元素
	element_init_G1(P,pairing);
	element_init_G1(Ppub,pairing);
	element_init_G1(Qu,pairing);
    element_init_G1(Su,pairing);
	element_init_G1(V,pairing);
	element_init_G1(X,pairing);
	element_init_G1(Z,pairing);
	element_init_G1(temp2,pairing);
	//将变量初始化为GT中的元素
	element_init_GT(T1,pairing);
	element_init_GT(T2,pairing);
	//判断所用的配对是否为对称配对，若不是，则输出错误的提示信息并停止运行
	if(!pairing_is_symmetric(pairing)){
		fprintf(stderr,"只能对对称的配对才会有效\n");
		exit(1);
	}
	printf("***************************进入IBCES模式*******************************\n");
	
	//系统建立阶段
	printf("------------------------系统建立阶段------------------------------\n");
	time1=get_time();
	element_random(s);//生成PKG随机选择的主密钥s
	element_random(P);//生成G1的生成元P
	element_mul_zn(Ppub,P,s);//计算Ppub=sP
	time2=get_time();
	element_printf("P=%B\n",P);
	element_printf("s=%B\n",s);
	element_printf("Ppub=%B\n",Ppub);
	//输出系统建立阶段所用的时间
	printf("系统建立阶段所用的时间=%fs\n",time2-time1);
	
	//密钥提取阶段
	printf("------------------------密钥提取阶段------------------------------\n");
	time1=get_time();
	//使用随机值来模拟ID对应的H1(IDU)，也可以用element_from_hash(Qu,"IDu",3)
	element_random(Qu);
	element_mul_zn(Su,Qu,s);//计算私钥Su=sQu
	time2=get_time();
	element_printf("Qu=%B\n",Qu);
	element_printf("Su=%B\n",Su);
	//输出密钥提取阶段所用的时间
	printf("密钥提取阶段所用的时间=%fs\n",time2-time1);

	//加密阶段
	printf("------------------------加密阶段------------------------------\n");
	time1=get_time();
	element_random(r);//产生一个随机r来模拟H3(a,m)的值
	element_mul_zn(V,P,r);//计算V=rP
	pairing_apply(T1,Ppub,Qu,pairing);//计算T1=e(Ppub,Qu)
	element_pow_zn(T1,T1,r);//T1=e(Ppub,Qu)^r
	time2=get_time();
	element_printf("r=%B\n",r);
	element_printf("V=%B\n",V);
	element_printf("e(Ppub,Qu)^r=%B\n",T1);
	//输出加密阶段所用的时间
	printf("加密阶段所用的时间=%fs\n",time2-time1);

	//解密阶段
	printf("------------------------解密阶段------------------------------\n");
	time1=get_time();
	pairing_apply(T2,V,Su,pairing);//计算T2=e(V,Su)
	element_mul_zn(temp2,P,r);//计算temp2=rP
	if(!element_cmp(V,temp2))//判断解密是否成功
		printf("------------这个解密是正确的！---------------\n");
	else
		printf("----------错误！这个解密是不正确的！-----------\n");
	time2=get_time();
	//输出解密阶段所用的时间
	printf("解密阶段所用的时间=%fs\n",time2-time1);

	//签名阶段
	printf("------------------------签名阶段------------------------------\n");
	time1=get_time();
	element_mul_zn(X,Qu,r);//计算X=rQu
	element_random(h);
	element_add(temp1,r,h);
	element_mul_zn(Z,Su,temp1);
	time2=get_time();
	printf("--------这个签名的消息是：---------\n");
	element_printf("X=%B\n",X);
	element_printf("Z=%B\n",Z);
	//输出签名阶段所用的时间
	printf("签名阶段所用的时间=%fs\n",time2-time1);

	//验证阶段
	printf("------------------------验证阶段------------------------------\n");
	time1=get_time();
	pairing_apply(T1,P,Z,pairing);
	element_mul_zn(temp2,Qu,h);//计算temp2=hQu
	element_add(temp2,temp2,X);
	pairing_apply(T2,Ppub,temp2,pairing);
	//判断签名是否有效
	if(element_cmp(T1,T2))
		printf("-----------这个签名是有效的!---------\n");
	else
		printf("-----------这个签名是无效的!---------\n");
	time2=get_time();
	element_printf("e(P,Z)=%B\n",T1);
	element_printf("e(Ppub,X+hQu)=%B\n",T2);
	//输出验证阶段所用的时间
	printf("验证阶段所用的时间=%fs\n",time2-time1);
	byte1=element_length_in_bytes(V);
	printf("密文的字节长度=%d\n",byte1+128+128);
	byte1=element_length_in_bytes(X);
	byte2=element_length_in_bytes(Z);
	//输出签名的字节长度
	printf("签名的字节长度=%d\n",byte1+byte2);
	//释放占用的资源
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