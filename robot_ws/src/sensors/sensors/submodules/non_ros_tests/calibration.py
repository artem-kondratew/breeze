import numpy as np
import os
import pickle


file = open(os.path.join(os.path.dirname(__file__), 'logs', 'data0.log'), 'rb')
data = pickle.load(file)


def low_pass_filter(prev_value, new_value):
    return 0 * prev_value + 1 * new_value


mag_x, mag_y, mag_z = data[0]
min_x = max_x = mag_x
min_y = max_y = mag_y
min_z = max_z = mag_z

for dot in data:
	mag_x = low_pass_filter(mag_x, dot[0]) 
	mag_y = low_pass_filter(mag_y, dot[1])
	mag_z = low_pass_filter(mag_z, dot[2])

	min_x = min(min_x, mag_x)
	min_y = min(min_y, mag_y)
	min_z = min(min_z, mag_z)

	max_x = max(max_x, mag_x)
	max_y = max(max_y, mag_y)
	max_z = max(max_z, mag_z)

	offset_x = (max_x + min_x) / 2
	offset_y = (max_y + min_y) / 2
	offset_z = (max_z + min_z) / 2

	field_x = (max_x - min_x) / 2
	field_y = (max_y - min_y) / 2
	field_z = (max_z - min_z) / 2
 
mag_field       = (field_x,  field_y,  field_z)
mag_calibration = (offset_x, offset_y, offset_z)

print(f"magnetometer.offset = {mag_calibration}")
print(f"magnetometer.field  = {mag_field}")

new_data = []
for point in data:
	dot = [(i - j)/k  for i, j, k in zip(point, mag_calibration, mag_field)]
	new_data.append(dot)
 
points = np.array(new_data)

# fig = plt.figure()
# ax = fig.add_subplot(111, projection='3d')
# ax.scatter(points[:, 0], points[:, 1], points[:, 2])
# plt.show()
