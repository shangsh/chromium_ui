
#include <cassert>
#include <iostream>

#include <shlobj.h>

#include "base/at_exit.h"
#include "base/bit_util.h"
#include "base/command_line.h"
#include "base/cpu.h"
#include "base/environment.h"
#include "base/float_util.h"
#include "base/file_path.h"
#include "base/file_util.h"
#include "base/file_version_info.h"
#include "base/lock.h"
#include "base/pickle.h"
#include "base/registry.h"
#include "base/scoped_comptr.h"
#include "base/scoped_ptr.h"
#include "base/singleton.h"
#include "base/string_number_conversions.h"
#include "base/string_piece.h"
#include "base/string_split.h"
#include "base/string_util.h"
#include "base/stringprintf.h"
#include "base/time.h"
#include "base/tuple.h"
#include "base/utf_string_conversions.h"
#include "base/values.h"
#include "base/version.h"
#include "base/win_util.h"
#include "base/windows_version.h"

#include "rfc_algorithm/base64/base64.h"
#include "rfc_algorithm/json/string_escape.h"
#include "rfc_algorithm/json/json_writer.h"
#include "rfc_algorithm/json/json_reader.h"
#include "rfc_algorithm/md5/md5.h"
#include "rfc_algorithm/sha1/sha1.h"
#include "rfc_algorithm/sha2/sha2.h"

#include "message_framework/callback.h"
#include "message_framework/message_loop.h"
#include "message_framework/thread.h"

class FooClass
{
public:
    void Bar() {}

private:
    FooClass() {}
    friend struct base::DefaultSingletonTraits<FooClass>;

    DISALLOW_COPY_AND_ASSIGN(FooClass);
};

class ObjClass
{
public:
    void Bar() {}
};


base::Lock lock;
base::Thread thred("test_thread");
int cnt = 0;
class MainThreadPrintTask : public Task
{
public:
    void Run()
    {
        base::AutoLock al(lock);
        std::cout << "MainThreadPrintTask Run()" << std::endl;
        if(++cnt > 10)
        {
            MessageLoop::current()->Quit();
            return;
        }
        MessageLoop::current()->PostDelayedTask(new MainThreadPrintTask(), 1000);
    }
};

class PrintTask : public Task
{
public:
    void Run()
    {
        base::AutoLock al(lock);
        std::cout << "PrintTask Run()" << std::endl;
        MessageLoop::current()->PostDelayedTask(new PrintTask(), 3000);
    }
};

