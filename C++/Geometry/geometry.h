#include <math.h>
#include <vector>
#include <cassert>
struct Point {
  double x;
  double y;
  Point(double xo, double yo) {
    x = xo;
    y = yo;
  }
};
bool operator == (const Point& first, const Point& second) {
  return first.x == second.x && first.y == second.y;
}
bool operator != (const Point& first, const Point& second) {
  return !(first == second);
}
class Vector {
 public:
  double x;
  double y;
  Vector(Point second, Point first = {0, 0}) {
    x = second.x - first.x;
    y = second.y - first.y;
  }
  Vector() : x(0), y(0) {}
  void Normalize() {
    double abs = sqrt(y * y + x * x);
    x /= abs;
    y /= abs;
  }
};
Vector& operator * (double lambda, Vector& vector) {
  vector.x *= lambda;
  vector.y *= lambda;
  return vector;
}
double operator * (const Vector& first, const Vector& second) {
  return first.x * second.x + first.y * second.y;
}
double operator ^ (const Vector& first, const Vector& second) {
  return first.x * second.y - first.y * second.x;
}
Vector operator + (const Vector& first, const Vector& second) {
  return Vector({first.x + second.x, first.y + second.y});
}
Vector operator - (const Vector& first, Vector second) {
  second = (-1) * second;
  return first + second;
}
bool AreCollinear (Vector& first, Vector& second) {
  return (first^second) == 0;
}
bool AreOrtoganal (Vector& first, Vector& second) {
  return first * second == 0;
}
class Line {
 public:
    Line(const Point& first, const Point& second) {
      guiding_ = Vector(second, first);
      //guiding_.Normalize();
      point_ = Vector(first);
    }
    Line(double angle_koeff, double shift) {
      Line({0, shift}, {1, angle_koeff + shift});
    }
    Line(const Point& point, double angle_koeff) {
      Line(angle_koeff, point.y - angle_koeff * point.x);
    }
    Line(const Point& point, const Vector& vector) {
      point_ = Vector(point);
      guiding_ = vector;
    }
 private:
  Vector guiding_;
  Vector point_;
  friend bool operator == (Line&, Line&);
};
bool operator == (Line& first, Line& second) {
  Vector delta = first.point_ - second.point_;
  return (AreCollinear(first.guiding_, second.guiding_)
      && AreCollinear(delta, first.guiding_));
}
class Shape {
 public:
  virtual double perimeter() const = 0;
  virtual double area() const = 0;
  virtual bool operator==(const Shape& another) const = 0;
  virtual bool isCongruentTo(const Shape& another) const = 0;
  virtual bool isSimilarTo(const Shape& another) const = 0;
  virtual bool containsPoint(const Point& point) const = 0;
  virtual ~Shape(){};
};
