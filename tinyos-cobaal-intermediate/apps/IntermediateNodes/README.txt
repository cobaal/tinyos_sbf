************************* 2020-04-11 *************************

[ modify ]

1. Makefile

┌─[BEFORE]───────────────────────────────────────────────────────────────────────────┐
│  (NONE)
│
├─[AFTER]────────────────────────────────────────────────────────────────────────────┤
│  PFLAGS += -DCC2420_DEF_CHANNEL=15
│
└────────────────────────────────────────────────────────────────────────────────────┘

2. IntermediateNodesP.nc

┌─[BEFORE]───────────────────────────────────────────────────────────────────────────┐
│  uses {
│    interface Boot;
│    interface SplitControl as SerialControl;
│    interface SplitControl as RadioControl;
│
│    interface AMSend as UartSend[am_id_t id];
│    interface Receive as UartReceive[am_id_t id];
│    interface Packet as UartPacket;
│    interface AMPacket as UartAMPacket;
│
│    interface AMSend as RadioSend[am_id_t id];
│    interface Receive as RadioReceive[am_id_t id];
│    interface Packet as RadioPacket;
│    interface AMPacket as RadioAMPacket;
│
│    interface Leds;
│  }
│
├─[AFTER]────────────────────────────────────────────────────────────────────────────┤
│  uses {
│    interface Boot;
│    interface SplitControl as RadioControl;
│
│    interface AMSend as RadioSend[am_id_t id];
│    interface Receive as RadioReceive[am_id_t id];
│    interface Packet as RadioPacket;
│    interface AMPacket as RadioAMPacket;
│
│    interface Leds;
│  }
│
└────────────────────────────────────────────────────────────────────────────────────┘


┌─[BEFORE]───────────────────────────────────────────────────────────────────────────┐
│  enum {
│    UART_QUEUE_LEN = 12,
│    RADIO_QUEUE_LEN = 12,
│  };
│
│  message_t  uartQueueBufs[UART_QUEUE_LEN];
│  message_t  * ONE_NOK uartQueue[UART_QUEUE_LEN];
│  uint8_t    uartIn, uartOut;
│  bool       uartBusy, uartFull;
│
│  message_t  radioQueueBufs[RADIO_QUEUE_LEN];
│  message_t  * ONE_NOK radioQueue[RADIO_QUEUE_LEN];
│  uint8_t    radioIn, radioOut;
│  bool       radioBusy, radioFull;
│
│  task void uartSendTask();
│  task void radioSendTask();
│
├─[AFTER]────────────────────────────────────────────────────────────────────────────┤
│  enum {
│    RADIO_QUEUE_LEN = 12,
│  };
│
│  message_t  radioQueueBufs[RADIO_QUEUE_LEN];
│  message_t  * ONE_NOK radioQueue[RADIO_QUEUE_LEN];
│  uint8_t    radioIn, radioOut;
│  bool       radioBusy, radioFull;
│
│  task void radioSendTask();
│
└────────────────────────────────────────────────────────────────────────────────────┘


┌─[BEFORE]───────────────────────────────────────────────────────────────────────────┐
│  event void Boot.booted() {
│    uint8_t i;
│
│    for (i = 0; i < UART_QUEUE_LEN; i++)
│      uartQueue[i] = &uartQueueBufs[i];
│    uartIn = uartOut = 0;
│    uartBusy = FALSE;
│    uartFull = TRUE;
│
│    for (i = 0; i < RADIO_QUEUE_LEN; i++)
│      radioQueue[i] = &radioQueueBufs[i];
│    radioIn = radioOut = 0;
│    radioBusy = FALSE;
│    radioFull = TRUE;
│
│    if (call RadioControl.start() == EALREADY)
│      radioFull = FALSE;
│    if (call SerialControl.start() == EALREADY)
│      uartFull = FALSE;
│  }
│
├─[AFTER]────────────────────────────────────────────────────────────────────────────┤
│  event void Boot.booted() {
│    uint8_t i;
│
│    for (i = 0; i < RADIO_QUEUE_LEN; i++)
│      radioQueue[i] = &radioQueueBufs[i];
│    radioIn = radioOut = 0;
│    radioBusy = FALSE;
│    radioFull = TRUE;
│
│    if (call RadioControl.start() == EALREADY)
│      radioFull = FALSE;
│  }
│
└────────────────────────────────────────────────────────────────────────────────────┘