int main()
{
    base::AtExitManager exit_manager;
    base::CommandLine::Init(0, NULL);
    BaseInitLoggingImpl(L"debug.log", base::LOG_ONLY_TO_FILE,
        base::LOCK_LOG_FILE, base::DELETE_OLD_LOG_FILE);

    int loga = base::Log2Floor(1);

    base::Environment* e = base::Environment::Create();
    std::string sys_path;
    e->GetVar("path", &sys_path);

    base::CPU cpu;

    bool finite = base::IsFinite(1.01);

    base::FilePath path;
    std::vector<std::wstring> components;
    path.GetComponents(&components);
    path.Append(L"c:");
    path.Append(L"pWRD");
    path.Append(L"testDir");
    path.Append(L"\\wlw.txt");
    path.GetComponents(&components);
    bool absolute = path.IsAbsolute();

    base::FilePath path2(L"wlwtxt");
    path2.GetComponents(&components);
    absolute = path2.IsAbsolute();

    base::FilePath search_path(L"C:\\");
    base::CountFilesCreatedAfter(search_path, base::Time::Now());

    int err = base::EnsureDirectoryForFile(path, NULL);
    base::Delete(base::FilePath(L"c:\\pWRD"), true);

    FileVersionInfo* fvi = FileVersionInfo::CreateFileVersionInfoForCurrentModule();
    if(fvi)
    {
        std::wstring file_ver = fvi->file_version();
        delete fvi;
    }

    base::FileEnumerator file_iter(search_path, false,
        base::FileEnumerator::DIRECTORIES, L"3rd");
    for(base::FilePath current=file_iter.Next(); !current.Empty();
        current=file_iter.Next())
    {
        std::wcout << current.value() << std::endl;
        base::FileEnumerator::FindInfo fi;
        file_iter.GetFindInfo(&fi);
        if(file_iter.IsDirectory(fi))
        {
            int64 size = base::ComputeDirectorySize(current);
            std::wcout << L"Directory size is: " << size << std::endl;
            size = base::ComputeFilesSize(current, L"");
            std::wcout << L"Files size is: " << size << std::endl;
        }
    }

    Pickle p;
    p.WriteInt(1);
    p.WriteString("I'm wlw!");
    p.WriteWString(L"I'm WLW!");
    int p_i;
    std::string p_str;
    std::wstring p_wstr;
    void* iter = NULL;
    p.ReadInt(&iter, &p_i);
    p.ReadString(&iter, &p_str);
    p.ReadWString(&iter, &p_wstr);

    base::RegKey reg(HKEY_CURRENT_USER, L"Environment", KEY_QUERY_VALUE);
    std::wstring reg_temp;
    if(reg.Valid())
    {
        reg.ReadValue(L"TEMP", &reg_temp);
    }

    CoInitialize(NULL);
    base::ScopedComPtr<IDropTargetHelper, &IID_IDropTargetHelper> scomp;
    if(SUCCEEDED(scomp.CreateInstance(CLSID_DragDropHelper)))
    {
        scomp = NULL;
    }
    CoUninitialize();

    scoped_ptr<double> spd(new double(3.1));
    spd.reset();

    base::Singleton<FooClass>::get()->Bar();

    int cvt = 0;
    base::StringToInt("123", &cvt);
    std::string str_d = base::DoubleToString(2.123);

    base::StringPiece s1;
    assert(s1.length() == 0);
    assert(s1.size() == 0);
    assert(s1.data() == NULL);

    base::StringPiece s2("I love you");
    assert(s2.find('I') != base::StringPiece::npos);

    std::vector<std::string> v;
    base::SplitString("wlw&el", '&', &v);

    std::vector<std::string> subst;
    subst.push_back("10");
    subst.push_back("20");
    subst.push_back("30");
    std::string add = ReplaceStringPlaceholders("$2+$1=$3", subst, NULL);
    string16 bytes = FormatBytes(5*1024, DATA_UNITS_KIBIBYTE, true);

    std::string profile = base::StringPrintf("wlw's age is %d", 29);

    LOG(WARNING) << "This is a warning!";
    //DCHECK(1 == 0);

    base::Time::EnableHighResolutionTimer(true);
    base::Time t = base::Time::Now();
    base::Time::Exploded te;
    t.LocalExplode(&te);
    base::TimeTicks tt = base::TimeTicks::Now();
    base::TimeTicks tth = base::TimeTicks::HighResNow();

    std::string utf8 = WideToUTF8(L"wan lian wen - ÍòÁ¬ÎÄ");
    std::wstring wide = UTF8ToWide(utf8);

    DictionaryValue root;
    root.SetString("global.pages.homepage", "http://goateleporter.com");
    std::string homepage = "http://google.com";
    root.GetString("global.pages.homepage", &homepage);

    Version* ver = Version::GetVersionFromString("2.0.0.1");
    delete ver;

    base::WinVersion version = base::GetWinVersion();

    std::wstring user_sid;
    base::GetUserSidString(&user_sid);

    std::string base64;
    base::Base64Encode("I'm wlw.", &base64);
    std::string md5 = MD5String("I'm wlw.");
    std::string sha1 = base::SHA1HashString("I'm wlw.");
    std::string sha2 = base::SHA256HashString("I'm wlw.");

    std::string json = base::GetDoubleQuotedJson("a<>b;\nb = 0;");
    std::string json_write;
    base::JSONWriter::Write(&root, false, &json_write);
    Value* json_read = base::JSONReader::Read(json_write, false);
    delete json_read;

    Tuple3<int, double, bool> t3 = MakeTuple(10, 2.5, false);

    ObjClass obj;
    Callback0::Type* callback = NewCallback(&obj, &ObjClass::Bar);
    callback->Run();
    delete callback;

    thred.Start();
    thred.message_loop()->PostDelayedTask(new PrintTask(), 1000);

    MessageLoop msg_loop;
    msg_loop.PostDelayedTask(new MainThreadPrintTask(), 3000);
    msg_loop.Run();

    thred.Stop();

    return 0;
}