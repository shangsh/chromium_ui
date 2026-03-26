
#ifndef __time_h__
#define __time_h__

#pragma once

#include <sys/types.h>
#ifndef _WIN32
#include <time.h>
#include <sys/time.h>
#include <cstdint>
#endif

#include "basic_types.h"
#include "win_types.h"

// Time��ʾһ�����Ե�ʱ���, ��������ƽ̨����ʼʱ��, �ڲ���΢��
// (s/1,000,000)��ʾ.
//
// TimeDelta��ʾһ��ʱ��, �ڲ���΢���ʾ.
//
// TimeTicks��ʾһ�������ĳ���ʱ��, ���ڼ���ʱ���, �ڲ���΢���ʾ.
// ���޷�ת���ɿɶ���ʱ��, ȷ�������С.(�û��޸�ʱ��, Time::Now()
// ���ܻ��С��������).
//
// �����ڲ���64λ��ֵ��ʾ, ���԰�ֵ�����Ǹ�Ч��.

namespace base
{

    class Time;
    class TimeTicks;

    class TimeDelta
    {
    public:
        TimeDelta() : delta_(0) {}

        // ת��ʱ�䵽TimeDelta.
        static TimeDelta FromDays(int64 days);
        static TimeDelta FromHours(int64 hours);
        static TimeDelta FromMinutes(int64 minutes);
        static TimeDelta FromSeconds(int64 secs);
        static TimeDelta FromMilliseconds(int64 ms);
        static TimeDelta FromMicroseconds(int64 us);

        // ����TimeDelta������ڲ���ֵ. ��Ҫֱ������������, Ӧ��ʹ���ṩ�Ĳ�������.
        int64 ToInternalValue() const
        {
            return delta_;
        }

        // ����TimeDelta��һЩ��λ. F�汾���ظ�������, �����ķ���ȡ�������ֵ.
        //
        // InMillisecondsRoundedUp()ȡ������һ΢��.
        int InDays() const;
        int InHours() const;
        int InMinutes() const;
        double InSecondsF() const;
        int64 InSeconds() const;
        double InMillisecondsF() const;
        int64 InMilliseconds() const;
        int64 InMillisecondsRoundedUp() const;
        int64 InMicroseconds() const;

        TimeDelta& operator=(TimeDelta other)
        {
            delta_ = other.delta_;
            return *this;
        }

        // ������TimeDelta����.
        TimeDelta operator+(TimeDelta other) const
        {
            return TimeDelta(delta_ + other.delta_);
        }
        TimeDelta operator-(TimeDelta other) const
        {
            return TimeDelta(delta_ - other.delta_);
        }

        TimeDelta& operator+=(TimeDelta other)
        {
            delta_ += other.delta_;
            return *this;
        }
        TimeDelta& operator-=(TimeDelta other)
        {
            delta_ -= other.delta_;
            return *this;
        }
        TimeDelta operator-() const
        {
            return TimeDelta(-delta_);
        }

        // ����������, ֻ�����˳���, �Ӽ�����Ӧ��ʹ������Ĳ���.
        TimeDelta operator*(int64 a) const
        {
            return TimeDelta(delta_ * a);
        }
        TimeDelta operator/(int64 a) const
        {
            return TimeDelta(delta_ / a);
        }
        TimeDelta& operator*=(int64 a)
        {
            delta_ *= a;
            return *this;
        }
        TimeDelta& operator/=(int64 a)
        {
            delta_ /= a;
            return *this;
        }
        int64 operator/(TimeDelta a) const
        {
            return delta_ / a.delta_;
        }

        // ʵ���ں���, ��Ϊ����������Ķ���.
        Time operator+(Time t) const;
        TimeTicks operator+(TimeTicks t) const;

        // �Ƚϲ���.
        bool operator==(TimeDelta other) const
        {
            return delta_ == other.delta_;
        }
        bool operator!=(TimeDelta other) const
        {
            return delta_ != other.delta_;
        }
        bool operator<(TimeDelta other) const
        {
            return delta_ < other.delta_;
        }
        bool operator<=(TimeDelta other) const
        {
            return delta_ <= other.delta_;
        }
        bool operator>(TimeDelta other) const
        {
            return delta_ > other.delta_;
        }
        bool operator>=(TimeDelta other) const
        {
            return delta_ >= other.delta_;
        }