┌─[BEFORE]───────────────────────────────────────────────────────────────────────────┐
│  event void SerialControl.startDone(error_t error) {
│    if (error == SUCCESS) {
│      uartFull = FALSE;
│    }
│  }
│
│
│  event void SerialControl.stopDone(error_t error) {}
│
├─[AFTER]────────────────────────────────────────────────────────────────────────────┤
│  (NONE)
│
└────────────────────────────────────────────────────────────────────────────────────┘


┌─[BEFORE]───────────────────────────────────────────────────────────────────────────┐
│  event message_t *RadioSnoop.receive[am_id_t id](message_t *msg,
│						    void *payload,
│						    uint8_t len) {
│    return receive(msg, payload, len);
│  }
│
├─[AFTER]────────────────────────────────────────────────────────────────────────────┤
│  (NONE)
│
└────────────────────────────────────────────────────────────────────────────────────┘


┌─[BEFORE]───────────────────────────────────────────────────────────────────────────┐
│  message_t* receive(message_t *msg, void *payload, uint8_t len) {
│    message_t *ret = msg;
│
│    atomic {
│      if (!uartFull)
│	{
│	  ret = uartQueue[uartIn];
│	  uartQueue[uartIn] = msg;
│
│	  uartIn = (uartIn + 1) % UART_QUEUE_LEN;
│
│	  if (uartIn == uartOut)
│	    uartFull = TRUE;
│
│	  if (!uartBusy)
│	    {
│	      post uartSendTask();
│	      uartBusy = TRUE;
│	    }
│	}
│      else
│	dropBlink();
│    }
│
│    return ret;
│  }
│
├─[AFTER]────────────────────────────────────────────────────────────────────────────┤
│  message_t* receive(message_t *msg, void *payload, uint8_t len) {
│    message_t *ret = msg;
│
│    atomic
│      if (!radioFull)
│	{
│	  ret = radioQueue[radioIn];
│	  radioQueue[radioIn] = msg;
│	  if (++radioIn >= RADIO_QUEUE_LEN)
│	    radioIn = 0;
│	  if (radioIn == radioOut)
│	    radioFull = TRUE;
│
│	  if (!radioBusy)
│	    {
│	      post radioSendTask();
│	      radioBusy = TRUE;
│	    }
│	}
│      else
│	dropBlink();
│
│    return ret;
│  }
│
└────────────────────────────────────────────────────────────────────────────────────┘


