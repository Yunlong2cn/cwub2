#ifndef CRC__H
#define CRC__H

#include <stdio.h>
#include <string.h>

/*
unsigned long CRC32_STR(const char *buf)
{
	unsigned long nAccum = 0xFFFFFFFF;
	while (*buf)
	{
		nAccum = UPDC32(*buf, nAccum);
		buf++;
	}
	return ~nAccum;
}
unsigned long File_Size(const char *file)
{
	unsigned long file_size = 0;
	FILE* fp;
	fp = fopen( file, "rb");
	fseek( fp, 0, SEEK_END);
	file_size = ftell(fp);
	fclose(fp);
	return file_size;
}
*/

unsigned long CRC32_MEM(const unsigned char* InStr, unsigned long len)
{
	//生成Crc32的查询表
	unsigned int Crc32Table[256] = {0};
	unsigned int Crc;
	for (unsigned int i = 0; i < 256; i++)
	{
		Crc = i;
		for (unsigned int j = 0; j < 8; j++)
		{
			if (Crc & 1)
				Crc = (Crc >> 1) ^ 0xEDB88320;
			else
				Crc >>= 1;
		}
		Crc32Table[i] = Crc;
	}

	//开始计算CRC32校验值
	Crc = 0xFFFFFFFF;
	for (unsigned int i = 0; i < len; i++)
	{
		Crc = (Crc >> 8) ^ Crc32Table[(Crc & 0xFF) ^ InStr[i]];
	}

	Crc ^= 0xFFFFFFFF;
	return Crc;
}
/*
unsigned long CRC32_FLE(const wchar_t *file)
{
	unsigned long nAccum = 0xFFFFFFFF;

	HANDLE hfile = CreateFile(file,
			GENERIC_READ,
			FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL, NULL);
	if (hfile == INVALID_HANDLE_VALUE)
	{
		return 0;
	}

	BY_HANDLE_FILE_INFORMATION file_info;
	GetFileInformationByHandle(hfile, &file_info);
	unsigned long file_size = file_info.nFileSizeLow;

	HANDLE hfilemap = CreateFileMapping(hfile, NULL, PAGE_READONLY, 0, 0, NULL);
	CloseHandle(hfile);

	unsigned char *buf = (unsigned char*) MapViewOfFile(hfilemap, FILE_MAP_READ, 0, 0, 0);
	CloseHandle(hfilemap);

	nAccum = CRC32_MEM(buf,file_size);
	
	UnmapViewOfFile(buf);
	return nAccum;
}
*/

#define MX (z>>5^y<<2) + (y>>3^z<<4)^(sum^y) + (btea_k[p&3^e]^z);
DWORD btea_k[4];

