#include "hashgrid.h"
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
      std::size_t key{hashXy(x, y, m_size)};
      std::cout << key << '\n';
      auto it{m_cells.find(key)}; // note that find returns an iterator to the
                                  // item but if no key then cell.end() returned
      if (it == m_cells.end()) {
        std::cout << key << " is not in the list." << '\n';
        m_cells[key] = std::unordered_set<clientDict, clientHash>();
      }
      m_cells[key].insert(client);
    }
  }
};

std::pair<int, int> SpatialHashGrid::getCellIndex(const float &x,
                                                  const float &y) {
  return {3, 5};
};

std::size_t SpatialHashGrid::hashXy(int x, int y, std::size_t n) {
  const std::size_t p1 = 73856093;
  const std::size_t p2 = 83492791;

  std::size_t h =
      static_cast<std::size_t>(x) * p1 ^ static_cast<std::size_t>(y) * p2;

  return h % n;
}
