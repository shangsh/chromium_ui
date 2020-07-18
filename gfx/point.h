
#ifndef __point_h__
#define __point_h__

#pragma once

#include <iosfwd>

typedef unsigned long DWORD;
typedef struct tagPOINT POINT;

namespace gfx
{

    // (x, y)�����.
    class Point
    {
    public:
        Point();
        Point(int x, int y);
        // |point|��һ��DWORDֵ��ʾ����. x�����ǵ�λ��short, y�����Ǹ�λ��short.
        // ����������ֵһ�㶼�Ǵ�GetMessagePos/GetCursorPos����.
        explicit Point(DWORD point);
        explicit Point(const POINT& point);
        Point& operator=(const POINT& point);

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

        // һ���������һ����С�ǱȽ�˭��yֵ���ӽ�ԭ��. ���yֵ��ͬ, ��
        // �űȽ�˭��xֵ�ӽ�ԭ��.
        // ��sets�л���vectors����ʱʹ��Points, ��Ҫ<�Ƚ�.
        bool operator<(const Point& rhs) const
        {
            return (y_==rhs.y_) ? (x_<rhs.x_) : (y_<rhs.y_);
        }

        POINT ToPOINT() const;

    private:
        int x_;
        int y_;
    };

    std::ostream& operator<<(std::ostream& out, const gfx::Point& p);

} //namespace gfx

#endif //__point_h__