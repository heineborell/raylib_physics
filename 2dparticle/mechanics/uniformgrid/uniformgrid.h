#include <cstddef>
#include <iostream>
#include <raylib.h>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

struct clientDict {
  int id{};
  Vector2 position{};   // x, y
  Vector2 dimensions{}; // width, height
  std::pair<std::pair<int, int>, std::pair<int, int>> indices{};

  // Compare ONLY the id
  bool operator==(const clientDict &other) const { return id == other.id; }
};

class SpatialGrid {
public:
  std::vector<std::vector<float>> m_bounds; // size of the world
  std::pair<int, int>
      m_dimensions; // the basically will control how many cells we have
  std::vector<std::vector<clientDict>> m_cells;

  SpatialGrid(std::vector<std::vector<float>> bounds,
              std::pair<int, int> dimensions,
              const std::vector<std::vector<clientDict>> &cells)
      : m_bounds{bounds}, m_dimensions(dimensions), m_cells{cells} {
    std::cout << "hashgrid initialized!" << '\n';
  };
  clientDict newClient(const int id, const Vector2 &position,
                       const Vector2 &dimensions);
  std::size_t hashXy(int x, int y, int n = 10);

private:
  void insert(clientDict &client);
  std::pair<int, int> getCellIndex(const float &x, const float &y);
};
