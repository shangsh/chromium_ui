
#ifndef __logging_h__
#define __logging_h__

#pragma once

#include <string>
#include <sstream>

#include "basic_types.h"

// ��ѡ�ĵ�����Ϣ����
// ------------------
// ����ʧ����Ϣ(assertion failed messages)����������(fatal errors)�ڳ����˳�֮ǰ
// ��ʾ�ڶԻ�����. ���жԻ��򴴽�����Ϣѭ���ᴦ��Ӧ�ó�����Ϣ���ַ�����Ӧ�Ĵ���.
// ���ԶԻ������ʱӦ�ñ����Ѿ����ڷǳ�����״̬, ��Щ��Ϣ���ܵò������������
// �Ի�����ߵ��³�����ָ���Ĵ���.
//
// ���������ͨ��һ�������Ľ�������ʾ����Ի�����һ�ֺõĽ���취, ��logging system
// ��Ҫ��ʾ��������Ի����ʱ��������Ŀ¼�²��ҿ�ִ�г���"DebugMessage.exe". ͨ��
// �����еķ�ʽ����"DebugMessage.exe"����, Ϊ�˽�������ͨ����������������������.
//
// DebugMessage.exe��WinMain�������������ֻ��һ�д���:
//     MessageBoxW(NULL, GetCommandLineW(), L"Fatal Error", 0);
//
// ���û���ҵ���ִ�г���"DebugMessage.exe", �Ͳ����������۹��Ļᵼ������ķ�ʽ,
// ֱ�ӵ���MessageBox.


// ʹ��˵��
// --------
// ͨ��һϵ�к���ʵ����־(logging). ����־�ķ�ʽ������LOG(<a particular severity level>)
// ����:
//     LOG(INFO) << "Found " << num_cookies << " cookies";
//
// Ҳ���Լ�����������:
//     LOG_IF(INFO, num_cookies>10) << "Got lots of cookies";
//
// CHECK(condition)����debug��release�汾�ж�������, ͨ��LOG(FATAL)�������̲��һ�
// ����crashdump, ���ǽ����Ѿ������˵�����.
//
// ��һЩ����������debugģʽ�µ�, ����������������:
//     DLOG(INFO) << "Found cookies";
//     DLOG_IF(INFO, num_cookies>10) << "Got lots of cookies";
//
// ����debugģʽ����Ч�ĺ��ڷ�debug�汾�����лᱻ����, ���������κ�ָ��. LOG_IF��
// ��������Ҳ�ܹ�����������Ϊ����ᱻ����������.
//
// ��һЩ���õ��﷨�Ǻ�:
//     LOG_ASSERT(assertion);
//     DLOG_ASSERT(assertion);
// ��ͬ��:
//     {,D}LOG_IF(FATAL, assert fails) << assertion;
//
// ����һЩ"��ϸ�ȼ�"����־��, ����
//     VLOG(1) << "I'm printed when you run the program with --v=1 or more";
//     VLOG(2) << "I'm printed when you run the program with --v=2 or more";
// ���������INFO�ȼ�(�����������Ķ������).
// ��ϸ��־���԰���ģ�鿪��, ����
//     --vmodule=profile=2,icon_loader=1,browser_*=3 --v=0
// �ᵼ��:
//     a. ��profile.{h,cc}��VLOG(2)�͵͵ȼ�����Ϣ�����
//     b. ��icon_loader.{h,cc}��VLOG(1)�͵͵ȼ�����Ϣ�����
//     c. ��"browser"Ϊǰ׺���ļ���VLOG(3)�͵͵ȼ�����Ϣ�����
//     d. ����ط���VLOG(0)�͵͵ȼ�����Ϣ�����
// (c)��չʾ��'*'(����0���߶���ַ�)��'?'(����1���ַ�)ͨ�������.
// ��"��ϸ�ȼ�"������VLOG_IS_ON(n), ����
//     if(VLOG_IS_ON(2)) {
//       // ��һЩVLOG(2) << ...;�����޷�ʵ�ֵ���־��¼
//     }
// ������"��ϸ�ȼ�"������VLOG_IF��ʾ��, ����������ֵ�ж��Ƿ���Ҫ������־:
//     VLOG_IF(1, (size>1024))
//       << "I'm printed when size is more than 1024 and when you run the "
//         "program with --v=1 or more";
//
// �����׼��'assert'Ϊ'DLOG_ASSERT'.
//
// ���, ��һ���ḽ�������һ��ϵͳ���ô���(GetLastError())����:
//     PLOG(ERROR) << "Couldn't do foo";
//     DPLOG(ERROR) << "Couldn't do foo";
//     PLOG_IF(ERROR, cond) << "Couldn't do foo";
//     DPLOG_IF(ERROR, cond) << "Couldn't do foo";
//     PCHECK(condition) << "Couldn't do foo";
//     DPCHECK(condition) << "Couldn't do foo";
//
// ��֧�ֵ����صȼ���(���صȼ�������)INFO, WARNING, ERROR, ERROR_REPORT��FATAL.
//
// ��Ҫ���ѵ���: ��¼һ�����صȼ�ΪFATAL����Ϣ�ᵼ�³�����ֹ(��Ϣ��¼֮��).
//
// ע��releaseģʽ�����صȼ�ΪERROR_REPORT����ʾ����Ի��򵫲�����ֹ����, 
// ���صȼ�ΪERROR�Լ����µĲ�����ʾ����Ի���.
//
// DFATAL���صȼ��Ƚ��ر�, ��debugģʽ�µ�ͬFATAL, releaseģʽ�µ�ͬERROR.

