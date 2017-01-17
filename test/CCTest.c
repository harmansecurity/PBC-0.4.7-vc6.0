#include <pbc.h>
#include <pbc_time.h>

int main(void){
	pairing_t pairing;
	element_t P,Ppub,Qu,Su,V,W,temp;
	element_t s,r,h,temp1;
	element_t T1,T2;
	double time1,time2;
	int byte1,byte2;
	a_param_input(pairing);//自动输入类型为A的配对的参数
	//将变量初始化为G1中的元素
	element_init_G1(P,pairing);
	element_init_G1(Ppub,pairing);
	element_init_G1(Qu,pairing);
	element_init_G1(Su,pairing);
	element_init_G1(V,pairing);
	element_init_G1(W,pairing);
	element_init_G1(temp,pairing);
	//将变量初始化为Zr中的元素
	element_init_Zr(s,pairing);
	element_init_Zr(r,pairing);
	element_init_Zr(h,pairing);
	element_init_Zr(temp1,pairing);
	//将变量初始化为GT中的元素
	element_init_GT(T1,pairing);
	element_init_GT(T2,pairing);
	//判断所用的配对是否为对称配对，不是则输出错误提示信息
	if(!pairing_is_symmetric(pairing)){
		fprintf(stderr,"--必须要是对称配对才能工作！--\n");
		exit(1);
	}
	
	printf("***************************进入CC模式*******************************\n");
	
	//系统建立阶段
	printf("------------------------系统建立阶段------------------------------\n");
	time1=get_time();
	element_random(s);//生成随机选择的主密钥
	element_random(P);//生成G1的生成元P
	element_mul_zn(Ppub,P,s);//计算Ppub=sP
	time2=get_time();
	element_printf("P=%B\n",P);
	element_printf("Ppub=%B\n",Ppub);
	//输出系统建立阶段所用的时间
	printf("系统建立阶段所用的时间=%fs\n",time2-time1);

	//密钥提取阶段
	printf("------------------------密钥提取阶段------------------------------\n");
	time1=get_time();
	//模拟Qu=H1(IDu),此处并不是真正的Hash值
	element_from_hash(Qu,"IDu",3);
	element_mul_zn(Su,Qu,s);//计算私钥Su=sQu
	time2=get_time();
	element_printf("Qu=%B\n",Qu);
	element_printf("Su=%B\n",Su);
	//输出密钥提取阶段所用的时间
	printf("密钥提取阶段所用的时间=%fs\n",time2-time1);

	//签名阶段
	printf("------------------------签名阶段------------------------------\n");
	time1=get_time();
	element_random(r);//选取随机数r
	element_mul_zn(V,Qu,r);//计算V=rQu
	element_random(h);//选取随机值来模拟h=H2(m,V)
	element_add(temp1,h,r);//计算temp1=r+h
	element_mul_zn(W,Su,temp1);//计算W=temp1Su，即W=(r+h)Su
	time2=get_time();
	printf("------------这个签名的消息是：-----------\n");
	element_printf("V=%B\n",V);
	element_printf("W=%B\n",W);
	//输出签名阶段所用的时间
	printf("签名阶段所用的时间=%fs\n",time2-time1);

	//验证阶段
	printf("------------------------验证阶段------------------------------\n");
	time1=get_time();
	pairing_apply(T1,P,W,pairing);//计算T1=e(P,W)
	element_mul_zn(temp,Qu,h);//计算temp=hQu
	//计算temp=temp+V,即temp=V+hQu
	element_add(temp,temp,V);
	//计算T2=e(Ppub,temp),即T2=e(Ppub,V+hQu)
	pairing_apply(T2,Ppub,temp,pairing);
	//判断签名是否有效
	if(!element_cmp(T1,T2))
		printf("--------------这个签名是有效的！-------------\n");
	else
		printf("-------------这个签名不是有效的！------------\n");
	time2=get_time();
	element_printf("e(P,W)=%B\n",T1);
	element_printf("e(Ppub,V+hQu)=%B\n",T2);
	//输出验证阶段所用的时间
	printf("验证阶段所用的时间=%fs\n",time2-time1);
	//求W的字节长度
	byte1=element_length_in_bytes(W);
	//求V的字节长度
    byte2=element_length_in_bytes(V);
	//输出签名的字节长度
	printf("签名的字节长度=%d\n",byte1+byte2);
	//释放所占用的资源
	element_clear(P);
	element_clear(Ppub);                    
	element_clear(Qu);
	element_clear(Su);
	element_clear(W);
	element_clear(V);
	element_clear(temp);
	element_clear(s);
	element_clear(h);
	element_clear(r);
	element_clear(temp1);
	element_clear(T1);
	element_clear(T2);
	pairing_clear(pairing);
	return 0;
}