// Copyright (c) 2011 Google, Inc.

/*
//测试代码
#include <windows.h>
#include "cityhash.h"

int main()
{
	uint128 seed = CityHash128("Maipu",5);

	for(int i=0;i<100;i++)
	{
		uint128 x = CityHash128WithSeed("123123123123123123",18,seed);
		//uint128 x = CityHash128("123",3);
		printf("%I64X%llX\n",x.first,x.second);
	}
	getchar();
}
*/
#pragma once
#ifndef CITY_HASH_H_
#define CITY_HASH_H_

#include <stdlib.h>  // for size_t.
//#include <stdint.h>
#include <utility>


typedef long ssize_t;


#include <algorithm>

using namespace std;

typedef unsigned char   uint8_t;
typedef unsigned short  uint16_t;
typedef unsigned   uint32_t;
typedef unsigned long long   uint64_t;
//typedef int ssize_t;

typedef uint8_t uint8;
typedef uint32_t uint32;
typedef uint64_t uint64;
typedef std::pair<uint64, uint64> uint128;


#define UNALIGNED_LOAD64(p) (*(const uint64*)(p))
#define UNALIGNED_LOAD32(p) (*(const uint32*)(p))

#if !defined(LIKELY)
#if defined(__GNUC__)
#define LIKELY(x) (__builtin_expect(!!(x), 1))
#else
#define LIKELY(x) (x)
#endif
#endif

// Some primes between 2^63 and 2^64 for various uses.
static const uint64 k0 = 0xc3a5c85c97cb3127ULL;
static const uint64 k1 = 0xb492b66fbe98f273ULL;
static const uint64 k2 = 0x9ae16a3b2f90404fULL;
static const uint64 k3 = 0xc949d7c7509e6557ULL;


inline uint64 Uint128Low64(const uint128& x)
{
    return x.first;
}
inline uint64 Uint128High64(const uint128& x)
{
    return x.second;
}

// Hash function for a byte array.
uint64 CityHash64(const char *buf, size_t len);

// Hash function for a byte array.  For convenience, a 64-bit seed is also
// hashed into the result.
uint64 CityHash64WithSeed(const char *buf, size_t len, uint64 seed);

// Hash function for a byte array.  For convenience, two seeds are also
// hashed into the result.
uint64 CityHash64WithSeeds(const char *buf, size_t len,
                           uint64 seed0, uint64 seed1);

// Hash function for a byte array.
uint128 CityHash128(const char *s, size_t len);

// Hash function for a byte array.  For convenience, a 128-bit seed is also
// hashed into the result.
uint128 CityHash128WithSeed(const char *s, size_t len, uint128 seed);

// Hash 128 input bits down to 64 bits of output.
// This is intended to be a reasonably good hash function.
inline uint64 Hash128to64(const uint128& x)
{
    // Murmur-inspired hashing.
    const uint64 kMul = 0x9ddfea08eb382d69ULL;
    uint64 a = (Uint128Low64(x) ^ Uint128High64(x)) * kMul;
    a ^= (a >> 47);
    uint64 b = (Uint128High64(x) ^ a) * kMul;
    b ^= (b >> 47);
    b *= kMul;
    return b;
}


// Bitwise right rotate.  Normally this will compile to a single
// instruction, especially if the shift is a manifest constant.
static uint64 Rotate(uint64 val, int shift)
{
    // Avoid shifting by 64: doing so yields an undefined result.
    return shift == 0 ? val : ((val >> shift) | (val << (64 - shift)));
}

// Equivalent to Rotate(), but requires the second arg to be non-zero.
// On x86-64, and probably others, it's possible for this to compile
// to a single instruction if both args are already in registers.
static uint64 RotateByAtLeast1(uint64 val, int shift)
{
    return (val >> shift) | (val << (64 - shift));
}

static uint64 ShiftMix(uint64 val)
{
    return val ^ (val >> 47);
}

static uint64 HashLen16(uint64 u, uint64 v)
{
    return Hash128to64(uint128(u, v));
}

