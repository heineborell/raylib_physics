#include "uniformgrid.h"
#include "config.h"
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <mutex>
#include <raylib.h>
#include <raymath.h>
#include <thread>
#include <utility>

bool isRunning = true;
std::mutex gLock;

void SpatialGrid::newClient(const RealVector2 &position,
                            const RealVector2 &dimensions,
                            const RealVector2 &velocity, const double &mass,
                            const Shape &shape) {
  m_clients.emplace_back(position, dimensions, velocity, mass, shape);
  insert(m_clients.back()); // insert client to cell list back basically give
                            // last element reference
}

std::pair<std::pair<int, int>, std::pair<int, int>>
SpatialGrid::getCellIndices(clientDict &client) {
  std::pair<int, int> i1{
      getCellIndex(client.m_position.x - client.m_dimensions.x / 2,
                   client.m_position.y -
                       client.m_dimensions.y / 2)}; // left lower corner index

  std::pair<int, int> i2{
      getCellIndex(client.m_position.x + client.m_dimensions.x / 2,
                   client.m_position.y +
                       client.m_dimensions.y / 2)}; // right upper corner index

  return {i1, i2};
}

void SpatialGrid::insert(clientDict &client) {

  std::pair<std::pair<int, int>, std::pair<int, int>> indices{
      getCellIndices(client)};
  // this for loop  is weird as the boxes for y is starting on top we have to
  // change the places of i2 and i1 because of the box coordinate system.
  // According to world coordinates i1 is still lower left and i2 is upper right
  for (int y{indices.second.second}; y <= indices.first.second; ++y) {
    for (int x{indices.first.first}; x <= indices.second.first; ++x) {
      int cellNo{y * m_dimensions.first + x};
      m_cells.data()[cellNo].push_back(&client);
      client.m_cellInfo.emplace_back(cellNo,
                                     static_cast<int>(m_cells[cellNo].size()));
    }
  }
}

std::pair<int, int> SpatialGrid::getCellIndex(const double &x,
                                              const double &y) {
  double x_pos{std::clamp(
      ((x - m_bounds[0][0]) / (m_bounds[1][0] - m_bounds[0][0])), 0.0, 1.0)};
  double y_pos{std::clamp(
      ((m_bounds[1][1] - y) / (m_bounds[1][1] - m_bounds[0][1])), 0.0, 1.0)};

  int xIndex{static_cast<int>((x_pos * (m_dimensions.first)))};
  int yIndex{static_cast<int>((y_pos * (m_dimensions.second)))};
  return {xIndex, yIndex};
}

void SpatialGrid::update() {
  {
    updatePos(dt);
    wallCollision();
    updateCells();
    for (auto &client : this->m_clients) {
      findNearby(client);
      for (auto &other : client.m_nearby) {
        if (collide(client, other)) {
          resolveCollision(client, *other);
          counter += 0.5f;
        } else
          continue;
      }
    }
  }
}

void SpatialGrid::updatePos(double dt) {
  // update position in real world coordinates
  for (auto &client : m_clients)
    client.m_position = RealVector2Add(client.m_position,
                                       RealVector2Scale(client.m_velocity, dt));
}

void SpatialGrid::findNearby(clientDict &client) {
  this->queryId++;
  client.m_nearby.clear();

  for (auto &cellInfo : client.m_cellInfo) {
    for (auto &other : m_cells[cellInfo.cellNumber]) {
      if (other == &client)
        continue;

      // check out queryid to get the other client once
      if (other->lastQueryId != queryId) {
        other->lastQueryId = queryId;
        client.m_nearby.push_back(other);
      }
    }
  }
}

void SpatialGrid::wallCollision() {

  constexpr double restitution{1.0};
  for (auto &c : m_clients) {
    double halfW = c.m_dimensions.x * 0.5;
    double halfH = c.m_dimensions.y * 0.5;

    // left
    if (c.m_position.x - halfW < m_bounds[0][0]) {
      c.m_position.x = m_bounds[0][0] + halfW;
      c.m_velocity.x *= -restitution;
      counter += 1.0;
    }

    // right
    // if (c.m_position.x + halfW > m_bounds[1][0]) {
    //   c.m_position.x = m_bounds[1][0] - halfW;
    //   c.m_velocity.x *= -restitution;
    // }

    // bottom
    if (c.m_position.y - halfH < m_bounds[0][1]) {
      c.m_position.y = m_bounds[0][1] + halfH;
      c.m_velocity.y *= -restitution;
    }

    // top
    if (c.m_position.y + halfH > m_bounds[1][1]) {
      c.m_position.y = m_bounds[1][1] - halfH;
      c.m_velocity.y *= -restitution;
    }
  }
}

void SpatialGrid::updateCells() {
  for (auto &cell : m_cells)
    cell.clear();
  for (auto &client : m_clients) {
    client.m_cellInfo.clear();
    insert(client);
  }
}

