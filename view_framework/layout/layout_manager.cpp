
#include "layout_manager.h"

#include "gfx/size.h"

namespace view
{

    int LayoutManager::GetPreferredHeightForWidth(View* host, int width)
    {
        return GetPreferredSize(host).height();
    }

} //namespace view