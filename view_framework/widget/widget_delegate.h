
#ifndef __widget_delegate_h__
#define __widget_delegate_h__

#pragma once

namespace view
{

    // WidgetDelegate interface
    // Handles events on Widgets in context-specific ways.
    class WidgetDelegate
    {
    public:
        virtual ~WidgetDelegate() {}

        // Called with the display changes (color depth or resolution).
        virtual void DisplayChanged() {}

        // Called when widget active state has changed.
        virtual void IsActiveChanged(bool active) {}

        // Called when the work area (the desktop area minus taskbars,
        // menubars, etc.) changes in size.
        virtual void WorkAreaChanged() {}
    };

} //namespace view

#endif //__widget_delegate_h__