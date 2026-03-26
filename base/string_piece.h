
#ifndef __string_piece_h__
#define __string_piece_h__

#pragma once

#include <string>
#include <cstring>
#include <cstdlib>

namespace base
{

    // ָ��̶���С�ڴ���ַ�����.
    //
    // �������߷�������ʹ��const StringPiece&����������
    // "const char*"��"string"ֵ, ������������ʽת��,
    // ����һ��ֱ�Ӱ�����ͷ�ļ�, û��Ҫǰ������StringPiece.
    //
    // �����ʹ��StringPiece�Լ���"const char*"��"string"֮��ת������.
    class StringPiece
    {
    public:
        typedef size_t size_type;

    private:
        const char* ptr_;
        size_type length_;

    public:
        StringPiece() : ptr_(NULL), length_(0) {}
        StringPiece(const char* str)
            : ptr_(str), length_((str==NULL)?0:strlen(str)) {}
        StringPiece(const std::string& str)
            : ptr_(str.data()), length_(str.size()) {}
        StringPiece(const char* offset, size_type len)
            : ptr_(offset), length_(len) {}

        // data()���ص��ڴ��ָ�����Ƕ��NULs, ���Ҹ��ڴ�����
        // ����null��β. ��data()���ݸ���NUL��β�Ĺ��̵����Ǵ����.
        const char* data() const { return ptr_; }
        size_type size() const { return length_; }
        size_type length() const { return length_; }
        bool empty() const { return length_ == 0; }

        void clear()
        {
            ptr_ = NULL;
            length_ = 0;
        }
        void set(const char* data, size_type len)
        {
            ptr_ = data;
            length_ = len;
        }
        void set(const char* str)
        {
            ptr_ = str;
            length_ = str ? strlen(str) : 0;
        }
        void set(const void* data, size_type len)
        {
            ptr_ = reinterpret_cast<const char*>(data);
            length_ = len;
        }

        char operator[](size_type i) const { return ptr_[i]; }

        void remove_prefix(size_type n)
        {
            ptr_ += n;
            length_ -= n;
        }

        void remove_suffix(size_type n)
        {
            length_ -= n;
        }

        int compare(const StringPiece& x) const
        {
            int r = wordmemcmp(ptr_, x.ptr_, __min(length_, x.length_));
            if(r == 0)
            {
                if(length_ < x.length_) r = -1;
                else if(length_ > x.length_) r = +1;
            }
            return r;
        }

        std::string as_string() const
        {
            // std::string��֧��NULLָ��, ��ʹsizeΪ0.
            return std::string(!empty()?data():"", size());
        }

        void CopyToString(std::string& target) const;
        void AppendToString(std::string& target) const;

        // �Ƿ���"x"��ʼ
        bool starts_with(const StringPiece& x) const
        {
            return ((length_>=x.length_) &&
                (wordmemcmp(ptr_, x.ptr_, x.length_)==0));
        }

        // �Ƿ���"x"��β
        bool ends_with(const StringPiece& x) const
        {
            return ((length_>=x.length_) &&
                (wordmemcmp(ptr_+(length_-x.length_), x.ptr_, x.length_)==0));
        }

        // ��׼��STL��������֧��
        typedef char value_type;
        typedef const char* pointer;
        typedef const char& reference;
        typedef const char& const_reference;
        typedef ptrdiff_t difference_type;
        static const size_type npos;
        typedef const char* const_iterator;
        typedef const char* iterator;
        typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
        typedef std::reverse_iterator<iterator> reverse_iterator;
        iterator begin() const { return ptr_; }
        iterator end() const { return ptr_ + length_; }
        const_reverse_iterator rbegin() const
        {
            return const_reverse_iterator(ptr_+length_);
        }
        const_reverse_iterator rend() const
        {
            return const_reverse_iterator(ptr_);
        }

        size_type max_size() const { return length_; }
        size_type capacity() const { return length_; }

        size_type copy(char* buf, size_type n, size_type pos=0) const;

        size_type find(const StringPiece& s, size_type pos=0) const;
        size_type find(char c, size_type pos=0) const;
        size_type rfind(const StringPiece& s, size_type pos=npos) const;
        size_type rfind(char c, size_type pos=npos) const;

        size_type find_first_of(const StringPiece& s, size_type pos=0) const;
        size_type find_first_of(char c, size_type pos=0) const
        {
            return find(c, pos);
        }
        size_type find_first_not_of(const StringPiece& s, size_type pos=0) const;
        size_type find_first_not_of(char c, size_type pos=0) const;
        size_type find_last_of(const StringPiece& s, size_type pos=npos) const;
        size_type find_last_of(char c, size_type pos = npos) const
        {
            return rfind(c, pos);
        }
        size_type find_last_not_of(const StringPiece& s, size_type pos=npos) const;
        size_type find_last_not_of(char c, size_type pos=npos) const;

        StringPiece substr(size_type pos, size_type n=npos) const;

        static int wordmemcmp(const char* p, const char* p2, size_type N)
        {
            return memcmp(p, p2, N);
        }
    };

    bool operator==(const StringPiece& x, const StringPiece& y);

    inline bool operator!=(const StringPiece& x, const StringPiece& y)
    {
        return !(x == y);
    }

    inline bool operator<(const StringPiece& x, const StringPiece& y)
    {
        const int r = StringPiece::wordmemcmp(x.data(), y.data(),
            __min(x.size(), y.size()));
        return ((r<0) || ((r==0) && (x.size()<y.size())));
    }

    inline bool operator>(const StringPiece& x, const StringPiece& y)
    {
        return y < x;
    }

    inline bool operator<=(const StringPiece& x, const StringPiece& y)
    {
        return !(x > y);
    }

    inline bool operator>=(const StringPiece& x, const StringPiece& y)
    {
        return !(x < y);
    }

    extern std::ostream& operator<<(std::ostream& o, const StringPiece& piece);

} //namespace base

#endif //__string_piece_h__