import telnetlib
import logging
import sys
import datetime
import time

today = datetime.date.today().isoformat();
bodies = ["Mercury", "Venus", "Earth", "Mars", "Jupiter", "Saturn", "Neptune", "Uranus"];
not_default = ["j", "+0", "cal", "min", "deg", "no", "refracted", "km", "yes", " ", " ", "n", "y", " ", " ", "yes"];
longitude = 42;
latitude = 18;
kasl = 0.3;
HOST = "horizons.jpl.nasa.gov";
PORT = 6775;
global first_time;
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

    t.write((today + " 22:00\r\n").encode('ascii'));
    t.read_until(b": ", timeout=2).decode('ascii');
    t.write((today + " 23:59\r\n").encode('ascii'));
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
    for i in range(199, (len(bodies)+1)*100, 100):
        if (i != 399):
            f = open(bodies[int(i/100)-1] + ".txt", 'w');
            logging.info("  Gathering ephemeris for %s", bodies[int(i/100)-1]);
            fill_fields(i, t);
            change_default_options(t);
            write_to_file(f, t);
            logging.info("  Closing \"%s\"\n", f.name);
            f.close();
                
def main():
    init_logger();
    t = init_telnet();
    gather_ephemeris(t);


main();
