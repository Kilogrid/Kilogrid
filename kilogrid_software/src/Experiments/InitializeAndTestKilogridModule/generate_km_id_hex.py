from getopt import getopt, GetoptError
from intelhex import IntelHex  # use class IntelHex

# memory values
MEMORY = {
    'size': 1024,          # memory size
    'coordinate_x': 0xb2,  # address for coordinate X
    'coordinate_y': 0xb3   # address for coordinate Y
}

# default configurations
DEFAULTS = {
    'dirname': 'eeproms',
    'xlim': 10,
    'ylim': 20,
    'filler': 0xff
}


# Generate hex file for module (X,Y)
# Param filename :: hex file name
# Param x :: module coordinate X
# Param y :: module coordinate Y
# [Note] If filename includes directories,
#        the directories should be created before calling this function
def generate_hex(filename, x, y):
    # put data on memory
    ih = IntelHex()
    for address in range(0, MEMORY['size']):
        if address == MEMORY['coordinate_x']:    # put coordinate x
            ih[address] = x
        elif address == MEMORY['coordinate_y']:  # put coordinate y
            ih[address] = y
        else:                                    # fill rest of memory
            ih[address] = DEFAULTS['filler']

    # write to hex file
    ih.write_hex_file(filename)


# main
if __name__ == "__main__":
    import sys
    import os

    # get options
    try:
        opts, args = getopt(
            sys.argv[1:],
            "x:y:h",
            ["xlim=", "ylim=", "dirname=", "help"]
        )
    except GetoptError:
        sys.stderr.write("try `python {0} --help`".format(__file__))
        sys.exit(1)

    # set variables
    X = DEFAULTS['xlim']
    Y = DEFAULTS['ylim']
    directory = DEFAULTS['dirname']
    for opt, arg in opts:
        if opt == "--dirname":
            directory = arg
        elif opt in ("-x", "--xlim"):
            X = int(arg)
        elif opt in ("-y", "--ylim"):
            Y = int(arg)
        elif opt in ("-h", "--help"):
            print("Usage > python {0} [options]".format(__file__))
            print("  --xlim=[val], -x [val] :: set X limit of module")
            print("  --ylim=[val], -y [val] :: set Y limit of module")
            print("  --dirname=[val]        :: set directory for hex files")
            exit(0)

    # make directory unless it exists
    if not os.path.isdir(directory):
        try:
            os.makedirs(directory)
            sys.stderr.write("Make dir '{}'\n".format(directory))
        except OSError:
            sys.stderr.write("Could not make dirs '{}'".format(directory))
            sys.exit(1)

    # write hex file for each module
    for x in range(0, X):
        for y in range(0, Y):
            filename = '{0}/eeprom_{1}_{2}.hex'.format(directory, x, y)
            generate_hex(filename, x, y)
