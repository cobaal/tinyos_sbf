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

2. HdlcTranslateC.nc

┌─[BEFORE]───────────────────────────────────────────────────────────────────────────┐
│  async command error_t SerialFrameComm.putData(uint8_t data) {
│    if (data == HDLC_CTLESC_BYTE || data == HDLC_FLAG_BYTE) {
│      state.sendEscape = 1;
│      txTemp = data ^ 0x20;
│      m_data = HDLC_CTLESC_BYTE;
│    }
│    else {
│      m_data = data;
│    }
│    return call UartStream.send(&m_data, 1);
│  }
│
├─[AFTER]────────────────────────────────────────────────────────────────────────────┤
│  async command error_t SerialFrameComm.putData(uint8_t data) {
│    m_data = data;
│    return call UartStream.send(&m_data, 1);
│  }
│
└────────────────────────────────────────────────────────────────────────────────────┘

3. CC2420ActiveMessageP.nc

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
│    header->type = id;
│    header->dest = addr;
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

4. TerminalNodesP.nc

┌─[BEFORE]───────────────────────────────────────────────────────────────────────────┐
│  event message_t *RadioSnoop.receive[am_id_t id](message_t *msg,
│						    void *payload,
│						    uint8_t len) {
│    return receive(msg, payload, len);
│  }
│
│
├─[AFTER]────────────────────────────────────────────────────────────────────────────┤
│  (NONE)
│
└────────────────────────────────────────────────────────────────────────────────────┘

┌─[BEFORE]───────────────────────────────────────────────────────────────────────────┐
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
├─[AFTER]────────────────────────────────────────────────────────────────────────────┤
│  task void uartSendTask() {
│    uint8_t len;
│    am_id_t id;
│    am_addr_t addr, src;
│    message_t* msg;
│    am_group_t grp;
│
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
│
│    call UartPacket.clear(msg);
│    call UartAMPacket.setSource(msg, src);
│    call UartAMPacket.setGroup(msg, grp);
│
│    if (call UartSend.send[id](addr, msg, len) == SUCCESS)
│      call Leds.led1Toggle();
│    else
│      {
│	failBlink();
│	post uartSendTask();
│      }
│  }
│
└────────────────────────────────────────────────────────────────────────────────────┘

┌─[BEFORE]───────────────────────────────────────────────────────────────────────────┐
│  task void radioSendTask() {
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
│    am_addr_t addr, src;
│    message_t* msg;
│    uint16_t grp;
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
│    id = call UartAMPacket.type(msg);
│    src = call UartAMPacket.source(msg);
│    grp = call UartAMPacket.group(msg);
│
│    call RadioPacket.clear(msg);
│    call RadioAMPacket.setSource(msg, src);
│    call RadioAMPacket.setGroup(msg, grp);
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
└────────────────────────────────────────────────────────────────────────────────────┘



5. UniqueReceiveP.nc

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
│    uint8_t msgDest = header->dest;
│
│    if(msgDest != TOS_NODE_ID || hasSeen(msgSource, msgDsn)) {
│      return signal DuplicateReceive.receive(msg, payload, len);
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
│
│      if((activation_code & my_code) != my_code || hasSeen(id, dsn)) {
│        return signal DuplicateReceive.receive(msg, payload, len);
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
│
│
├─[AFTER]────────────────────────────────────────────────────────────────────────────┤
│
│
└────────────────────────────────────────────────────────────────────────────────────┘
