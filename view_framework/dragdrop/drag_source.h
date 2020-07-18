
#ifndef __drag_source_h__
#define __drag_source_h__

#pragma once

#include <objidl.h>

#include "base/ref_counted.h"

namespace view
{

    // IDropSource�Ļ���ʵ��. ����ǰ��ק�������û������Ŀ������ͣ��֪ͨ��Ϣ.
    // �������Windows��ק�Ƿ񻹼�������, ���ṩ��ȷ�Ĺ��.
    class DragSource : public IDropSource,
        public base::RefCountedThreadSafe<DragSource>
    {
    public:
        DragSource();
        virtual ~DragSource() {}

        // ����һ��ִ��ʱֹͣ��ק����. ����ͬ��ֹͣ��ק(��Ϊ���̱�Windows����),
        // ���ǻ����Windows���´��ƶ���ʱ��ȡ����ק.
        void CancelDrag()
        {
            cancel_drag_ = true;
        }

        // IDropSourceʵ��:
        HRESULT __stdcall QueryContinueDrag(BOOL escape_pressed, DWORD key_state);
        HRESULT __stdcall GiveFeedback(DWORD effect);

        // IUnknownʵ��:
        HRESULT __stdcall QueryInterface(const IID& iid, void** object);
        ULONG __stdcall AddRef();
        ULONG __stdcall Release();

    protected:
        virtual void OnDragSourceCancel() {}
        virtual void OnDragSourceDrop() {}
        virtual void OnDragSourceMove() {}

    private:
        // �����Ҫȡ����ק����, ����Ϊtrue.
        bool cancel_drag_;

        DISALLOW_COPY_AND_ASSIGN(DragSource);
    };

} //namespace view

#endif //__drag_source_h__