    private:
        friend class Time;
        friend class TimeTicks;
        friend TimeDelta operator*(int64 a, TimeDelta td);

        // ��΢��ʱ����TimeDelta����. ����˽�л��Ա����û�ֱ�ӹ���.
        // ʹ��FromSeconds��FromMilliseconds�Ⱥ����滻.
        explicit TimeDelta(int64 delta_us) : delta_(delta_us) {}

        // ΢��ʱ���.
        int64 delta_;
    };

    class Time
    {
    public:
        static const int64 kMillisecondsPerSecond = 1000;
        static const int64 kMicrosecondsPerMillisecond = 1000;
        static const int64 kMicrosecondsPerSecond = kMicrosecondsPerMillisecond *
            kMillisecondsPerSecond;
        static const int64 kMicrosecondsPerMinute = kMicrosecondsPerSecond * 60;
        static const int64 kMicrosecondsPerHour = kMicrosecondsPerMinute * 60;
        static const int64 kMicrosecondsPerDay = kMicrosecondsPerHour * 24;
        static const int64 kMicrosecondsPerWeek = kMicrosecondsPerDay * 7;
        static const int64 kNanosecondsPerMicrosecond = 1000;
        static const int64 kNanosecondsPerSecond = kNanosecondsPerMicrosecond *
            kMicrosecondsPerSecond;

        // ��ֹ���ʱ��, �Ա���õĸ�ʽ��. ������Win32��SYSTEMTIME�ṹ��.
        struct Exploded
        {
            int year;          // ��λ���ֱ�ʾ�� "2007"
            int month;         // ��1��ʼ���·� (��ֵ1==һ��, ����)
            int day_of_week;   // ��0��ʼ�Ĺ����� (0==������, ����)
            int day_of_month;  // ��1��ʼ���� (1-31)
            int hour;          // ʱ (0-23)
            int minute;        // �� (0-59)
            int second;        // �� (0-59, ��������ܻᵼ�³���60)
            int millisecond;   // ���� (0-999)

            // �򵥵�У�����ݳ�Ա�Ƿ��ڸ��Է�Χ����. 'true'�����������Գɹ�
            // ת����һ��Time.
            bool HasValidValues() const;
        };

        // NULLʱ��. ʹ��Time::Now()��ȡ��ǰʱ��.
        explicit Time() : us_(0) {}

        // �������û�г�ʼ��, ����true.
        bool is_null() const
        {
            return us_ == 0;
        }

        // ������Unix����ϵͳ����ʼʱ��(Jan 1, 1970).
        static Time UnixEpoch();

        // ���ص�ǰʱ��. ע��ϵͳʱ��ĸı�ᵼ��ʱ���˺�, �޷���֤ʱ�����ϸ����
        // ��, Ҳ�����޷���֤���ε���Now()��ʱ��һ���ǲ�ͬ��.
        static Time Now();

        // ���ص�ǰʱ��. ��Now()����, ������ʼ��ʹ��ϵͳʱ��, ���Է��ص�ʱ���ϵͳ
        // ʱ����һ�µ�.
        static Time NowFromSystemTime();

        // UTC��time_t��Time��֮���ת��.
        static Time FromTimeT(time_t tt);
        time_t ToTimeT() const;

        // Time��double��ʾ������ʼʱ��(Jan 1, 1970)����֮���ת��. Webkitʹ������
        // ��ʾ��ʽ. ��ΪWebKit��ʼ��doubleʱ��ֵ0��ʾδ��ʼ��, ����Ҳӳ��һ���յ�
        // �����ʾδ��ʼ��.
        static Time FromDoubleT(double dt);
        double ToDoubleT() const;

#ifdef _WIN32
        static Time FromFileTime(FILETIME ft);
        FILETIME ToFileTime() const;
#endif

        // ��Сʱ�侫��. windowsƽ̨��Լ����15.6ms. һЩ�ϵĲ���ϵͳ�汾���ܲ�һ��,
        // ������һ�¶Դ�.
        static const int kMinLowResolutionThresholdMs = 16;

