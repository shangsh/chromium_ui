
#include "time.h"

#pragma comment(lib, "winmm.lib")

#include "cpu.h"
#include "lock.h"
#include "singleton.h"
#include "sys_string_conversions.h"

#include "third_party/nspr/prtime.h"

namespace base
{

    int TimeDelta::InDays() const
    {
        return static_cast<int>(delta_ / Time::kMicrosecondsPerDay);
    }

    int TimeDelta::InHours() const
    {
        return static_cast<int>(delta_ / Time::kMicrosecondsPerHour);
    }

    int TimeDelta::InMinutes() const
    {
        return static_cast<int>(delta_ / Time::kMicrosecondsPerMinute);
    }

    double TimeDelta::InSecondsF() const
    {
        return static_cast<double>(delta_) / Time::kMicrosecondsPerSecond;
    }

    int64 TimeDelta::InSeconds() const
    {
        return delta_ / Time::kMicrosecondsPerSecond;
    }

    double TimeDelta::InMillisecondsF() const
    {
        return static_cast<double>(delta_) / Time::kMicrosecondsPerMillisecond;
    }

    int64 TimeDelta::InMilliseconds() const
    {
        return delta_ / Time::kMicrosecondsPerMillisecond;
    }

    int64 TimeDelta::InMillisecondsRoundedUp() const
    {
        return (delta_ + Time::kMicrosecondsPerMillisecond - 1) /
            Time::kMicrosecondsPerMillisecond;
    }

    int64 TimeDelta::InMicroseconds() const
    {
        return delta_;
    }

    // static
    Time Time::FromTimeT(time_t tt)
    {
        if(tt == 0)
        {
            return Time(); // ά��0��ʾ������.
        }
        return Time((tt*kMicrosecondsPerSecond) + kTimeTToMicrosecondsOffset);
    }

    time_t Time::ToTimeT() const
    {
        if(us_ == 0)
        {
            return 0; // ά��0��ʾ������.
        }
        return (us_-kTimeTToMicrosecondsOffset) / kMicrosecondsPerSecond;
    }

    // static
    Time Time::FromDoubleT(double dt)
    {
        if(dt == 0)
        {
            return Time(); // ά��0��ʾ������.
        }
        return Time(static_cast<int64>((dt *
            static_cast<double>(kMicrosecondsPerSecond)) +
            kTimeTToMicrosecondsOffset));
    }

    double Time::ToDoubleT() const
    {
        if(us_ == 0)
        {
            return 0; // ά��0��ʾ������.
        }
        return (static_cast<double>(us_-kTimeTToMicrosecondsOffset) /
            static_cast<double>(kMicrosecondsPerSecond));
    }

    // static
    Time Time::UnixEpoch()
    {
        Time time;
        time.us_ = kTimeTToMicrosecondsOffset;
        return time;
    }

    Time Time::LocalMidnight() const
    {
        Exploded exploded;
        LocalExplode(&exploded);
        exploded.hour = 0;
        exploded.minute = 0;
        exploded.second = 0;
        exploded.millisecond = 0;
        return FromLocalExploded(exploded);
    }

    // static
    bool Time::FromString(const wchar_t* time_string, Time* parsed_time)
    {
        DCHECK((time_string!=NULL) && (parsed_time!=NULL));
        std::string ascii_time_string = SysWideToUTF8(time_string);
        if(ascii_time_string.length() == 0)
        {
            return false;
        }
        PRTime result_time = 0;
        PRStatus result = PR_ParseTimeString(ascii_time_string.c_str(),
            PR_FALSE, &result_time);
        if(PR_SUCCESS != result)
        {
            return false;
        }
        result_time += kTimeTToMicrosecondsOffset;
        *parsed_time = Time(result_time);
        return true;
    }

    inline bool is_in_range(int value, int lo, int hi)
    {
        return lo<=value && value<=hi;
    }

