from collections import Counter
import telnetlib
import logging
import sys
import datetime
import time

bodies = ["Mercury", "Venus", "Earth", "Mars", "Jupiter", "Saturn", "Neptune", "Uranus"];
not_default = ["j", "+0", "cal", "min", "deg", "no", "refracted", "km", "yes", " ", " ", "n", "y", " ", " ", "yes"];
targets = [];
longitude = -1;
latitude = -1;
kasl = -1;
start_datetime = '-1';
end_datetime = '-1';
HOST = "horizons.jpl.nasa.gov";
PORT = 6775;
first_time = True;

def init_telnet():
    logging.info("  Connecting to \"%s:%d\"", HOST, PORT);
    t = telnetlib.Telnet(host=HOST, port=PORT, timeout=10)
    logging.info("  ################################################\n");
    return t;

def init_logger():
    logging.basicConfig(stream=sys.stderr, level=logging.DEBUG);

def fill_fields(i, t):
    logging.info("      Filling fields");
    t.read_until(b"Horizons>", timeout=5).decode('ascii');
    t.write((str(i) + "\r\n").encode('ascii'));
    t.read_until(b": ", timeout=2).decode('ascii');
    t.write(("e\r\n").encode('ascii'));                 #ephemeris
    t.read_until(b": ", timeout=2).decode('ascii');
    t.write(("o\r\n").encode('ascii'));                 #observe
    t.read_until(b": ", timeout=2).decode('ascii');

    if (first_time):                                    #set coordinates for the first time
        t.write(("coord\r\n").encode('ascii'));         #coordinate
        t.read_until(b": ", timeout=2).decode('ascii');
        t.write(("g\r\n").encode('ascii'));             #geodetic
        t.read_until(b": ", timeout=2).decode('ascii');
        ### Input units must be DEGREES and KILOMETRS ABOVE SEE LEVEL (kasl) ###
        t.write((str(longitude) + ", " + str(latitude) + ", " + str(kasl) + "\r\n").encode('ascii'));
        t.read_until(b": ", timeout=2).decode('ascii');
    else:
        t.write(("y\r\n").encode('ascii'));             #confirm to use previous coordinates
        t.read_until(b": ", timeout=2).decode('ascii');

    t.write((start_datetime + "\r\n").encode('ascii'));
    t.read_until(b": ", timeout=2).decode('ascii');
    t.write((end_datetime + "\r\n").encode('ascii'));
    t.read_until(b": ", timeout=2).decode('ascii');
    t.write(("1m\r\n").encode('ascii'));
    t.read_until(b": ", timeout=2).decode('ascii');
    t.write(("no\r\n").encode('ascii'));
    t.read_until(b": ", timeout=2).decode('ascii');
    t.write(("1, 2\r\n").encode('ascii'));
    t.read_until(b": ", timeout=2).decode('ascii');

def change_default_options(t):
    logging.info("      Changing default options");
    for option in not_default:
        t.write((option + "\r\n").encode('ascii'));
        t.read_until(b": ", timeout=2).decode('ascii');
    t.read_until(b"$$SOE", timeout=2).decode('ascii');

def write_to_file(f, t):
    logging.info("      Parsing and writing \"%s\"", f.name);
    buff = t.read_until(b"$$EOE", timeout=3).decode('ascii');
    f.write(buff[1:len(buff) - 5]);
    t.read_until(b", ? :", timeout=2).decode('ascii');
    t.write(("n\r\n").encode('ascii'));
    global first_time;
    first_time = False;

def gather_ephemeris(t):
    for body in targets:
        i = bodies.index(body);
        f = open(bodies[i] + ".txt", 'w');
        logging.info("  Gathering ephemeris for %s", bodies[i]);
        fill_fields((i+2)*100 - 1, t);
        change_default_options(t);
        write_to_file(f, t);
        logging.info("  Closing \"%s\"\n", f.name);
        f.close();

def validate(string):
    a = 1
    try:
        datetime.datetime.strptime(string, '%Y-%m-%d %H:%M');
    except ValueError:
        print("Please, use the following format\tYYYY-MM-DD hh:mm\n");
        a = 0;
    return a;

def set_datetime():
    global start_datetime;
    global end_datetime;
    flag = 0;
    while not (flag):
        start_datetime = input("Start of the observation (YYYY-MM-DD HH:mm): ");
        flag = validate(start_datetime);
    flag = 0;
    while not (flag):
        end_datetime = input("End the observation (YYYY-MM-DD HH:mm): ");
        flag = validate(end_datetime);
    while (start_datetime > end_datetime):
        print("\tWe can't travel back in time (yet ;))\n")
        flag = 0;
        while not (flag):
            end_datetime = input("End the observation (YYYY-MM-DD HH:mm): ");
            flag = validate(end_datetime);

def set_location():
    global longitude;
    global latitude;
    global kasl;
    while not (0 <= longitude <= 90):
        longitude = int(input("Insert the east longitude of your point of observation (deg): "));
        if not (0 <= longitude <= 90):
            print("Not in range [0, 90]. Try again!\n");
    while not (0 <= latitude <= 180):
        latitude = int(input("Insert the north latitude of your point of observation (deg): "));
        if not (0 <= latitude <= 180):
            print("Not in range [0, 180]. Try again!\n");
    while not (kasl > -1):
        kasl = float(input("Insert the altitude of your point of observation (km above sea level): "));
        if not (kasl > -1):
            print("Not in range [> 0]. Try again!\n");

def select_targets():
    print("Rastroberry\n\tVersion 0.0.0.0 alpha\n\n");
    global targets;
    while (len(targets) != 1):
        seq = input("Select the celestial bodies (Currently available: Mercury, Venus, Mars, Jupiter, Saturn, Neptune, Uranus): ");
        logging.info("  SEQ: %s", seq);
        seq = seq.replace(" ", "");
        seq = seq.split(',');
        targets = set(seq);
        logging.info("  Targets: %s", targets);
        if 'Earth' in targets:
            print("\tEarth observation is not available yet. Have to wait for Musk to bring us on Mars ;)\n");
            targets.remove('Earth');
        if (len(targets) != 1):
            print("Please, select just one celestial body!\n");   

def recap():
    print("\n###################### Recap ######################\n");
    print("Target:", next(iter(targets)), "\n");
    print("Location:\n\t\t{0}°E\n\t\t{1}°N\n\t\t{2} km above sea level\n".format(longitude, latitude, kasl))
    print("Observation starts:\t" + start_datetime + "\nObservation ends:\t" + end_datetime + "\n");
    print("###################################################\n");
                
def main():
#    init_logger();
    select_targets();
    set_location();
    set_datetime();
    recap();
    t = init_telnet();
    gather_ephemeris(t);

main();