static uint64 HashLen0to16(const char *s, size_t len)
{
    if (len > 8)
    {
        uint64 a = UNALIGNED_LOAD64(s);
        uint64 b = UNALIGNED_LOAD64(s + len - 8);
        return HashLen16(a, RotateByAtLeast1(b + len, len)) ^ b;
    }
    if (len >= 4)
    {
        uint64 a = UNALIGNED_LOAD32(s);
        return HashLen16(len + (a << 3), UNALIGNED_LOAD32(s + len - 4));
    }
    if (len > 0)
    {
        uint8 a = s[0];
        uint8 b = s[len >> 1];
        uint8 c = s[len - 1];
        uint32 y = static_cast<uint32>(a) + (static_cast<uint32>(b) << 8);
        uint32 z = len + (static_cast<uint32>(c) << 2);
        return ShiftMix(y * k2 ^ z * k3) * k2;
    }
    return k2;
}

// This probably works well for 16-byte strings as well, but it may be overkill
// in that case.
static uint64 HashLen17to32(const char *s, size_t len)
{
    uint64 a = UNALIGNED_LOAD64(s) * k1;
    uint64 b = UNALIGNED_LOAD64(s + 8);
    uint64 c = UNALIGNED_LOAD64(s + len - 8) * k2;
    uint64 d = UNALIGNED_LOAD64(s + len - 16) * k0;
    return HashLen16(Rotate(a - b, 43) + Rotate(c, 30) + d,
                     a + Rotate(b ^ k3, 20) - c + len);
}

// Return a 16-byte hash for 48 bytes.  Quick and dirty.
// Callers do best to use "random-looking" values for a and b.
static pair<uint64, uint64> WeakHashLen32WithSeeds(
    uint64 w, uint64 x, uint64 y, uint64 z, uint64 a, uint64 b)
{
    a += w;
    b = Rotate(b + a + z, 21);
    uint64 c = a;
    a += x;
    a += y;
    b += Rotate(a, 44);
    return make_pair(a + z, b + c);
}

// Return a 16-byte hash for s[0] ... s[31], a, and b.  Quick and dirty.
static pair<uint64, uint64> WeakHashLen32WithSeeds(
    const char* s, uint64 a, uint64 b)
{
    return WeakHashLen32WithSeeds(UNALIGNED_LOAD64(s),
                                  UNALIGNED_LOAD64(s + 8),
                                  UNALIGNED_LOAD64(s + 16),
                                  UNALIGNED_LOAD64(s + 24),
                                  a,
                                  b);
}

// Return an 8-byte hash for 33 to 64 bytes.
static uint64 HashLen33to64(const char *s, size_t len)
{
    uint64 z = UNALIGNED_LOAD64(s + 24);
    uint64 a = UNALIGNED_LOAD64(s) + (len + UNALIGNED_LOAD64(s + len - 16)) * k0;
    uint64 b = Rotate(a + z, 52);
    uint64 c = Rotate(a, 37);
    a += UNALIGNED_LOAD64(s + 8);
    c += Rotate(a, 7);
    a += UNALIGNED_LOAD64(s + 16);
    uint64 vf = a + z;
    uint64 vs = b + Rotate(a, 31) + c;
    a = UNALIGNED_LOAD64(s + 16) + UNALIGNED_LOAD64(s + len - 32);
    z = UNALIGNED_LOAD64(s + len - 8);
    b = Rotate(a + z, 52);
    c = Rotate(a, 37);
    a += UNALIGNED_LOAD64(s + len - 24);
    c += Rotate(a, 7);
    a += UNALIGNED_LOAD64(s + len - 16);
    uint64 wf = a + z;
    uint64 ws = b + Rotate(a, 31) + c;
    uint64 r = ShiftMix((vf + ws) * k2 + (wf + vs) * k0);
    return ShiftMix(r * k0 + vs) * k2;
}

