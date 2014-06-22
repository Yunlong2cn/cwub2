/* 

  FHD - Fast Hash Digest Algorithm

 */
/*
 *FHD - 快速散列摘要算法简介：
 *
 *1. 本算法由HappyTown (wxr277@163.com)研制并实现。
 *2. 能够把任意长度的字节串压缩，并输出128比特的消息摘要；
 *3. 迭代次数可变；
 */
/*
 * 在本算法的实现代码注释中，交互使用byte和bit(比特)，
 * 均是为了方便描述，但也有可能导致理解上的混乱。
 */

#include <memory.h>

#define SIZE_OF_FHD_CTX_ST 32
#define LENGTH_OF_DEGIST 16

//
typedef struct FHD_CTX_st
{
  unsigned int state[4];    //保存摘要
  unsigned char pad[16];    //保存data中(length(data) % 128) 比特部分，在FHD_Final中处理
}FHD_CTX;

//初始化函数
//用于初始化FHD_CTX结构的内部数组
int FHD_Init(FHD_CTX *c);

//加密数据data长度大于128 bits的部分，并填充FHD_CTX结构的pad数组
//data:将要被处理的数据
//len:将要被处理的数据长度，单位为byte
int FHD_Update(FHD_CTX *c, unsigned char *data, unsigned int len);

//压缩pad，并输出摘要到md
//md:长度为16 byte的摘要
int FHD_Final(unsigned char *md, FHD_CTX *c);

//压缩函数
//data:将要被压缩的数据
//FHD_Digest:传入及返回的摘要
int FHD_Compress(FHD_CTX *c, unsigned char *data);


//32位变量循环左移函数
//x:将被移位的变量
//y:要移的位数
//输出：被移位后的值
/*
unsigned int ROL(unsigned int x, unsigned int y)
{
    return ((x<<y) | (x>>(32-y)));
}
*/
#define ROL(value, bits) (((value) << (bits)) | ((value) >> (32 - (bits))))

unsigned int F(unsigned int a, unsigned int b, unsigned int c, unsigned int d)
{
    return ((a^b) | (c^d)) + 0x5A827999;
}

unsigned int G(unsigned int a, unsigned int b, unsigned int c, unsigned int d)
{
    return ((a | (~c)) | (b & (~d))) + 0x6ED9EBA1;
}

unsigned int H(unsigned int a, unsigned int b, unsigned int c, unsigned int d)
{
    return ((a & (~d)) ^ (b & (~c))) + 0xCA62C1D6;
}

unsigned int I(unsigned int a, unsigned int b, unsigned int c, unsigned int d)
{
    return (((~a) ^ b) | (c ^ (~d))) + 0x8F1BBCDC;
}



static long x = 0;
static long y = 0;
static long z = 0;
static long w = 0;

long rand32(void)
{
    long t = x ^ (x << 11);
    x = y; y = z; z = w;
    return w = w ^ (w >> 19) ^ (t ^ (t >> 8));
}

//初始化FHD_CTX结构
int FHD_Init(FHD_CTX *context)
{
    int i;
    
    x = 0x714688D8;
    y = 0xF9CE0050;
    z = 0x8EB97727;
    w = 0x631BF2C9;

	for(int n=0;n<4;n++)
	{
		for ( i = 0; i < 0x1000; i++ ) rand32();
		context->state[n] = rand32();
	}

	
    //把pad预处理为0
    for (i=0; i<16; i++)
    {
        context->pad[i] = 0; 
    }

    return 1;
}

