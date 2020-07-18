
#ifndef __view_storage_h__
#define __view_storage_h__

#pragma once

#include <map>

#include "base/singleton.h"

#include "../view.h"

// This class is a simple storage place for storing/retrieving views.  It is
// used for example in the FocusManager to store/restore focused views when the
// main window becomes active/inactive.
// It automatically removes a view from the storage if the view is removed from
// the tree hierarchy.
//
// To use it, you first need to create a view storage id that can then be used
// to store/retrieve views.

namespace view
{

    class ViewStorage
    {
    public:
        // Returns the global ViewStorage instance.
        // It is guaranted to be non NULL.
        static ViewStorage* GetSharedInstance();

        // Returns a unique storage id that can be used to store/retrieve views.
        int CreateStorageID();

        // Associates |view| with the specified |storage_id|.
        void StoreView(int storage_id, View* view);

        // Returns the view associated with |storage_id| if any, NULL otherwise.
        View* RetrieveView(int storage_id);

        // Removes the view associated with |storage_id| if any.
        void RemoveView(int storage_id);

        // Notifies the ViewStorage that a view was removed from its parent somewhere.
        void ViewRemoved(View* parent, View* removed);

    private:
        friend struct base::DefaultSingletonTraits<ViewStorage>;

        ViewStorage();
        ~ViewStorage();

        // Removes the view associated with |storage_id|. If |remove_all_ids| is true,
        // all other mapping pointing to the same view are removed as well.
        void EraseView(int storage_id, bool remove_all_ids);

        // Next id for the view storage.
        int view_storage_next_id_;

        // The association id to View used for the view storage.
        std::map<int, View*> id_to_view_;

        // Association View to id, used to speed up view notification removal.
        std::map<View*, std::vector<int>*> view_to_ids_;

        DISALLOW_COPY_AND_ASSIGN(ViewStorage);
    };

} //namespace view

#endif //__view_storage_h__