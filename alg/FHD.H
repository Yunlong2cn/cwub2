/* 

  FHD - Fast Hash Digest Algorithm

 */
/*
 *FHD - ����ɢ��ժҪ�㷨��飺
 *
 *1. ���㷨��HappyTown (wxr277@163.com)���Ʋ�ʵ�֡�
 *2. �ܹ������ⳤ�ȵ��ֽڴ�ѹ���������128���ص���ϢժҪ��
 *3. ���������ɱ䣻
 */
/*
 * �ڱ��㷨��ʵ�ִ���ע���У�����ʹ��byte��bit(����)��
 * ����Ϊ�˷�����������Ҳ�п��ܵ��������ϵĻ��ҡ�
 */

#include <memory.h>

#define SIZE_OF_FHD_CTX_ST 32
#define LENGTH_OF_DEGIST 16

//
typedef struct FHD_CTX_st
{
  unsigned int state[4];    //����ժҪ
  unsigned char pad[16];    //����data��(length(data) % 128) ���ز��֣���FHD_Final�д���
}FHD_CTX;

//��ʼ������
//���ڳ�ʼ��FHD_CTX�ṹ���ڲ�����
int FHD_Init(FHD_CTX *c);

//��������data���ȴ���128 bits�Ĳ��֣������FHD_CTX�ṹ��pad����
//data:��Ҫ������������
//len:��Ҫ�����������ݳ��ȣ���λΪbyte
int FHD_Update(FHD_CTX *c, unsigned char *data, unsigned int len);

//ѹ��pad�������ժҪ��md
//md:����Ϊ16 byte��ժҪ
int FHD_Final(unsigned char *md, FHD_CTX *c);

//ѹ������
//data:��Ҫ��ѹ��������
//FHD_Digest:���뼰���ص�ժҪ
int FHD_Compress(FHD_CTX *c, unsigned char *data);


//32λ����ѭ�����ƺ���
//x:������λ�ı���
//y:Ҫ�Ƶ�λ��
//���������λ���ֵ
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

//��ʼ��FHD_CTX�ṹ
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

	
    //��padԤ����Ϊ0
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
    int loopCount;  //��data�ֽ���len����16ʱ���ڴ��ӳ�����FHD_Compress������ִ�д���
    int padLen;     //��������,��λΪ byte

    loopCount = len / 16;
    padLen = len % 16;

    //������data���ȸպ���16�ֽڵı���ʱ��ѹ��ǰloopCount-1��16�ֽڣ����һ��16�ֽ��������pad
    if ((padLen==0) && (loopCount > 0))
    {
        //��ѹ��ǰ(loopCount-1)��16�ֽڵ�����
        for (i=0; i<(loopCount-1); i++)
        {
            FHD_Compress(context, data);
            data += 16;
        }
        //Ȼ������һ��16�ֽ����ڲ�pad
        for (i=0; i<16; i++)
        {
            context->pad[i] = data[i];
        }

    }

    //������data���Ȳ���16�ֽڵı�����������16�ֽ�
    else if ((padLen>0) && (loopCount > 0))
    {
        //��ѹ��16�ֽڵı�������
        for (i=0; i<loopCount; i++)
        {
            FHD_Compress(context, data);
            data += 16;
        }
        //Ȼ��pad
        for (i=0; i<padLen; i++)
        {
            context->pad[i] = data[i];
        }
    }

    //������data����С��16�ֽ�ʱ
    else if ((padLen>0) && (loopCount == 0))
    {
        //dataȫ�����ڲ�pad
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

    //ѹ��16�ֽڵ�pad
    p = context->pad;
    FHD_Compress(context, p);

    //�����ϢժҪ
    memcpy(digest,context->state,16);
    
    //���FHD_CTX�ṹ
    memset(context, 0, SIZE_OF_FHD_CTX_ST);
    return 1;
}


//ѹ������
int FHD_Compress(FHD_CTX *context, unsigned char *data)
{
    unsigned int i,j;
    unsigned int *p;
    unsigned int a,b,c,d;   //���潫��ѹ�������ݣ���128 bits
    unsigned int sS[4];     //������ʱժҪ

    //����FHDѹ��������4������
    int aa = 0xC32F1E4B;
    int bb = 0x596807CD;
    int cc = 0xDE6F723A;
    int dd = 0xA18945BC;

    //���ݽ�Ҫѹ����128 bits����
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

    //ѹ���������Ǻ�MD5������ɢ���㷨��ͬ�Ĺؼ�֮��
    for (i=0; i<35; i++)  //��������i�������趨������Ҫ̫С
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
    
    //����state��FHD_Digest
    for (i=0; i<4; i++)
    {
        context->state[i] = sS[i];
    }
	
    return 1;
}