┌─[BEFORE]───────────────────────────────────────────────────────────────────────────┐
│  uint8_t tmpLen;
│
│  task void uartSendTask() {
│    uint8_t len;
│    am_id_t id;
│    am_addr_t addr, src;
│    message_t* msg;
│    am_group_t grp;
│    atomic
│      if (uartIn == uartOut && !uartFull)
│	{
│	  uartBusy = FALSE;
│	  return;
│	}
│
│    msg = uartQueue[uartOut];
│    tmpLen = len = call RadioPacket.payloadLength(msg);
│    id = call RadioAMPacket.type(msg);
│    addr = call RadioAMPacket.destination(msg);
│    src = call RadioAMPacket.source(msg);
│    grp = call RadioAMPacket.group(msg);
│    call UartPacket.clear(msg);
│    call UartAMPacket.setSource(msg, src);
│    call UartAMPacket.setGroup(msg, grp);
│
│    if (call UartSend.send[id](addr, uartQueue[uartOut], len) == SUCCESS)
│      call Leds.led1Toggle();
│    else
│      {
│	failBlink();
│	post uartSendTask();
│      }
│  }
│
│  event void UartSend.sendDone[am_id_t id](message_t* msg, error_t error) {
│    if (error != SUCCESS)
│      failBlink();
│    else
│      atomic
│	if (msg == uartQueue[uartOut])
│	  {
│	    if (++uartOut >= UART_QUEUE_LEN)
│	      uartOut = 0;
│	    if (uartFull)
│	      uartFull = FALSE;
│	  }
│    post uartSendTask();
│  }
│
│  event message_t *UartReceive.receive[am_id_t id](message_t *msg,
│						   void *payload,
│						   uint8_t len) {
│    message_t *ret = msg;
│    bool reflectToken = FALSE;
│
│    atomic
│      if (!radioFull)
│	{
│	  reflectToken = TRUE;
│	  ret = radioQueue[radioIn];
│	  radioQueue[radioIn] = msg;
│	  if (++radioIn >= RADIO_QUEUE_LEN)
│	    radioIn = 0;
│	  if (radioIn == radioOut)
│	    radioFull = TRUE;
│
│	  if (!radioBusy)
│	    {
│	      post radioSendTask();
│	      radioBusy = TRUE;
│	    }
│	}
│      else
│	dropBlink();
│
│    if (reflectToken) {
│      //call UartTokenReceive.ReflectToken(Token);
│    }
│
│    return ret;
│  }
│
├─[AFTER]────────────────────────────────────────────────────────────────────────────┤
│  (NONE)
│
└────────────────────────────────────────────────────────────────────────────────────┘


┌─[BEFORE]───────────────────────────────────────────────────────────────────────────┐
│    task void radioSendTask() {
│    uint8_t len;
│    am_id_t id;
│    am_addr_t addr,source;
│    message_t* msg;
│
│    atomic
│      if (radioIn == radioOut && !radioFull)
│	{
│	  radioBusy = FALSE;
│	  return;
│	}
│
│    msg = radioQueue[radioOut];
│    len = call UartPacket.payloadLength(msg);
│    addr = call UartAMPacket.destination(msg);
│    source = call UartAMPacket.source(msg);
│    id = call UartAMPacket.type(msg);
│
│    call RadioPacket.clear(msg);
│    call RadioAMPacket.setSource(msg, source);
│
│    if (call RadioSend.send[id](addr, msg, len) == SUCCESS)
│      call Leds.led0Toggle();
│    else
│      {
│	failBlink();
│	post radioSendTask();
│      }
│  }
│
├─[AFTER]────────────────────────────────────────────────────────────────────────────┤
│  task void radioSendTask() {
│    uint8_t len;
│    am_id_t id;
│    am_addr_t addr;
│    message_t* msg;
│
│    atomic
│      if (radioIn == radioOut && !radioFull)
│	{
│	  radioBusy = FALSE;
│	  return;
│	}
│
│    msg = radioQueue[radioOut];
│    len = call RadioPacket.payloadLength(msg);
│    addr = call RadioAMPacket.destination(msg);
│    id = call RadioAMPacket.type(msg);
│
│    if (call RadioSend.send[id](addr, msg, len) == SUCCESS)
│      call Leds.led0Toggle();
│    else
│      {
│	failBlink();
│	post radioSendTask();
│      }
│  }
│
│
└────────────────────────────────────────────────────────────────────────────────────┘


3. IntermediateNodesC.nc

