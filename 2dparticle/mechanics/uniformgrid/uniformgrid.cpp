#include "uniformgrid.h"
#include <algorithm>
#include <cstddef>
#include <iostream>
#include <raylib.h>
#include <raymath.h>
#include <utility>

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

  return {i1, i2};
}

void SpatialGrid::insert(clientDict &client) {

  client.m_indices = getCellIndices(client);
  // this for loop  is weird as the boxes for y is starting on top we have to
  // change the places of i2 and i1 because of the box coordinate system.
  // According to world coordinates i1 is still lower left and i2 is upper right
  for (int y{client.m_indices.second.second};
       y <= client.m_indices.first.second; ++y) {
    for (int x{client.m_indices.first.first};
         x <= client.m_indices.second.first; ++x) {
      m_cells.data()[y * m_dimensions.first + x].push_back(&client);
    }
  }
};

std::pair<int, int> SpatialGrid::getCellIndex(const float &x, const float &y) {
  float x_pos{std::clamp(
      ((x - m_bounds[0][0]) / (m_bounds[1][0] - m_bounds[0][0])), 0.0f, 1.0f)};
  float y_pos{std::clamp(
      ((m_bounds[1][1] - y) / (m_bounds[1][1] - m_bounds[0][1])), 0.0f, 1.0f)};

  int xIndex{static_cast<int>((x_pos * (m_dimensions.first)))};
  int yIndex{static_cast<int>((y_pos * (m_dimensions.second)))};
  return {xIndex, yIndex};
};
void SpatialGrid::update(float dt) {
  updatePos(dt);
  wallCollision();
  updateCells();
}

void SpatialGrid::updatePos(float dt) {
  // update position in real world coordinates
  for (auto &client : m_clients)
    client.m_position =
        Vector2Add(client.m_position, Vector2Scale(client.m_velocity, dt));
}

void SpatialGrid::wallCollision() {
  // wall collisions
  for (auto &client : m_clients) {
    if (client.m_position.x + client.m_dimensions.x / 2 > m_bounds[1][1])
      client.m_velocity.x = -client.m_velocity.x;
    if (client.m_position.x - client.m_dimensions.x / 2 < m_bounds[0][0])
      client.m_velocity.x = -client.m_velocity.x;
    if (client.m_position.y + client.m_dimensions.y / 2 > m_bounds[1][1])
      client.m_velocity.y = -client.m_velocity.y;
    if (client.m_position.y - client.m_dimensions.y / 2 < m_bounds[0][1])
      client.m_velocity.y = -client.m_velocity.y;
  }
}
void SpatialGrid::updateCells() {
  for (auto &client : m_clients) {
    std::cout << &client << '\n';
    for (int y{client.m_indices.second.second};
         y <= client.m_indices.first.second; ++y) {
      for (int x{client.m_indices.first.first};
           x <= client.m_indices.second.first; ++x) {
        // std::erase(m_cells[y * m_dimensions.first + x], &client);
      }
    }
  }
  std::cout << "--------------" << '\n';
}
