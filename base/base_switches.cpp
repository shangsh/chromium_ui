
#include "base_switches.h"

namespace base
{

    // releaseģʽ������DCHECKs.
    const char kEnableDCHECK[]                  = "enable-dcheck";

    // ȱʡֵ��0, ����V-logging����ȼ�, һ��ʹ������.
    const char kV[]                             = "v";

    // ÿ��ģ�������V-logging�ȼ�, ����ȱʡֵ.
    // ����: "my_module=2,foo*=3"���ı�����"my_module.*"��"foo*.*"(
    // ����ƥ����"-inl"��׺�ᱻԤ�Ⱥ��Ե�)�����ļ�����־�ȼ�.
    const char kVModule[]                       = "vmodule";

} //namespace base