#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("FirstScript");

int
main (int argc, char *argv[])
{
  CommandLine cmd;
  cmd.Parse (argc, argv);
  
  Time::SetResolution (Time::NS);
  LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
  LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);

  NodeContainer nodes1;
  nodes1.Create (2);

  NodeContainer nodes2;
  nodes2.Add(nodes1.Get(1));
  nodes2.Create(1);

  PointToPointHelper pointToPoint1;
  pointToPoint1.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  pointToPoint1.SetChannelAttribute ("Delay", StringValue ("2ms"));

  PointToPointHelper pointToPoint2;
  pointToPoint2.SetDeviceAttribute ("DataRate", StringValue ("7Mbps"));
  pointToPoint2.SetChannelAttribute ("Delay", StringValue ("1ms"));

  NetDeviceContainer devices1,devices2;
  devices1 = pointToPoint1.Install (nodes1);
  devices2 = pointToPoint2.Install (nodes2);

  InternetStackHelper stack;
  stack.Install (nodes1);
  stack.Install (nodes2.Get(1));

  Ipv4AddressHelper address1,address2;
  address1.SetBase ("10.1.1.0", "255.255.255.0");
  address2.SetBase ("10.1.2.0", "255.255.255.0");

  Ipv4InterfaceContainer interfaces1 = address1.Assign (devices1);
  Ipv4InterfaceContainer interfaces2 = address2.Assign (devices2);


  UdpEchoServerHelper echoServer1 (9);
  UdpEchoServerHelper echoServer2 (10);


  ApplicationContainer serverApps1 = echoServer1.Install (nodes1.Get (1));
  ApplicationContainer serverApps2 = echoServer2.Install (nodes2.Get (0));

  serverApps1.Start (Seconds (1.0));
  serverApps1.Stop (Seconds (10.0));
  serverApps2.Start (Seconds (1.0));
  serverApps2.Stop (Seconds (10.0));

  UdpEchoClientHelper echoClient1 (interfaces1.GetAddress (1), 9);
  echoClient1.SetAttribute ("MaxPackets", UintegerValue (1));
  echoClient1.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  echoClient1.SetAttribute ("PacketSize", UintegerValue (1024));

  UdpEchoClientHelper echoClient2 (interfaces2.GetAddress (0), 10);
  echoClient2.SetAttribute ("MaxPackets", UintegerValue (1));
  echoClient2.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  echoClient2.SetAttribute ("PacketSize", UintegerValue (1024));

  ApplicationContainer clientApps1 = echoClient1.Install (nodes1.Get (0));
  clientApps1.Start (Seconds (2.0));
  clientApps1.Stop (Seconds (10.0));

  ApplicationContainer clientApps2 = echoClient2.Install (nodes2.Get (1));
  clientApps2.Start (Seconds (2.0));
  clientApps2.Stop (Seconds (10.0));

  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}