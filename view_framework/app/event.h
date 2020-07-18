
#ifndef __event_h__
#define __event_h__

#pragma once

#include "base/basic_types.h"

#include "gfx/point.h"

#include "keyboard_codes_win.h"

class OSExchangeData;

namespace view
{

    class View;

    ////////////////////////////////////////////////////////////////////////////////
    //
    // Event类
    //
    // 输入事件的封装, 在view树中传递. 事件包含类型、一些标记和时间戳.
    // 每个主要的事件都有对应的子类.
    // 事件是不可变的, 但是支持拷贝.
    //
    ////////////////////////////////////////////////////////////////////////////////
    class Event
    {
    public:
        // 事件类型. (由于会跟windows的头文件冲突, 所以加上前缀)
        enum EventType
        {
            ET_UNKNOWN = 0,
            ET_MOUSE_PRESSED,
            ET_MOUSE_DRAGGED,
            ET_MOUSE_RELEASED,
            ET_MOUSE_MOVED,
            ET_MOUSE_ENTERED,
            ET_MOUSE_EXITED,
            ET_KEY_PRESSED,
            ET_KEY_RELEASED,
            ET_MOUSEWHEEL,
            ET_DROP_TARGET_EVENT
        };

        // 当前支持的事件标记.
        enum EventFlags
        {
            EF_SHIFT_DOWN         = 1 << 0,
            EF_CONTROL_DOWN       = 1 << 1,
            EF_ALT_DOWN           = 1 << 2,
            EF_LEFT_BUTTON_DOWN   = 1 << 3,
            EF_MIDDLE_BUTTON_DOWN = 1 << 4,
            EF_RIGHT_BUTTON_DOWN  = 1 << 5,
            EF_COMMAND_DOWN       = 1 << 6, // 只用于OSX上.
        };

        // 返回事件类型.
        EventType GetType() const
        {
            return type_;
        }

        // 返回触发时的时间戳.
        int GetTimeStamp() const
        {
            return time_stamp_;
        }

        // 返回标记.
        int GetFlags() const
        {
            return flags_;
        }

        // 设置标记.
        void set_flags(int flags)
        {
            flags_ = flags;
        }

        // 返回shift键是否按下.
        bool IsShiftDown() const
        {
            return (flags_ & EF_SHIFT_DOWN) != 0;
        }

        // 返回control键是否按下.
        bool IsControlDown() const
        {
            return (flags_ & EF_CONTROL_DOWN) != 0;
        }

        // 返回alt键是否按下.
        bool IsAltDown() const
        {
            return (flags_ & EF_ALT_DOWN) != 0;
        }

        bool IsMouseEvent() const
        {
            return type_==ET_MOUSE_PRESSED ||
                type_==ET_MOUSE_DRAGGED ||
                type_==ET_MOUSE_RELEASED ||
                type_==ET_MOUSE_MOVED ||
                type_==ET_MOUSE_ENTERED ||
                type_==ET_MOUSE_EXITED ||
                type_==ET_MOUSEWHEEL;
        }

        // 以windows的标记格式返回EventFlags.
        int GetWindowsFlags() const;

        // windows的标记格式转换为EventFlags.
        static int ConvertWindowsFlags(uint32 win_flags);

    protected:
        Event(EventType type, int flags);

        Event(const Event& model)
            : type_(model.GetType()),
            time_stamp_(model.GetTimeStamp()),
            flags_(model.GetFlags()) {}

    private:
        void operator=(const Event&);

        EventType type_;
        int time_stamp_;
        int flags_;
    };

    ////////////////////////////////////////////////////////////////////////////////
    //
    // LocatedEvent类
    //
    // 事件用于定位屏幕上的指定位置.
    //
    ////////////////////////////////////////////////////////////////////////////////
    class LocatedEvent : public Event
    {
    public:
        LocatedEvent(EventType type, const gfx::Point& location, int flags)
            : Event(type, flags), location_(location) {}

        // 创建一个与model相同的LocatedEvent. 如果提供了from/to视图, model位置
        // 从'from'坐标系统转换到'to'坐标系统.
        LocatedEvent(const LocatedEvent& model, View* from, View* to);

        // 返回X坐标.
        int x() const
        {
            return location_.x();
        }

        // 返回Y坐标.
        int y() const
        {
            return location_.y();
        }

        // 返回位置的点坐标.
        const gfx::Point& location() const
        {
            return location_;
        }

    private:
        gfx::Point location_;
    };

    ////////////////////////////////////////////////////////////////////////////////
    //
    // MouseEvent类
    //
    // 鼠标相关的输入事件.
    //
    ////////////////////////////////////////////////////////////////////////////////
    class MouseEvent : public LocatedEvent
    {
    public:
        // 鼠标事件的详细标记.
        enum MouseEventFlags
        {
            EF_IS_DOUBLE_CLICK = 1 << 16,
            EF_IS_NON_CLIENT = 1 << 17
        };

