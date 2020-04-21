configuration TerminalNodesC {
}
implementation {
  components MainC, TerminalNodesP, LedsC;
  components ActiveMessageC as Radio, SerialActiveMessageC as Serial;
  
  MainC.Boot <- TerminalNodesP;

  TerminalNodesP.RadioControl -> Radio;
  TerminalNodesP.SerialControl -> Serial;
  
  TerminalNodesP.UartSend -> Serial;
  TerminalNodesP.UartReceive -> Serial.Receive;
  TerminalNodesP.UartPacket -> Serial;
  TerminalNodesP.UartAMPacket -> Serial;
  
  TerminalNodesP.RadioSend -> Radio;
  TerminalNodesP.RadioReceive -> Radio.Receive;
  TerminalNodesP.RadioPacket -> Radio;
  TerminalNodesP.RadioAMPacket -> Radio;
  
  TerminalNodesP.Leds -> LedsC;
}
