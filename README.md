# PBC-0.4.7-vc6.0
　 　这是修改成功的在VC6.0下的可用整套源码，注意解压后要打开PBCVC文件夹下的`PBCVC.dsw`工作区运行，可以分别将`example`文件夹下的例子分别放入该工作区，但要删除原工作区的`testpairing.c`文件，因为`testpairing.c`本身就是一个带有`main`函数的可用实例。我写的一些关于身份认证和签名的配对测试用例放在了`test`文件夹中。注意请使用`git clone`下载。

　 　PBC库使用G1、G2和GT分别表示双线性映射:`e:G1xG2->GT`中的G1、G2和GT，他们的阶都为r。Zr表示模r的整数环。配对运算输入两个元素，一个元素来自群G1，另一个元素来自群G2，输出结果是GT中的一个元素。G1和G2也可以是同一个群，即是一个对称配对，这种情况下它们的元素可以相互混淆使用。(**如果觉得不错，请先在这个仓库上点个 star 吧**，我会继续跟大家分享自己的学习过程)。

-----

#### 1.配对的初始化方式
##### 1) 以标准的方式初始化配对
```C
pairing_t pairing;//声明一个配对类型的变量pairing
pairing_init_inp_str(pairing,stdin);//参数stdin表示配对的输入参数方式为标准输入
```
##### 2) 指定类型的方式初始化配对
```C
a_param_input(pairing);//将配对变量pairing初始化为类型A的配对
d_param_input(pairing);//将配对变量pairing初始化为类型D的配对
f_param_input(pairing);//将配对变量pairing初始化为类型F的配对
pairing_clear(pairing);//调用该函数释放pairing占用的资源
```

----

#### 2.元素的初始化
```C
void element_init(element_t e,field_ptr f);//将元素变量e初始化为代数结构f中的元素
void element_init_G1(element_t e,pairing_t pairing);//将元素变量e初始化为群G1中的元素
void element_init_G2(element_t e,pairing_t pairing);//将元素变量e初始化为群G2中的元素
void element_init_GT(element_t e,pairing_t pairing);//将元素变量e初始化为群GT中的元素
void element_init_Zr(element_t e,pairing_t pairing);//将元素变量e初始化为群Zr中的元素
void element_init_same_as(element_t e,element_t e2);//将元素变量e初始化为元素变量e2所在的代数结构中的元素
void element_clear(element_t e);//清楚元素变量e并释放其所占用的资源
```

----

#### 3.运算函数
```C
void element_add(element_t n,element_t a,element_t b);//输入a和b，输出n=a+b
void element_sub(element_t n,element_t a,element_t b);//输入a和b，输出n=a-b
void element_mul(element_t n,element_t a,element_t b);//输入a和b，输出n=axb
void element_mul_si(element_t n,element_t a,signed long int z);//输入a和z，输出n=axz,z是一个长整型数据
void element_mul_zn(element_t n,element_t a,element_t z);//输入a和z，输出n=axz,z是整数环中的一个元素
void element_div(element_t n,element_t a,element_t b);//输入a和b，输出n=a/b
void element_double(element_t n,element_t a);//输入a，输出n=a+a
void element_halve(element_t n,element_t a);//输入a，输出n=a/2
void element_square(element_t n,element_t a);//输入a，输出n=a^2
void element_neg(element_t n,element_t a);//输入a，输出n=-a
void element_invert(element_t n,element_t a);//输入a，输出a的逆元n
void element_pow_mpz(element_t x,element_t a,mpz_t n);//输入a和n，输出x=a^n
void element_pow_zn(element_t x,element_t a,element_t n);//输入a和n，输出x=a^n，n是整数环中的元素
//输入a1,a2和n1,n2，输出x=a1^n1xa2^n2
void element_pow2_mpz(element_t x,element_t a1,mpz_t n1,element_t a2,mpz_t n2);
//输入a1,a2和n1,n2，输出x=a1^n1xa2^n2,n1,n2是整数环中的元素
void element_pow2_zn(element_t x,element_t a1,element_t n1,element_t a2,element_t n2);
```

-----

#### 4.其它函数
```C
int element_length_in_bytes(element_t e);//获得元素变量e的字节数
void element_random(element_t e);//函数从这个代数结构中均匀随机选择一个元素并赋值给e
int element_cmp(element_t a,element_t b);//若变量a和b相等，则函数返回0否则返回1
void element_to_mpz(mpz_t z,element_t e);//将元素e转换为一个GMP整数并存储在mpz_t类型的变量z中
//PBC库中没有提供计算Hash值的函数，这里只是以获得的Hash值作为输入，从中截取一部分表示域中的某个元素
void element_from_hash(element_t e,void * data,int len);
int element_printf(const char * format,...);//格式化打印元素值
```

----

*以上列举了PBC库中的一些常用函数及其代表的意思，如有特殊需求可查阅PBC官方文档。*

# 联系作者

- [Harman's Personal Website](http://harmansecurity.cn/)
- 邮箱：`lianghui_1994@163.com`

-----

# Lisence

Lisenced under [Apache 2.0 lisence](http://opensource.org/licenses/Apache-2.0)