    bool Time::Exploded::HasValidValues() const
    {
        return is_in_range(month, 1, 12) &&
            is_in_range(day_of_week, 0, 6) &&
            is_in_range(day_of_month, 1, 31) &&
            is_in_range(hour, 0, 23) &&
            is_in_range(minute, 0, 59) &&
            is_in_range(second, 0, 60) &&
            is_in_range(millisecond, 0, 999);
    }

} //namespace base


// Windows��ʱ����������
//
// һƪ���������: http://www.ddj.com/windows/184416651
// һ��mozilla bug: http://bugzilla.mozilla.org/show_bug.cgi?id=363258
//
// Windowsȱʡ�ļ�����, GetSystemTimeAsFileTime���Ȳ�����.
// ��������Լ��15.5ms.
//
// QueryPerformanceCounter��ʵ�ָ߾���ʱ�Ӳ����ѡ��, ����ĳЩӲ����������,
// �е�ʱ�����Ծ. �ʼǱ���, QPC���ô��ۺܸ�. ̨ʽ���ϱ�timeGetTime()��3-4
// ��, �ʼǱ��ϻ���10��.
//
// ����һ��ѡ�����ʹ��timeGetTime(), ������1ms, ����ֻҪ����APIs(
// timeBeginPeriod())�ͻ�Ӱ��ϵͳ��������Ӧ��. ȱʡ����ֻ��15.5ms.
// ��˺ܲ���, ���ǲ��ܵ���timeBeginPeriod��Ϊ����Ӱ������Ӧ��.
// �������ƶ�ƽ̨���ø���Ķ�ý���ʱ�����𺦵������. �μ�intel����:
// http://softwarecommunity.intel.com/articles/eng/1086.htm
//
// Ϊ�˽������, ����ʹ��timeGetTime(), ֻ�ڲ����Ե�ع������е�ʱ������
// ϵͳ�ļ���������. ʹ��timeBeginPeriod(1)�Ա���Ϣѭ���ȴ���ʱ���������
// ����һ��, ����WaitForSingleObject(..., 1)�ڲ������ѵ�ʱ�����ٶ���15ms.

using base::Time;
using base::TimeDelta;
using base::TimeTicks;

namespace
{

    // ��MSDN: FILETIME "��64λ��ֵ��ʾ��January 1, 1601 (UTC)�����İ�������".
    int64 FileTimeToMicroseconds(const FILETIME& ft)
    {
        // bit_cast����ֽڶ�������, ����10ת�������뵽����.
        // �����ʺ�little-endian����.
        return bit_cast<int64, FILETIME>(ft) / 10;
    }

    void MicrosecondsToFileTime(int64 us, FILETIME* ft)
    {
        DCHECK(us >= 0) << "Time is less than 0, negative values are not "
            "representable in FILETIME";

        // ����10ת�����뵽������. bit_cast����ֽڶ�������.
        // �����ʺ�little-endian����.
        *ft = bit_cast<FILETIME, int64>(us * 10);
    }

    int64 CurrentWallclockMicroseconds()
    {
        FILETIME ft;
        ::GetSystemTimeAsFileTime(&ft);
        return FileTimeToMicroseconds(ft);
    }

    // ʱ��Ƶ���ز�����ʱ����. 60��.
    const int kMaxMillisecondsToAvoidDrift = 60 * Time::kMillisecondsPerSecond;

    int64 initial_time = 0;
    TimeTicks initial_ticks;

    void InitializeClock()
    {
        initial_ticks = TimeTicks::Now();
        initial_time = CurrentWallclockMicroseconds();
    }

} //namespace

// Time���ڲ���FILETIME��ʾ, ��ʼ��1601-01-01 00:00:00 UTC.
// ((1970-1601)*365+89)*24*60*60*1000*1000, ���е�89��1601��1970�����������ܺ�:
// (1970-1601)/4������1700, 1800, and 1900.
// static
const int64 Time::kTimeTToMicrosecondsOffset = GG_INT64_C(11644473600000000);

bool Time::high_resolution_timer_enabled_ = false;