void SpatialGrid::DrawGridlines() {
  RealVector2 projectedBoundsLower{
      projectedVector({m_bounds[0][0], m_bounds[0][1]}, m_bounds[1][1])};
  RealVector2 projectedBoundsUpper{
      projectedVector({m_bounds[1][0], m_bounds[1][1]}, m_bounds[1][1])};
  for (int y{0}; y <= HEIGHT; y = y + HEIGHT / NCELLS) {
    DrawLine(projectedBoundsLower.x, y, projectedBoundsUpper.x, y, GRAY);
  }
  for (int x{0}; x <= WIDTH; x = x + WIDTH / NCELLS) {
    DrawLine(x, projectedBoundsLower.y, x, projectedBoundsUpper.y, GRAY);
  }
}

bool SpatialGrid::collide(const clientDict &a, const clientDict *b) {
  int type_collision{static_cast<int>(a.m_shape) +
                     static_cast<int>(b->m_shape)};
  RealVector2 d{RealVector2Subtract(a.m_position, b->m_position)};
  switch (type_collision) {
  case 0: {
    double ds2{RealVector2LengthSqr(d)};
    double r{a.m_dimensions.x * 0.5 + b->m_dimensions.x * 0.5};
    return ds2 <= r * r;
  }
  case 2: {
    return (std::abs(d.x) <= (a.m_dimensions.x + b->m_dimensions.x) * 0.5 &&
            std::abs(d.y) <= (a.m_dimensions.y + b->m_dimensions.y) * 0.5);
  }
  default:
    return false;
  }
}

void SpatialGrid::resolveCollision(clientDict &a, clientDict &b) {
  RealVector2 difference{RealVector2Subtract(b.m_position, a.m_position)};
  double length{RealVector2Length(difference)};
  // check if the particles on top of each other!!
  if (length == 0.0) {
    b.m_position.y += 0.001;
    difference = RealVector2Subtract(b.m_position, a.m_position);
    length = RealVector2Length(difference);
  }

  RealVector2 normal = RealVector2Scale(difference, 1.0 / length);
  RealVector2 rv = RealVector2Subtract(b.m_velocity, a.m_velocity);
  double velAlongNormal = RealVector2DotProduct(rv, normal);

  // only resolve if approaching
  if (velAlongNormal > 0)
    return;

  double restitution = 1.0; // 1 = perfectly elastic, 0 = inelastic
  double invMassA{1.0 / a.m_mass};
  double invMassB{1.0 / b.m_mass};

  double j = -(1 + restitution) * velAlongNormal / (invMassA + invMassB);

  RealVector2 impulse = RealVector2Scale(normal, j);
  a.m_velocity =
      RealVector2Subtract(a.m_velocity, RealVector2Scale(impulse, invMassA));
  b.m_velocity =
      RealVector2Add(b.m_velocity, RealVector2Scale(impulse, invMassB));
}

// Draw clients

void DrawTexturedCircle(Texture2D &tex, RealVector2 &pos, float radius,
                        float order) {
  Rectangle src = {order * 32, 0, 32, 32};

  Rectangle dst = {static_cast<float>(pos.x), static_cast<float>(pos.y),
                   radius * 2.0f, radius * 2.0f};

  Vector2 origin = {radius, radius};

  DrawTexturePro(tex, src, dst, origin, 0.0, WHITE);
}

void plotter(SpatialGrid &grid, Texture2D &circleTex, double xRange,
             double scaleX) {
  for (auto &client : grid.m_clients) {
    RealVector2 projectedClientpos{projectedVector(client.m_position, xRange)};
    if (client.m_shape == Shape::ball)
      DrawTexturedCircle(circleTex, projectedClientpos,
                         client.m_dimensions.x * scaleX * 0.5, 1.0);
    else
      DrawTexturedCircle(circleTex, projectedClientpos,
                         client.m_dimensions.x * scaleX * 0.5, 1.0);
  }
}

// Add two vectors (v1 + v2)
RealVector2 RealVector2Add(const RealVector2 &v1, const RealVector2 &v2) {
  RealVector2 result = {v1.x + v2.x, v1.y + v2.y};
  return result;
}

// Scale vector
RealVector2 RealVector2Scale(RealVector2 &v, double scale) {
  RealVector2 result = {v.x * scale, v.y * scale};

  return result;
}

// Subtract two vectors (v1 - v2)
RealVector2 RealVector2Subtract(const RealVector2 &v1, const RealVector2 &v2) {
  RealVector2 result = {v1.x - v2.x, v1.y - v2.y};
  return result;
}

// Calculate vector length
double RealVector2Length(RealVector2 &v) {
  double result{sqrtf((v.x * v.x) + (v.y * v.y))};
  return result;
}

// Calculate vector square length
double RealVector2LengthSqr(RealVector2 &v) {
  double result{(v.x * v.x) + (v.y * v.y)};
  return result;
}

// Calculate two vectors dot product
double RealVector2DotProduct(RealVector2 &v1, RealVector2 &v2) {
  double result{(v1.x * v2.x + v1.y * v2.y)};
  return result;
}
