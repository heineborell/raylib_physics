#include "config.h"
#include <cstddef>
#include <iostream>
#include <raylib.h>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

class clientDict {
public:
  Vector2 m_position{};   // x, y
  Vector2 m_dimensions{}; // width, height
  Vector2 m_velocity{};
  std::pair<std::pair<int, int>, std::pair<int, int>> m_indices{};

  // Compare ONLY the id
  // bool operator==(const clientDict &other) const { return id == other.id; }
  clientDict(const Vector2 &position, const Vector2 &dimension,
             const Vector2 &velocity)
      : m_position(position), m_dimensions(dimension), m_velocity(velocity) {};

  void applyForce(Vector2 &force);
  void applyAcc(Vector2 &accelaration, float &dt);
  void momentumConservation(std::vector<clientDict> &particleArray);
  void addToSpeeds(std::size_t i);
  static void printSpeeds();
  void binIndex();
  std::vector<int> &getBins();
  void updatePar(Vector2 &accelaration, float &dt, const float &xRange,
                 std::vector<clientDict> &pparticle);
  void getTrace();
  void showTrace(Color col);
  void showVel(double length, double xRange, Color c, const Vector2 &start_vel);
  void show();
};

class SpatialGrid {
public:
  std::vector<std::vector<float>> m_bounds; // size of the world
  std::pair<int, int>
      m_dimensions; // the basically will control how many cells we have
  std::vector<std::vector<clientDict *>> m_cells;
  std::vector<clientDict> m_clients;

  SpatialGrid(std::vector<std::vector<float>> bounds,
              std::pair<int, int> dimensions)
      : m_bounds{bounds}, m_dimensions(dimensions),
        m_cells{
            static_cast<std::size_t>(dimensions.first * dimensions.second)} {
    std::cout << "grid initialized!" << '\n';
  };
  void newClient(const Vector2 &position, const Vector2 &dimensions,
                 const Vector2 &velocity);
  void update(float dt);
  void updatePos(float dt);
  void wallCollision();

private:
  void insert(clientDict &client);
  std::pair<int, int> getCellIndex(const float &x, const float &y);
};