uint64 CityHash64(const char *s, size_t len)
{
    if (len <= 32)
    {
        if (len <= 16)
        {
            return HashLen0to16(s, len);
        }
        else
        {
            return HashLen17to32(s, len);
        }
    }
    else if (len <= 64)
    {
        return HashLen33to64(s, len);
    }

    // For strings over 64 bytes we hash the end first, and then as we
    // loop we keep 56 bytes of state: v, w, x, y, and z.
    uint64 x = UNALIGNED_LOAD64(s);
    uint64 y = UNALIGNED_LOAD64(s + len - 16) ^ k1;
    uint64 z = UNALIGNED_LOAD64(s + len - 56) ^ k0;
    pair<uint64, uint64> v = WeakHashLen32WithSeeds(s + len - 64, len, y);
    pair<uint64, uint64> w = WeakHashLen32WithSeeds(s + len - 32, len * k1, k0);
    z += ShiftMix(v.second) * k1;
    x = Rotate(z + x, 39) * k1;
    y = Rotate(y, 33) * k1;

    // Decrease len to the nearest multiple of 64, and operate on 64-byte chunks.
    len = (len - 1) & ~static_cast<size_t>(63);
    do
    {
        x = Rotate(x + y + v.first + UNALIGNED_LOAD64(s + 16), 37) * k1;
        y = Rotate(y + v.second + UNALIGNED_LOAD64(s + 48), 42) * k1;
        x ^= w.second;
        y ^= v.first;
        z = Rotate(z ^ w.first, 33);
        v = WeakHashLen32WithSeeds(s, v.second * k1, x + w.first);
        w = WeakHashLen32WithSeeds(s + 32, z + w.second, y);
        std::swap(z, x);
        s += 64;
        len -= 64;
    }
    while (len != 0);
    return HashLen16(HashLen16(v.first, w.first) + ShiftMix(y) * k1 + z,
                     HashLen16(v.second, w.second) + x);
}

uint64 CityHash64WithSeed(const char *s, size_t len, uint64 seed)
{
    return CityHash64WithSeeds(s, len, k2, seed);
}

uint64 CityHash64WithSeeds(const char *s, size_t len,
                           uint64 seed0, uint64 seed1)
{
    return HashLen16(CityHash64(s, len) - seed0, seed1);
}

// A subroutine for CityHash128().  Returns a decent 128-bit hash for strings
// of any length representable in ssize_t.  Based on City and Murmur.
static uint128 CityMurmur(const char *s, size_t len, uint128 seed)
{
    uint64 a = Uint128Low64(seed);
    uint64 b = Uint128High64(seed);
    uint64 c = 0;
    uint64 d = 0;
    ssize_t l = len - 16;
    if (l <= 0)    // len <= 16
    {
        c = b * k1 + HashLen0to16(s, len);
        d = Rotate(a + (len >= 8 ? UNALIGNED_LOAD64(s) : c), 32);
    }
    else      // len > 16
    {
        c = HashLen16(UNALIGNED_LOAD64(s + len - 8) + k1, a);
        d = HashLen16(b + len, c + UNALIGNED_LOAD64(s + len - 16));
        a += d;
        do
        {
            a ^= ShiftMix(UNALIGNED_LOAD64(s) * k1) * k1;
            a *= k1;
            b ^= a;
            c ^= ShiftMix(UNALIGNED_LOAD64(s + 8) * k1) * k1;
            c *= k1;
            d ^= c;
            s += 16;
            l -= 16;
        }
        while (l > 0);
    }
    a = HashLen16(a, c);
    b = HashLen16(d, b);
    return uint128(a ^ b, HashLen16(b, a));
}