// static
Time Time::Now()
{
    if(initial_time == 0)
    {
        InitializeClock();
    }

    // �߾��ȼ�����ʵ�ּ�ʱ, �ܹ��õ���10-15msС�ĳ�ʱ. ��ͨ��
    // CurrentWallclockMicroseconds(), �޷��õ�ϸ���ȵĶ�ʱ��.
    //
    // ʹ��ʱ, ��ʼ��ʱ��(initial_time)�ͼ�����(initial_ctr). ͨ��
    // �ͳ�ʼʱ�ӱȽ��ܵõ����ŵ�ʱ����, Ȼ����ܵó�ʱ���.
    //
    // Ϊ�������, ���������ڵĺ�ϵͳʱ��ͬ��.
    while(true)
    {
        TimeTicks ticks = TimeTicks::Now();

        // �����Կ�ʼ������ʱ�����.
        TimeDelta elapsed = ticks - initial_ticks;

        // ����Ƿ���Ҫͬ��ʱ��.
        if(elapsed.InMilliseconds() > kMaxMillisecondsToAvoidDrift)
        {
            InitializeClock();
            continue;
        }

        return Time(elapsed + Time(initial_time));
    }
}

// static
Time Time::NowFromSystemTime()
{
    // ǿ��ͬ��.
    InitializeClock();
    return Time(initial_time);
}

// static
Time Time::FromFileTime(FILETIME ft)
{
    return Time(FileTimeToMicroseconds(ft));
}

FILETIME Time::ToFileTime() const
{
    FILETIME utc_ft;
    MicrosecondsToFileTime(us_, &utc_ft);
    return utc_ft;
}

// static
void Time::EnableHighResolutionTimer(bool enable)
{
    // �������߳��Ƿ�Ψһ.
    static DWORD my_thread = ::GetCurrentThreadId();
    DCHECK(::GetCurrentThreadId() == my_thread);

    if(high_resolution_timer_enabled_ == enable)
    {
        return;
    }

    high_resolution_timer_enabled_ = enable;
}

// static
bool Time::ActivateHighResolutionTimer(bool activate)
{
    if(!high_resolution_timer_enabled_)
    {
        return false;
    }

    // ��1ms����, ����ʹ������ֵ���ü�ʱ����.
    const int kMinTimerIntervalMs = 1;
    MMRESULT result;
    if(activate)
    {
        result = timeBeginPeriod(kMinTimerIntervalMs);
    }
    else
    {
        result = timeEndPeriod(kMinTimerIntervalMs);
    }
    return result == TIMERR_NOERROR;
}

// static
Time Time::FromExploded(bool is_local, const Exploded& exploded)
{
    // ����ϵͳ���ʱ��ṹ, ������ʱ�����UTC.
    SYSTEMTIME st;
    st.wYear = exploded.year;
    st.wMonth = exploded.month;
    st.wDayOfWeek = exploded.day_of_week;
    st.wDay = exploded.day_of_month;
    st.wHour = exploded.hour;
    st.wMinute = exploded.minute;
    st.wSecond = exploded.second;
    st.wMilliseconds = exploded.millisecond;

    // ת����FILETIME.
    FILETIME ft;
    if(!SystemTimeToFileTime(&st, &ft))
    {
        NOTREACHED() << "Unable to convert time";
        return Time(0);
    }

    // ȷ����UTC.
    if(is_local)
    {
        FILETIME utc_ft;
        LocalFileTimeToFileTime(&ft, &utc_ft);
        return Time(FileTimeToMicroseconds(utc_ft));
    }
    return Time(FileTimeToMicroseconds(ft));
}