        // ���û���ͣ��Windows�ĸ߾���ʱ��. ͣ�ø߾���ʱ��, ����
        // ActivateHighResolutionTimer��ʧ��. ��ͣ�ø߾���ʱ��ʱ, ��������ʹ֮��Ч,
        // ���Ǻ���ļ���.
        // ���������߳��е���.
        static void EnableHighResolutionTimer(bool enable);

        // ����|activate|��Ǽ���߾���ʱ�����ʹ֮��Ч. ���HighResolutionTimer
        // ͣ��(�μ�EnableHighResolutionTimer), ��������false. ���򷵻�true.
        // ���м���߾���ʱ��ĵ��ñ�����Ե���ʹ֮��Ч.
        static bool ActivateHighResolutionTimer(bool activate);

        // ����ʱ�����UTCת����Time.
        static Time FromUTCExploded(const Exploded& exploded)
        {
            return FromExploded(false, exploded);
        }
        static Time FromLocalExploded(const Exploded& exploded)
        {
            return FromExploded(true, exploded);
        }

        // ת������ֵ��Time, ����֪�ļ������ݷ����л�|Time|�ṹ. ���ṩ������
        // ���캯������Ϊ�ӵ����ߵ��ӽ����������޷�ȷ���ǺϷ���ʱ��.
        static Time FromInternalValue(int64 us)
        {
            return Time(us);
        }

        // ���ַ���ת����ʱ�����. ����: "Tue, 15 Nov 1994 12:45:26 GMT".
        // ���û��ָ��ʱ��, ʹ�ñ���ʱ��.
        static bool FromString(const wchar_t* time_string, Time* parsed_time);

        // ���л�, ��FromInternalValue�����¹������.
        // ��Ҫֱ������������, Ӧ��ʹ���ṩ�Ĳ�������.
        int64 ToInternalValue() const
        {
            return us_;
        }

        // ʹ�ñ���ʱ�����UTC����ֽṹ.
        void UTCExplode(Exploded* exploded) const
        {
            return Explode(false, exploded);
        }
        void LocalExplode(Exploded* exploded) const
        {
            return Explode(true, exploded);
        }

        // ʱ������ȡ�����������ҹ(��ҹ12����).
        Time LocalMidnight() const;

        Time& operator=(Time other)
        {
            us_ = other.us_;
            return *this;
        }

        // ����ʱ���.
        TimeDelta operator-(Time other) const
        {
            return TimeDelta(us_ - other.us_);
        }

        // �޸�ʱ��.
        Time& operator+=(TimeDelta delta)
        {
            us_ += delta.delta_;
            return *this;
        }
        Time& operator-=(TimeDelta delta)
        {
            us_ -= delta.delta_;
            return *this;
        }

        // ������ʱ��.
        Time operator+(TimeDelta delta) const
        {
            return Time(us_ + delta.delta_);
        }
        Time operator-(TimeDelta delta) const
        {
            return Time(us_ - delta.delta_);
        }

        // �Ƚϲ���.
        bool operator==(Time other) const
        {
            return us_ == other.us_;
        }
        bool operator!=(Time other) const
        {
            return us_ != other.us_;
        }
        bool operator<(Time other) const
        {
            return us_ < other.us_;
        }
        bool operator<=(Time other) const
        {
            return us_ <= other.us_;
        }
        bool operator>(Time other) const
        {
            return us_ > other.us_;
        }
        bool operator>=(Time other) const
        {
            return us_ >= other.us_;
        }

    private:
        friend class TimeDelta;

        // ���Time������ʱ��(|is_local = true|)����UTC(|is_local = false|).
        void Explode(bool is_local, Exploded* exploded) const;

        // ����ʱ��(|is_local = true|)����UTC(|is_local = false|)��ϳ�Time.
        static Time FromExploded(bool is_local, const Exploded& exploded);

        explicit Time(int64 us) : us_(us) {}

        // ��ʾ��ʼʱ��Jan 1, 1970 UTC��΢����, ����ƽ̨���.
        static const int64 kTimeTToMicrosecondsOffset;

        // ��ʾ��ǰ�Ƿ�ʹ�ÿ���ʱ��. �����ڵ�ع���ʱ, Ӧ��ѡ��ʹ�ø߾���ʱ��
        // �Խ�ʡ�õ�.
        static bool high_resolution_timer_enabled_;

        // UTC΢������ʾʱ��.
        int64 us_;
    };

