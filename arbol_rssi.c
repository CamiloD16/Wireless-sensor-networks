#include "contiki.h"
#include "net/rime/rime.h"
#include "random.h"
#include "dev/button-sensor.h"
#include "dev/leds.h"
#include <stdio.h>
#include "construccion_arbol.h"
#include "lib/list.h"
#include "/home/electronica/contiki/dev/cc2420/cc2420.h"

/*---------------------------------------------------------------------------*/
PROCESS(send_beacon, "enviar Beacons");
PROCESS(select_parent, "selecionar padre");
AUTOSTART_PROCESSES(&send_beacon, &select_parent);
/*---------------------------------------------------------------------------*/

LIST(node_list);
MEMB(node_list_mem, lista, 6);

static void 
broadcast_recv(struct broadcast_conn *c, const linkaddr_t *from) {
  //Muestreo de broadcast y rssi
 struct beacon_enviar b_recv;
  void *msg = packetbuf_dataptr();
  b_recv = *( (struct beacon_enviar*) msg );

  signed int rssival= cc2420_last_rssi;
  signed int rssioffset= -45;
  signed int rssienlace = rssival+ rssioffset;

  printf("broadcast message received from %d with rssi =%d with rssi_enlace=%d\n",
         b_recv.id.u8[0], b_recv.rssi_c, rssienlace);
  process_post(&select_parent, PROCESS_EVENT_CONTINUE, NULL);
  printf("Hi, I'm %d\n", b_recv.id.u8[0]);

  lista *linked_list = NULL;
  beacon_recibir *packetreceived=NULL;

  packetreceived=((beacon_recibir*) packetbuf_dataptr());
  packetreceived->rssi_m=packetbuf_attr(PACKETBUF_ATTR_RSSI);

  if(linked_list==NULL) {
    linked_list=memb_alloc(&node_list_mem);
    linkaddr_copy(&linked_list->receive_list.id, from);
    list_add(node_list, linked_list);
  }

  linked_list->receive_list.rssi=packetreceived->rssi;
  linked_list->receive_list.rssi_m=packetreceived->rssi_m;
  
  process_post(&select_parent, PROCESS_EVENT_CONTINUE, NULL);
}

static void recv_uc(struct unicast_conn *c, const linkaddr_t *from) {  
  printf("unicast message received from %d\n",
         from->u8[0]);
}

static const struct unicast_callbacks unicast_callbacks= {
  recv_uc
};

static const struct broadcast_callbacks broadcast_call= {
  broadcast_recv
};
static struct broadcast_conn broadcast;
static struct unicast_conn unicast;

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(send_beacon, ev, data) {
  static struct etimer et;

  PROCESS_EXITHANDLER(broadcast_close(&broadcast); ) PROCESS_BEGIN();

  broadcast_open(&broadcast, 129, &broadcast_call);

//Seleccionamos el nodo 1 como padre
 if(linkaddr_node_addr.u8[0]==1)
 {
   a.rssi_c_f=-1;
 }else{
   a.rssi_c_f=INF_NEG;
 }

  while(1) {
    //Envio de beacon cada 2 segundos
    etimer_set(&et, CLOCK_SECOND * 2 + random_rand() % (CLOCK_SECOND * 2));
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
    llenar_beacon(&b, linkaddr_node_addr, a.rssi_c_f);
    packetbuf_copyfrom(&b, sizeof(beacon_enviar));
    broadcast_send(&broadcast);
    printf("Message sent\n");
  }

  PROCESS_END();
}

PROCESS_THREAD(select_parent, ev, data) {

  PROCESS_EXITHANDLER(unicast_close(&unicast); ) PROCESS_BEGIN();
  unicast_open(&unicast, 146, &unicast_callbacks);

  lista *data_list;
  packetbuf_attr_t less=INF_NEG;

  while(1) {

    PROCESS_YIELD();
    less=INF_NEG;
    //Resetea la linea por si encuentra uno con mejor RSSI
    printf("#L %d 0\n",a.padre_preferido.u8[0]);
    if(a.rssi_c_f !=-1) {
      //Recorremos la lista
      for(data_list=(lista *)list_head(node_list); data_list !=NULL; data_list=(lista *)list_item_next(data_list)) {
        // se comparan los rssi y se selecciona el mayor como padre
        if(data_list->receive_list.rssi > less) {
          less=data_list->receive_list.rssi;
          a.rssi_c_f=data_list->receive_list.rssi_m+data_list->receive_list.rssi;
          a.padre_preferido=data_list->receive_list.id;
        }
      }
      printf("#L %d 1\n", a.padre_preferido.u8[0]);
     unicast_send(&unicast, &a.padre_preferido); 
    }
  }

  PROCESS_END();
}
