
#include "window.h"

#include "gfx/font.h"
#include "gfx/size.h"

#include "../widget/widget.h"

namespace view
{

    // static
    int Window::GetLocalizedContentsWidth(int col_resource_id)
    {
        // WLW TODO: 获取本地基本字体.
        return 12;
    }

    // static
    int Window::GetLocalizedContentsHeight(int row_resource_id)
    {
        // WLW TODO: 获取本地基本字体.
        return 12;
    }

    // static
    gfx::Size Window::GetLocalizedContentsSize(int col_resource_id,
        int row_resource_id)
    {
        return gfx::Size(GetLocalizedContentsWidth(col_resource_id),
            GetLocalizedContentsHeight(row_resource_id));
    }

    // static
    void Window::CloseSecondaryWidget(Widget* widget)
    {
        if(!widget)
        {
            return;
        }

        // Close widget if it's identified as a secondary window.
        Window* window = widget->GetWindow();
        if(window)
        {
            if(!window->IsAppWindow())
            {
                window->Close();
            }
        }
        else
        {
            // If it's not a Window, then close it anyway since it probably is
            // secondary.
            widget->Close();
        }
    }

} //namespace view