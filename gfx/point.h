#ifndef __point_h__
#define __point_h__

#pragma once

#include <iosfwd>
#include <cstdint>

#ifdef _WIN32
typedef unsigned long DWORD;
typedef struct tagPOINT POINT;
#else
typedef uint32_t DWORD;
#endif

namespace gfx
{

    class Point
    {
    public:
        Point();
        Point(int x, int y);
#ifdef _WIN32
        explicit Point(DWORD point);
        explicit Point(const POINT& point);
        Point& operator=(const POINT& point);
#else
        explicit Point(uint32_t point);
#endif

        ~Point() {}

        int x() const { return x_; }
        int y() const { return y_; }

        void SetPoint(int x, int y)
        {
            x_ = x;
            y_ = y;
        }

        void set_x(int x) { x_ = x; }
        void set_y(int y) { y_ = y; }

        void Offset(int delta_x, int delta_y)
        {
            x_ += delta_x;
            y_ += delta_y;
        }

        Point Add(const Point& other) const
        {
            Point copy = *this;
            copy.Offset(other.x_, other.y_);
            return copy;
        }

        Point Subtract(const Point& other) const
        {
            Point copy = *this;
            copy.Offset(-other.x_, -other.y_);
            return copy;
        }

        bool operator==(const Point& rhs) const
        {
            return x_==rhs.x_ && y_==rhs.y_;
        }

        bool operator!=(const Point& rhs) const
        {
            return !(*this == rhs);
        }

        bool operator<(const Point& rhs) const
        {
            return (y_==rhs.y_) ? (x_<rhs.x_) : (y_<rhs.y_);
        }

#ifdef _WIN32
        POINT ToPOINT() const;
#endif

    private:
        int x_;
        int y_;
    };

    std::ostream& operator<<(std::ostream& out, const gfx::Point& p);

} //namespace gfx

#endif //__point_h__
