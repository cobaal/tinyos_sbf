#ifndef REBOOTER_H
#define REBOOTER_H

typedef nx_struct MyRadioMsg {
  nx_uint64_t data;
  nx_uint8_t seq;
  nx_uint8_t state;
} MyRadioMsg;

#endif
