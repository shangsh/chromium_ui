
#ifndef __drop_helper_h__
#define __drop_helper_h__

#pragma once

#include "base/basic_types.h"

class OSExchangeData;

namespace gfx
{
    class Point;
}

namespace view
{

    class RootView;
    class View;

    // DropHelperΪ������ק������Ŀ����ͼ�ṩ֧��. DropHelper����ϵͳ�ϷŽ�������
    // ��ʹ��. ϵͳ��������ƶ�ʱ�����OnDragOver, ���ϷŽ���ʱ����OnDragExit����
    // OnDrop.
    class DropHelper
    {
    public:
        explicit DropHelper(RootView* root_view);
        ~DropHelper();

        // ��ǰ�Ϸ��¼���Ŀ����ͼ, ����ΪNULL.
        View* target_view() const { return target_view_; }

        // ���ش���DropHelper��RootView.
        RootView* root_view() const { return root_view_; }

        // ����target_view_ΪNULL, �������|view|.
        //
        // ����ͼ��RootView�Ƴ�ʱ����, ȷ��Ŀ����ͼ��Ϊ�Ƿ�ֵ.
        void ResetTargetViewIfEquals(View* view);

        // ��ק�����е�����ϵ�����ͼ��ʱ����. ����ΪĿ����ͼ����DragDropTypes���͵�
        // ���. ���û����ͼ�����Ϸ�, ����DRAG_NONE.
        int OnDragOver(const OSExchangeData& data,
            const gfx::Point& root_view_location,
            int drag_operation);

        // ��ק�����е�����ϵ�����ͼ����ʱ����.
        void OnDragExit();

        // ��ק�����е�����Ϸŵ�����ͼʱ����. ����ֵ�μ�OnDragOver.
        //
        // ע��: ʵ���ڵ��ñ�����֮ǰ�������OnDragOver, �ṩ����ֵ����
        // drag_operation.
        int OnDrop(const OSExchangeData& data,
            const gfx::Point& root_view_location,
            int drag_operation);

        // ���ݸ���ͼ����ϵ�и���λ�ü����Ϸŵ�Ŀ����ͼ. ����������target_view_
        // ��, �����ظ���ѯCanDrop.
        View* CalculateTargetView(const gfx::Point& root_view_location,
            const OSExchangeData& data, bool check_can_drop);

    private:
        // CalculateTargetView��ʵ��. ���|deepest_view|�ǿ�, �᷵��RootView�����
        // ����|root_view_location|�������ͼ.
        View* CalculateTargetViewImpl(const gfx::Point& root_view_location,
            const OSExchangeData& data,
            bool check_can_drop,
            View** deepest_view);

        // ������ȷ���Ϸ�֪ͨ��Ϣ��Ŀ����ͼ. ���Ŀ����ͼΪ��, ʲôҲ����.
        void NotifyDragEntered(const OSExchangeData& data,
            const gfx::Point& root_view_location,
            int drag_operation);
        int NotifyDragOver(const OSExchangeData& data,
            const gfx::Point& root_view_location,
            int drag_operation);
        void NotifyDragExit();

        // �����������RootView.
        RootView* root_view_;

        // �����¼���Ŀ����ͼ.
        View* target_view_;

        // ��ǰ�Ϸ��������������ͼ.
        View* deepest_view_;

        DISALLOW_COPY_AND_ASSIGN(DropHelper);
    };

} //namespace view

#endif //__drop_helper_h__