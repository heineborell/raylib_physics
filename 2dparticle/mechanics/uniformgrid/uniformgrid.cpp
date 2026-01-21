#include "uniformgrid.h"
#include <algorithm>
#include <cstddef>
#include <iostream>
#include <raylib.h>
#include <raymath.h>
#include <utility>

clientDict SpatialGrid::newClient(const int id, const Vector2 &position,
                                  const Vector2 &dimensions) {
  clientDict client{
      id, position, dimensions, {}}; // position of the client, dimensions of
                                     // the client, indices of the client

  this->insert(client); // actually you dont need this here but for the sake
                        // of being explicit i just write it
  return client;
}

void SpatialGrid::insert(clientDict &client) {

  std::pair<int, int> i1{getCellIndex(
      client.position.x - client.dimensions.x / 2,
      -client.position.y - client.dimensions.y / 2)}; // left lower corner index

  std::pair<int, int> i2{
      getCellIndex(client.position.x + client.dimensions.x / 2,
                   -client.position.y +
                       client.dimensions.y / 2)}; // right upper corner index
  client.indices = {i1, i2};

  for (int y{i1.second}; y <= i2.second; ++y) {
    for (int x{i1.first}; x <= i2.first; ++x) {
      std::cout << y << x << '\n';
      m_cells.data()[y * m_dimensions.first + x].push_back(&client);
    }
    // std::cout << "client with key " << key << " and indices " << x << y
    //           << " is inserted." << '\n';
    // std::cout << "number of boxes of the region " << m_dimensions.first
    //           << '\n';
    // std::cout << "client with key " << key << " and i1 indices "
    //           << client.indices.first.first << client.indices.first.second
    //           << " i2 indices " << client.indices.second.first
    //           << client.indices.second.second
    //
    //           << " is inserted." << '\n';
  }
};

std::pair<int, int> SpatialGrid::getCellIndex(const float &x, const float &y) {
  float x_pos{std::clamp(
      ((x - m_bounds[0][0]) / (m_bounds[1][0] - m_bounds[0][0])), 0.0f, 1.0f)};
  float y_pos{std::clamp(
      ((y - m_bounds[0][1]) / (m_bounds[1][1] - m_bounds[0][1])), 0.0f, 1.0f)};

  int xIndex{static_cast<int>((x_pos * (m_dimensions.first - 1)))};
  int yIndex{static_cast<int>((y_pos * (m_dimensions.second - 1)))};
  return {xIndex, yIndex};
};
