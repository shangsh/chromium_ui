
#ifndef __data_pack_h__
#define __data_pack_h__

#pragma once

#include <map>

#include "basic_types.h"
#include "file_util.h"
#include "scoped_ptr.h"

namespace base
{

    class FilePath;
    class RefCountedStaticMemory;
    class StringPiece;

    class DataPack
    {
    public:
        DataPack();
        ~DataPack();

        // ��|path|���ش���ļ�, ��������ʱ����false.
        bool Load(const FilePath& path);

        // ͨ��|resource_id|��ȡ��Դ, ������ݵ�|data|. ���ݹ�DataPack��������,
        // ��Ҫ�޸�. ���û�ҵ���Դid, ����false.
        bool GetStringPiece(uint32 resource_id, StringPiece* data) const;

        // ����GetStringPiece(), ���Ƿ����ڴ��ָ��. ���ӿ�����ͼ������,
        // StringPiece�ӿ�һ�����ڱ����ַ���.
        RefCountedStaticMemory* GetStaticMemory(uint32 resource_id) const;

        // ��|resources|д�뵽·��Ϊ|path|�Ĵ���ļ�.
        static bool WritePack(const FilePath& path,
            const std::map<uint32, StringPiece>& resources);

    private:
        // �ڴ�ӳ������.
        scoped_ptr<MemoryMappedFile> mmap_;

        // �����е���Դ����.
        size_t resource_count_;

        DISALLOW_COPY_AND_ASSIGN(DataPack);
    };

} //namespace base

#endif //__data_pack_h__