┌─[BEFORE]───────────────────────────────────────────────────────────────────────────┐
│configuration IntermediateNodesC {
│}
│implementation {
│  components MainC, IntermediateNodesP, LedsC;
│  components ActiveMessageC as Radio, SerialActiveMessageC as Serial;
│
│  MainC.Boot <- IntermediateNodesP;
│
│  IntermediateNodesP.RadioControl -> Radio;
│  IntermediateNodesP.SerialControl -> Serial;
│
│  IntermediateNodesP.UartSend -> Serial;
│  IntermediateNodesP.UartReceive -> Serial.Receive;
│  IntermediateNodesP.UartPacket -> Serial;
│  IntermediateNodesP.UartAMPacket -> Serial;
│
│  IntermediateNodesP.RadioSend -> Radio;
│  IntermediateNodesP.RadioReceive -> Radio.Receive;
│  IntermediateNodesP.RadioPacket -> Radio;
│  IntermediateNodesP.RadioAMPacket -> Radio;
│
│  IntermediateNodesP.Leds -> LedsC;
│}
│
├─[AFTER]────────────────────────────────────────────────────────────────────────────┤
│configuration IntermediateNodesC {
│}
│implementation {
│  components MainC, IntermediateNodesP, LedsC;
│  components ActiveMessageC as Radio;
│
│  MainC.Boot <- IntermediateNodesP;
│
│  IntermediateNodesP.RadioControl -> Radio;
│
│  IntermediateNodesP.RadioSend -> Radio;
│  IntermediateNodesP.RadioReceive -> Radio.Receive;
│  IntermediateNodesP.RadioPacket -> Radio;
│  IntermediateNodesP.RadioAMPacket -> Radio;
│
│  IntermediateNodesP.Leds -> LedsC;
│}
│
└────────────────────────────────────────────────────────────────────────────────────┘


4. CC2420ActiveMessageP.nc

┌─[BEFORE]───────────────────────────────────────────────────────────────────────────┐
│  command error_t AMSend.send[am_id_t id](am_addr_t addr,
│					  message_t* msg,
│					  uint8_t len) {
│    cc2420_header_t* header = call CC2420PacketBody.getHeader( msg );
│
│    if (len > call Packet.maxPayloadLength()) {
│      return ESIZE;
│    }
│
│    header->type = id;
│    header->dest = addr;
│    header->destpan = call CC2420Config.getPanAddr();
│    header->src = call AMPacket.address();
│    header->fcf |= ( 1 << IEEE154_FCF_INTRAPAN ) |
│      ( IEEE154_ADDR_SHORT << IEEE154_FCF_DEST_ADDR_MODE ) |
│      ( IEEE154_ADDR_SHORT << IEEE154_FCF_SRC_ADDR_MODE ) ;
│    header->length = len + CC2420_SIZE;
│
│    if (call RadioResource.immediateRequest() == SUCCESS) {
│      error_t rc;
│      signal SendNotifier.aboutToSend[id](addr, msg);
│
│      rc = call SubSend.send( msg, len );
│      if (rc != SUCCESS) {
│        call RadioResource.release();
│      }
│
│      return rc;
│    } else {
│      pending_length  = len;
│      pending_message = msg;
│      return call RadioResource.request();
│    }
│  }
│
├─[AFTER]────────────────────────────────────────────────────────────────────────────┤
│  command error_t AMSend.send[am_id_t id](am_addr_t addr,
│					  message_t* msg,
│					  uint8_t len) {
│    cc2420_header_t* header = call CC2420PacketBody.getHeader( msg );
│
│    if (len > call Packet.maxPayloadLength()) {
│      return ESIZE;
│    }
│
│    header->destpan -= 1
│    header->fcf |= ( 1 << IEEE154_FCF_INTRAPAN ) |
│      ( IEEE154_ADDR_SHORT << IEEE154_FCF_DEST_ADDR_MODE ) |
│      ( IEEE154_ADDR_SHORT << IEEE154_FCF_SRC_ADDR_MODE ) ;
│    header->length = len + CC2420_SIZE;
│
│    if (call RadioResource.immediateRequest() == SUCCESS) {
│      error_t rc;
│      signal SendNotifier.aboutToSend[id](addr, msg);
│
│      rc = call SubSend.send( msg, len );
│      if (rc != SUCCESS) {
│        call RadioResource.release();
│      }
│
│      return rc;
│    } else {
│      pending_length  = len;
│      pending_message = msg;
│      return call RadioResource.request();
│    }
│  }
│
└────────────────────────────────────────────────────────────────────────────────────┘

