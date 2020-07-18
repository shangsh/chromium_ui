
#include "insets.h"

#include "base/stringprintf.h"

namespace gfx
{

    std::string Insets::ToString() const
    {
        // ���չ��캯��������˳���ӡ����Ա����.
        return base::StringPrintf("%d,%d,%d,%d",
            top_, left_, bottom_, right_);
    }

} //namespace gfx