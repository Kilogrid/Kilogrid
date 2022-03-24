# -*- coding: utf-8 -*-

WA_X_WIDTH = 1
WA_Y_WIDTH = 1
MAX_X = 9
MAX_Y = 19
NEST_START = 5
NEST_END = 14

GRAD_ID_WALL = 0x01
GRAD_ID_TO_SITE_A = 0x02
GRAD_ID_TO_SITE_B = 0x03
GRAD_ID_TO_NEST = 0x04
ID_SITE_TYPE = 0x05

def create_matrix():
	return [[0 for x in range((MAX_Y+1)*2)] for x in range((MAX_X+1)*2)]


def get_sc_coordinates(x,y):
	base_x = x*2
	base_y = y*2

	return [(base_x, base_y+1), (base_x+1, base_y+1), (base_x, base_y), (base_x+1, base_y)]


def write_values(f, m, addr_type, address, grad_id, vals):
	vals_str = '\n'.join(map(hex, vals))

	f.write('address\n')
	f.write('%s:%d\n' % (addr_type, address))
	f.write('data\n')
	f.write('%s\n%s\n\n' % (hex(grad_id), vals_str))

	if(addr_type == 'x'):
		for y in range(0, MAX_Y+1):
			sc_addresses = get_sc_coordinates(address, y)
			for i in range(4):
				scx, scy = sc_addresses[i]
				m[scx][scy] += vals[i]

	elif(addr_type == 'y'):
		for x in range(0, MAX_X+1):
			sc_addresses = get_sc_coordinates(x, address)
			for i in range(4):
				scx, scy = sc_addresses[i]
				m[scx][scy] += vals[i]

def write_values2(f, addr_type, address, grad_id, vals):
	vals_str = '\n'.join(map(hex, vals))

	f.write('address\n')
	f.write('%s:%d\n' % (addr_type, address))
	f.write('data\n')
	f.write('%s\n%s\n\n' % (hex(grad_id), vals_str))


def write_wall_avoidance_gradient(f, m):
	for i in range(0, WA_X_WIDTH):
		val = (WA_X_WIDTH-i)*2
		vals = [val, val-1, val, val-1]

		write_values(f, m, 'x', i, GRAD_ID_WALL, vals)
		write_values(f, m, 'x', MAX_X-i, GRAD_ID_WALL, list(reversed(vals)))

	for i in range(0, WA_Y_WIDTH):
		val = (WA_Y_WIDTH-i)*2
		vals = [val-1, val-1, val, val]

		write_values(f, m, 'y', i, GRAD_ID_WALL, vals)
		write_values(f, m, 'y', MAX_Y-i, GRAD_ID_WALL, list(reversed(vals)))


def write_to_site_a_gradient(f, m):
	width = NEST_END-NEST_START + 2
	for i in range(0, width+1):
		addr = NEST_START-1+i
		val = (width-i+1)*2
		vals = [val-1, val-1, val, val]

		write_values(f, m, 'y', addr, GRAD_ID_TO_SITE_A, vals)


def write_to_site_b_gradient(f, m):
	width = NEST_END-NEST_START + 2
	for i in range(0, width+1):
		addr = NEST_START-1+i
		val = (i+1)*2
		vals = [val, val, val-1, val-1]

		write_values(f, m, 'y', addr, GRAD_ID_TO_SITE_B, vals)


def write_to_nest_gradient(f, m):
	for i in range(0, NEST_START+1):
		val = (NEST_START+1-i)*2
		addr = i
		vals = [val-1, val-1, val, val]

		write_values(f, m, 'y', addr, GRAD_ID_TO_NEST, vals)

	for i in range(0, NEST_START+1):
		val = (NEST_START+1-i)*2
		addr = MAX_Y-i
		vals = [val, val, val-1, val-1]

		write_values(f, m, 'y', addr, GRAD_ID_TO_NEST, vals)


def write_matrix(fname, m):
	with open(fname, 'w') as fout:
		for x in range(0, (MAX_X+1)*2):
			fout.write('%s\n' % '\t'.join(map(str, m[x])))


def write_areas(f):
	for i in range(0, NEST_START):
		write_values2(f, 'y', i, ID_SITE_TYPE, [0x01])

	for i in range(NEST_START, NEST_END+1):
		write_values2(f, 'y', i, ID_SITE_TYPE, [0x02])

	for i in range(NEST_END+1, MAX_Y+1):
		write_values2(f, 'y', i, ID_SITE_TYPE, [0x03])


m = create_matrix()
with open('wa.kconf', 'w') as fout:
	write_wall_avoidance_gradient(fout, m)
write_matrix('wa.matrix', m)

m = create_matrix()
with open('to_site_a.kconf', 'w') as fout:
	write_to_site_a_gradient(fout, m)
write_matrix('to_site_a.matrix', m)

m = create_matrix()
with open('to_site_b.kconf', 'w') as fout:
	write_to_site_b_gradient(fout, m)
write_matrix('to_site_b.matrix', m)

m = create_matrix()
with open('to_nest.kconf', 'w') as fout:
	write_to_nest_gradient(fout, m)
write_matrix('to_nest.matrix', m)

m = create_matrix()
with open('all.kconf', 'w') as fout:
	write_wall_avoidance_gradient(fout, m)
	write_to_site_a_gradient(fout, m)
	write_to_site_b_gradient(fout, m)
	write_to_nest_gradient(fout, m)
	write_areas(fout)