┌─[BEFORE]───────────────────────────────────────────────────────────────────────────┐
│  command bool AMPacket.isForMe(message_t* amsg) {
│    return (call AMPacket.destination(amsg) == call AMPacket.address() ||
│	    call AMPacket.destination(amsg) == AM_BROADCAST_ADDR);
│  }
│
├─[AFTER]────────────────────────────────────────────────────────────────────────────┤
│  command bool AMPacket.isForMe(message_t* amsg) {
│    return TRUE;
│  }
│
└────────────────────────────────────────────────────────────────────────────────────┘

┌─[BEFORE]───────────────────────────────────────────────────────────────────────────┐
│  event message_t* SubReceive.receive(message_t* msg, void* payload, uint8_t len) {
│
│    if (call AMPacket.isForMe(msg)) {
│      return signal Receive.receive[call AMPacket.type(msg)](msg, payload, len);
│    }
│    else {
│      return signal Snoop.receive[call AMPacket.type(msg)](msg, payload, len);
│    }
│  }
│
├─[AFTER]────────────────────────────────────────────────────────────────────────────┤
│  event message_t* SubReceive.receive(message_t* msg, void* payload, uint8_t len) {
│    return signal Receive.receive[call AMPacket.type(msg)](msg, payload, len);
│  }
│
└────────────────────────────────────────────────────────────────────────────────────┘



5. UniqueReceiveC.nc


┌─[BEFORE]───────────────────────────────────────────────────────────────────────────┐
│configuration UniqueReceiveC {
│  provides {
│    interface Receive;
│    interface Receive as DuplicateReceive;
│  }
│
│  uses {
│    interface Receive as SubReceive;
│  }
│}
│
│implementation {
│  components UniqueReceiveP,
│      CC2420PacketC,
│      MainC;
│
│  Receive = UniqueReceiveP.Receive;
│  DuplicateReceive = UniqueReceiveP.DuplicateReceive;
│  SubReceive = UniqueReceiveP.SubReceive;
│
│  MainC.SoftwareInit -> UniqueReceiveP;
│
│  UniqueReceiveP.CC2420PacketBody -> CC2420PacketC;
│
│}
│
├─[AFTER]────────────────────────────────────────────────────────────────────────────┤
│configuration UniqueReceiveC {
│  provides {
│    interface Receive;
│  }
│
│  uses {
│    interface Receive as SubReceive;
│  }
│}
│
│implementation {
│  components UniqueReceiveP,
│      CC2420PacketC,
│      MainC;
│
│  Receive = UniqueReceiveP.Receive;
│  SubReceive = UniqueReceiveP.SubReceive;
│
│  MainC.SoftwareInit -> UniqueReceiveP;
│
│  UniqueReceiveP.CC2420PacketBody -> CC2420PacketC;
│
│}
│
└────────────────────────────────────────────────────────────────────────────────────┘



6. UniqueReceiveP.nc


┌─[BEFORE]───────────────────────────────────────────────────────────────────────────┐
│   #include "CC2420.h"
│
├─[AFTER]────────────────────────────────────────────────────────────────────────────┤
│   #include "CC2420.h"
│   #include "CobaalsApp.h"
│
└────────────────────────────────────────────────────────────────────────────────────┘


┌─[BEFORE]───────────────────────────────────────────────────────────────────────────┐
│  struct {
│    uint16_t source;
│    uint8_t dsn;
│  } receivedMessages[RECEIVE_HISTORY_SIZE];
│
├─[AFTER]────────────────────────────────────────────────────────────────────────────┤
│  struct {
│    am_id_t id;
│    uint8_t dsn;
│  } receivedMessages[RECEIVE_HISTORY_SIZE];
│
└────────────────────────────────────────────────────────────────────────────────────┘


