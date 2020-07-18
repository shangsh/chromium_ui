
#ifndef __pickle_h__
#define __pickle_h__

#pragma once

#include <string>

#include "basic_types.h"
#include "logging.h"
#include "string16.h"

// Ϊ���������ݷ������ṩ������ʩ.
//
// Pickle��֧����ӻ�������(ints��strings��), ��̬�����ڴ��Դ洢��������.
// ͨ��"data"��¶Pickle�ڲ��洢, �����ڳ�ʼ��Pickle�����Զ�ȡ����.
//
// ��Pickle�����ȡ����ʱ, ʹ������Ҫ֪����ȡ���������ͺʹ���, ��ΪPickle
// д���ݵ�ʱ��û��ά��������Ϣ, ��һ�����Ҫ.
//
// Pickle������ǰ����һ��ͷ, ��¼�����ݵ���Ч����, Ҳ��֧��������Ϣ�洢.
// ͷռ�õĴ�Сͨ��Pickle���캯����header_size����ָ��.

class Pickle
{
public:
    virtual ~Pickle();

    // ʹ��ȱʡ��ͷ���ȳ�ʼ��Pickle����.
    Pickle();

    // ʹ��ָ����С��ͷ����(�ֽ�)��ʼ��Pickle����, ������ڵ���
    // sizeof(Pickle::Header). header_size������ȡ����֤32λ����.
    explicit Pickle(int header_size);

    // ʹ��һ���ڴ����ݳ�ʼ��Pickle����, Pickleֻ�����ö�����������. ���ַ�ʽ
    // ��ʼ����Pickleֻ�������const����. ͷռ�ô�С����ͨ��data_len�Ƶ�����.
    Pickle(const char* data, int data_len);

    // �������.
    Pickle(const Pickle& other);

    // �ֵ����.
    Pickle& operator=(const Pickle& other);

    // ����Pickle�����ݳ���.
    int size() const
    {
        return static_cast<int>(header_size_+header_->payload_size);
    }

    // ����Pickle������.
    const void* data() const { return header_; }

    // ��ȡPickle��Ч���ݵķ���. ��ʼ��*iterΪNULL��ʾ��ͷ��ʼ��. ����ɹ�, ����
    // ����true, ����false��ʾ�޷���ȡ���ݵ�result.
    bool ReadBool(void** iter, bool* result) const;
    bool ReadInt(void** iter, int* result) const;
    bool ReadLong(void** iter, long* result) const;
    bool ReadSize(void** iter, size_t* result) const;
    bool ReadUInt32(void** iter, uint32* result) const;
    bool ReadInt64(void** iter, int64* result) const;
    bool ReadUInt64(void** iter, uint64* result) const;
    bool ReadString(void** iter, std::string* result) const;
    bool ReadWString(void** iter, std::wstring* result) const;
    bool ReadString16(void** iter, string16* result) const;
    bool ReadData(void** iter, const char** data, int* length) const;
    bool ReadBytes(void** iter, const char** data, int length) const;

    // ��ReadInt()��ȫ, �������Ƿ�Ϊ����, ���ڶ�ȡ���󳤶�.
    bool ReadLength(void** iter, int* result) const;

    // Pickle�����Ч���ݵķ���, �Ӻ�˼���. ��ȡ��ʱ���������˳��һ��.
    bool WriteBool(bool value)
    {
        return WriteInt(value ? 1 : 0);
    }
    bool WriteInt(int value)
    {
        return WriteBytes(&value, sizeof(value));
    }
    bool WriteLong(long value)
    {
        return WriteBytes(&value, sizeof(value));
    }
    bool WriteSize(size_t value)
    {
        return WriteBytes(&value, sizeof(value));
    }
    bool WriteUInt32(uint32 value)
    {
        return WriteBytes(&value, sizeof(value));
    }
    bool WriteInt64(int64 value)
    {
        return WriteBytes(&value, sizeof(value));
    }
    bool WriteUInt64(uint64 value)
    {
        return WriteBytes(&value, sizeof(value));
    }
    bool WriteString(const std::string& value);
    bool WriteWString(const std::wstring& value);
    bool WriteString16(const string16& value);
    bool WriteData(const char* data, int length);
    bool WriteBytes(const void* data, int data_len);

