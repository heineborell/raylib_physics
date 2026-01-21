#include "hashgrid.h"
#include <algorithm>
#include <cstddef>
#include <iostream>
#include <raylib.h>
#include <raymath.h>
#include <utility>

clientDict SpatialHashGrid::newClient(const int id, const Vector2 &position,
                                      const Vector2 &dimensions) {
  clientDict client{
      id, position, dimensions, {}}; // position of the client, dimensions of
                                     // the client, indices of the client

  this->insert(client); // actually you dont need this here but for the sake
                        // of being explicit i just write it
  return client;
}

void SpatialHashGrid::insert(clientDict &client) {

  std::pair<int, int> i1{getCellIndex(
      client.position.x - client.dimensions.x / 2,
      client.position.y - client.dimensions.y / 2)}; // left lower corner index

  std::pair<int, int> i2{getCellIndex(
      client.position.x + client.dimensions.x / 2,
      client.position.y + client.dimensions.y / 2)}; // right upper corner index
  client.indices = {i1, i2};

  for (int x{i1.first}; x <= i2.first; ++x) {
    for (int y{i1.second}; y <= i2.second; ++y) {
      std::size_t key{hashXy(x, y, m_dimensions.first * m_dimensions.second)};
      std::cout << key << '\n';
      auto it{m_cells.find(key)}; // note that find returns an iterator to the
                                  // item but if no key then cell.end() returned
      if (it == m_cells.end()) {
        // std::cout << key << " is not in the list." << '\n';
        m_cells[key] =
            std::unordered_set<clientDict,
                               clientHash>(); // as clientDict is a user defined
                                              // you need to define hash to
                                              // compare
      }
      m_cells[key].insert(client);
      std::cout << "client with key " << key << " and indices " << x << y
                << " is inserted." << '\n';
      // std::cout << "number of boxes of the region " << m_dimensions.first
      //           << '\n';
      // std::cout << "client with key " << key << " and i1 indices "
      //           << client.indices.first.first << client.indices.first.second
      //           << " i2 indices " << client.indices.second.first
      //           << client.indices.second.second
      //
      //           << " is inserted." << '\n';
    }
  }
};

std::pair<int, int> SpatialHashGrid::getCellIndex(const float &x,
                                                  const float &y) {
  float x_pos{std::clamp(
      ((x - m_bounds[0][0]) / (m_bounds[1][0] - m_bounds[0][0])), 0.0f, 1.0f)};
  float y_pos{std::clamp(
      ((y - m_bounds[0][1]) / (m_bounds[1][1] - m_bounds[0][1])), 0.0f, 1.0f)};

  int xIndex{static_cast<int>((x_pos * (m_dimensions.first - 1)))};
  int yIndex{static_cast<int>((y_pos * (m_dimensions.second - 1)))};
  return {xIndex, yIndex};
};

std::size_t SpatialHashGrid::hashXy(int x, int y, int n) {
  const std::size_t p1 = 73856093;
  const std::size_t p2 = 83492791;

  std::size_t h =
      static_cast<std::size_t>(x) * p1 ^ static_cast<std::size_t>(y) * p2;

  return h % n;
}
