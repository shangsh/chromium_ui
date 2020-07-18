
#ifndef __string_number_conversions_h__
#define __string_number_conversions_h__

#pragma once

#include <string>
#include <vector>

#include "basic_types.h"
#include "string16.h"

// ----------------------------------------------------------------------------
// ����ߵ��Ҹ�
//
// ���ļ�������"wstring"�汾, �´���Ӧ��ʹ��string16, ͨ��UTF8ת�������ϵĴ���.
// ��Ҫ���"wstring"�汾.
//
// �����Ӻ��Գɹ�/ʧ��ֱ�ӷ�����ֵ���ַ���������ת����ν��"���õ�"����. ����
// ������ʹ���߱�д�������������Ĵ���.
// ----------------------------------------------------------------------------

namespace base
{

    // ���� -> �ַ��� ת�� ------------------------------------------------
    std::string IntToString(int value);
    string16 IntToString16(int value);

    std::string UintToString(unsigned value);
    string16 UintToString16(unsigned value);

    std::string Int64ToString(int64 value);
    string16 Int64ToString16(int64 value);

    std::string Uint64ToString(uint64 value);
    string16 Uint64ToString16(uint64 value);

    std::string DoubleToString(double value);

    // �ַ��� -> ���� ת�� ------------------------------------------------

    // �ַ���ת��Ϊ��������Ч�ķ�ʽ.
    // |*output|��ת���Ľ��. ת���ɹ�����true; �����������false:
    // - ���/Խ��. |*output|����ֵΪ������֧�ֵ����ֵ.
    // - �����ַ�������������ַ�. |*output|����ֵΪ�������ֲ��ֵ�ֵ.
    // - �ַ�����ͷû�н����������ַ�. |*output|����ֵΪ0.
    // - ���ַ���. |*output|����ֵΪ0.
    bool StringToInt(const std::string& input, int* output);
    bool StringToInt(const string16& input, int* output);
    bool StringToInt64(const std::string& input, int64* output);
    bool StringToInt64(const string16& input, int64* output);

    // ��������ת��ֻ֧��10���Ƹ�ʽ, ��֧��16���ƺ�����ֵ(����NaN).
    bool StringToDouble(const std::string& input, double* output);

    // 16���Ʊ��� ----------------------------------------------------------------

    // ���ض����ƻ�������16�����ַ���, ��д��ʽ���. ������У��|size|���ȵĺ�����.
    // ������ɸ�ʽ���Ľ���ܴ�, |size|�����ֵӦ����
    //     std::numeric_limits<size_t>::max() / 2
    std::string HexEncode(const void* bytes, size_t size);

    // ע��StringToInt������.
    bool HexStringToInt(const std::string& input, int* output);

    // ��ǰһ��������, ���Ϊbytes��vector. |*output|������������ǰ�����ܶ���ֽ�.
    // ���ﲻ�����, ����input.size()������ż��. 0x����+/-ǰ׺�ǲ������.
    bool HexStringToBytes(const std::string& input, std::vector<uint8>* output);

} //namespace base

#endif //__string_number_conversions_h__