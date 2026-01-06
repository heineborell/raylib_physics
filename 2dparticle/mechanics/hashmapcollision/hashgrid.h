#include <raylib.h>
#include <string>
#include <unordered_map>
#include <utility>

struct clientDict {
  Vector2 position{};   // x, y
  Vector2 dimensions{}; // width, height
  std::pair<int, int> indices{};
};

class SpatialHashGrid {
  std::pair<int, int> m_bounds; // size of the world
  std::pair<float, float>
      m_dimensions; // the basically will control how many cells we have
  std::unordered_map<std::string, clientDict>
      m_cells; // a dictionary with string keys and client values
  clientDict newClient(const Vector2 &position, const Vector2 &dimensions);

private:
  void insert(clientDict &client);
  int getCellIndex(float &x, float &y);
};