    // ����WriteData, �����������ֱ��дPickle����. ����д���ݲ����ڴ���ʱ
    // ���Խ�ʡһ�ο�������.��������Ҫע��д���ݳ��Ȳ�Ҫ���������. ��ȡ
    // ������ReadData. ʧ�ܷ���NULL.
    //
    // ���ص�ָ�����´�д����ǰ����Ч��.
    char* BeginWriteData(int length);

    // ���ڿɱ䳤�ȵĻ�����(����ͨ��BeginWriteData������), ���������Ҫ�ĳ���
    // С��ԭʼ�����, ���������. ������˵, �㴴����һ��10K�����ݻ�����, ��
    // ʵ��ֻ�����10�ֽ�����. ʹ������������԰�ʣ���9990�ֽ�������. ������
    // ��С��������, ֻ�ܼ���. �����ٶ����������ȴ�δ�ı��.
    void TrimWriteData(int length);

    // ��Ч���ݸ���Header����(ͷ���ȿɶ���).
    struct Header
    {
        uint32 payload_size; // ��Ч���ݳ���.
    };

    // ����ͷ��Ϣ, ǿת���û�ָ������. T���ͱ�����Header������, ��С�ʹ���
    // ��Pickle���캯����header_size����һ��.
    template<class T>
    T* headerT()
    {
        DCHECK(sizeof(T) == header_size_);
        return static_cast<T*>(header_);
    }
    template<class T>
    const T* headerT() const
    {
        DCHECK(sizeof(T) == header_size_);
        return static_cast<const T*>(header_);
    }

    // ���������ָ������ݳ���>=len����true. ����ռ䲻�㷵��false.
    bool IteratorHasRoomFor(const void* iter, int len) const
    {
        if((len<0) || (iter<header_) || iter>end_of_payload())
        {
            return false;
        }
        const char* end_of_region = reinterpret_cast<const char*>(iter) + len;
        // ���ķ�װ������ָ���������.
        return (iter<=end_of_region) && (end_of_region<=end_of_payload());
    }

protected:
    size_t payload_size() const { return header_->payload_size; }

    char* payload()
    {
        return reinterpret_cast<char*>(header_) + header_size_;
    }
    const char* payload() const
    {
        return reinterpret_cast<const char*>(header_) + header_size_;
    }

    // ������Ч���ݺ�����ڴ��ַ.
    char* end_of_payload()
    {
        return payload() + payload_size();
    }
    const char* end_of_payload() const
    {
        return payload() + payload_size();
    }

    size_t capacity() const
    {
        return capacity_;
    }

    // Ϊд����׼��������, ���ص�ָ����д��ַ, ��������ʱ����NULL.
    // ����EndWriteΪ��һ��д������������.
    char* BeginWrite(size_t length);

    // ��NULL�ֽڲ���ʹ���ݶ���, ���д����. Ӧ����BeginWrite���ʹ��, ����д��
    // �󲻱�Ҫ�ٵ���.
    void EndWrite(char* dest, int length);

    // ��������, ע��new_capacity��СӦ�ð���ͷ��С:
    // new_capacity = sizeof(Header) + desired_payload_capacity.
    // realloc()ʧ�ܽ��ᵼ�º���ʧ��, ��������Ҫ��鷵��ֵ�Ƿ�Ϊtrue.
    bool Resize(size_t new_capacity);

    // 'i'����ȡ��Ϊ'alignment'�ı���.
    static size_t AlignInt(size_t i, int alignment)
    {
        return i + (alignment - (i % alignment)) % alignment;
    }

    // ��������ǰ�ƶ�һ���������ֽ�, ȷ������. �����ָ�벻�Ƕ����, ���޸ĺ�
    // һ����sizeof(uint32)�ı���.
    static void UpdateIter(void** iter, int bytes)
    {
        *iter = static_cast<char*>(*iter) + AlignInt(bytes, sizeof(uint32));
    }

    // ��Ч���ݷ�������.
    static const int kPayloadUnit;

private:
    Header* header_;
    size_t header_size_; // ֧��������Ϣ�洢.
    // ��Ч�����ڴ����Ĵ�С(-1��ʾֻ��).
    size_t capacity_;
    size_t variable_buffer_offset_; // �����0, ��ʾ������ƫ��.
};

#endif //__pickle_h__