void Time::Explode(bool is_local, Exploded* exploded) const
{
    // FILETIME��UTC.
    FILETIME utc_ft;
    MicrosecondsToFileTime(us_, &utc_ft);

    // FILETIMEת��������ʱ��.
    BOOL success = TRUE;
    FILETIME ft;
    if(is_local)
    {
        success = FileTimeToLocalFileTime(&utc_ft, &ft);
    }
    else
    {
        ft = utc_ft;
    }

    // FILETIME��SYSTEMTIME(���).
    SYSTEMTIME st;
    if(!success || !FileTimeToSystemTime(&ft, &st))
    {
        NOTREACHED() << "Unable to convert time, don't know why";
        ZeroMemory(exploded, sizeof(exploded));
        return;
    }

    exploded->year = st.wYear;
    exploded->month = st.wMonth;
    exploded->day_of_week = st.wDayOfWeek;
    exploded->day_of_month = st.wDay;
    exploded->hour = st.wHour;
    exploded->minute = st.wMinute;
    exploded->second = st.wSecond;
    exploded->millisecond = st.wMilliseconds;
}

namespace
{

    // We define a wrapper to adapt between the __stdcall and __cdecl call of the
    // mock function, and to avoid a static constructor.  Assigning an import to a
    // function pointer directly would require setup code to fetch from the IAT.
    DWORD timeGetTimeWrapper()
    {
        return timeGetTime();
    }

    DWORD (*tick_function)(void) = &timeGetTimeWrapper;

    // �����ӳ��ۼƶ�ʧ��ʱ��(��λ�Ǻ���).
    int64 rollover_ms = 0;

    // ���һ��timeGetTime��ֵ, ���ڼ���ӳ�.
    DWORD last_seen_now = 0;

    // rollover_ms��last_seen_now�ı�����.
    // ע��: һ����˵Ӧ�ñ���ʹ��ȫ�ֶ���, �����ǵײ����, ����ʹ��Singletons.
    // (Singleton�ǳ��������ڲ���Ҫ�������κ��˽�, ������Ҳ�ᵼ��һЩ����).
    // Singleton������ʱ���Ӱ����΢�������, ֻȡ���ڵײ���뱾��.
    base::Lock rollover_lock;

    // ͨ��timeGetTime()ʵ��TimeTicks::Now(). ����ֵ��Windows�����������ܺ�����,
    // 32λƽ̨��ÿ��Լ49������, �����һ��������. ���������Լ�����, ���ÿ49��
    // TimeTicks::Now()�Ƿ񱻵��ù�.
    TimeDelta RolloverProtectedNow()
    {
        base::AutoLock locked(rollover_lock);
        // ����tick_functionʱ��Ҫ����ȷ��last_seen_now����ͬ��.
        DWORD now = tick_function();
        if(now < last_seen_now)
        {
            rollover_ms += 0x100000000I64; // ~49.7 days.
        }
        last_seen_now = now;
        return TimeDelta::FromMilliseconds(now + rollover_ms);
    }

    // ʱ�Ӽ���������:
    // (1) CPUʱ�Ӽ�����. (ͨ��RDTSC��ȡ)
    // CPU�ļ������ṩ����߾��ȵ�ʱ����Ҵ������. Ȼ��, CPU�ļ������ǲ��ɿ���,
    // ��Ӧ���ڲ�Ʒ��ʹ��. ���������Ǵ�����֮���Ƕ�����, û��ͬ��. ������Щ����
    // ��, ������������¶Ⱥ͵����ı�Ƶ��, ��Щ����»�ֹͣ.
    //
    // (2) QueryPerformanceCounter (QPC). QPC�������ṩ�˸߷ֱ���(������)��ʱ���,
    // �����۱Ƚϰ���. QueryPerformanceCounterʵ�ʵ��õ���HAL(ͨ��ACPI).
    // ����http://blogs.msdn.com/oldnewthing/archive/2005/09/02/459952.aspx,
    // ��������, ��ͨ�����ж�ʱ��ģ��ʵ��. ��õ������, HAL�ƶ�RDTSC��������
    // Ƶ��Ϊ����, ֱ��ʹ��. �ڶദ����������, ������֤ÿ�����������ص�RDTSC�Ƿ�
    // һ��, ������һЩ��֪��Ӳ������. ����֮, QPC�ڶദ���������ϳ��Ը���һ�µ�
    // ���, ������һЩBIOS����HAL��bugs������ȫ�ɿ�, ������һЩ�ϵĵ�����. ���
    // ���µ�HAL�ͽ��µ�BIOS��, QPC�Ѿ��ȽϿɿ���ʹ��ʱ������С��.
    //
    // (3) ϵͳʱ��. ϵͳʱ���ṩ�˵ͷֱ���(һ��10-55ms)��ʱ���, ���۽ϵ��ҷǳ�
    // �ɿ�.
    class HighResNowSingleton
    {
    public:
        HighResNowSingleton() : ticks_per_microsecond_(0.0), skew_(0)
        {
            InitializeClock();

            // ��Athlon X2 CPU(����: model 15)��, QueryPerformanceCounter��
            // ���ɿ���. ʹ�õͷֱ��ʵ�ʱ��.
            base::CPU cpu;
            if(cpu.vendor_name()=="AuthenticAMD" && cpu.family()==15)
            {
                DisableHighResClock();
            }
        }

