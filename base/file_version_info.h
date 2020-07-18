
#ifndef __file_version_info_h__
#define __file_version_info_h__

#include <windows.h>

#include <string>

namespace base
{
    class FilePath;
}

// ����Ļ�ȡ�ļ��İ汾��Ϣ.
class FileVersionInfo
{
public:
    ~FileVersionInfo();

    // ����ָ��·���ļ���FileVersionInfo����. ����(ͨ���ǲ����ڻ����޷���)����
    // NULL. ���صĶ���ʹ����֮����Ҫɾ��.
    static FileVersionInfo* CreateFileVersionInfo(const base::FilePath& file_path);

    // ������ǰģ���FileVersionInfo����. ���󷵻�NULL. ���صĶ���ʹ����֮����Ҫ
    // ɾ��.
    static FileVersionInfo* CreateFileVersionInfoForCurrentModule();

    // ��ȡ�汾��Ϣ�ĸ�������, �����ڷ��ؿ�.
    virtual std::wstring company_name();
    virtual std::wstring company_short_name();
    virtual std::wstring product_name();
    virtual std::wstring product_short_name();
    virtual std::wstring internal_name();
    virtual std::wstring product_version();
    virtual std::wstring private_build();
    virtual std::wstring special_build();
    virtual std::wstring comments();
    virtual std::wstring original_filename();
    virtual std::wstring file_description();
    virtual std::wstring file_version();
    virtual std::wstring legal_copyright();
    virtual std::wstring legal_trademarks();
    virtual std::wstring last_change();
    virtual bool is_official_build();

    // ��ȡ��������, �����������.
    bool GetValue(const wchar_t* name, std::wstring& value);

    // ��GetValueһ��, ֻ�ǲ����ڵ�ʱ�򷵻ؿ��ַ���.
    std::wstring GetStringValue(const wchar_t* name);

    // ��ȡ�̶��İ汾��Ϣ, ��������ڷ���NULL.
    VS_FIXEDFILEINFO* fixed_file_info() { return fixed_file_info_; }

private:
    FileVersionInfo(void* data, int language, int code_page);

    FileVersionInfo(const FileVersionInfo&);
    void operator=(const FileVersionInfo&);

    void* data_;
    int language_;
    int code_page_;
    // ָ��data_�ڲ���ָ��, NULL��ʾ������.
    VS_FIXEDFILEINFO* fixed_file_info_;
};

#endif //__file_version_info_h__