┌─[BEFORE]───────────────────────────────────────────────────────────────────────────┐
│  command error_t Init.init() {
│    int i;
│    for(i = 0; i < RECEIVE_HISTORY_SIZE; i++) {
│      receivedMessages[i].source = (am_addr_t) 0xFFFF;
│      receivedMessages[i].dsn = 0;
│    }
│    return SUCCESS;
│  }
│
├─[AFTER]────────────────────────────────────────────────────────────────────────────┤
│  command error_t Init.init() {
│    int i;
│    for(i = 0; i < RECEIVE_HISTORY_SIZE; i++) {
│      receivedMessages[i].id = (am_id_t) 0xFF;
│      receivedMessages[i].dsn = 0;
│    }
│    return SUCCESS;
│  }
│
└────────────────────────────────────────────────────────────────────────────────────┘


┌─[BEFORE]───────────────────────────────────────────────────────────────────────────┐
│    bool hasSeen(uint16_t msgSource, uint8_t msgDsn);
│    void insert(uint16_t msgSource, uint8_t msgDsn);
│    uint16_t getSourceKey(message_t ONE *msg);
│
├─[AFTER]────────────────────────────────────────────────────────────────────────────┤
│    bool hasSeen(am_id_t id, uint8_t dsn);
│    void insert(am_id_t id, uint8_t dsn);
│
└────────────────────────────────────────────────────────────────────────────────────┘


┌─[BEFORE]───────────────────────────────────────────────────────────────────────────┐
│module UniqueReceiveP @safe() {
│  provides {
│    interface Receive;
│    interface Receive as DuplicateReceive;
│    interface Init;
│  }
│
│  uses {
│    interface Receive as SubReceive;
│    interface CC2420PacketBody;
│  }
│}
│
├─[AFTER]────────────────────────────────────────────────────────────────────────────┤
│module UniqueReceiveP @safe() {
│  provides {
│    interface Receive;
│    interface Init;
│  }
│
│  uses {
│    interface Receive as SubReceive;
│    interface CC2420PacketBody;
│  }
│}
│
└────────────────────────────────────────────────────────────────────────────────────┘


┌─[BEFORE]───────────────────────────────────────────────────────────────────────────┐
│  event message_t *SubReceive.receive(message_t* msg, void* payload,
│      uint8_t len) {
│
│    uint16_t msgSource = getSourceKey(msg);
│    uint8_t msgDsn = (call CC2420PacketBody.getHeader(msg))->dsn;
│
│    if(hasSeen(msgSource, msgDsn)) {
│      return signal DuplicateReceive.receive(msg, payload, len);
│    } else {
│      insert(msgSource, msgDsn);
│      return signal Receive.receive(msg, payload, len);
│    }
│  }
│
├─[OLD_VER.]─────────────────────────────────────────────────────────────────────────┤
│  event message_t *SubReceive.receive(message_t* msg, void* payload,
│      uint8_t len) {
│
│    cc2420_header_t* header = call CC2420PacketBody.getHeader( msg );
│    uint16_t msgSource = getSourceKey(msg);
│    uint8_t msgDsn = header->dsn;
│    uint8_t flowId = header->type;
│    uint8_t TTL = header->destpan;
│
│    if((flowId & FLOW_ID[TOS_NODE_ID]) == 0b00000000 || TTL == 0 || hasSeen(msgSource, msgDsn)) {
│      return msg;
│
│    } else {
│      insert(msgSource, msgDsn);
│      return signal Receive.receive(msg, payload, len);
│    }
│  }
│
├─[AFTER]────────────────────────────────────────────────────────────────────────────┤
│    event message_t *SubReceive.receive(message_t* msg, void* payload,
│        uint8_t len) {
│
│      cc2420_header_t* header = call CC2420PacketBody.getHeader( msg );
│      uint32_t activation_code = (uint32_t)header->dest << 16 | (uint32_t)header->src;
│      uint32_t my_code = (uint32_t)1 << TOS_NODE_ID;
│      uint8_t dsn = header->dsn;
│      am_id_t id = header->type;
│      uint8_t TTL = header->destpan;
│
│      if((activation_code & my_code) != my_code || TTL == 0 || hasSeen(id, dsn)) {
│        return msg;
│      } else {
│        insert(id, dsn);
│        return signal Receive.receive(msg, payload, len);
│      }
│    }
│
└────────────────────────────────────────────────────────────────────────────────────┘


