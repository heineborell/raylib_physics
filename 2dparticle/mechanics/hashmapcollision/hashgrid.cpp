#include "hashgrid.h"
#include <iostream>
#include <raylib.h>
#include <raymath.h>
#include <utility>

clientDict SpatialHashGrid::newClient(const Vector2 &position,
                                      const Vector2 &dimensions) {
  clientDict client{position,
                    dimensions,
                    {{0, 0}, {1, 1}}}; // position of the client, dimensions of
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
  for (int x{i1.first}; x <= i2.second; ++x) {
    std::cout << x << '\n';
  }
};

std::pair<int, int> SpatialHashGrid::getCellIndex(const float &x,
                                                  const float &y) {
  return {3, 5};
};
