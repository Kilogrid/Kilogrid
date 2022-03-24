from datetime import datetime
import numpy as np


conf_file_name = 'ASB_experiment_1'
option_1_quality = 156
option_2_quality = 210
option_3_quality = 210

wall_value = 42


module_grid = np.zeros((10, 20), dtype=np.uint8)
cell_grid = np.zeros((20, 40), dtype=np.uint8)

def make_walls(grid):
    grid[:, 0:1] = wall_value
    grid[:, -1:] = wall_value
    grid[0:1, :] = wall_value
    grid[-1:, :] = wall_value
    return grid

def distribute_tiles(grid):
    op1 = np.ones((option_1_quality,)) * 1
    op2 = np.ones((option_2_quality,)) * 2
    op3 = np.ones((option_3_quality,)) * 3
    op = np.concatenate((op1, op2, op3))
    np.random.shuffle(op)
    op =np.reshape(op, (16, 36))
    grid[2:18, 2:38] = op
    return grid


if __name__ == "__main__":

    # set up module grid
    module_grid = make_walls(module_grid)

    # set up cell grid
    cell_grid = distribute_tiles(cell_grid)

    # write file
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
                print(hex(module_grid[x, y]), file=f)
                print(hex(cell_grid[x * 2, y * 2 + 1]), file=f)
                print(hex(cell_grid[x * 2 + 1, y * 2 + 1]), file=f)
                print(hex(cell_grid[x * 2, y * 2]), file=f)
                print(hex(cell_grid[x * 2 + 1, y * 2]), file=f)
                # needed for structure
                print('', file=f)