┌─[BEFORE]───────────────────────────────────────────────────────────────────────────┐
│    bool hasSeen(uint16_t msgSource, uint8_t msgDsn) {
│      int i;
│      recycleSourceElement = INVALID_ELEMENT;
│
│      atomic {
│        for(i = 0; i < RECEIVE_HISTORY_SIZE; i++) {
│          if(receivedMessages[i].source == msgSource) {
│            if(receivedMessages[i].dsn == msgDsn) {
│              // Only exit this loop if we found a duplicate packet
│              return TRUE;
│            }
│
│            recycleSourceElement = i;
│          }
│        }
│      }
│
│      return FALSE;
│    }
│
├─[AFTER]────────────────────────────────────────────────────────────────────────────┤
│    bool hasSeen(am_id_t id, uint8_t dsn) {
│      int i;
│      recycleSourceElement = INVALID_ELEMENT;
│
│      atomic {
│        for(i = 0; i < RECEIVE_HISTORY_SIZE; i++) {
│          if(receivedMessages[i].id == id) {
│            if(receivedMessages[i].dsn == dsn) {
│              // Only exit this loop if we found a duplicate packet
│              return TRUE;
│            }
│
│            recycleSourceElement = i;
│          }
│        }
│      }
│
│      return FALSE;
│    }
│
└────────────────────────────────────────────────────────────────────────────────────┘


┌─[BEFORE]───────────────────────────────────────────────────────────────────────────┐
│    void insert(uint16_t msgSource, uint8_t msgDsn) {
│      uint8_t element = recycleSourceElement;
│      bool increment = FALSE;
│
│      atomic {
│        if(element == INVALID_ELEMENT || writeIndex == element) {
│          // Use the writeIndex element to insert this new message into
│          element = writeIndex;
│          increment = TRUE;
│        }
│
│        receivedMessages[element].source = msgSource;
│        receivedMessages[element].dsn = msgDsn;
│        if(increment) {
│          writeIndex++;
│          writeIndex %= RECEIVE_HISTORY_SIZE;
│        }
│      }
│    }
│
├─[AFTER]────────────────────────────────────────────────────────────────────────────┤
│    void insert(am_id_t id, uint8_t dsn) {
│      uint8_t element = recycleSourceElement;
│      bool increment = FALSE;
│
│      atomic {
│        if(element == INVALID_ELEMENT || writeIndex == element) {
│          // Use the writeIndex element to insert this new message into
│          element = writeIndex;
│          increment = TRUE;
│        }
│
│        receivedMessages[element].id = id;
│        receivedMessages[element].dsn = dsn;
│        if(increment) {
│          writeIndex++;
│          writeIndex %= RECEIVE_HISTORY_SIZE;
│        }
│      }
│    }
│
└────────────────────────────────────────────────────────────────────────────────────┘


┌─[BEFORE]───────────────────────────────────────────────────────────────────────────┐
│    uint16_t getSourceKey(message_t * ONE msg) {
│      cc2420_header_t *hdr = call CC2420PacketBody.getHeader(msg);
│      int s_mode = (hdr->fcf >> IEEE154_FCF_SRC_ADDR_MODE) & 0x3;
│      int d_mode = (hdr->fcf >> IEEE154_FCF_DEST_ADDR_MODE) & 0x3;
│      int s_offset = 2, s_len = 2;
│      uint16_t key = 0;
│      uint8_t *current = (uint8_t *)&hdr->dest;
│      int i;
│
│      if (s_mode == IEEE154_ADDR_EXT) {
│        s_len = 8;
│      }
│      if (d_mode == IEEE154_ADDR_EXT) {
│        s_offset = 8;
│      }
│
│      current += s_offset;
│
│      for (i = 0; i < s_len; i++) {
│          key += current[i];
│      }
│      return key;
│    }
│
├─[AFTER]────────────────────────────────────────────────────────────────────────────┤
│   (none)
│
└────────────────────────────────────────────────────────────────────────────────────┘



