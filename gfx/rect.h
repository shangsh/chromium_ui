#ifndef __rect_h__
#define __rect_h__

#pragma once

#include "point.h"
#include "size.h"

#ifdef _WIN32
typedef struct tagRECT RECT;
#endif

namespace gfx
{

    class Insets;

    class Rect
    {
    public:
        Rect();
        Rect(int width, int height);
        Rect(int x, int y, int width, int height);
#ifdef _WIN32
        explicit Rect(const RECT& r);
        explicit Rect(const gfx::Size& size);
        Rect(const gfx::Point& origin, const gfx::Size& size);
        Rect& operator=(const RECT& r);
#else
        explicit Rect(const gfx::Size& size);
        Rect(const gfx::Point& origin, const gfx::Size& size);
#endif

        ~Rect() {}

        int x() const { return origin_.x(); }
        void set_x(int x) { origin_.set_x(x); }

        int y() const { return origin_.y(); }
        void set_y(int y) { origin_.set_y(y); }

        int width() const { return size_.width(); }
        void set_width(int width) { size_.set_width(width); }

        int height() const { return size_.height(); }
        void set_height(int height) { size_.set_height(height); }

        const gfx::Point& origin() const { return origin_; }
        void set_origin(const gfx::Point& origin) { origin_ = origin; }

        const gfx::Size& size() const { return size_; }
        void set_size(const gfx::Size& size) { size_ = size; }

        int right() const { return x() + width(); }
        int bottom() const { return y() + height(); }

        void SetRect(int x, int y, int width, int height);

        void Inset(int horizontal, int vertical)
        {
            Inset(horizontal, vertical, horizontal, vertical);
        }

        void Inset(const gfx::Insets& insets);

        void Inset(int left, int top, int right, int bottom);

        void Offset(int horizontal, int vertical);
        void Offset(const gfx::Point& point)
        {
            Offset(point.x(), point.y());
        }

        bool IsEmpty() const { return size_.IsEmpty(); }

        bool operator==(const Rect& other) const;

        bool operator!=(const Rect& other) const
        {
            return !(*this == other);
        }

        bool operator<(const Rect& other) const;

#ifdef _WIN32
        RECT ToRECT() const;
#endif

        bool Contains(int point_x, int point_y) const;

        bool Contains(const gfx::Point& point) const
        {
            return Contains(point.x(), point.y());
        }

        bool Contains(const Rect& rect) const;

        bool Intersects(const Rect& rect) const;

        Rect Intersect(const Rect& rect) const;

        Rect Union(const Rect& rect) const;

        Rect Subtract(const Rect& rect) const;

        bool Equals(const Rect& rect) const
        {
            return *this == rect;
        }

        Rect AdjustToFit(const Rect& rect) const;

        Point CenterPoint() const;

        bool SharesEdgeWith(const gfx::Rect& rect) const;

    private:
        gfx::Point origin_;
        gfx::Size size_;
    };

    std::ostream& operator<<(std::ostream& out, const gfx::Rect& r);

} //namespace gfx

#endif //__rect_h__
