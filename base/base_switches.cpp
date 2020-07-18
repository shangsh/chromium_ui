
#include "base_switches.h"

namespace base
{

    // release模式下启用DCHECKs.
    const char kEnableDCHECK[]                  = "enable-dcheck";

    // 缺省值是0, 最大的V-logging激活等级, 一般使用正数.
    const char kV[]                             = "v";

    // 每个模块的最大的V-logging等级, 覆盖缺省值.
    // 例如: "my_module=2,foo*=3"将改变所有"my_module.*"和"foo*.*"(
    // 这种匹配中"-inl"后缀会被预先忽略掉)代码文件的日志等级.
    const char kVModule[]                       = "vmodule";

} //namespace base