from datetime import datetime
import numpy as np
import cv2 as cv

# TODO Parameters to choose
conf_file_name = 'christmas_final'  # name of the generated config file
image_name = "pixil-frame-0-2.png"  # name of the image file

configuration_grid = np.zeros((10, 20))
wall_value = 42

def make_walls(grid):
    grid[:, 0:1] = wall_value
    grid[:, -1:] = wall_value
    grid[0:1, :] = wall_value
    grid[-1:, :] = wall_value
    return grid


if __name__ == "__main__":
    # setting up the configuration
    # walls -> check if you need this configuration!!
    configuration_grid = make_walls(configuration_grid)

    im = cv.imread(image_name)

    gray = cv.cvtColor(im, cv.COLOR_BGR2GRAY)
    gray = gray.transpose()
    gray = np.fliplr(gray)
    gray = np.rot90(gray, 2)

    # TODO change values according to your image
    gray[gray == 99] = 3
    gray[gray == 87] = 1
    gray[gray == 135] = 2
    gray[gray == 94] = 4
    gray[gray == 224] = 5
    print(gray)

    print(gray.shape)

    with open(conf_file_name + '.kconf', 'w') as f:
        now = datetime.now()
        dt_string = now.strftime("%d/%m/%Y %H:%M:%S")
        print('# Experiment: ', conf_file_name, file=f)
        print('# Generation Time: ', dt_string, file=f)
        print('\n', file=f)

        # writing for each module
        for x in range(10):
            for y in range(20):
                # print header
                print('address', file=f)
                print('module:' + str(x) + '-' + str(y) + '\n', file=f)
                print('data', file=f)
                # fill with values ..
                print(hex(x), file=f)
                print(hex(y), file=f)
                print(hex(int(configuration_grid[x, y])), file=f)
                print(hex(gray[x * 2, y * 2 + 1]), file=f)
                print(hex(gray[x * 2 + 1, y * 2 + 1]), file=f)
                print(hex(gray[x * 2, y * 2]), file=f)
                print(hex(gray[x * 2 + 1, y * 2]), file=f)
                # needed for structure
                print('', file=f)
