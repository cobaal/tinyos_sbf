configuration IntermediateNodesC {
}
implementation {
  components MainC, IntermediateNodesP, LedsC;
  components ActiveMessageC as Radio;

  MainC.Boot <- IntermediateNodesP;

  IntermediateNodesP.RadioControl -> Radio;

  IntermediateNodesP.RadioSend -> Radio;
  IntermediateNodesP.RadioReceive -> Radio.Receive;
  IntermediateNodesP.RadioPacket -> Radio;
  IntermediateNodesP.RadioAMPacket -> Radio;

  IntermediateNodesP.Leds -> LedsC;
}
