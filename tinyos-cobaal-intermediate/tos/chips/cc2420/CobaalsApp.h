#ifndef COBAALSAPP_H
#define COBAALSAPP_H

enum {
  PLANT_RX_NODE_ID = 1,
  CONTROLLER_TX_NODE_ID = 0
};

const uint8_t FLOW_ID[] = {0b00000000, 0b00000000, 0b00000001, 0b00000001, 0b00010101,
                           0b00000101, 0b00000101, 0b00000010, 0b00000010, 0b00000010,
                           0b00010010, 0b00010010, 0b00010010, 0b00001000, 0b00001000,
                           0b00001000, 0b00001000, 0b00001000, 0b00010100, 0b00000100,
                           0b00010100, 0b00010000, 0b00010000, 0b00010000, 0b11111111};

#endif
