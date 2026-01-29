#include "uniformgrid.h"
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <mutex>
#include <raylib.h>
#include <raymath.h>
#include <thread>
#include <utility>

bool isRunning = true;
std::mutex gLock;
void SpatialGrid::newClient(const Vector2 &position, const Vector2 &dimensions,
                            const Vector2 &velocity) {
  m_clients.emplace_back(position, dimensions, velocity);
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

  // std::cout << i1.first << "-----" << i1.second << '\n';
  // std::cout << i2.first << "-----" << i2.second << '\n';
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

void SpatialGrid::update(float dt) {
  using clock = std::chrono::steady_clock;
  auto next = clock::now(); // take a note of current time
  while (isRunning) {
    next += std::chrono::milliseconds(16); // increment your time by delta t
    {
      std::unique_lock<std::mutex> lock(gLock);
      updatePos(dt);
      wallCollision();
      updateCells();
      for (auto &client : this->m_clients) {
        findNearby(client);
        for (auto &other : client.m_nearby) {
          if (collide(client, other))
            // momentumConservation(client, other);
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
  for (auto &client : m_clients) {
    for (auto &clientInfo : client.m_cellInfo) {
      // // remove client indices m_cells
      // int idx{clientInfo.indexInCell};
      // int last{static_cast<int>(m_cells[clientInfo.cellNumber].size() - 1)};
      // if (idx != last) {
      //   clientDict *moved{
      //       m_cells[clientInfo.cellNumber]
      //           .back()}; // save the pointer of last element then change it
      //                     // with the one you want to remove (so you
      //                     // removed!),
      //                     // finally popback the end so that you kill the
      //                     // double.
      //   m_cells[clientInfo.cellNumber][clientInfo.indexInCell] = moved;
      //   for (auto &entry : moved->m_cellInfo) {
      //     if (entry.cellNumber == clientInfo.cellNumber) {
      //       entry.indexInCell = idx;
      //       break;
      //     }
      //   }
      // }
      m_cells[clientInfo.cellNumber].pop_back();
    }
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
  Vector2 d{Vector2Subtract(a.m_position, b->m_position)};
  float distance{Vector2LengthSqr(d)};
  if (distance == 0)
    return false;
  else {
    float r{a.m_dimensions.x + b->m_dimensions.x};
    return Vector2LengthSqr(d) <= r * r;
  }
}

void SpatialGrid::momentumConservation(clientDict &a, clientDict *b) {
  Vector2 normal{Vector2Normalize(a.m_position - b->m_position)};
  Vector2 velocityDifference{a.m_velocity - b->m_velocity};
  a.m_velocity =
      a.m_velocity -
      Vector2Scale(normal, Vector2DotProduct(velocityDifference, normal));
  b->m_velocity =
      b->m_velocity +
      Vector2Scale(normal, Vector2DotProduct(velocityDifference, normal));
}

void SpatialGrid::resolveCollision(clientDict &a, clientDict &b) {
  Vector2 normal = Vector2Normalize(b.m_position - a.m_position);
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

void DrawTexturedCircle(Texture2D tex, Vector2 pos, float radius) {
  Rectangle src = {0, 0, (float)tex.width, (float)tex.height};

  Rectangle dst = {pos.x, pos.y, radius * 2.0f, radius * 2.0f};

  Vector2 origin = {radius, radius};

  DrawTexturePro(tex, src, dst, origin, 0.0f, WHITE);
}

void plotter(SpatialGrid &grid, Texture2D &circleTex, float xRange,
             float scaleX) {
  for (auto &client : grid.m_clients) {
    std::unique_lock<std::mutex> lock(gLock);
    // std::cout << client << '\n';
    Vector2 projectedClientpos{projectedVector(client.m_position, xRange)};
    DrawTexturedCircle(circleTex, projectedClientpos,
                       client.m_dimensions.x * scaleX * 0.5f);
    //            client.m_dimensions.x * scaleX * 0.5f, RED);
    // DrawCircle(projectedClientpos.x, projectedClientpos.y,
    //            client.m_dimensions.x * scaleX * 0.5f, RED);
  }
}
