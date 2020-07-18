
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
    // Event��
    //
    // �����¼��ķ�װ, ��view���д���. �¼��������͡�һЩ��Ǻ�ʱ���.
    // ÿ����Ҫ���¼����ж�Ӧ������.
    // �¼��ǲ��ɱ��, ����֧�ֿ���.
    //
    ////////////////////////////////////////////////////////////////////////////////
    class Event
    {
    public:
        // �¼�����. (���ڻ��windows��ͷ�ļ���ͻ, ���Լ���ǰ׺)
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

        // ��ǰ֧�ֵ��¼����.
        enum EventFlags
        {
            EF_SHIFT_DOWN         = 1 << 0,
            EF_CONTROL_DOWN       = 1 << 1,
            EF_ALT_DOWN           = 1 << 2,
            EF_LEFT_BUTTON_DOWN   = 1 << 3,
            EF_MIDDLE_BUTTON_DOWN = 1 << 4,
            EF_RIGHT_BUTTON_DOWN  = 1 << 5,
            EF_COMMAND_DOWN       = 1 << 6, // ֻ����OSX��.
        };

        // �����¼�����.
        EventType GetType() const
        {
            return type_;
        }

        // ���ش���ʱ��ʱ���.
        int GetTimeStamp() const
        {
            return time_stamp_;
        }

        // ���ر��.
        int GetFlags() const
        {
            return flags_;
        }

        // ���ñ��.
        void set_flags(int flags)
        {
            flags_ = flags;
        }

        // ����shift���Ƿ���.
        bool IsShiftDown() const
        {
            return (flags_ & EF_SHIFT_DOWN) != 0;
        }

        // ����control���Ƿ���.
        bool IsControlDown() const
        {
            return (flags_ & EF_CONTROL_DOWN) != 0;
        }

        // ����alt���Ƿ���.
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

        // ��windows�ı�Ǹ�ʽ����EventFlags.
        int GetWindowsFlags() const;

        // windows�ı�Ǹ�ʽת��ΪEventFlags.
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
    // LocatedEvent��
    //
    // �¼����ڶ�λ��Ļ�ϵ�ָ��λ��.
    //
    ////////////////////////////////////////////////////////////////////////////////
    class LocatedEvent : public Event
    {
    public:
        LocatedEvent(EventType type, const gfx::Point& location, int flags)
            : Event(type, flags), location_(location) {}

        // ����һ����model��ͬ��LocatedEvent. ����ṩ��from/to��ͼ, modelλ��
        // ��'from'����ϵͳת����'to'����ϵͳ.
        LocatedEvent(const LocatedEvent& model, View* from, View* to);

        // ����X����.
        int x() const
        {
            return location_.x();
        }

        // ����Y����.
        int y() const
        {
            return location_.y();
        }

        // ����λ�õĵ�����.
        const gfx::Point& location() const
        {
            return location_;
        }

    private:
        gfx::Point location_;
    };

    ////////////////////////////////////////////////////////////////////////////////
    //
    // MouseEvent��
    //
    // �����ص������¼�.
    //
    ////////////////////////////////////////////////////////////////////////////////
    class MouseEvent : public LocatedEvent
    {
    public:
        // ����¼�����ϸ���.
        enum MouseEventFlags
        {
            EF_IS_DOUBLE_CLICK = 1 << 16,
            EF_IS_NON_CLIENT = 1 << 17
        };

        // �����µ�����¼�.
        MouseEvent(EventType type, int x, int y, int flags)
            : LocatedEvent(type, gfx::Point(x, y), flags) {}

        // �������ͺ͵����괴������¼�. ����ṩ��from/to��ͼ, �����꽫���'from'
        // ����ϵͳת����'to'����ϵͳ.
        MouseEvent(EventType type,
            View* from,
            View* to,
            const gfx::Point& l,
            int flags);

        // ����һ����model��ͬ��MouseEvent. ����ṩ��from/to��ͼ, modelλ��
        // ��'from'����ϵͳת����'to'����ϵͳ.
        MouseEvent(const MouseEvent& model, View* from, View* to);

        // ���ڿ����ж��ĸ�������.
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
    // KeyEvent��
    //
    // ������ص������¼�.
    // ע��: �¼��ǰ�����ص�, �����������ַ�.
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

        // ���ض�����keyboard_codes_win.h�е�VKEY_ֵ.
        KeyboardCode GetKeyCode() const
        {
            return key_code_;
        }

        bool IsExtendedKey() const;

        int GetRepeatCount() const
        {
            return repeat_count_;
        }

        // ����KeyState�ĵ�ǰ״̬.
        static int GetKeyStateFlags();

    private:
        KeyboardCode key_code_;
        int repeat_count_;
        int message_flags_;

        DISALLOW_COPY_AND_ASSIGN(KeyEvent);
    };

    ////////////////////////////////////////////////////////////////////////////////
    //
    // MouseWheelEvent��
    //
    // �����������û��¼�.
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
    // DropTargetEvent��
    //
    // ����ק����ʱ���͵������ͣ����ͼ.
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

        // ��ק�����й���������.
        const OSExchangeData& GetData() const { return data_; }

        // ֧����קԴ��DragDropTypes::DragOperation������λ����.
        int GetSourceOperations() const { return source_operations_; }

    private:
        const OSExchangeData& data_;
        int source_operations_;

        DISALLOW_COPY_AND_ASSIGN(DropTargetEvent);
    };

} //namespace view

#endif //__event_h__