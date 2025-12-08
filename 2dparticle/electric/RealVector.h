#pragma once

class RealVector {
public:
  double x{};
  double y{};
  RealVector(); // constructor default starts vector with no values
  RealVector(double x, double y); // constructor parametrized

  RealVector add(RealVector &vec);
  RealVector sub(RealVector &vec);
  RealVector mult(double num);
  RealVector limit(double min_speed, double max_speed);
  double getMag();
  double angleOf();
};
