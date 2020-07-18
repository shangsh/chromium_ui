
#ifndef __resource_bundle_h__
#define __resource_bundle_h__

#pragma once

#include <string>

#include "base/data_pack.h"
#include "base/file_path.h"
#include "base/lock.h"
#include "base/ref_counted_memory.h"
#include "base/scoped_ptr.h"
#include "base/string16.h"

#include "gfx/bitmap.h"

namespace base
{
    class StringPiece;
}

namespace gfx
{
    class Font;
}

// ResourceBundle is a central facility to load images and other resources,
// such as theme graphics.
// Every resource is loaded only once.
class ResourceBundle
{
public:
    // An enumeration of the various font styles used throughout Chrome.
    // The following holds true for the font sizes:
    // Small <= Base <= Bold <= Medium <= MediumBold <= Large.
    enum FontStyle
    {
        SmallFont,
        BaseFont,
        BoldFont,
        MediumFont,
        // NOTE: depending upon the locale, this may *not* result in a bold font.
        MediumBoldFont,
        LargeFont,
    };

    // 初始化进程的ResourceBundle. 如果成功加载本地资源DLL, 返回true.
    static bool InitSharedInstance(const base::FilePath& locale_resource_path);

    // Changes the locale for an already-initialized ResourceBundle.  Future
    // calls to get strings will return the strings for this new locale.  This
    // has no effect on existing or future image resources.  This has no effect
    // on existing or future image resources, and thus does not use the lock to
    // guarantee thread-safety, since all string access is expected to happen on
    // the UI thread.
    static bool ReloadSharedInstance(const base::FilePath& locale_resource_path);

    // Registers additional data pack files with the global ResourceBundle.  When
    // looking for a DataResource, we will search these files after searching the
    // main module.  This method is not thread safe!  You should call it
    // immediately after calling InitSharedInstance.
    static void AddDataPackToSharedInstance(const base::FilePath& path);

    // Delete the ResourceBundle for this process if it exists.
    static void CleanupSharedInstance();

    // Return the global resource loader instance.
    static ResourceBundle& GetSharedInstance();

    // Gets the bitmap with the specified resource_id from the current module
    // data. Returns a pointer to a shared instance of the Bitmap. This shared
    // bitmap is owned by the resource bundle and should not be freed.
    //
    // The bitmap is assumed to exist. This function will log in release, and
    // assert in debug mode if it does not. On failure, this will return a
    // pointer to a shared empty placeholder bitmap so it will be visible what
    // is missing.
    const gfx::Bitmap& GetBitmapNamed(int resource_id);

    // Loads the raw bytes of a data resource into |bytes|,
    // without doing any processing or interpretation of
    // the resource. Returns whether we successfully read the resource.
    base::RefCountedStaticMemory* LoadDataResourceBytes(int resource_id) const;

    // Return the contents of a resource in a StringPiece given the resource id.
    base::StringPiece GetRawDataResource(int resource_id) const;

    // Get a localized string given a message id.  Returns an empty
    // string if the message_id is not found.
    string16 GetLocalizedString(int message_id);

    // Returns the font for the specified style.
    const gfx::Font& GetFont(FontStyle style);

    // Returns the gfx::NativeImage, the native platform type, named resource.
    // Internally, this makes use of GetNSImageNamed(), GetPixbufNamed(), or
    // GetBitmapNamed() depending on the platform (see gfx/native_widget_types.h).
    // NOTE: On Mac the returned resource is autoreleased.
    const gfx::Bitmap& GetNativeImageNamed(int resource_id);

    // Loads and returns an icon from the app module.
    HICON LoadThemeIcon(int icon_id);

    // Loads and returns a cursor from the app module.
    HCURSOR LoadCursor(int cursor_id);

    // TODO(glen): Move these into theme provider (dialogs still depend on
    //    ResourceBundle).
    static const gfx::Color frame_color;
    static const gfx::Color frame_color_inactive;
    static const gfx::Color frame_color_app_panel;
    static const gfx::Color frame_color_app_panel_inactive;
    static const gfx::Color frame_color_incognito;
    static const gfx::Color frame_color_incognito_inactive;
    static const gfx::Color toolbar_color;
    static const gfx::Color toolbar_separator_color;

    enum AppId
    {
        BITMAP_FRAME,
        BITMAP_FRAME_INACTIVE,
        BITMAP_WINDOW_TOP_LEFT_CORNER,
        BITMAP_WINDOW_TOP_CENTER,
        BITMAP_WINDOW_TOP_RIGHT_CORNER,
        BITMAP_WINDOW_LEFT_SIDE,
        BITMAP_WINDOW_RIGHT_SIDE,
        BITMAP_WINDOW_BOTTOM_LEFT_CORNER,
        BITMAP_WINDOW_BOTTOM_CENTER,
        BITMAP_WINDOW_BOTTOM_RIGHT_CORNER,
        BITMAP_APP_TOP_LEFT,
        BITMAP_APP_TOP_CENTER,
        BITMAP_APP_TOP_RIGHT,
        BITMAP_CONTENT_BOTTOM_LEFT_CORNER,
        BITMAP_CONTENT_BOTTOM_CENTER,
        BITMAP_CONTENT_BOTTOM_RIGHT_CORNER,
        BITMAP_CONTENT_LEFT_SIDE,
        BITMAP_CONTENT_RIGHT_SIDE,
        BITMAP_CLOSE,
        BITMAP_CLOSE_SA,
        BITMAP_CLOSE_H,
        BITMAP_CLOSE_SA_H,
        BITMAP_CLOSE_P,
        BITMAP_CLOSE_SA_P,
        BITMAP_RESTORE,
        BITMAP_RESTORE_H,
        BITMAP_RESTORE_P,
        BITMAP_MAXIMIZE,
        BITMAP_MAXIMIZE_H,
        BITMAP_MAXIMIZE_P,
        BITMAP_MINIMIZE,
        BITMAP_MINIMIZE_H,
        BITMAP_MINIMIZE_P,

