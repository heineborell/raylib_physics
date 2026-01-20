#include <cstddef>
#include <iostream>
#include <raylib.h>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>

struct clientDict {
  int id{};
  Vector2 position{};   // x, y
  Vector2 dimensions{}; // width, height
  std::pair<std::pair<int, int>, std::pair<int, int>> indices{};

  // Compare ONLY the id
  bool operator==(const clientDict &other) const { return id == other.id; }
};

struct clientHash {
  std::size_t operator()(const clientDict &u) const {
    return std::hash<int>{}(u.id);
  }
};

class SpatialHashGrid {
public:
  std::pair<int, int> m_bounds; // size of the world
  float m_dimensions; // the basically will control how many cells we have
  int m_size{static_cast<int>(m_bounds.first / m_dimensions) *
             static_cast<int>(m_bounds.second / m_dimensions)};
  std::unordered_map<std::size_t, std::unordered_set<clientDict, clientHash>>
      m_cells; // a dictionary with string keys and client values

  SpatialHashGrid(
      std::pair<int, int> bounds, float dimensions,
      const std::unordered_map<
          std::size_t, std::unordered_set<clientDict, clientHash>> &cells)
      : m_bounds{bounds}, m_dimensions(dimensions), m_cells{cells} {
    std::cout << "hashgrid initialized!" << '\n';
  };
  clientDict newClient(const int id, const Vector2 &position,
                       const Vector2 &dimensions);
  std::size_t hashXy(int x, int y, std::size_t n);

private:
  void insert(clientDict &client);
  std::pair<int, int> getCellIndex(const float &x, const float &y);
};
