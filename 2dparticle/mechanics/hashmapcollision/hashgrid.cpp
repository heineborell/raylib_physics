#include "hashgrid.h"
#include <raylib.h>
#include <raymath.h>

clientDict SpatialHashGrid::newClient(const Vector2 &position,
                                      const Vector2 &dimensions) {
  clientDict client{
      position, dimensions, {0, 0}}; // position of the client, dimensions of
                                     // the client, indices of the client

  this->insert(client); // actually you dont need this here but for the sake of
                        // being explicit i just write it
  return client;
}

void SpatialHashGrid::insert(clientDict &client) {

};

int SpatialHashGrid::getCellIndex(float &x, float &y) { return 3; };