    inline Time TimeDelta::operator+(Time t) const
    {
        return Time(t.us_ + delta_);
    }

    // static
    inline TimeDelta TimeDelta::FromDays(int64 days)
    {
        return TimeDelta(days * Time::kMicrosecondsPerDay);
    }

    // static
    inline TimeDelta TimeDelta::FromHours(int64 hours)
    {
        return TimeDelta(hours * Time::kMicrosecondsPerHour);
    }

    // static
    inline TimeDelta TimeDelta::FromMinutes(int64 minutes)
    {
        return TimeDelta(minutes * Time::kMicrosecondsPerMinute);
    }

    // static
    inline TimeDelta TimeDelta::FromSeconds(int64 secs)
    {
        return TimeDelta(secs * Time::kMicrosecondsPerSecond);
    }

    // static
    inline TimeDelta TimeDelta::FromMilliseconds(int64 ms)
    {
        return TimeDelta(ms * Time::kMicrosecondsPerMillisecond);
    }

    // static
    inline TimeDelta TimeDelta::FromMicroseconds(int64 us)
    {
        return TimeDelta(us);
    }

    class TimeTicks
    {
    public:
        TimeTicks() : ticks_(0) {}

        // ƽ̨��صļ�������ʾ"right now".
        // ʱ�ӵķֱ�����1-15ms, ��������Ӳ��/��������.
        static TimeTicks Now();

        // ����ƽ̨��صĸ߷ֱ��ʼ�����. ʵ������Ӳ��, �п��ܷ��رȺ���ߵľ���,
        // ����һ��. ����������ñ�Now()��ʱ, ���ޱ�Ҫ������Ҫʹ��.
        static TimeTicks HighResNow();

        // QPC��ʱ�ľ���ʱ���.
        static int64 GetQPCDriftMicroseconds();

        // ϵͳ���и߾���ʱ���򷵻�true.
        static bool IsHighResClockWorking();

        // �������û��ʼ��, ����true.
        bool is_null() const
        {
            return ticks_ == 0;
        }

        // ����TimeTicks�����ڲ���ֵ.
        int64 ToInternalValue() const
        {
            return ticks_;
        }

        TimeTicks& operator=(TimeTicks other)
        {
            ticks_ = other.ticks_;
            return *this;
        }

        // �����ֵ.
        TimeDelta operator-(TimeTicks other) const
        {
            return TimeDelta(ticks_ - other.ticks_);
        }

        // �޸�.
        TimeTicks& operator+=(TimeDelta delta)
        {
            ticks_ += delta.delta_;
            return *this;
        }
        TimeTicks& operator-=(TimeDelta delta)
        {
            ticks_ -= delta.delta_;
            return *this;
        }

        // ������ֵ.
        TimeTicks operator+(TimeDelta delta) const
        {
            return TimeTicks(ticks_ + delta.delta_);
        }
        TimeTicks operator-(TimeDelta delta) const
        {
            return TimeTicks(ticks_ - delta.delta_);
        }

        // �Ƚϲ���.
        bool operator==(TimeTicks other) const
        {
            return ticks_ == other.ticks_;
        }
        bool operator!=(TimeTicks other) const
        {
            return ticks_ != other.ticks_;
        }
        bool operator<(TimeTicks other) const
        {
            return ticks_ < other.ticks_;
        }
        bool operator<=(TimeTicks other) const
        {
            return ticks_ <= other.ticks_;
        }
        bool operator>(TimeTicks other) const
        {
            return ticks_ > other.ticks_;
        }
        bool operator>=(TimeTicks other) const
        {
            return ticks_ >= other.ticks_;
        }

    protected:
        friend class TimeDelta;

        // ��ʹ��Now()�����¶���. �����캯���ڲ�ʹ��. ��΢�����.
        explicit TimeTicks(int64 ticks) : ticks_(ticks) {}

        // ΢�������.
        int64 ticks_;

#ifdef _WIN32
        typedef DWORD (*TickFunctionType)(void);
        static TickFunctionType SetMockTickFunction(TickFunctionType ticker);
#endif
    };

    inline TimeTicks TimeDelta::operator+(TimeTicks t) const
    {
        return TimeTicks(t.ticks_ + delta_);
    }

} //namespace base

#endif //__time_h__