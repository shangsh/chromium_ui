
#include "resource_bundle.h"

#include "base/debug_util.h"
#include "base/logging.h"
#include "base/resource_util.h"
#include "base/stl_utilinl.h"
#include "base/string_piece.h"
#include "base/windows_version.h"

#include "gfx/font.h"
#include "gfx/bitmap_operations.h"

ResourceBundle* ResourceBundle::g_shared_instance_ = NULL;

/* static */
// TODO(glen): Finish moving these into theme provider (dialogs still
//    depend on these colors).
const gfx::Color ResourceBundle::frame_color = gfx::Color(66, 116, 201);
const gfx::Color ResourceBundle::frame_color_inactive = gfx::Color(161, 182, 228);
const gfx::Color ResourceBundle::frame_color_app_panel = gfx::ColorWHITE;
const gfx::Color ResourceBundle::frame_color_app_panel_inactive = gfx::ColorWHITE;
const gfx::Color ResourceBundle::frame_color_incognito = gfx::Color(83, 106, 139);
const gfx::Color ResourceBundle::frame_color_incognito_inactive = gfx::Color(126, 139, 156);
const gfx::Color ResourceBundle::toolbar_color = gfx::Color(210, 225, 246);
const gfx::Color ResourceBundle::toolbar_separator_color = gfx::Color(182, 186, 192);

/* static */
bool ResourceBundle::InitSharedInstance(const base::FilePath& locale_resource_path)
{
    DCHECK(g_shared_instance_ == NULL) << "ResourceBundle initialized twice";
    g_shared_instance_ = new ResourceBundle();

    g_shared_instance_->LoadCommonResources();
    return g_shared_instance_->LoadLocaleResources(locale_resource_path);
}

/* static */
bool ResourceBundle::ReloadSharedInstance(
    const base::FilePath& locale_resource_path)
{
    DCHECK(g_shared_instance_ != NULL) << "ResourceBundle not initialized";

    g_shared_instance_->UnloadLocaleResources();
    return g_shared_instance_->LoadLocaleResources(locale_resource_path);
}

/* static */
void ResourceBundle::AddDataPackToSharedInstance(const base::FilePath& path)
{
    DCHECK(g_shared_instance_ != NULL) << "ResourceBundle not initialized";
    g_shared_instance_->data_packs_.push_back(new LoadedDataPack(path));
}

/* static */
void ResourceBundle::CleanupSharedInstance()
{
    if(g_shared_instance_)
    {
        delete g_shared_instance_;
        g_shared_instance_ = NULL;
    }
}

/* static */
ResourceBundle& ResourceBundle::GetSharedInstance()
{
    // 之前必须调用过InitSharedInstance函数.
    CHECK(g_shared_instance_ != NULL);
    return *g_shared_instance_;
}

ResourceBundle::ResourceBundle() : lock_(new base::Lock),
resources_data_(NULL), locale_resources_data_(NULL) {}

void ResourceBundle::FreeImages()
{
    images_.clear();
}

namespace
{

    Gdiplus::Bitmap* BitmapFromMemory(const void* buffer, size_t size)
    {
        Gdiplus::Bitmap* bitmap = NULL;

        HGLOBAL global = ::GlobalAlloc(GMEM_MOVEABLE, size);
        if(global)
        {
            void* copy_buffer = ::GlobalLock(global);
            if(copy_buffer)
            {
                CopyMemory(copy_buffer, buffer, size);
                IStream* stream = NULL;
                if(::CreateStreamOnHGlobal(copy_buffer, FALSE, &stream) == S_OK)
                {
                    bitmap = Gdiplus::Bitmap::FromStream(stream);
                    stream->Release();
                    DCHECK(bitmap->GetLastStatus() == Gdiplus::Ok);
                }
                ::GlobalUnlock(global);
            }
            ::GlobalFree(global);
        }

        return bitmap;
    }

}

/* static */
Gdiplus::Bitmap* ResourceBundle::LoadBitmap(DataHandle data_handle, int resource_id)
{
    scoped_refptr<base::RefCountedMemory> memory(
        LoadResourceBytes(data_handle, resource_id));
    if(!memory)
    {
        return NULL;
    }

    Gdiplus::Bitmap* bitmap = BitmapFromMemory(memory->front(), memory->size());
    if(!bitmap)
    {
        NOTREACHED() << "Unable to decode theme image resource " << resource_id;
        return NULL;
    }

    return bitmap;
}

