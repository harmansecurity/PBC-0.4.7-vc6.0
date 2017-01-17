#include <pbc.h>
#include <pbc_time.h>
int main(void){
	pairing_t pairing;
	element_t P,U,V,W,temp1,temp3;
	element_t x,y,m,r,temp2;
	element_t T1,T2;
	int byte1,byte2;
	double time1,time2;
	a_param_input(pairing);//自动输入类型A的配对的参数
	//将变量初始化为G1中的元素
	element_init_G1(P,pairing);
	element_init_G1(U,pairing);
	element_init_G1(V,pairing);
	element_init_G1(W,pairing);
	element_init_G1(temp1,pairing);
	element_init_G1(temp3,pairing);
	//将变量初始化为Zr中的元素
	element_init_Zr(y,pairing);
	element_init_Zr(x,pairing);
	element_init_Zr(m,pairing);
	element_init_Zr(r,pairing);
	element_init_Zr(temp2,pairing);
	//将变量初始化为GT中的元素
	element_init_GT(T1,pairing);
	element_init_GT(T2,pairing);
	//判断所用的配对是否为对称配对，不是则输出错误提示信息
	if(!pairing_is_symmetric(pairing)){
		fprintf(stderr,"--必须要是对称配对才能工作！--\n");
		exit(1);
	}
	
	printf("***************************进入BB模式*******************************\n");
	
	//密钥生成阶段
	printf("------------------------密钥生成阶段------------------------------\n");
	time1=get_time();
	//生成签名者随机选择的私钥x和y
	element_random(x);
	element_random(y);
	element_random(P);//生成G1的生成元P
	element_mul_zn(U,P,x);//计算公钥U=xP
	element_mul_zn(V,P,y);//计算公钥V=yP
	time2=get_time();
	printf("--------------G1的生成元是：-----------\n");
	element_printf("P=%B\n",P);
	printf("---------------这个私钥是：-------------\n");
	element_printf("x=%B\n",x);
	element_printf("y=%B\n",y);
	printf("---------------这个公钥是：-------------\n");
	element_printf("U=%B\n",U);
	element_printf("V=%B\n",V);
	//输出密钥生成阶段所用的时间
	printf("密钥生成阶段所用的时间=%fs\n",time2-time1);

	//签名阶段
	printf("------------------------签名阶段------------------------------\n");
	time1=get_time();
	element_random(m);//生成要签名的消息m
	element_random(r);//生成签名者选择的随机数r
	element_mul(temp2,y,r);//计算temp2=ry
	//计算temp2=temp2+m,即temp2=m+ry
	element_add(temp2,temp2,m);
	//计算temp2=temp2+x,即temp2=x+m+ry
	element_add(temp2,temp2,x);
	//若x+m+ry不为0，则求x+m+ry的逆元1/(x+m+ry)
	if (!element_is0(temp2)){
		element_invert(temp2,temp2);//求temp2=1/(x+m+ry)
		//计算W=temp2P
		element_mul_zn(W,P,temp2);
	}else{
		//重新选择随机数r
		printf("选择另一个随机数r!");
		exit(1);
	}
	time2=get_time();
	element_printf("-----------这个消息m=-----------%B\n",m);
	printf("-----------这个签名的消息是：-----------\n");
	element_printf("W=%B\n",W);
	element_printf("r=%B\n",r);
	//输出签名阶段所用的时间
	printf("签名阶段所用的时间=%fs\n",time2-time1);

	//验证阶段
	printf("------------------------验证阶段------------------------------\n");
	time1=get_time();
	element_mul_zn(temp1,V,r);//计算temp1=rV
	element_mul_zn(temp3,P,m);//计算temp3=mP
	//计算temp1=temp1+temp3，即temp1=mP+rV
	element_add(temp1,temp1,temp3);
	//计算temp1=temp1+U,即temp1=U+mP+rV
	element_add(temp1,temp1,U);
	//计算T1=e(temp1,W),即T1=e(U+mP+rV,W)
	pairing_apply(T1,temp1,W,pairing);
	pairing_apply(T2,P,P,pairing);//计算T2=e(P,P)
	//判断签名是否有效
	if(!element_cmp(T1,T2))
		printf("--------------这个签名是有效的！-------------\n");
	else
		printf("-------------这个签名不是有效的！------------\n");
	time2=get_time();
	element_printf("e(P,P)=%B\n",T2);
	element_printf("e(U+mP+rV,w)=%B\n",T1);
	//输出验证阶段所用的时间
	printf("验证阶段所用的时间=%fs\n",time2-time1);
	byte1=element_length_in_bytes(W);//求W的字节长度
	byte2=element_length_in_bytes(r);//求r的字节长度
	//输出签名的字节长度
	printf("这个签名的字节长度=%d\n",byte1+byte2);
	//释放所占用的资源
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