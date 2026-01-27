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
    for (auto &clientInfo : client.m_cellInfo) {
      // remove client indices m_cells
      if (m_cells[clientInfo.cellNumber].size() != 0) {
        clientDict *moved{
            m_cells[clientInfo.cellNumber]
                .back()}; // save the pointer of last element then change it
                          // with the one you want to remove (so you removed!),
                          // finally popback the end so that you kill the
                          // double.
        m_cells[clientInfo.cellNumber][clientInfo.indexInCell] = moved;
        m_cells[clientInfo.cellNumber].pop_back();
      }
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
