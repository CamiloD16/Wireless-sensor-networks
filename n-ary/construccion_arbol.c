#include "construccion_arbol.h"

///////////////////////////////////
//////////////FUNCIONES////////////
///////////////////////////////////

void llenar_beacon( struct beacon_enviar *b, linkaddr_t id, signed int rssi_c){
  linkaddr_copy(&b->id, &id);
  //b->id=id;
  b->rssi_c = rssi_c;
}