uint128 CityHash128WithSeed(const char *s, size_t len, uint128 seed)
{
    if (len < 128)
    {
        return CityMurmur(s, len, seed);
    }

    // We expect len >= 128 to be the common case.  Keep 56 bytes of state:
    // v, w, x, y, and z.
    pair<uint64, uint64> v, w;
    uint64 x = Uint128Low64(seed);
    uint64 y = Uint128High64(seed);
    uint64 z = len * k1;
    v.first = Rotate(y ^ k1, 49) * k1 + UNALIGNED_LOAD64(s);
    v.second = Rotate(v.first, 42) * k1 + UNALIGNED_LOAD64(s + 8);
    w.first = Rotate(y + z, 35) * k1 + x;
    w.second = Rotate(x + UNALIGNED_LOAD64(s + 88), 53) * k1;

    // This is the same inner loop as CityHash64(), manually unrolled.
    do
    {
        x = Rotate(x + y + v.first + UNALIGNED_LOAD64(s + 16), 37) * k1;
        y = Rotate(y + v.second + UNALIGNED_LOAD64(s + 48), 42) * k1;
        x ^= w.second;
        y ^= v.first;
        z = Rotate(z ^ w.first, 33);
        v = WeakHashLen32WithSeeds(s, v.second * k1, x + w.first);
        w = WeakHashLen32WithSeeds(s + 32, z + w.second, y);
        std::swap(z, x);
        s += 64;
        x = Rotate(x + y + v.first + UNALIGNED_LOAD64(s + 16), 37) * k1;
        y = Rotate(y + v.second + UNALIGNED_LOAD64(s + 48), 42) * k1;
        x ^= w.second;
        y ^= v.first;
        z = Rotate(z ^ w.first, 33);
        v = WeakHashLen32WithSeeds(s, v.second * k1, x + w.first);
        w = WeakHashLen32WithSeeds(s + 32, z + w.second, y);
        std::swap(z, x);
        s += 64;
        len -= 128;
    }
    while (LIKELY(len >= 128));
    y += Rotate(w.first, 37) * k0 + z;
    x += Rotate(v.first + z, 49) * k0;
    // If 0 < len < 128, hash up to 4 chunks of 32 bytes each from the end of s.
    for (size_t tail_done = 0; tail_done < len; )
    {
        tail_done += 32;
        y = Rotate(y - x, 42) * k0 + v.second;
        w.first += UNALIGNED_LOAD64(s + len - tail_done + 16);
        x = Rotate(x, 49) * k0 + w.first;
        w.first += v.first;
        v = WeakHashLen32WithSeeds(s + len - tail_done, v.first, v.second);
    }
    // At this point our 48 bytes of state should contain more than
    // enough information for a strong 128-bit hash.  We use two
    // different 48-byte-to-8-byte hashes to get a 16-byte final result.
    x = HashLen16(x, v.first);
    y = HashLen16(y, w.first);
    return uint128(HashLen16(x + v.second, w.second) + y,
                   HashLen16(x + w.second, y + v.second));
}

uint128 CityHash128(const char *s, size_t len)
{
    if (len >= 16)
    {
        return CityHash128WithSeed(s + 16,
                                   len - 16,
                                   uint128(UNALIGNED_LOAD64(s) ^ k3,
                                           UNALIGNED_LOAD64(s + 8)));
    }
    else if (len >= 8)
    {
        return CityHash128WithSeed(NULL,
                                   0,
                                   uint128(UNALIGNED_LOAD64(s) ^ (len * k0),
                                           UNALIGNED_LOAD64(s + len - 8) ^ k1));
    }
    else
    {
        return CityHash128WithSeed(s, len, uint128(k0, k1));
    }
}

void tea_decrypt(uint64 &pData)
{
	DWORD  Key1 =  0x9e3779b9;

	DWORD  FKey1 = 0xF03E934F;
	DWORD  FKey2 = 0x27BDB886;
	DWORD  LKey1 = 0xD0AAE945;
	DWORD  LKey2 = 0x993BA3AE;
	DWORD FirstData, LastData, Count, Key;

	Count = 32;
	Key = Key1 << 5;//Key=（Key1*循环次数），这里是32即2的5次方
	FirstData = pData>>32;
	LastData = (DWORD)pData;
    
	do
	{
		LastData = LastData - ((LKey2 + (FirstData << 4)) ^ (LKey1 + (FirstData >> 5)) ^ (Key + FirstData));
		FirstData = FirstData - ((FKey2 + (LastData << 4)) ^ (FKey1 + (LastData >> 5)) ^ (Key + LastData));
		Key = Key - Key1;
		Count--;
	}
	while (Count != 0);
	
	pData = FirstData;
	pData = pData<<32;
	pData += LastData;
}
#endif  // CITY_HASH_H_
