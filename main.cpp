#include "tins/tins.h"
#include <fstream>
#include <iostream>
#include <iomanip>
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
  string interface = "";
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

  // Table setup for output
cout << left << setw(10) << "Count"
     << left << setw(20) << "Timestamp"
     << left << setw(30) << "Src Address"
     << left << setw(30) << "SSID"
     << left << setw(20) << "Signal (dBm)"
     << left << setw(20) << "Rate (Mbps)"
     << left << setw(20) << "Channel Freq (MHz)" << endl;

cout << setfill('-') << setw(150) << "-" << setfill(' ') << endl;

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
      string parse_result = parse_probe(packet);
      stringstream ss(parse_result);
      string timestamp, src_addr, ssid, dbm_signal, rate, channel_freq;
      getline(ss, timestamp, ' ');
      getline(ss, src_addr, ' ');
      getline(ss, ssid, ' ');
      getline(ss, dbm_signal, ' ');
      getline(ss, rate, ' ');
      getline(ss, channel_freq, ' ');

      // Extract the value after the colon
      timestamp = timestamp.substr(timestamp.find(":") + 1);
      src_addr = src_addr.substr(src_addr.find(":") + 1);
      ssid = ssid.substr(ssid.find(":") + 1);
      dbm_signal = dbm_signal.substr(dbm_signal.find(":") + 1);
      rate = rate.substr(rate.find(":") + 1);
      channel_freq = channel_freq.substr(channel_freq.find(":") + 1);

      // Print values in a tabular format
      cout << left << setw(10) << count
           << left << setw(20) << timestamp
           << left << setw(30) << src_addr
           << left << setw(30) << ssid
           << left << setw(20) << dbm_signal
           << left << setw(20) << rate
           << left << setw(20) << channel_freq << endl;

      count++;
    }
  } catch (...) {
  }
}
  return 0;
}
