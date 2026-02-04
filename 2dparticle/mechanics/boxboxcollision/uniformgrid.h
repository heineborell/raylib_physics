#pragma once
#include "constants.h"
#include <cstddef>
#include <cstdint>
#include <deque>
#include <iostream>
#include <raylib.h>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

struct RealVector2 {
  double x;
  double y;
};

enum class Shape {
  ball,
  rectangle,

};

struct cellEntry {
  int cellNumber;
  int indexInCell;
  cellEntry(int p_cellNumber, int p_indexInCell)
      : cellNumber(p_cellNumber), indexInCell(p_indexInCell) {}
};

class clientDict {
public:
  RealVector2 m_position{};   // x, y
  RealVector2 m_dimensions{}; // width, height
  RealVector2 m_velocity{};
  double m_mass{};
  Shape m_shape{};

  uint64_t lastQueryId{};
  std::vector<cellEntry> m_cellInfo;
  std::vector<clientDict *> m_nearby;

  // Compare ONLY the id
  clientDict(const RealVector2 &position, const RealVector2 &dimension,
             const RealVector2 &velocity, const double &mass,
             const Shape &shape)
      : m_position(position), m_dimensions(dimension), m_velocity(velocity),
        m_mass(mass), m_shape(shape) {};

  void applyForce(RealVector2 &force);
  void applyAcc(RealVector2 &accelaration, double &dt);
  void momentumConservation(std::vector<clientDict> &particleArray);
  void addToSpeeds(std::size_t i);
  static void printSpeeds();
  void binIndex();
  std::vector<int> &getBins();
  void updatePar(RealVector2 &accelaration, double &dt, const double &xRange,
                 std::vector<clientDict> &pparticle);
  void getTrace();
  void showTrace(Color col);
  void showVel(double length, double xRange, Color c,
               const RealVector2 &start_vel);
  void show();
};

class SpatialGrid {
public:
  std::vector<std::vector<double>> m_bounds; // size of the world
  std::pair<int, int>
      m_dimensions; // the basically will control how many cells we have
  std::vector<std::vector<clientDict *>> m_cells;
  std::vector<clientDict> m_clients;
  uint64_t queryId{0};

  SpatialGrid(std::vector<std::vector<double>> bounds,
              std::pair<int, int> dimensions)
      : m_bounds{bounds}, m_dimensions(dimensions),
        m_cells{static_cast<std::size_t>((dimensions.first + 3) *
                                         (dimensions.second + 3))} {
    std::cout << "grid initialized!" << '\n';
    m_clients.reserve(
        NUM_PARTICLES); // not used client(50000) so that we don't start
                        // 50000 immediately also have to do this because
                        // when its not reserved and vector grows all the
                        // pointers are moved! then you defer a null pointer
  };
  void DrawGridlines();
  void newClient(const RealVector2 &position, const RealVector2 &dimensions,
                 const RealVector2 &velocity, const double &mass,
                 const Shape &shape);

  void update();
  void updatePos(double dt);
  void findNearby(clientDict &client);
  void wallCollision();
  bool collide(const clientDict &a, const clientDict *b);
  void momentumConservation(clientDict &a, clientDict *b);
  void resolveCollision(clientDict &a, clientDict &b);
  void updateCells();

private:
  void insert(clientDict &client);
  void removeClient(clientDict &client);
  std::pair<int, int> getCellIndex(const double &x, const double &y);
  std::pair<std::pair<int, int>, std::pair<int, int>>
  getCellIndices(clientDict &client);
};

void DrawTexturedCircle(Texture2D tex, RealVector2 pos, double radius);
void plotter(SpatialGrid &grid, Texture2D &circleTex, double xRange,
             double scaleX);

// Add two vectors (v1 + v2)
RealVector2 RealVector2Add(const RealVector2 &v1, const RealVector2 &v2);
RealVector2 RealVector2Scale(RealVector2 &v, double scale);
RealVector2 RealVector2Subtract(const RealVector2 &v1, const RealVector2 &v2);
double RealVector2Length(RealVector2 &v);
double RealVector2LengthSqr(RealVector2 &v);
double RealVector2DotProduct(RealVector2 &v1, RealVector2 &v2);
