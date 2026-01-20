#include <cstddef>
#include <iostream>
#include <raylib.h>
#include <string>
#include <unordered_map>
#include <utility>

struct clientDict {
  Vector2 position{};   // x, y
  Vector2 dimensions{}; // width, height
  std::pair<std::pair<int, int>, std::pair<int, int>> indices{};
};

class SpatialHashGrid {
public:
  std::pair<int, int> m_bounds; // size of the world
  std::pair<float, float>
      m_dimensions; // the basically will control how many cells we have
  std::unordered_map<std::size_t, clientDict>
      m_cells; // a dictionary with string keys and client values

  SpatialHashGrid(std::pair<int, int> bounds,
                  std::pair<float, float> dimensions,
                  const std::unordered_map<std::size_t, clientDict> &cells)
      : m_bounds{bounds}, m_dimensions(dimensions), m_cells{cells} {
    std::cout << "hashgrid initialized!" << '\n';
  };
  clientDict newClient(const Vector2 &position, const Vector2 &dimensions);

private:
  void insert(clientDict &client);
  std::pair<int, int> getCellIndex(const float &x, const float &y);
};