DWORD btea(DWORD* v, long n)
{
    DWORD z = v[n-1], y = v[0], sum = 0, e, DELTA = 0x9e3779b9;
    DWORD p, q ;
    if (n > 1)
    {
        q = 6 + 52 / n;
        while (q-- > 0)
        {
            sum += DELTA;
            e = (sum >> 2) & 3;
            for (p = 0; p < n - 1; p++) y = v[p+1], z = v[p] += MX;
            y = v[0];
            z = v[n-1] += MX;
        }
        return 0 ;
    }
    else if (n < -1)
    {
        n = -n;
        q = 6 + 52 / n;
        sum = q * DELTA ;
        while (sum != 0)
        {
            e = (sum >> 2) & 3;
            for (p = n - 1; p > 0; p--) z = v[p-1], y = v[p] -= MX;
            z = v[n-1];
            y = v[0] -= MX;
            sum -= DELTA;
        }
        return 0;
    }
    return 1;
}
/*
static char base64_table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

unsigned char *base64_encode(const unsigned char *str, int length)
{
	static const char base64_pad = '=';
	const unsigned char *current = str;

	unsigned char *p;
	unsigned char *result;

	if ((length + 2) < 0 || ((length + 2) / 3) >= (1 << (sizeof(int) * 8 - 2)))
	{
		return NULL;
	}

	result = (unsigned char *)malloc(((length + 2) / 3) * 4 * sizeof(char) + 1);
	p = result;

	while (length > 2)
	{
		*p++ = base64_table[current[0] >> 2];
		*p++ = base64_table[((current[0] & 0x03) << 4) + (current[1] >> 4)];
		*p++ = base64_table[((current[1] & 0x0f) << 2) + (current[2] >> 6)];
		*p++ = base64_table[current[2] & 0x3f];

		current += 3;
		length -= 3;
	}

	if (length != 0)
	{
		*p++ = base64_table[current[0] >> 2];
		if (length > 1)
		{
			*p++ = base64_table[((current[0] & 0x03) << 4) + (current[1] >> 4)];
			*p++ = base64_table[(current[1] & 0x0f) << 2];
			*p++ = base64_pad;
		}
		else
		{
			*p++ = base64_table[(current[0] & 0x03) << 4];
			*p++ = base64_pad;
			*p++ = base64_pad;
		}
	}
	*p = 0;
	return result;
}
*/
unsigned long __fastcall base64_encode(char *p_encode, const char *p_src, unsigned long ul_src_len)
{
	static char base64_alphabet[]	=	"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+-";

	__asm{
		mov	esi, edx
			mov	edi, ecx
			push	edi
			mov	ecx, ul_src_len
			mov	ebx, offset base64_alphabet;

aa10:
		cmp	ecx, 4
			jb		aa11
			sub	ecx, 3
			lodsd
			dec	esi
			bswap	eax

			rol	eax, 6
			and	al, 63
			xlat
			stosb

			rol	eax, 6
			and	al, 63
			xlat
			stosb

			rol	eax, 6
			and	al, 63
			xlat
			stosb

			rol	eax, 6
			and	al, 63
			xlat
			stosb

			jmp	aa10
aa11:
		mov	edx, ecx
aa20:
		dec	ecx
			jl		aa21
			lodsb
			shl	eax, 8
			jmp	aa20
aa21:
		mov	ecx, edx
			sub	ecx, 3
			neg	ecx
			mov	edx, ecx
aa30:
		dec	ecx
			jl		aa31
			shl	eax, 8
			jmp	aa30
aa31:
		rol	eax, 6
			and	al, 63
			xlat
			stosb

			rol	eax, 6
			and	al, 63
			xlat
			stosb

			rol	eax, 6
			and	al, 63
			xlat
			stosb

			rol	eax, 6
			and	al, 63
			xlat
			stosb

			sub	edi, edx
			mov	al, '='
			mov	ecx, edx
			repne	stosb
			mov	al, 0
			stosb
			pop	eax
			sub	eax, edi
			neg	eax
			dec	eax
	}
}
unsigned long __fastcall base64_decode(char *p_decode, const char *p_src, unsigned long ul_src_len)
{
	__asm{
		mov	esi, edx
			mov	edi, ecx
			mov	ebx, ecx
			mov	ecx, ul_src_len
			shr	ecx, 2

aa10:
		dec	ecx
			jl		aa18
			xor	edx, edx
			lodsd

aa11:		 
		//	A..Z
		cmp	al, 'A'
			jb		aa13		//	0..9 , + , /
			cmp	al, 'Z'
			jg		aa12		//	a..z
			sub	al, 'A'
			jmp	aa17		//	success

aa12:	//	a..z
		cmp	al, 'a'
			jb		aa16		//	error: bad symbol
			cmp	al, 'z'
			jg		aa16		//	error: bad symbol
			sub	al, 'a' - 'Z' + 'A' - 1
			jmp	aa17		//	success

aa13:	//	0..9
		cmp	al, '0'
			jb		aa14		//	+ , /
			cmp	al, '9'
			jg		aa16		//	error: bad symbol
			sub	al, '0' - 'Z' + 'A' - 'z' + 'a' - 2;
		jmp	aa17		//	success

aa14:	//	+ , /
		cmp	al, '+'
			jnz	aa15		//	/
			mov	al, 62
			jmp	aa17		//	success
aa15:
		cmp	al, '-'
			jnz	aa16		//	error: bad symbol
			mov	al, 63
			jmp	aa17		//	success
aa16:
		xor	ecx, ecx

aa20:
		dec	ecx
			shl	edx, 6
			shr	eax, 8
			jnz	aa20		 
			shl	edx, 2
			mov	eax, edx
			bswap	eax
			stosd
			sub	ebx, edi
			neg	ebx
			lea	ebx, [ebx + ecx - 1]

		jmp	aa19


aa17:	//	symbol was decoding
		or		dl, al
			shl	edx, 6
			shr	eax, 8
			jnz	aa11

			shl	edx, 2
			bswap	edx
			mov	eax, edx

			stosd
			dec	edi
			jmp	aa10
aa18:
		sub	ebx, edi
			neg	ebx	
aa19:
		mov	eax, ebx
	}
}
#endif /* CRC__H */