base::RefCountedStaticMemory* ResourceBundle::LoadDataResourceBytes(
    int resource_id) const
{
    base::RefCountedStaticMemory* bytes = LoadResourceBytes(
        resources_data_, resource_id);

    if(!bytes && locale_resources_data_)
    {
        bytes = LoadResourceBytes(locale_resources_data_, resource_id);
    }

    // 如果主资源以及本地资源中没加载成功, 检查所有的附加数据包.
    for(std::vector<LoadedDataPack*>::const_iterator it=data_packs_.begin();
        !bytes&&it!=data_packs_.end(); ++it)
    {
        bytes = (*it)->GetStaticMemory(resource_id);
    }

    return bytes;
}

const gfx::Bitmap& ResourceBundle::GetBitmapNamed(int resource_id)
{
    // Check to see if we already have the image in the cache.
    {
        base::AutoLock lock_scope(*lock_);
        ImageMap::const_iterator found = images_.find(resource_id);
        if(found != images_.end())
        {
            return found->second;
        }
    }

    Gdiplus::Bitmap* native_bitmap = LoadBitmap(resources_data_, resource_id);
    if(native_bitmap == NULL)
    {
        native_bitmap = LoadBitmap(locale_resources_data_, resource_id);
    }

    if(native_bitmap)
    {
        gfx::Bitmap bitmap(native_bitmap);

        // We loaded successfully.  Cache the gfx version of the bitmap.
        base::AutoLock lock_scope(*lock_);

        // Another thread raced us, and has already cached the gfx image.
        if(images_.count(resource_id))
        {
            return images_[resource_id];
        }

        images_[resource_id] = bitmap;
        return images_[resource_id];
    }

    // We failed to retrieve the bitmap, show a debugging red square.
    {
        LOG(WARNING) << "Unable to load bitmap with id " << resource_id;
        NOTREACHED(); // Want to assert in debug mode.

        base::AutoLock lock_scope(*lock_);

        // The placeholder bitmap is bright red so people notice the problem.
        // This bitmap will be leaked, but this code should never be hit.
        images_[resource_id] = gfx::BitmapOperations::CreateDebugBitmap();
        return images_[resource_id];
    }
}

void ResourceBundle::LoadFontsIfNecessary()
{
    base::AutoLock lock_scope(*lock_);
    if(!base_font_.get())
    {
        base_font_.reset(new gfx::Font());

        bold_font_.reset(new gfx::Font());
        *bold_font_ = base_font_->DeriveFont(0,
            base_font_->GetStyle()|gfx::Font::BOLD);

        small_font_.reset(new gfx::Font());
        *small_font_ = base_font_->DeriveFont(-2);

        medium_font_.reset(new gfx::Font());
        *medium_font_ = base_font_->DeriveFont(3);

        medium_bold_font_.reset(new gfx::Font());
        *medium_bold_font_ = base_font_->DeriveFont(3,
            base_font_->GetStyle()|gfx::Font::BOLD);

        large_font_.reset(new gfx::Font());
        *large_font_ = base_font_->DeriveFont(8);
    }
}

const gfx::Font& ResourceBundle::GetFont(FontStyle style)
{
    LoadFontsIfNecessary();
    switch(style)
    {
    case BoldFont:
        return *bold_font_;
    case SmallFont:
        return *small_font_;
    case MediumFont:
        return *medium_font_;
    case MediumBoldFont:
        return *medium_bold_font_;
    case LargeFont:
        return *large_font_;
    default:
        return *base_font_;
    }
}

const gfx::Bitmap& ResourceBundle::GetNativeImageNamed(int resource_id)
{
    ResourceBundle& rb = ResourceBundle::GetSharedInstance();
    return rb.GetBitmapNamed(resource_id);
}

// LoadedDataPack implementation
ResourceBundle::LoadedDataPack::LoadedDataPack(const base::FilePath& path)
: path_(path)
{
    // Always preload the data packs so we can maintain constness.
    Load();
}

