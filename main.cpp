#include "tins/tins.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <tins/packet.h>
#include <tins/sniffer.h>

using namespace std;
using namespace Tins;

// needed to get MAC addrs
template <class T> HWAddress<6> get_src_addr(const T &data) {
  if (!data.from_ds() && !data.to_ds())
    return data.addr2();
  if (!data.from_ds() && data.to_ds())
    return data.addr2();
  return data.addr3();
}

string parse_probe(const Packet &packet) {
    stringstream ss;
    try {
        // first check that it's a probe request frame
        if (const auto *probe = packet.pdu()->find_pdu<Dot11ProbeRequest>()) {
            string ssid = probe->ssid();
            if (ssid.empty()) {
                ssid = "NULL";
            }
            ss << "timestamp:" << packet.timestamp().seconds()
               << " src_addr:" << probe->addr2()
               << " ssid:" << ssid;
            if (const auto *rt = packet.pdu()->find_pdu<RadioTap>()) {
                ss << " dbm_signal:" << (int)rt->dbm_signal()
                   << " rate:" << (int)rt->rate()
                   << " channel_freq:" << rt->channel_freq();
            }
            ss << endl;
        }
    } catch (...) {
    }
    return ss.str();
}


int main(int argc, char *argv[]) {
  int limit = 1000;
  string interface = "wlx00c0cab05cec";
  if (argc > 2) {
    interface = argv[1];
    limit = atoi(argv[2]);
  } else {
    cerr << "Format error: "
         << "\n";
    cerr << "Usage: sudo ./sniff-report <interface> <packet limit>";
    return 1;
  }

  int count = 0;

  SnifferConfiguration config;
  config.set_promisc_mode(true);
  config.set_rfmon(true);
  config.set_filter("type data or type mgt subtype probe-req or type mgt subtype beacon");
  Sniffer sniffer(interface, config);

  bool running = true;
  while (running) {

    if (count >= limit) {
      running = false;
      break;
    }
    try {
      Packet packet = sniffer.next_packet();

      // checking if packet has data we care about from parse_probe function
      if (parse_probe(packet) != "") {
        cout << count << ": " << parse_probe(packet) << endl;
        count++;
      }

    } catch (...) {
    }
  }

  return 0;
}
