#include "RealVector.h"
#include <cmath>

RealVector::RealVector() {};
RealVector::RealVector(float x_, float y_)
    : x{x_}, y{y_} {} // member initilaizer list

RealVector RealVector::add(RealVector &vec) {
  return RealVector(x + vec.x, y + vec.y);
}

RealVector RealVector::sub(RealVector &vec) {
  return RealVector(x - vec.x, y - vec.y);
}

RealVector RealVector::mult(float num) { return RealVector(num * x, num * y); }

RealVector RealVector::limit(float min_speed, float max_speed) {
  float mag{getMag()};
  RealVector res{*this}; // *this is a dereferrncing the object you are applying
                         // this function like v.limit
  if (mag == 0)
    return res;

  if (mag > max_speed)
    res =
        this->mult(max_speed / mag); // pointers use -> to call a method on them
  else if (mag < min_speed)
    res = this->mult(min_speed / mag);
  return res;
}

float RealVector::getMag() { return sqrt(pow(x, 2) + pow(y, 2)); }

float RealVector::angleOf() { return atan2(y, x); }
