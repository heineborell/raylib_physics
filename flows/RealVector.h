#pragma once

class RealVector {
public:
  float x{};
  float y{};
  RealVector(); // constructor default starts vector with no values
  RealVector(float x, float y); // constructor paramterized

  RealVector add(RealVector &vec);
  RealVector sub(RealVector &vec);
  RealVector mult(float num);
  RealVector limit(float min_speed, float max_speed);
  float getMag();
  float angleOf();
};