ResourceBundle::LoadedDataPack::~LoadedDataPack() {}

void ResourceBundle::LoadedDataPack::Load()
{
    DCHECK(!data_pack_.get());
    data_pack_.reset(new base::DataPack);
    bool success = data_pack_->Load(path_);
    CHECK(success) << "Failed to load " << path_.value();
}

bool ResourceBundle::LoadedDataPack::GetStringPiece(
    int resource_id, base::StringPiece* data) const
{
    return data_pack_->GetStringPiece(static_cast<uint32>(resource_id), data);
}

base::RefCountedStaticMemory* ResourceBundle::LoadedDataPack::GetStaticMemory(
    int resource_id) const
{
    return data_pack_->GetStaticMemory(resource_id);
}

namespace
{

#ifndef LOAD_LIBRARY_AS_DATAFILE_EXCLUSIVE
#define LOAD_LIBRARY_AS_DATAFILE_EXCLUSIVE  0x00000040
#endif
#ifndef LOAD_LIBRARY_AS_IMAGE_RESOURCE
#define LOAD_LIBRARY_AS_IMAGE_RESOURCE      0x00000020
#endif

    // Returns the flags that should be passed to LoadLibraryEx.
    DWORD GetDataDllLoadFlags()
    {
        if(base::GetWinVersion() >= base::WINVERSION_VISTA)
        {
            return LOAD_LIBRARY_AS_DATAFILE_EXCLUSIVE |
                LOAD_LIBRARY_AS_IMAGE_RESOURCE;
        }

        return DONT_RESOLVE_DLL_REFERENCES;
    }

}

ResourceBundle::~ResourceBundle()
{
    FreeImages();
    UnloadLocaleResources();
    STLDeleteContainerPointers(data_packs_.begin(), data_packs_.end());
    resources_data_ = NULL;
}

void ResourceBundle::UnloadLocaleResources()
{
    if(locale_resources_data_)
    {
        BOOL rv = FreeLibrary(locale_resources_data_);
        DCHECK(rv);
        locale_resources_data_ = NULL;
    }
}

// http://blogs.msdn.com/oldnewthing/archive/2004/10/25/247180.aspx
extern "C" IMAGE_DOS_HEADER __ImageBase;
void ResourceBundle::LoadCommonResources()
{
    // As a convenience, set resources_data_ to the current resource module.
    DCHECK(NULL == resources_data_) << "common resources already loaded";
    resources_data_ = reinterpret_cast<HMODULE>(&__ImageBase);
}

bool ResourceBundle::LoadLocaleResources(
    const base::FilePath& locale_resource_path)
{
    DCHECK(NULL == locale_resources_data_) << "locale dll already loaded";

    // 纯资源DLL, 没有可执行代码.
    locale_resources_data_ = LoadLibraryExW(locale_resource_path.value(),
        NULL, GetDataDllLoadFlags());

    return locale_resources_data_ != NULL;
}

// static
base::RefCountedStaticMemory* ResourceBundle::LoadResourceBytes(
    DataHandle module, int resource_id)
{
    void* data_ptr;
    size_t data_size;
    if(base::GetDataResourceFromModule(module, resource_id, &data_ptr,
        &data_size))
    {
        return new base::RefCountedStaticMemory(
            reinterpret_cast<const unsigned char*>(data_ptr), data_size);
    }
    else
    {
        return NULL;
    }
}

HICON ResourceBundle::LoadThemeIcon(int icon_id)
{
    HICON icon = ::LoadIcon(resources_data_, MAKEINTRESOURCE(icon_id));
    if(icon == NULL)
    {
        icon = ::LoadIcon(locale_resources_data_, MAKEINTRESOURCE(icon_id));
    }

    return icon;
}

base::StringPiece ResourceBundle::GetRawDataResource(int resource_id) const
{
    void* data_ptr;
    size_t data_size;
    if(base::GetDataResourceFromModule(resources_data_, resource_id, &data_ptr,
        &data_size))
    {
        return base::StringPiece(static_cast<const char*>(data_ptr), data_size);
    }
    else if(locale_resources_data_ && base::GetDataResourceFromModule(
        locale_resources_data_, resource_id, &data_ptr, &data_size))
    {
        return base::StringPiece(static_cast<const char*>(data_ptr), data_size);
    }

    base::StringPiece data;
    for(size_t i=0; i<data_packs_.size(); ++i)
    {
        if(data_packs_[i]->GetStringPiece(resource_id, &data))
        {
            return data;
        }
    }

    return base::StringPiece();
}