namespace base
{

    // ��־���������? �ļ���/��ͨ��OutputDebugString��ϵͳ�������.
    // ȱʡ��LOG_ONLY_TO_FILE.
    enum LoggingDestination
    {
        LOG_NONE,
        LOG_ONLY_TO_FILE,
        LOG_ONLY_TO_SYSTEM_DEBUG_LOG,
        LOG_TO_BOTH_FILE_AND_SYSTEM_DEBUG_LOG
    };

    // ָ��д��־�ļ�ʱ�Ƿ���Ҫ����.
    // ���̳߳���д��־����Ҫ����, ���ڶ��̳߳�����Կ���ͬʱ�ж���߳�
    // д��־, д����ʱ��Ҫ���ļ������Ա�֤ÿ����¼����ԭ�ӵ�. 
    // ����д��־�ļ��Ľ��̶��������ؼ��������Ա�������¼��־.
    // ȱʡ��DONT_LOCK_LOG_FILE.
    enum LogLockingState { LOCK_LOG_FILE, DONT_LOCK_LOG_FILE };

    // ����ʱ, ɾ������־�ļ����Ǹ���������־�ļ���?
    // ȱʡ��APPEND_TO_OLD_LOG_FILE
    enum OldFileDeletionState { DELETE_OLD_LOG_FILE, APPEND_TO_OLD_LOG_FILE };

    typedef wchar_t PathChar;

    // �����Ƿ���NDEBUG�궨�岻ͬ��BaseInitLoggingImpl()������, ��ֹ������ʹ�ÿ�
    // ʱ��NDEBUG����Ĳ�һ��.
#if NDEBUG
#define BaseInitLoggingImpl BaseInitLoggingImpl_built_with_NDEBUG
#else
#define BaseInitLoggingImpl BaseInitLoggingImpl_built_without_NDEBUG
#endif

    // ������־�ļ���������ȫ��״̬, �����ڳ����ʼ����ʱ����ô˺���.
    // ���������, ����״̬��Ƕ���ȱʡֵ, �ҵ�һ��д��־������2���߳�ͬʱ����,
    // ��ʼ��critical section����ʱ�ᷢ����ͻ������й©.
    // ȱʡֵ�μ�ö�ٶ���.
    // ȱʡ��־�ļ�����"debug.log", λ�ڳ�������Ŀ¼��. һ������²�ϣ�����,
    // �����ǳ���Ŀ¼�������û���ϵͳ�ϲ�һ����д.
    void BaseInitLoggingImpl(const PathChar* log_file, LoggingDestination logging_dest,
        LogLockingState lock_log, OldFileDeletionState delete_old);

    // ������־��¼�ȼ�, ���ڵ��ڸõȼ��ļ�¼��д�ļ�/��ʾ���û�, �͵ȼ���
    // ��¼�ᱻ���Ե�, ȱʡ�ȼ���0(������־���ᱻ��¼).
    void SetMinLogLevel(int level);

    // ��ȡ��ǰ��־��¼�ȼ�
    int GetMinLogLevel();

    // ��ȡָ���ļ�(һ����__FILE__)�ĵ�ǰvlog�ȼ�.

    // ע��|N|�ǰ�����βnull�Ĵ�С.
    int GetVlogLevelHelper(const char* file_start, size_t N);

    template<size_t N>
    int GetVlogLevel(const char (&file)[N])
    {
        return GetVlogLevelHelper(file, N);
    }

    // ����ÿ����Ϣ��ǰ׺��ʾ����. ���̺��߳�IDĬ�Ϲر�(����ʾ), ʱ���Ĭ�ϴ�.
    // ȱʡ�����, ��־��¼��ֻ����ʱ���ǰ׺����.
    void SetLogItems(bool enable_process_id, bool enable_thread_id,
        bool enable_timestamp, bool enable_tickcount);

