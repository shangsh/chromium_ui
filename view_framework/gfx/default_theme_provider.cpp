
#include "default_theme_provider.h"

#include "../app/resource_bundle.h"
#include "../app/windows_util.h"

namespace view
{

    gfx::Bitmap DefaultThemeProvider::GetBitmapNamed(int id) const
    {
        return ResourceBundle::GetSharedInstance().GetBitmapNamed(id);
    }

    bool DefaultThemeProvider::ShouldUseNativeFrame() const
    {
        return ShouldUseVistaFrame();
    }

} //namespace view