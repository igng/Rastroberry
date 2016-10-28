import time
import datetime
from pprint import pprint
from skyfield.api import load

availables = ["mercury", "venus", "mars", "saturn", "jupiter", "neptune", "uranus", "pluto"]

def load_ephemeris():
    global ts
    ts = load.timescale()
    global data
    data = input("Select the ephemeris to download (default = de421.bsp): ")
    if (data == ""):
        data = "de421.bsp"
    print("Using", data, "as ephemeris\n")
    global planets
    planets = load(data)
    global home
    home = planets['earth']

def select_planets():
    global selected
    selected = [];
    while (len(selected) < 1):
        selected = input("Select the planets that you want to observe: ")
        selected = selected.replace(" ", "")
        selected = list(set(selected.split(",")))
        for i in range(len(selected)):
            selected[i] = selected[i].strip().lower()
        if "earth" in selected:
            print("\tYou can't observe the Earth (yet); you have to wait that Elon bring us on Mars")
            selected.remove('earth')
        if ("jupiter" in selected and (data != "jup310.bsp")):
            print("\tUnfortunaly, jupiter is only available in the \"jup310.bsp\" ephemeris (932 MB)")
            selected.remove("jupiter")
        for i in range(len(selected) -1, -1, -1):
            if (selected[i] not in availables):
                print("\t\"" + selected[i] + "\" is not an available planet; are you sure you're in the right Solar System?")
                selected.remove(selected[i])
        if (len(selected) < 1):
            print("     # WARNING: there is no object to track!\n")
    print("Tracking the following planets: ", [elem for elem in selected])
    selected = [selected[i] + " barycenter" for i in range(len(selected))]

def validate(string):
    a = 1
    try:
        datetime.datetime.strptime(string, '%Y-%m-%d %H:%M:%S')
    except ValueError:
        print("\nPlease, use the following format\tYYYY-MM-DD hh:mm:ss")
        a = 0
    return a

def parse_date(time_string):
    s_split = time_string.split(" ")
    date = s_split[0]
    time = s_split[1]
    d_split = [int(elem) for elem in date.split("-")]
    year = d_split[0]
    month = d_split[1]
    day = d_split[2]
    t_split = [int(elem) for elem in time.split(":")]
    hours = t_split[0]
    minutes = t_split[1]
    seconds = t_split[2]
    return [year, month, day, hours, minutes, seconds]

def time_selection():
    v = 0
    start_time = ""
    end_time = ""
    while (v == 0):
        while (not validate(start_time)):
            start_time = input("Select the starting time of observation (default is now (NOT RECCOMANDED)): ")
            if (start_time == ""):
                start_time = datetime.datetime.strftime(datetime.datetime.today(), '%Y-%m-%d %H:%M:%S')
        while (not validate(end_time)):
            end_time = input("Select the ending time of observation (default is now (NOT RECCOMANDED)): ")
            if (end_time == ""):
                end_time = datetime.datetime.strftime(datetime.datetime.today(), '%Y-%m-%d %H:%M:%S')
        s_time = datetime.datetime.strptime(start_time, '%Y-%m-%d %H:%M:%S')
        e_time = datetime.datetime.strptime(end_time, '%Y-%m-%d %H:%M:%S')
        print("\n\nS:", start_time)
        print("E:", end_time)
        global diff
        diff = (e_time - s_time).seconds
        v = 1 if diff > 0 else 0
        if (v == 0):
            print("Martin, you know that we can no more travel back in time!")
            start_time = ""
            end_time = ""
    [s_y, s_m, s_d, s_h, s_mm, s_ss] = parse_date(start_time)
    global time_span
    time_span = ts.utc(s_y, s_m, s_d, s_h, s_mm, range(1, diff))
    print("\nGreat Scott! Martin get the car ready, we're going to the future!\n")

def valid_coordinates(latitude, longitude):
    latitude = latitude.split(" ")
    [lat_val, lat_dir] = [float(latitude[0]), latitude[1].upper()]
    longitude = longitude.split(" ")
    [lon_val, lon_dir] = [float(longitude[0]), longitude[1].upper()]
    if (lat_val < 0 or lat_val > 90 or (lat_dir != "N" and lat_dir != "S")):
        return 0
    if (lon_val < 0 or lon_val > 180 or (lon_dir != "E" and lon_dir != "W")):
        return 0
    return 1

def location_selection():
    global home
    print("Insert the point of observation")
    print("(The use of non-valid coordinates will result in the use of the default ones (Earth's barycenter))")
    latitude = input("Latitude (e.g. 42 N): ")
    longitude = input("Longitude (e.g. 42 E): ")
    if (valid_coordinates(latitude, longitude)):
        home = planets['earth'].topos(latitude, longitude)
    else:
        print("Invalid coordinates; using the Earth's barycenter")
    print(home)

def dump_ephemeris():
    time = time_span.utc
    year = [int(x) for x in time[0]]
    month = [int(x) for x in time[1]]
    day = [int(x) for x in time[2]]
    hour = [int(x) for x in time[3]]
    minute = [int(x) for x in time[4]]
    second = [int(round(x)) for x in time[5]]
    for planet in selected:
        astrometric = home.at(time_span).observe(planets[planet])
        filename = planet.split(" ")[0] + ".txt"
        f = open(filename, "w")
        ra, dec, distance = astrometric.radec()
        ra_arr = ra.hms()
        de_arr = dec.dms()
        print("\tDumping \"" + filename + "\"")
        for i in range(diff-1):
            timestamp = str(year[i]) + "-" + str(month[i]).zfill(2) + "-" + str(day[i]).zfill(2) + " " + str(hour[i]).zfill(2) + ":" + str(minute[i]).zfill(2) + ":" + str(second[i]).zfill(2) + "\t"
            coords = str(ra_arr[0][i]) + ", " + str(ra_arr[1][i]) + ", " + str(ra_arr[2][i]) + '\n'
            f.write(timestamp + coords)
        f.close()

def main():
    load_ephemeris() 
    select_planets()
    time_selection()
    location_selection()
    dump_ephemeris()

main()