        bool IsUsingHighResClock()
        {
            return ticks_per_microsecond_ != 0.0;
        }

        void DisableHighResClock()
        {
            ticks_per_microsecond_ = 0.0;
        }

        TimeDelta Now()
        {
            if(IsUsingHighResClock())
            {
                return TimeDelta::FromMicroseconds(UnreliableNow());
            }

            // ���˵���ʱ��.
            return RolloverProtectedNow();
        }

        int64 GetQPCDriftMicroseconds()
        {
            if(!IsUsingHighResClock())
            {
                return 0;
            }

            return abs((UnreliableNow() - ReliableNow()) - skew_);
        }

    private:
        // QPCʱ�Ӻ�GetSystemTimeAsFileTimeͬ��.
        void InitializeClock()
        {
            LARGE_INTEGER ticks_per_sec = { 0 };
            if(!QueryPerformanceFrequency(&ticks_per_sec))
            {
                return; // ����, ����֤��������.
            }
            ticks_per_microsecond_ = static_cast<float>(ticks_per_sec.QuadPart) /
                static_cast<float>(Time::kMicrosecondsPerSecond);

            skew_ = UnreliableNow() - ReliableNow();
        }

        // ���ؿ���������΢����, ��һ����ȷ.
        int64 UnreliableNow()
        {
            LARGE_INTEGER now;
            QueryPerformanceCounter(&now);
            return static_cast<int64>(now.QuadPart / ticks_per_microsecond_);
        }

        // ���ؿ���������΢����, �ǳ���ȷ.
        int64 ReliableNow()
        {
            return RolloverProtectedNow().InMicroseconds();
        }

        // ����ʱ��Ƶ��->΢��. �ٶ�ʱ��Ƶ�ʱ�1΢���(1MHz, �ſ���).
        float ticks_per_microsecond_;  // 0��ʾQPFʧ��, �޷�ʹ��.
        int64 skew_; // �ͷֱ��ʺ͸߷ֱ���ʱ��ֱ�ӵ����(���ڵ���).

        DISALLOW_COPY_AND_ASSIGN(HighResNowSingleton);
    };

}  //namespace

// static
TimeTicks::TickFunctionType TimeTicks::SetMockTickFunction(
    TickFunctionType ticker)
{
    TickFunctionType old = tick_function;
    tick_function = ticker;
    return old;
}

// static
TimeTicks TimeTicks::Now()
{
    return TimeTicks() + RolloverProtectedNow();
}

// static
TimeTicks TimeTicks::HighResNow()
{
    return TimeTicks() + Singleton<HighResNowSingleton>::get()->Now();
}

// static
int64 TimeTicks::GetQPCDriftMicroseconds()
{
    return Singleton<HighResNowSingleton>::get()->GetQPCDriftMicroseconds();
}

// static
bool TimeTicks::IsHighResClockWorking()
{
    return Singleton<HighResNowSingleton>::get()->IsUsingHighResClock();
}