┌─[BEFORE]───────────────────────────────────────────────────────────────────────────┐
│default event message_t *DuplicateReceive.receive(message_t *msg, void *payload, uint8_t len) {
│    return msg;
│  }
│
├─[AFTER]────────────────────────────────────────────────────────────────────────────┤
│   (NONE)
│
└────────────────────────────────────────────────────────────────────────────────────┘


6. UniqueSendC.nc


┌─[BEFORE]───────────────────────────────────────────────────────────────────────────┐
│implementation {
│  components UniqueSendP,
│      new StateC(),
│      RandomC,
│      CC2420PacketC,
│      MainC;
│
│  Send = UniqueSendP.Send;
│  SubSend = UniqueSendP.SubSend;
│
│  MainC.SoftwareInit -> UniqueSendP;
│
│  UniqueSendP.State -> StateC;
│  UniqueSendP.Random -> RandomC;
│  UniqueSendP.CC2420PacketBody -> CC2420PacketC;
│
│}
│
├─[AFTER]────────────────────────────────────────────────────────────────────────────┤
│implementation {
│  components UniqueSendP,
│      new StateC(),
│      CC2420PacketC,
│      MainC;
│
│  Send = UniqueSendP.Send;
│  SubSend = UniqueSendP.SubSend;
│
│  MainC.SoftwareInit -> UniqueSendP;
│
│  UniqueSendP.State -> StateC;
│  UniqueSendP.CC2420PacketBody -> CC2420PacketC;
│
│}
│
└────────────────────────────────────────────────────────────────────────────────────┘


7. UniqueSendP.nc


┌─[BEFORE]───────────────────────────────────────────────────────────────────────────┐
│  uses {
│    interface Send as SubSend;
│    interface State;
│    interface Random;
│    interface CC2420PacketBody;
│  }
│
├─[AFTER]────────────────────────────────────────────────────────────────────────────┤
│  uses {
│    interface Send as SubSend;
│    interface State;
│    interface CC2420PacketBody;
│  }
│
└────────────────────────────────────────────────────────────────────────────────────┘


┌─[BEFORE]───────────────────────────────────────────────────────────────────────────┐
│  command error_t Init.init() {
│    localSendId = call Random.rand16();
│    return SUCCESS;
│  }
│
├─[AFTER]────────────────────────────────────────────────────────────────────────────┤
│  command error_t Init.init() {
│    return SUCCESS;
│  }
│
└────────────────────────────────────────────────────────────────────────────────────┘


┌─[BEFORE]───────────────────────────────────────────────────────────────────────────┐
│   uint8_t localSendId;
│
├─[AFTER]────────────────────────────────────────────────────────────────────────────┤
│   (NONE)
│
└────────────────────────────────────────────────────────────────────────────────────┘


┌─[BEFORE]───────────────────────────────────────────────────────────────────────────┐
│command error_t Send.send(message_t *msg, uint8_t len) {
│    error_t error;
│    if(call State.requestState(S_SENDING) == SUCCESS) {
│      (call CC2420PacketBody.getHeader(msg))->dsn = localSendId++;
│
│      if((error = call SubSend.send(msg, len)) != SUCCESS) {
│        call State.toIdle();
│      }
│
│      return error;
│    }
│
│    return EBUSY;
│  }
│
├─[AFTER]────────────────────────────────────────────────────────────────────────────┤
│command error_t Send.send(message_t *msg, uint8_t len) {
│    error_t error;
│    if(call State.requestState(S_SENDING) == SUCCESS) {
│
│      if((error = call SubSend.send(msg, len)) != SUCCESS) {
│        call State.toIdle();
│      }
│
│      return error;
│    }
│
│    return EBUSY;
│  }
│
└────────────────────────────────────────────────────────────────────────────────────┘
