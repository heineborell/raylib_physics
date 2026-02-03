#include "uniformgrid.h"
#include <algorithm>
#include <iostream>
#include <mutex>
#include <raylib.h>
#include <raymath.h>
#include <thread>
#include <utility>

bool isRunning = true;
std::mutex gLock;

void SpatialGrid::newClient(const Vector2 &position, const Vector2 &dimensions,
                            const Vector2 &velocity, const float &mass,
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

std::pair<int, int> SpatialGrid::getCellIndex(const float &x, const float &y) {
  float x_pos{std::clamp(
      ((x - m_bounds[0][0]) / (m_bounds[1][0] - m_bounds[0][0])), 0.0f, 1.0f)};
  float y_pos{std::clamp(
      ((m_bounds[1][1] - y) / (m_bounds[1][1] - m_bounds[0][1])), 0.0f, 1.0f)};

  int xIndex{static_cast<int>((x_pos * (m_dimensions.first)))};
  int yIndex{static_cast<int>((y_pos * (m_dimensions.second)))};
  return {xIndex, yIndex};
}

void SpatialGrid::update() {
  using clock = std::chrono::steady_clock;
  auto next = clock::now(); // take a note of current time
  constexpr int deltat = 1000 / FPS;
  constexpr float deltaT{1.0f / FPS};
  while (isRunning) {
    next += std::chrono::milliseconds(deltat); // increment your time by delta t
    {
      std::unique_lock<std::mutex> lock(gLock);
      updatePos(deltaT);
      wallCollision();
      updateCells();
      for (auto &client : this->m_clients) {
        findNearby(client);
        for (auto &other : client.m_nearby) {
          if (collide(client, other))
            resolveCollision(client, *other);
          else
            continue;
        }
      }
    }
    std::this_thread::sleep_until(
        next); // sleep this thread until noted time+delta t, so good thing is
               // if this your computation is longer than the delta t this
               // immediately continues
  }
}

void SpatialGrid::updatePos(float dt) {
  // update position in real world coordinates
  for (auto &client : m_clients)
    client.m_position =
        Vector2Add(client.m_position, Vector2Scale(client.m_velocity, dt));
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

  constexpr float restitution{1.0f};
  for (auto &c : m_clients) {
    float halfW = c.m_dimensions.x * 0.5f;
    float halfH = c.m_dimensions.y * 0.5f;

    // left
    if (c.m_position.x - halfW < m_bounds[0][0]) {
      c.m_position.x = m_bounds[0][0] + halfW;
      c.m_velocity.x *= -restitution;
    }

    // right
    if (c.m_position.x + halfW > m_bounds[1][0]) {
      c.m_position.x = m_bounds[1][0] - halfW;
      c.m_velocity.x *= -restitution;
    }

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
  Vector2 projectedBoundsLower{
      projectedVector({m_bounds[0][0], m_bounds[0][1]}, m_bounds[1][1])};
  Vector2 projectedBoundsUpper{
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
  switch (type_collision) {
  case 0: {
    Vector2 d{Vector2Subtract(a.m_position, b->m_position)};
    float ds2{Vector2LengthSqr(d)};
    float r{a.m_dimensions.x * 0.5f + b->m_dimensions.x * 0.5f};
    return ds2 <= r * r;
  }
  default:
    return false;
  }
}

void SpatialGrid::resolveCollision(clientDict &a, clientDict &b) {
  Vector2 difference{Vector2Subtract(b.m_position, a.m_position)};
  float length{Vector2Length(difference)};
  // check if the particles on top of each other!!
  if (length == 0.0f) {
    b.m_position.y += 0.001f;
    difference = Vector2Subtract(b.m_position, a.m_position);
    length = Vector2Length(difference);
  }

  Vector2 normal = Vector2Scale(difference, 1.0f / length);
  Vector2 rv = Vector2Subtract(b.m_velocity, a.m_velocity);
  float velAlongNormal = Vector2DotProduct(rv, normal);

  // only resolve if approaching
  if (velAlongNormal > 0)
    return;

  float restitution = 1.0f; // 1 = perfectly elastic, 0 = inelastic
  float invMassA = 1.0f / 1.0f;
  float invMassB = 1.0f / 1.0f;

  float j = -(1 + restitution) * velAlongNormal / (invMassA + invMassB);

  Vector2 impulse = Vector2Scale(normal, j);
  a.m_velocity = Vector2Subtract(a.m_velocity, Vector2Scale(impulse, invMassA));
  b.m_velocity = Vector2Add(b.m_velocity, Vector2Scale(impulse, invMassB));
}

// Draw clients

void DrawTexturedCircle(Texture2D &tex, Vector2 &pos, float radius,
                        float order) {
  Rectangle src = {order * 32, 0, 32, 32};

  Rectangle dst = {pos.x, pos.y, radius * 2.0f, radius * 2.0f};

  Vector2 origin = {radius, radius};

  DrawTexturePro(tex, src, dst, origin, 0.0f, WHITE);
}

void plotter(SpatialGrid &grid, Texture2D &circleTex, float xRange,
             float scaleX) {
  for (auto &client : grid.m_clients) {
    std::unique_lock<std::mutex> lock(gLock);
    Vector2 projectedClientpos{projectedVector(client.m_position, xRange)};
    if (client.m_shape == Shape::ball)
      DrawTexturedCircle(circleTex, projectedClientpos,
                         client.m_dimensions.x * scaleX * 0.5f, 0.0f);
    else
      DrawTexturedCircle(circleTex, projectedClientpos,
                         client.m_dimensions.x * scaleX * 0.5f, 1.0f);
  }
}
