#include "tins/tins.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <fstream>

using namespace std;
using namespace Tins;

//needed to get MAC addrs
template <class T> HWAddress<6> get_src_addr(const T &data) {
  if (!data.from_ds() && !data.to_ds())
    return data.addr2();
  if (!data.from_ds() && data.to_ds())
    return data.addr2();
  return data.addr3();
}

string partial_parse_radiotap(const Packet &packet) {
  stringstream ss;

  try {
    const RadioTap &rt = packet.pdu()->rfind_pdu<RadioTap>();

    try {
      unsigned int timestamp = packet.timestamp().seconds();
      ss << "\"timestamp\":" << timestamp;
    } catch (...) {
    }

    try {
      int dbm_signal = rt.dbm_signal();
      ss << ",\"dbm_signal\":" << dbm_signal;
    } catch (...) {
    }

    try {
      int rate = rt.rate();
      ss << ",\"rate\":" << rate;
    } catch (...) {
    }

    try {
      int channel_freq = rt.channel_freq();
      ss << ",\"channel_freq\":" << channel_freq;
    } catch (...) {
    }

  } catch (...) {
  }

  return ss.str();
}

string parse_probe(const Packet &packet) {
  stringstream ss;
  try {
    // first check that it's a probe request frame
    packet.pdu()->rfind_pdu<Tins::Dot11ProbeRequest>();
    const Dot11ManagementFrame &data = packet.pdu()->rfind_pdu<Dot11ManagementFrame>();
    string ssid = data.ssid();
    if(!ssid.empty()){
      ssid = data.ssid();
    } else {
      ssid = "NULL";
    }
    HWAddress<6> src_addr = get_src_addr(data);
    ss << "{"
      << partial_parse_radiotap(packet) << ","
      << "\"src_addr\":\"" << src_addr << "\","
      << "\"ssid\":\"" << ssid << "\""
      << "}";
  } catch (...) {
  }
  return ss.str();
}


bool print0 = false;
string println(string str) {
  string result = "";
    if(str.size() > 0) {
        result += str;
        if(print0) {
            result += '\0';
        } else {
            result += '\n';
        }
        cout.flush();
    }
    return result;
}

int main(int argc, char *argv[])
{
  string interface = "wlx00c0cab05cec";
  if (argc > 1) {
    interface = argv[1];
  }

  SnifferConfiguration config;
  config.set_promisc_mode(true);
  config.set_rfmon(true);
  config.set_filter("type data or type mgt subtype probe-req or type mgt subtype beacon");
  Sniffer sniffer(interface, config);
  ofstream myfile;
  myfile.open("out.jsonl");

  while (true) {
    try {
      Packet packet = sniffer.next_packet();
      if (packet) {
        myfile << println(parse_probe(packet));
      }
    } catch (...) {
    }
  }
  myfile.close();

  return 0;
}