    // �����Ƿ���Ҫ����������Ϣ�Ի���. ȱʡ������.
    void SetShowErrorDialogs(bool enable_dialogs);

    // ����FATAL��������������ʧ��֪ͨ. ȱʡ�Ĵ����������ʾ������Ϣ��
    // ��������, �ͻ��˿��������Լ��Ĵ����������(���絥Ԫ����ʱ���жϳ���).
    typedef void (*LogAssertHandlerFunction)(const std::string& str);
    void SetLogAssertHandler(LogAssertHandlerFunction handler);

    // ����ERROR���溯���������շ�debugģʽ��ʧ��֪ͨ. ȱʡ�Ĵ����������ʾ
    // ������Ϣ������ִ��. �ͻ��˿��������Լ��Ĵ���ʵ��.
    typedef void (*LogReportHandlerFunction)(const std::string& str);
    void SetLogReportHandler(LogReportHandlerFunction handler);

    // ������־��Ϣ����������Ԥ������־��Ϣ. ����true�����Ѵ�����Ҫ�ٴ��ݵ�
    // �����κ���־��Ϣ������.
    typedef bool (*LogMessageHandlerFunction)(int severity, const std::string& str);
    void SetLogMessageHandler(LogMessageHandlerFunction handler);

    typedef int LogSeverity;
    const LogSeverity LOG_INFO = 0;
    const LogSeverity LOG_WARNING = 1;
    const LogSeverity LOG_ERROR = 2;
    const LogSeverity LOG_ERROR_REPORT = 3;
    const LogSeverity LOG_FATAL = 4;
    const LogSeverity LOG_NUM_SEVERITIES = 5;
    // LOG_DFATAL��debugģʽ�µ�ͬ��LOG_FATAL, releaseģʽ�µ�ͬ��ERROR.
#ifdef NDEBUG
    const LogSeverity LOG_DFATAL = LOG_ERROR;
#else
    const LogSeverity LOG_DFATAL = LOG_FATAL;
#endif

