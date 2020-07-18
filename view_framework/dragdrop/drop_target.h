
#ifndef __drop_target_h__
#define __drop_target_h__

#pragma once

#include <objidl.h>

#include "base/ref_counted.h"

// Windows�ӿ�.
struct IDropTargetHelper;

namespace view
{

    // DropTargetʵ�ַ�������ק���̵Ĺ���. ���ھ���ʱ, ������һ�㶼��Ҫ����
    // ����OnXXX����.
    //
    // ��ΪDropTargetʹ�������ü���, ���Բ�Ҫֱ��ɾ����, ���Ƿ���scoped_refptr
    // ��. ��ס�ڴ���ɾ��ǰ����RevokeDragDrop(m_hWnd).
    //
    // ��STA��ʹ�������, �������̰߳�ȫ��.
    class DropTarget : public IDropTarget
    {
    public:
        // ����һ���͸���HWND������DropTarget.
        explicit DropTarget(HWND hwnd);
        virtual ~DropTarget();

        // ��suspended����Ϊ|true|ʱ, ֹͣ�������ڳ�ʼ�����Ϸ�֪ͨ.
        bool suspended() const { return suspended_; }
        void set_suspended(bool suspended) { suspended_ = suspended; }

        // IDropTargetʵ��:
        HRESULT __stdcall DragEnter(IDataObject* data_object,
            DWORD key_state,
            POINTL cursor_position,
            DWORD* effect);
        HRESULT __stdcall DragOver(DWORD key_state,
            POINTL cursor_position,
            DWORD* effect);
        HRESULT __stdcall DragLeave();
        HRESULT __stdcall Drop(IDataObject* data_object,
            DWORD key_state,
            POINTL cursor_position,
            DWORD* effect);

        // IUnknownʵ��:
        HRESULT __stdcall QueryInterface(const IID& iid, void** object);
        ULONG __stdcall AddRef();
        ULONG __stdcall Release();

    protected:
        // ��������HWND.
        HWND GetHWND() { return hwnd_; }

        // ��ק�����е�����һ����ͣ�ڴ���ʱ����. ����ֵӦ����֧�ֵ��ϷŲ������:
        // DROPEFFECT_NONE��DROPEFFECT_COPY��DROPEFFECT_LINK��/��DROPEFFECT_MOVE.
        virtual DWORD OnDragEnter(IDataObject* data_object,
            DWORD key_state,
            POINT cursor_position,
            DWORD effect);

        // ��ק�����е�����ƶ���ͣ�ڴ���ʱ����. ����ֵӦ����֧�ֵ��ϷŲ������:
        // DROPEFFECT_NONE��DROPEFFECT_COPY��DROPEFFECT_LINK��/��DROPEFFECT_MOVE.
        virtual DWORD OnDragOver(IDataObject* data_object,
            DWORD key_state,
            POINT cursor_position,
            DWORD effect);

        // ��ק�����е�����Ƴ�������ʱ����.
        virtual void OnDragLeave(IDataObject* data_object);

        // �Ϸŵ�������ʱ����. ���ز�ȡ�Ĳ���.
        virtual DWORD OnDrop(IDataObject* data_object,
            DWORD key_state,
            POINT cursor_position,
            DWORD effect);

        // �����Ϸű�ʶ.
        static int32 GetDragIdentity() { return drag_identity_; }

    private:
        // ���ػ�����ϷŸ�����, ��Ҫʱ����һ��. ����ֵ����addrefed. ������󴴽�
        // ʧ�ܻ᷵��NULL.
        static IDropTargetHelper* DropHelper();

        // ��ǰ��ͣ���ϷŶ����ϵ����ݶ���.
        scoped_refptr<IDataObject> current_data_object_;

        // �������������ṩ�����ͣ����������ʱ����קͼ��.
        //
        // ��Ҫֱ�ӷ���! ʹ��DropHelper(), �����ڲ�����ʱ����������. ����Ĵ�������
        // ���Ѽ�ʮ����, ���ǲ���������ڴ򿪹���, ��������ק���ܾ��������õ�. ����
        // ��������ʵ���õ�ʱ��ǿ�ƴ���.
        static IDropTargetHelper* cached_drop_target_helper_;

        // �Ϸű�ʶ(id). һ�������ļ�����, ����ק�����й���һ���ƶ���HWNDʱ
        // (OnDragEnter)����. 0�Ǳ�������ʾ"no/unknown"��ʶ, ���ڳ�ʼ��. ��ʶ������
        // ֪ͨʱ������Ⱦ��.
        static int32 drag_identity_;

        // Դ���ھֲ�, ����ȷ������¼�ʱ�������, ��������Ⱦ��֪ͨ�����Ϸ�״̬.
        HWND hwnd_;

        // ��ǰ�Ƿ�ֹͣ�������ڳ�ʼ�����Ϸ�֪ͨ.
        bool suspended_;

        LONG ref_count_;

        DISALLOW_COPY_AND_ASSIGN(DropTarget);
    };

} //namespace view

#endif //__drop_target_h__