//FHD_Update
int FHD_Update(FHD_CTX *context, unsigned char *data, unsigned int len)
{
    int i;
    int loopCount;  //当data字节数len大于16时，在此子程序中FHD_Compress函数的执行次数
    int padLen;     //补丁长度,单位为 byte

    loopCount = len / 16;
    padLen = len % 16;

    //当数据data长度刚好是16字节的倍数时，压缩前loopCount-1个16字节，最后一个16字节用于填充pad
    if ((padLen==0) && (loopCount > 0))
    {
        //先压缩前(loopCount-1)个16字节的数据
        for (i=0; i<(loopCount-1); i++)
        {
            FHD_Compress(context, data);
            data += 16;
        }
        //然后把最后一个16字节用于补pad
        for (i=0; i<16; i++)
        {
            context->pad[i] = data[i];
        }

    }

    //当数据data长度不是16字节的倍数，但大于16字节
    else if ((padLen>0) && (loopCount > 0))
    {
        //先压缩16字节的倍数部分
        for (i=0; i<loopCount; i++)
        {
            FHD_Compress(context, data);
            data += 16;
        }
        //然后补pad
        for (i=0; i<padLen; i++)
        {
            context->pad[i] = data[i];
        }
    }

    //当数据data长度小于16字节时
    else if ((padLen>0) && (loopCount == 0))
    {
        //data全部用于补pad
        for (i=0; i<padLen; i++)
        {
            context->pad[i] = data[i];
        }
    }
    return 1;
}

int FHD_Final(unsigned char *digest, FHD_CTX *context)
{
    unsigned char *p;

    //压缩16字节的pad
    p = context->pad;
    FHD_Compress(context, p);

    //输出消息摘要
    memcpy(digest,context->state,16);
    
    //清空FHD_CTX结构
    memset(context, 0, SIZE_OF_FHD_CTX_ST);
    return 1;
}


//压缩函数
int FHD_Compress(FHD_CTX *context, unsigned char *data)
{
    unsigned int i,j;
    unsigned int *p;
    unsigned int a,b,c,d;   //保存将被压缩的数据，共128 bits
    unsigned int sS[4];     //保存临时摘要

    //用于FHD压缩函数的4个常数
    int aa = 0xC32F1E4B;
    int bb = 0x596807CD;
    int cc = 0xDE6F723A;
    int dd = 0xA18945BC;

    //传递将要压缩的128 bits数据
    p = (unsigned int *)data;
    a = *p;
    b = *(p+1);
    c = *(p+2);
    d = *(p+3);
    //
    sS[0] = context->state[0];
    sS[1] = context->state[1];
    sS[2] = context->state[2];
    sS[3] = context->state[3];

    //压缩，这里是和MD5等其它散列算法不同的关键之处
    for (i=0; i<35; i++)  //迭代次数i可自由设定，但不要太小
    {
        a = ROL((a + ROL(b,(i&5&a))^sS[2] + ~c + d&a + b|c + F(a,b,c,d)) ^sS[3], (i&24));
            sS[0] = a^(a + b + c + d + aa);

        for (j=0; j<((a+2)%5); j++)
        {
            b = ROL((b + ROL(c,(i&19&b))^sS[0] + ~d + a&b + c|d + G(a,b,c,d)) ^sS[1], (i&10));
            sS[1] = a + b + c + d + bb;
        }
        for (j=0; j<((b+3)%7); j++)
        {
            c = ROL((c + ROL(d,(i&13&c))^sS[1] + ~a + b&c + d|a + H(a,b,c,d)) ^sS[0], (i&11));
            sS[2] = a + b + c + d + cc;
        }
        for (j=0; j<((c+4)%8); j++)
        {
            d = ROL((d + ROL(a,(i&8&d))^sS[2] + ~b + c&d + a|b + I(a,b,c,d)) ^sS[2], (i&3));
            sS[3] = a + b + c + d + dd;
        }

        sS[0] = sS[1] + sS[2] + sS[3];
        sS[1] = sS[0] + sS[2] + sS[3];
        sS[2] = sS[0] + sS[1] + sS[3];
        sS[3] = sS[0] + sS[1] + sS[2];
    }
    
    //传回state到FHD_Digest
    for (i=0; i<4; i++)
    {
        context->state[i] = sS[i];
    }
	
    return 1;
}



