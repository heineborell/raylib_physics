#include <boost/math/quadrature/trapezoidal.hpp>
#include <cmath>
#include <iostream>

int main() {
  auto f = [](double x) { return pow(std::sin(x), 5); };

  double result = boost::math::quadrature::trapezoidal(f, 0.0, M_PI);
  std::cout << result << std::endl;

  return 0;
}