// Loads and returns a cursor from the current module.
HCURSOR ResourceBundle::LoadCursor(int cursor_id)
{
    HCURSOR cursor = ::LoadCursor(resources_data_, MAKEINTRESOURCE(cursor_id));
    if(!cursor)
    {
        cursor = ::LoadCursor(locale_resources_data_, MAKEINTRESOURCE(cursor_id));
    }

    return cursor;
}

void ResourceBundle::SetIdConveter(IdConveter* conveter)
{
    base::AutoLock lock_scope(*lock_);
    DCHECK(conveter);
    id_conveter_.reset(conveter);
}

const gfx::Bitmap& ResourceBundle::GetBitmapAppNamed(AppId app_id)
{
    int resource_id = 0;
    {
        base::AutoLock lock_scope(*lock_);
        resource_id = id_conveter_->AppIdToResId(app_id);
    }

    return GetBitmapNamed(resource_id);
}

namespace
{

#pragma warning(push)
#pragma warning(disable: 4200)
    struct STRINGRESOURCEIMAGE
    {
        WORD nLength;
        WCHAR achString[];
    };
#pragma warning(pop) // C4200

    inline const STRINGRESOURCEIMAGE* _GetStringResourceImage(HINSTANCE hInstance,
        HRSRC hResource, UINT id) throw()
    {
        const STRINGRESOURCEIMAGE* pImage;
        const STRINGRESOURCEIMAGE* pImageEnd;
        ULONG nResourceSize;
        HGLOBAL hGlobal;
        UINT iIndex;

        hGlobal = ::LoadResource(hInstance, hResource);
        if(hGlobal == NULL)
        {
            return NULL;
        }

        pImage = (const STRINGRESOURCEIMAGE*)::LockResource(hGlobal);
        if(pImage == NULL)
        {
            return NULL;
        }

        nResourceSize = ::SizeofResource(hInstance, hResource);
        pImageEnd = (const STRINGRESOURCEIMAGE*)(LPBYTE(pImage) + nResourceSize);
        iIndex = id & 0x000f;

        while((iIndex>0) && (pImage<pImageEnd))
        {
            pImage = (const STRINGRESOURCEIMAGE*)(LPBYTE(pImage) +
                (sizeof(STRINGRESOURCEIMAGE) + (pImage->nLength*sizeof(WCHAR))));
            iIndex--;
        }
        if(pImage >= pImageEnd)
        {
            return NULL;
        }
        if(pImage->nLength == 0)
        {
            return NULL;
        }

        return pImage;
    }

    inline const STRINGRESOURCEIMAGE* GetStringResourceImage(HINSTANCE hInstance,
        UINT id) throw()
    {
        HRSRC hResource;

        hResource = ::FindResource(hInstance, MAKEINTRESOURCE(((id>>4)+1)), RT_STRING);
        if(hResource == NULL)
        {
            return NULL;
        }

        return _GetStringResourceImage(hInstance, hResource, id);
    }

}

string16 ResourceBundle::GetLocalizedString(int message_id)
{
    // 如果没有本地纯资源DLL, 返回一个空字符串(比崩溃好).
    if(!locale_resources_data_)
    {
        base::StackTrace().PrintBacktrace(); // See http://crbug.com/21925.
        LOG(WARNING) << "locale resources are not loaded";
        return string16();
    }

    DCHECK(IS_INTRESOURCE(message_id));

    // Get a reference directly to the string resource.
    HINSTANCE hinstance = locale_resources_data_;
    const STRINGRESOURCEIMAGE* image = GetStringResourceImage(hinstance, message_id);
    if(!image)
    {
        base::StackTrace().PrintBacktrace(); // See http://crbug.com/21925.
        NOTREACHED() << "unable to find resource: " << message_id;
        return std::wstring();
    }

    // Copy into a string16 and return.
    return string16(image->achString, image->nLength);
}