    // һЩLOG��LOG_IF�õ��Ĵ���ĺ��װ, ���ڵ��������õ�, ���������Ա�֤��������.
#define COMPACT_LOG_EX_INFO(ClassName, ...) \
    base::ClassName(__FILE__, __LINE__, base::LOG_INFO, ##__VA_ARGS__)
#define COMPACT_LOG_EX_WARNING(ClassName, ...) \
    base::ClassName(__FILE__, __LINE__, base::LOG_WARNING, ##__VA_ARGS__)
#define COMPACT_LOG_EX_ERROR(ClassName, ...) \
    base::ClassName(__FILE__, __LINE__, base::LOG_ERROR, ##__VA_ARGS__)
#define COMPACT_LOG_EX_ERROR_REPORT(ClassName, ...) \
    base::ClassName(__FILE__, __LINE__, base::LOG_ERROR_REPORT, ##__VA_ARGS__)
#define COMPACT_LOG_EX_FATAL(ClassName, ...) \
    base::ClassName(__FILE__, __LINE__, base::LOG_FATAL, ##__VA_ARGS__)
#define COMPACT_LOG_EX_DFATAL(ClassName, ...) \
    base::ClassName(__FILE__, __LINE__, base::LOG_DFATAL, ##__VA_ARGS__)

#define COMPACT_LOG_INFO                    COMPACT_LOG_EX_INFO(LogMessage)
#define COMPACT_LOG_WARNING                 COMPACT_LOG_EX_WARNING(LogMessage)
#define COMPACT_LOG_ERROR                   COMPACT_LOG_EX_ERROR(LogMessage)
#define COMPACT_LOG_ERROR_REPORT            COMPACT_LOG_EX_ERROR_REPORT(LogMessage)
#define COMPACT_LOG_FATAL                   COMPACT_LOG_EX_FATAL(LogMessage)
#define COMPACT_LOG_DFATAL                  COMPACT_LOG_EX_DFATAL(LogMessage)

    // wingdi.h�н�ERROR����Ϊ0. LOG(ERROR)���ûᱻչ����COMPACT_LOG_0.
    // Ϊ�˱�֤�﷨��һ����, ������Ҫ����˺��ͬ��COMPACT_LOG_ERROR.
#define ERROR 0
#define COMPACT_LOG_EX_0(ClassName, ...)    COMPACT_LOG_EX_ERROR(ClassName, ##__VA_ARGS__)
#define COMPACT_LOG_0                       COMPACT_LOG_ERROR
    // LOG_IS_ON(ERROR)��Ҫ.
    const LogSeverity LOG_0 = LOG_ERROR;

#define LOG_IS_ON(severity)                 ((base::LOG_ ## severity) >= base::GetMinLogLevel())

    // ��--vmodule��ص�v-logging�������ܻ�Ƚ���.
#define VLOG_IS_ON(verboselevel)            ((verboselevel) <= base::GetVlogLevel(__FILE__))

    // ������, ��condition������������°�stream����ת��Ϊ��.
#define LAZY_STREAM(stream, condition) \
    !(condition) ? (void)0 : base::LogMessageVoidify()&(stream)

    // ʹ��Ԥ�����������Ӳ�������"##", ����LOG(INFO)�ͱ�չ����COMPACT_LOG_INFO.
    // ostream����������Ա����(����: ostream::operator<<(int))��ostream��������
    // �ǳ�Ա����(����: ::operator<<(ostream&, string&))֮����Щ����Ȥ�Ĳ��:
    // �޷��������ַ�������������ֱ������δ������ostream. ͨ������LogMessage��
    // stream()��Ա������������������ıܿ��������.
#define LOG_STREAM(severity)                COMPACT_LOG_ ## severity.stream()

#define LOG(severity)                       LAZY_STREAM(LOG_STREAM(severity), LOG_IS_ON(severity))
#define LOG_IF(severity, condition)         LAZY_STREAM(LOG_STREAM(severity), LOG_IS_ON(severity)&&(condition))

#define VLOG(verboselevel)                  LOG_IF(INFO, VLOG_IS_ON(verboselevel))
#define VLOG_IF(verboselevel, condition)    LOG_IF(INFO, VLOG_IS_ON(verboselevel)&&(condition))

#define LOG_ASSERT(condition)               LOG_IF(FATAL, !(condition)) << "Assert failed: " #condition ". "

#define LOG_GETLASTERROR_STREAM(severity) \
    COMPACT_LOG_EX_ ## severity(Win32ErrorLogMessage, base::GetLastSystemErrorCode()).stream()
#define LOG_GETLASTERROR(severity) \
    LAZY_STREAM(LOG_GETLASTERROR_STREAM(severity), LOG_IS_ON(severity))
#define LOG_GETLASTERROR_MODULE_STREAM(severity, module) \
    COMPACT_LOG_EX_ ## severity(Win32ErrorLogMessage, base::GetLastSystemErrorCode(), module).stream()
#define LOG_GETLASTERROR_MODULE(severity, module) \
    LAZY_STREAM(LOG_GETLASTERROR_MODULE_STREAM(severity, module), LOG_IS_ON(severity))
    // PLOGʹ��PLOG_STREAM, ���Ǹ�ƽ̨������־��õĺ�.
#define PLOG_STREAM(severity)               LOG_GETLASTERROR_STREAM(severity)

#define PLOG(severity)                      LAZY_STREAM(PLOG_STREAM(severity), LOG_IS_ON(severity))
#define PLOG_IF(severity, condition)        LAZY_STREAM(PLOG_STREAM(severity), LOG_IS_ON(severity)&&(condition))

    // CHECK������������������»�������������, ������NDEBUG����, ���κα�������
    // �¶���ִ��.
    //
    // ȷ��CHECK��Բ�����ֵ, ���糣���÷�CHECK(FunctionWithSideEffect()).
#define CHECK(condition) \
    LAZY_STREAM(LOG_STREAM(FATAL), !(condition)) << "Check failed: " #condition ". "
#define PCHECK(condition) \
    LAZY_STREAM(PLOG_STREAM(FATAL), !(condition)) << "Check failed: " #condition ". "

    // ����stringָ���Ƿ�Ϊ�շ���boolֵ�ķº���.
    struct CheckOpString
    {
        CheckOpString(std::string* str) : str_(str) {}
        operator bool() const { return str_!=NULL; }
        std::string* str_;
    };

    // ������Ϣ������. 
    template<typename t1, typename t2>
    std::string* MakeCheckOpString(const t1& v1, const t2& v2, const char* names)
    {
        std::ostringstream ss;
        ss << names << " (" << v1 << " vs. " << v2 << ")";
        std::string* msg = new std::string(ss.str());
        return msg;
    }

    // �����Ʋ���������. ��Ҫ�ڴ�����ֱ��ʹ��, ����CHECK_EQ���.
#define CHECK_OP(name, op, val1, val2) \
    if(base::CheckOpString res = \
        base::Check##name##Impl((val1), (val2), #val1 " " #op " " #val2)) \
        base::LogMessage(__FILE__, __LINE__, res).stream()

    // �ַ����Ƚϸ�������. Ϊ�˷�ֹ��������, ʵ����cpp��.
#define DECLARE_CHECK_STROP_IMPL(func, expected) \
    std::string* Check##func##expected##Impl(const char* s1, \
        const char* s2, const char* names);
    DECLARE_CHECK_STROP_IMPL(strcmp, true)
    DECLARE_CHECK_STROP_IMPL(strcmp, false)
    DECLARE_CHECK_STROP_IMPL(_stricmp, true)
    DECLARE_CHECK_STROP_IMPL(_stricmp, false)
#undef DECLARE_CHECK_STROP_IMPL

    // �ַ����Ƚϸ�����. ��Ҫ�ڴ�����ֱ��ʹ��, ����CHECK_STREQ���.
#define CHECK_STROP(func, op, expected, s1, s2) \
    while(CheckOpString res = \
        base::Check##func##expected##Impl((s1), (s2), #s1 " " #op " " #s2)) \
        LOG(FATAL) << *res.str_

    // �ַ���(char*)���/���ȵļ��. CASE�汾��ʾ��Сд������.
    // ע��"s1"��"s2"��������ʱ�ַ���, �����ڱ��ʽ����ʱ������.
    // ����: CHECK_STREQ(Foo().c_str(), Bar().c_str())
#define CHECK_STREQ(s1, s2)     CHECK_STROP(strcmp, ==, true, s1, s2)
#define CHECK_STRNE(s1, s2)     CHECK_STROP(strcmp, !=, false, s1, s2)
#define CHECK_STRCASEEQ(s1, s2) CHECK_STROP(_stricmp, ==, true, s1, s2)
#define CHECK_STRCASENE(s1, s2) CHECK_STROP(_stricmp, !=, false, s1, s2)

#define CHECK_INDEX(I, A)       CHECK(I < (sizeof(A)/sizeof(A[0])))
#define CHECK_BOUND(B, A)       CHECK(B <= (sizeof(A)/sizeof(A[0])))

#define CHECK_EQ(val1, val2)    CHECK_OP(EQ, ==, val1, val2)
#define CHECK_NE(val1, val2)    CHECK_OP(NE, !=, val1, val2)
#define CHECK_LE(val1, val2)    CHECK_OP(LE, <=, val1, val2)
#define CHECK_LT(val1, val2)    CHECK_OP(LT, < , val1, val2)
#define CHECK_GE(val1, val2)    CHECK_OP(GE, >=, val1, val2)
#define CHECK_GT(val1, val2)    CHECK_OP(GT, > , val1, val2)

#if (defined(OS_WIN) && defined(OFFICIAL_BUILD))
#define LOGGING_IS_OFFICIAL_BUILD

    // Ϊ���Ż�����, �ٷ��汾����ʱ�Ƴ�DLOGs��DCHECKs.
#define ENABLE_DLOG 0
#define ENABLE_DCHECK 0
#elif defined(NDEBUG)
    // ��release����ʱ�Ƴ�DLOGs����DCHECKs(�Կ�ͨ�������б�־����).
#define ENABLE_DLOG 0
#define ENABLE_DCHECK 1
#else
    // debug����ʱ����DLOGs��DCHECKs.
#define ENABLE_DLOG 1
#define ENABLE_DCHECK 1
#endif

    // ����DLOG���.
#if ENABLE_DLOG
#define DLOG_IF(severity, condition)        LOG_IF(severity, condition)
#define DLOG_ASSERT(condition)              LOG_ASSERT(condition)
#define DPLOG_IF(severity, condition)       PLOG_IF(severity, condition)
#define DVLOG_IF(verboselevel, condition)   VLOG_IF(verboselevel, condition)
#else //ENABLE_DLOG
    // ���ENABLE_DLOG�ر�, ��Ҫ�������е�|condition|����(��������ֻ��
    // ��NDEBUG�¶���), ����DCHECK��غ��в��.
#define DLOG_EAT_STREAM_PARAMETERS          true ? (void)0 : base::LogMessageVoidify()&LOG_STREAM(FATAL)

#define DLOG_IF(severity, condition)        DLOG_EAT_STREAM_PARAMETERS
#define DLOG_ASSERT(condition)              DLOG_EAT_STREAM_PARAMETERS
#define DPLOG_IF(severity, condition)       DLOG_EAT_STREAM_PARAMETERS
#define DVLOG_IF(verboselevel, condition)   DLOG_EAT_STREAM_PARAMETERS
#endif //ENABLE_DLOG

    // �û���������ʹ��DEBUG_MODE
    //   if(DEBUG_MODE) foo.CheckThatFoo();
    // ���滻
    //   #ifndef NDEBUG
    //     foo.CheckThatFoo();
    //   #endif
    //
    // DEBUG_MODE��ENABLE_DLOGһ��.
    enum { DEBUG_MODE = ENABLE_DLOG };

#undef ENABLE_DLOG

#define DLOG_IS_ON(severity)                (base::DEBUG_MODE && LOG_IS_ON(severity))

#define DLOG(severity)                      LAZY_STREAM(LOG_STREAM(severity), DLOG_IS_ON(severity))

#define DLOG_GETLASTERROR(severity) \
    LAZY_STREAM(LOG_GETLASTERROR_STREAM(severity), DLOG_IS_ON(severity))
#define DLOG_GETLASTERROR_MODULE(severity, module) \
    LAZY_STREAM(LOG_GETLASTERROR_MODULE_STREAM(severity, module), DLOG_IS_ON(severity))

#define DPLOG(severity)                     LAZY_STREAM(PLOG_STREAM(severity), DLOG_IS_ON(severity))

#define DVLOG(verboselevel)                 DLOG_IF(INFO, VLOG_IS_ON(verboselevel))

    // ����DCHECK���.
#if ENABLE_DCHECK

#if defined(NDEBUG)
    // �������release������dchecks, ��BaseInitLoggingImpl����g_enable_dcheckΪtrue.
    extern bool g_enable_dcheck;
#define DCHECK_IS_ON() (base::g_enable_dcheck)
#define DCHECK_SEVERITY ERROR_REPORT
    const LogSeverity LOG_DCHECK = LOG_ERROR_REPORT;
#else //defined(NDEBUG)
    // һ��debug����ʱ����DCHECKS.
#define DCHECK_IS_ON() (true)
#define DCHECK_SEVERITY FATAL
    const LogSeverity LOG_DCHECK = LOG_FATAL;
#endif //defined(NDEBUG)

#else //ENABLE_DCHECK

#define DCHECK_IS_ON() (false)
#define DCHECK_SEVERITY FATAL
    const LogSeverity LOG_DCHECK = LOG_FATAL;

#endif // ENABLE_DCHECK

    // ����CHECK����, DCHECK����ֻ���ڱ�Ҫ������¶Բ�����ֵ.
    // DCHECK��������DCHECKs�Ƿ����ö�������|condition|; ���Բ���õ�
    // ����δʹ�õľ�����Ϊ������DCHECK��ʹ�ù�, �����DLOG_IF��ͬ.

#define DCHECK(condition) \
    !DCHECK_IS_ON() ? (void)0 : \
        LOG_IF(DCHECK_SEVERITY, !(condition)) << "Check failed: " #condition ". "

#define DPCHECK(condition) \
    !DCHECK_IS_ON() ? (void)0 : \
        PLOG_IF(DCHECK_SEVERITY, !(condition)) << "Check failed: " #condition ". "

    // �����Ʋ���������. ��Ҫ�ڴ�����ֱ��ʹ��, ����CHECK_EQ���.
#define DCHECK_OP(name, op, val1, val2) \
    if(DLOG_IS_ON(DCHECK_SEVERITY)) \
        if(base::CheckOpString res = \
            base::Check##name##Impl((val1), (val2), #val1 " " #op " " #val2)) \
            base::LogMessage(__FILE__, __LINE__, base::LOG_DCHECK, res).stream()

    // ����ֵ���/���ȱȽ����ڷ�Ԥ�ڽ��ʱ��¼һ����������ֵ��LOG_FATAL��Ϣ��־.
    // ֵ������operator<<(ostream, ...)���������ذ汾.
    //
    // ����������ʹ��:
    //     DCHECK_NE(1, 2) << ": The world must be ending!";
    //
    // ��Щ����Ա�֤ÿ������������ܱ���ȷ����, ���кϷ��Ĳ������ܹ���ȷ����.
    // ��ʱ����Ҳ������������, ����:
    //     DCHECK_EQ(string("abc")[1], 'b');
    //
    // ����: ָ���NULL�����Ĳ������ܻᵼ���޷�����ͨ��, ����ǿ��ת��(static_cast)
    // NULL����Ӧ��ָ������.
#define DCHECK_EQ(val1, val2)        DCHECK_OP(EQ, ==, val1, val2)
#define DCHECK_NE(val1, val2)        DCHECK_OP(NE, !=, val1, val2)
#define DCHECK_LE(val1, val2)        DCHECK_OP(LE, <=, val1, val2)
#define DCHECK_LT(val1, val2)        DCHECK_OP(LT, < , val1, val2)
#define DCHECK_GE(val1, val2)        DCHECK_OP(GE, >=, val1, val2)
#define DCHECK_GT(val1, val2)        DCHECK_OP(GT, > , val1, val2)

    // �ַ����Ƚϸ�����.
    // ��Ҫ�ڴ�����ֱ��ʹ��, ����DCHECK_STREQ���.
#define DCHECK_STROP(func, op, expected, s1, s2) \
    if(DCHECK_IS_ON()) CHECK_STROP(func, op, expected, s1, s2)

    // ��֤�ַ������/����. CASE�汾��Сд�޹�.
    // ע��"s1"��"s2"��������ʱ�ַ���, �ڱ��ʽ��β���ᱻ����������.
    // (����: DCHECK_STREQ(Foo().c_str(), Bar().c_str())).
#define DCHECK_STREQ(s1, s2)        DCHECK_STROP(strcmp, ==, true, s1, s2)
#define DCHECK_STRNE(s1, s2)        DCHECK_STROP(strcmp, !=, false, s1, s2)
#define DCHECK_STRCASEEQ(s1, s2)    DCHECK_STROP(_stricmp, ==, true, s1, s2)
#define DCHECK_STRCASENE(s1, s2)    DCHECK_STROP(_stricmp, !=, false, s1, s2)

#define DCHECK_INDEX(I, A)          DCHECK(I < (sizeof(A)/sizeof(A[0])))
#define DCHECK_BOUND(B, A)          DCHECK(B <= (sizeof(A)/sizeof(A[0])))

    // CHECK_OP��ʵ�ָ�������. (int, int)�������汾��Ϊ�˽���������޷�
    // ʵ����δ����enum���Ͱ汾����.
#define DEFINE_CHECK_OP_IMPL(name, op) \
    template<typename t1, typename t2> \
    inline std::string* Check##name##Impl(const t1& v1, const t2& v2, const char* names) \
    { \
        if(v1 op v2) return NULL; \
        else return MakeCheckOpString(v1, v2, names); \
    } \
    inline std::string* Check##name##Impl(int v1, int v2, const char* names) \
    { \
        if(v1 op v2) return NULL; \
        else return MakeCheckOpString(v1, v2, names); \
    }
    DEFINE_CHECK_OP_IMPL(EQ, ==)
    DEFINE_CHECK_OP_IMPL(NE, !=)
    DEFINE_CHECK_OP_IMPL(LE, <=)
    DEFINE_CHECK_OP_IMPL(LT, < )
    DEFINE_CHECK_OP_IMPL(GE, >=)
    DEFINE_CHECK_OP_IMPL(GT, > )
#undef DEFINE_CHECK_OP_IMPL

#define NOTREACHED() DCHECK(false)

        // �ض����׼���assertʵ�ָ��õ���־.
#undef assert
#define assert(x) DLOG_ASSERT(x)

        // LogMessage����־��Ϣ��. ʵ����һ������Ȼ������һЩ��Ϣ, ����������ʱ��,
        // ���е���Ϣ�������뵽���ʵĵط�.
        //
        // û�б�Ҫֱ�ӹ�����������Ϣ������, LOG()��Щ������������װ.
    class LogMessage
    {
    public:
        LogMessage(const char* file, int line, LogSeverity severity, int ctr);

        // ����2�����캯����Ϊ��LOG���õļ򻯰汾.
        //
        // ��LOG(INFO)��ʹ��:
        // Ĭ��: severity = LOG_INFO, ctr = 0
        //
        // ʹ������汾�滻���ӹ���汾���Լ��ټ����ֽڵĵ�������.
        LogMessage(const char* file, int line);

        // ��LOG(severity)��ʹ��(severity != INFO):
        // Ĭ��: ctr = 0
        //
        // ʹ������汾�滻���ӹ���汾���Լ��ټ����ֽڵĵ�������.
        LogMessage(const char* file, int line, LogSeverity severity);

        // ����ʧ�ܼ�������Ĺ���汾.
        // Ĭ��: severity = LOG_FATAL
        LogMessage(const char* file, int line, const CheckOpString& result);

        // �������صȼ���ʧ�ܼ�������Ĺ���汾.
        LogMessage(const char* file, int line, LogSeverity severity,
            const CheckOpString& result);

        ~LogMessage();

        std::ostream& stream() { return stream_; }

    private:
        void Init(const char* file, int line);

        LogSeverity severity_;
        std::ostringstream stream_;
        size_t message_start_; // ��־��Ϣ����ʼƫ��(�п�����ǰ׺����: ����id �߳�id ...).
        // SaveLastError��洢��ǰGetLastError��ֵ�������������лָ�.
        // LogMessage����������˴�����Win32����, ������־�����Ĵ����ں���
        // ����ʱGLEֵ�Ѿ����ı�.
        class SaveLastError
        {
        public:
            SaveLastError();
            ~SaveLastError();

            unsigned long get_error() const { return last_error_; }

        protected:
            unsigned long last_error_;
        };

        SaveLastError last_error_;

        DISALLOW_COPY_AND_ASSIGN(LogMessage);
    };

    // ��־��¼�����ӿ�(����־��¼�ȼ��Ǳ�����ʱ������õ�)
    inline void LogAtLevel(const int log_level, const std::string& msg)
    {
        LogMessage(__FILE__, __LINE__, log_level).stream() << msg;
    }

    // LogMessageVoidify��������������־������ʽ����һЩֵ, ������뾯��,
    // ����: "value computed is not used"��"statement has no effect".
    class LogMessageVoidify
    {
    public:
        LogMessageVoidify() {}
        // &���ȼ�����<<���Ǹ���?:
        void operator&(std::ostream&) {}
    };

    typedef unsigned long SystemErrorCode;
    SystemErrorCode GetLastSystemErrorCode();

    // ����GetLastError()�����ʽ�����ַ���
    class Win32ErrorLogMessage
    {
    public:
        Win32ErrorLogMessage(const char* file, int line, LogSeverity severity,
            SystemErrorCode err, const char* module);

        Win32ErrorLogMessage(const char* file, int line, LogSeverity severity,
            SystemErrorCode err);

        std::ostream& stream() { return log_message_.stream(); }

        // Appends the error message before destructing the encapsulated class.
        ~Win32ErrorLogMessage();

    private:
        SystemErrorCode err_;
        // �����������ģ����(��ѡ)
        const char* module_;
        LogMessage log_message_;

        DISALLOW_COPY_AND_ASSIGN(Win32ErrorLogMessage);
    };

    // ��ʽ�ر���־�ļ�.
    // ע��: ��־�ļ��������־��¼����Ҫ����, �����޷���֤CloseLogFile��
    // �ļ�һ���ǹر�״̬.
    void CloseLogFile();

    // ֱ��д��־����.
    void RawLog(int level, const char* message);

#define RAW_LOG(level, message) base::RawLog(base::LOG_ ## level, message)

#define RAW_CHECK(condition) \
    do \
    { \
        if(!(condition)) \
            base::RawLog(base::LOG_FATAL, "Check failed: " #condition "\n"); \
    } while(0)

} //namespace base

// ͨ�����ķ�ʽΪ��־����ṩ����.
// ����������Unicode�ַ�������־�ļ�. ���ַ�ʽ�����˵����һЩ, һ����ò�Ҫʹ��.
// ���ַ���ת��ΪUTF-8ͨ������������.
std::ostream& operator<<(std::ostream& out, const wchar_t* wstr);
inline std::ostream& operator<<(std::ostream& out, const std::wstring& wstr)
{
    return out<<wstr.c_str();
}

// NOTIMPLEMENTED()��ʵ��
//
// ���ʵ������NOTIMPLEMENTED_POLICY�Ķ���
//   0 -- ��ʵ��(���ɱ�����)
//   1 -- ����ʱ����
//   2 -- ����ʱʧ��
//   3 -- ����ʱʧ��(DCHECK)
//   4 -- (ȱʡ)������ʱ����LOG(ERROR)
//   5 -- ������ʱ����LOG(ERROR), ֻ����һ��
#ifndef NOTIMPLEMENTED_POLICY
// ȱʡ����: LOG(ERROR)
#define NOTIMPLEMENTED_POLICY 4
#endif

#define NOTIMPLEMENTED_MSG "NOT IMPLEMENTED"

#if NOTIMPLEMENTED_POLICY == 0
#define NOTIMPLEMENTED()
#elif NOTIMPLEMENTED_POLICY == 1
// TODO, figure out how to generate a warning
#define NOTIMPLEMENTED() COMPILE_ASSERT(false, NOT_IMPLEMENTED)
#elif NOTIMPLEMENTED_POLICY == 2
#define NOTIMPLEMENTED() COMPILE_ASSERT(false, NOT_IMPLEMENTED)
#elif NOTIMPLEMENTED_POLICY == 3
#define NOTIMPLEMENTED() NOTREACHED()
#elif NOTIMPLEMENTED_POLICY == 4
#define NOTIMPLEMENTED() LOG(ERROR) << NOTIMPLEMENTED_MSG
#elif NOTIMPLEMENTED_POLICY == 5
#define NOTIMPLEMENTED()\
    do \
    { \
        static int count = 0; \
        LOG_IF(ERROR, 0==count++) << NOTIMPLEMENTED_MSG; \
    } while(0)
#endif

#endif //__logging_h__