        BITMAP_TEXTBUTTON_TOP_LEFT_H,
        BITMAP_TEXTBUTTON_TOP_H,
        BITMAP_TEXTBUTTON_TOP_RIGHT_H,
        BITMAP_TEXTBUTTON_LEFT_H,
        BITMAP_TEXTBUTTON_CENTER_H,
        BITMAP_TEXTBUTTON_RIGHT_H,
        BITMAP_TEXTBUTTON_BOTTOM_LEFT_H,
        BITMAP_TEXTBUTTON_BOTTOM_H,
        BITMAP_TEXTBUTTON_BOTTOM_RIGHT_H,
        BITMAP_TEXTBUTTON_TOP_LEFT_P,
        BITMAP_TEXTBUTTON_TOP_P,
        BITMAP_TEXTBUTTON_TOP_RIGHT_P,
        BITMAP_TEXTBUTTON_LEFT_P,
        BITMAP_TEXTBUTTON_CENTER_P,
        BITMAP_TEXTBUTTON_RIGHT_P,
        BITMAP_TEXTBUTTON_BOTTOM_LEFT_P,
        BITMAP_TEXTBUTTON_BOTTOM_P,
        BITMAP_TEXTBUTTON_BOTTOM_RIGHT_P,
    };
    class IdConveter
    {
    public:
        IdConveter() {}
        virtual ~IdConveter() {}

        virtual int AppIdToResId(AppId app_id) = 0;
    };
    // 设置内部ID转换到资源ID接口, 接管对象所有权.
    void SetIdConveter(IdConveter* conveter);
    const gfx::Bitmap& GetBitmapAppNamed(AppId app_id);

private:
    // Helper class for managing data packs.
    class LoadedDataPack
    {
    public:
        explicit LoadedDataPack(const base::FilePath& path);
        ~LoadedDataPack();
        bool GetStringPiece(int resource_id, base::StringPiece* data) const;
        base::RefCountedStaticMemory* GetStaticMemory(int resource_id) const;

    private:
        void Load();

        scoped_ptr<base::DataPack> data_pack_;
        base::FilePath path_;

        DISALLOW_COPY_AND_ASSIGN(LoadedDataPack);
    };

    // We define a DataHandle typedef to abstract across how data is stored
    // across platforms.
    // Windows stores resources in DLLs, which are managed by HINSTANCE.
    typedef HINSTANCE DataHandle;

    // Ctor/dtor are private, since we're a singleton.
    ResourceBundle();
    ~ResourceBundle();

    // Free images_.
    void FreeImages();

    // Load the main resources.
    void LoadCommonResources();

    // 尝试加载本地资源DLL, 成功返回true.
    bool LoadLocaleResources(const base::FilePath& locale_resource_path);

    // Unload the locale specific strings and prepares to load new ones. See
    // comments for ReloadSharedInstance().
    void UnloadLocaleResources();

    // Initialize all the gfx::Font members if they haven't yet been initialized.
    void LoadFontsIfNecessary();

    // Returns a handle to bytes from the resource |module|, without doing any
    // processing or interpretation of the resource. Returns whether we
    // successfully read the resource.  Caller does not own the data returned
    // through this method and must not modify the data pointed to by |bytes|.
    static base::RefCountedStaticMemory* LoadResourceBytes(
        DataHandle module, int resource_id);

    // Creates and returns a new Bitmap given the data file to look in and the
    // resource id.  It's up to the caller to free the returned bitmap when
    // done.
    static Gdiplus::Bitmap* LoadBitmap(DataHandle dll_inst, int resource_id);

    // Class level lock.  Used to protect internal data structures that may be
    // accessed from other threads (e.g., images_).
    scoped_ptr<base::Lock> lock_;

    // Handles for data sources.
    DataHandle resources_data_;
    DataHandle locale_resources_data_;

    // References to extra data packs loaded via AddDataPackToSharedInstance.
    std::vector<LoadedDataPack*> data_packs_;

    // Cached images. The ResourceBundle caches all retrieved bitmaps and keeps
    // ownership of the pointers.
    typedef std::map<int, gfx::Bitmap> ImageMap;
    ImageMap images_;

    // The various fonts used. Cached to avoid repeated GDI creation/destruction.
    scoped_ptr<gfx::Font> base_font_;
    scoped_ptr<gfx::Font> bold_font_;
    scoped_ptr<gfx::Font> small_font_;
    scoped_ptr<gfx::Font> medium_font_;
    scoped_ptr<gfx::Font> medium_bold_font_;
    scoped_ptr<gfx::Font> large_font_;

    scoped_ptr<IdConveter> id_conveter_;

    static ResourceBundle* g_shared_instance_;

    DISALLOW_COPY_AND_ASSIGN(ResourceBundle);
};

#endif //__resource_bundle_h__