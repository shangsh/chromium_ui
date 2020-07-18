
#include "fill_layout.h"

#include "base/logging.h"

#include "../view.h"

namespace view
{

    ///////////////////////////////////////////////////////////////////////////////
    // FillLayout

    FillLayout::FillLayout() {}

    FillLayout::~FillLayout() {}

    void FillLayout::Layout(View* host)
    {
        if(host->GetChildViewCount() == 0)
        {
            return;
        }

        View* frame_view = host->GetChildViewAt(0);
        frame_view->SetBounds(0, 0, host->width(), host->height());
    }

    gfx::Size FillLayout::GetPreferredSize(View* host)
    {
        DCHECK(host->GetChildViewCount() == 1);
        return host->GetChildViewAt(0)->GetPreferredSize();
    }

} //namespace view