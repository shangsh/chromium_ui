
#ifndef __md5_h__
#define __md5_h__

#pragma once

#include <string>

// MD5����Ϣ-ժҪ�㷨5(Message Digest algorithm 5).
// MD5�Ǽ������ȫ����㷺ʹ�õ�һ��ɢ�к���, ���������ļ�У��.
// ���븴������, ���ǳ�ͻ�ĸ��ʺ�С.
// �μ�:
//   http://en.wikipedia.org/wiki/MD5

// ���к�������MD5����. ��򵥵���ֱ�ӵ���MD5Sum()��ָ����������MD5�����.
//
// Ҳ����ͨ����ε���MD5Update()������MD5�����:
//     MD5Context ctx; // intermediate MD5 data: do not use
//     MD5Init(&ctx);
//     MD5Update(&ctx, data1, length1);
//     MD5Update(&ctx, data2, length2);
//     ...
//
//     MD5Digest digest; // the result of the computation
//     MD5Final(&digest, &ctx);
//
// ����MD5DigestToBase16()����ժҪ��.

// MD5�������.
typedef struct MD5Digest_struct
{
    unsigned char a[16];
} MD5Digest;

// �洢MD5������м�����. �����߲���Ҫֱ�ӷ��ʸ�����.
typedef char MD5Context[88];

// ���ڸ�����С�Ļ���������MD5ֵ. �ṹ��'digest'�����洢���.
void MD5Sum(const void* data, size_t length, MD5Digest* digest);

// Ϊ������MD5Update()�ĵ���ʼ��MD5 context�ṹ.
void MD5Init(MD5Context* context);

// ʹ�ø����Ļ��������ݸ���MD5 context�е�У���, ��������п��Զ�ε���,
// ���Ǳ�����MD5Init()֮�����.
void MD5Update(MD5Context* context, const void* buf, size_t len);

// ���MD5������������digest������.
void MD5Final(MD5Digest* digest, MD5Context* pCtx);

// ��ժҪת�����ʺ����Ķ���16����.
std::string MD5DigestToBase16(const MD5Digest& digest);

// �����ַ���string��MD5ֵ(16���ƴ�).
std::string MD5String(const std::string& str);

#endif //__md5_h__