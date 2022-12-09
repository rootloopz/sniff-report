import json
import wget
import subprocess, threading
from flask import Flask, render_template

app = Flask(__name__)

class known_device:
    def __init__(self, mac, timestamp, ss, ssid):
        self.mac = mac
        self.timestamp = timestamp
        self.ss = ss
        self.ssid = ssid

class known_ssid:
    def __init__(self, mac, timestamp, ssid):
        self.mac = mac
        self.timestamp = timestamp
        self.ssid = ssid

class Command(object):
    def __init__(self, cmd):
        self.cmd = cmd
        self.process = None

    def run(self, timeout):
        def target():
            print ('Thread started')
            self.process = subprocess.Popen(self.cmd, shell=True)
            self.process.communicate()
            print ('Thread finished')

        thread = threading.Thread(target=target)
        thread.start()

        thread.join(timeout)
        if thread.is_alive():
            print ('Terminating process')
            self.process.terminate()
            thread.join()
        print(self.process.returncode)


def run_cpp():
    command = Command("sudo ./probe")
    command.run(timeout=30)

# Function to grab data from jsonl
def grab_data():
    with open('out.jsonl') as json_file:
        data = [json.loads(line) for line in json_file]
    print(data)
    ssid_list = []
    known_list = []
    for items in data:
        mac = items['src_addr']
        items['src_addr'] = items['src_addr'].upper()
        if compare_oui("oui.txt", mac) is not None:
            items['src_addr'] = mac.upper() + ", " + compare_oui("oui.txt", mac)
            # if items['src_addr'] is not known_list:
            d1 = known_device(items['src_addr'], items['timestamp'], items['dbm_signal'], items['ssid'])
            known_list.append(d1)
        if str(items['ssid']) != "NULL":
            s1 = known_ssid(items['src_addr'], items['timestamp'], items['ssid'])
            ssid_list.append(s1)
    return data, known_list, ssid_list

def check_oui():
    print("Checking for oui.txt file..")
    try:
        open('oui.txt')
        print("file already downloaded..")
    except FileNotFoundError:
        url = 'http://standards-oui.ieee.org/oui/oui.txt'
        print("oui.txt not found, downloading from " + url)
        response = wget.download(url, "oui.txt")
        print("\n")
    return 'oui.txt'

def compare_oui(file, mac):
    oui = {}
    cellphone = [
        'Motorola Mobility LLC, a Lenovo Company',
        'GUANGDONG OPPO MOBILE TELECOMMUNICATIONS CORP.,LTD',
        'Huawei Symantec Technologies Co.,Ltd.',
        'Microsoft',
        'HTC Corporation',
        'Samsung Electronics Co.,Ltd',
        'SAMSUNG ELECTRO-MECHANICS(THAILAND)',
        'BlackBerry RTS',
        'LG ELECTRONICS INC',
        'Apple, Inc.',
        'LG Electronics',
        'OnePlus Tech (Shenzhen) Ltd',
        'Xiaomi Communications Co Ltd',
        'LG Electronics (Mobile Communications)']

    with open(file, 'r') as f:
        for line in f:
            if '(hex)' in line:
                data = line.split('(hex)')
                key = data[0].replace('-', ':').lower().strip()
                company = data[1].strip()
                oui[key] = company
    oui_id = None
    if mac[:8] in oui:
        oui_id = oui[mac[:8]]
    return(oui_id)

@app.route("/")
def index():
    return render_template('index.html')

@app.route("/run")
def run():
    run_cpp()
    return render_template('index.html')

@app.route("/report/")
def report():
    # run_cpp()
    dev_data, known_list, ssid_list = grab_data()
    return render_template('report.html', device_data=dev_data, known_list=known_list, len = len(known_list), ssid_list=ssid_list, len_2 = len(ssid_list))