        // 创建新的鼠标事件.
        MouseEvent(EventType type, int x, int y, int flags)
            : LocatedEvent(type, gfx::Point(x, y), flags) {}

        // 给定类型和点坐标创建鼠标事件. 如果提供了from/to视图, 点坐标将会从'from'
        // 坐标系统转换到'to'坐标系统.
        MouseEvent(EventType type,
            View* from,
            View* to,
            const gfx::Point& l,
            int flags);

        // 创建一个与model相同的MouseEvent. 如果提供了from/to视图, model位置
        // 从'from'坐标系统转换到'to'坐标系统.
        MouseEvent(const MouseEvent& model, View* from, View* to);

        // 便于快速判断哪个键按下.
        bool IsOnlyLeftMouseButton() const
        {
            return (GetFlags()&EF_LEFT_BUTTON_DOWN) &&
                !(GetFlags()&(EF_MIDDLE_BUTTON_DOWN|EF_RIGHT_BUTTON_DOWN));
        }

        bool IsLeftMouseButton() const
        {
            return (GetFlags() & EF_LEFT_BUTTON_DOWN) != 0;
        }

        bool IsOnlyMiddleMouseButton() const
        {
            return (GetFlags()&EF_MIDDLE_BUTTON_DOWN) &&
                !(GetFlags()&(EF_LEFT_BUTTON_DOWN|EF_RIGHT_BUTTON_DOWN));
        }

        bool IsMiddleMouseButton() const
        {
            return (GetFlags()&EF_MIDDLE_BUTTON_DOWN) != 0;
        }

        bool IsOnlyRightMouseButton() const
        {
            return (GetFlags()&EF_RIGHT_BUTTON_DOWN) &&
                !(GetFlags()&(EF_LEFT_BUTTON_DOWN|EF_MIDDLE_BUTTON_DOWN));
        }

        bool IsRightMouseButton() const
        {
            return (GetFlags()&EF_RIGHT_BUTTON_DOWN) != 0;
        }

    private:
        DISALLOW_COPY_AND_ASSIGN(MouseEvent);
    };

    ////////////////////////////////////////////////////////////////////////////////
    //
    // KeyEvent类
    //
    // 键盘相关的输入事件.
    // 注意: 事件是按键相关的, 而不是输入字符.
    //
    ////////////////////////////////////////////////////////////////////////////////
    class KeyEvent : public Event
    {
    public:
        KeyEvent(EventType type,
            KeyboardCode key_code,
            int event_flags,
            int repeat_count,
            int message_flags);

        // 返回定义在keyboard_codes_win.h中的VKEY_值.
        KeyboardCode GetKeyCode() const
        {
            return key_code_;
        }

        bool IsExtendedKey() const;

        int GetRepeatCount() const
        {
            return repeat_count_;
        }

        // 返回KeyState的当前状态.
        static int GetKeyStateFlags();

    private:
        KeyboardCode key_code_;
        int repeat_count_;
        int message_flags_;

        DISALLOW_COPY_AND_ASSIGN(KeyEvent);
    };

    ////////////////////////////////////////////////////////////////////////////////
    //
    // MouseWheelEvent类
    //
    // 生成鼠标滚轮用户事件.
    //
    ////////////////////////////////////////////////////////////////////////////////
    class MouseWheelEvent : public LocatedEvent
    {
    public:
        MouseWheelEvent(int offset, int x, int y, int flags)
            : LocatedEvent(ET_MOUSEWHEEL, gfx::Point(x, y), flags),
            offset_(offset) {}

        int GetOffset() const
        {
            return offset_;
        }

    private:
        int offset_;

        DISALLOW_COPY_AND_ASSIGN(MouseWheelEvent);
    };

    ////////////////////////////////////////////////////////////////////////////////
    //
    // DropTargetEvent类
    //
    // 在拖拽操作时发送到鼠标悬停的视图.
    //
    ////////////////////////////////////////////////////////////////////////////////
    class DropTargetEvent : public LocatedEvent
    {
    public:
        DropTargetEvent(const OSExchangeData& data,
            int x,
            int y,
            int source_operations)
            : LocatedEvent(ET_DROP_TARGET_EVENT, gfx::Point(x, y), 0),
            data_(data),
            source_operations_(source_operations) {}

        // 拖拽过程中关联的数据.
        const OSExchangeData& GetData() const { return data_; }

        // 支持拖拽源的DragDropTypes::DragOperation操作的位掩码.
        int GetSourceOperations() const { return source_operations_; }

    private:
        const OSExchangeData& data_;
        int source_operations_;

        DISALLOW_COPY_AND_ASSIGN(DropTargetEvent);
    };

} //namespace view

#endif //__event_h__