#include "contiki.h"
#include "net/rime/rime.h"
#include "random.h"
#include "dev/button-sensor.h"
#include "dev/leds.h"
#include <stdio.h>
#include "contiki.h"
#include "list.h"

#define INF_NEG -999 

////////////////////////
//////ESTRUCTURAS///////
///////////////////////

struct beacon_enviar {
  linkaddr_t id;
  packetbuf_attr_t rssi_c;
}beacon_enviar;


typedef struct beacon_recibir {
  linkaddr_t id;
  packetbuf_attr_t rssi;
  packetbuf_attr_t rssi_m;
}beacon_recibir;

typedef struct node {
  linkaddr_t padre_preferido;
  packetbuf_attr_t rssi_c_f;
}node;


struct lista {
  struct lista *next;
  beacon_recibir receive_list;
};


//////////////////////////////////
//////DIRECCION ESTRUCTURAS///////
//////////////////////////////////

typedef struct lista lista;

struct beacon_enviar b;

struct node a;

/////////////////////////////////
//////////FUNCIONES/////////////
////////////////////////////////

void llenar_beacon(struct beacon_enviar * b, linkaddr_t id, signed int rssi_c);