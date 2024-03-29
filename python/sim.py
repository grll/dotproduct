import struct
import time
import numpy as np

SIZE = 1048576
SIM = 10000

with open("../c/data.binary", "rb") as f:
    x_iter = struct.iter_unpack('f', f.read(4*SIZE))
    w_iter = struct.iter_unpack('f', f.read(4*SIZE))

x = [x[0] for x in x_iter]
w = [w[0] for w in w_iter]

def dot(x, w):
    a = 0
    for i in range(SIZE):
        a += x[i] * w[i]
    return a

# start_time = time.time()
# for _ in range(SIM):
#     a = dot(x, w)
# end_time = time.time()

# print("PY  -- It took: %.9f seconds" % ((end_time - start_time) / SIM))
# print("PY  -- The result was: %f" % a)

x = np.array(x)
w = np.array(w)

start_time = time.time()
for _ in range(SIM):
    a = np.dot(x, w)
end_time = time.time()

print("NP  -- It took: %.9f seconds" % ((end_time - start_time) / SIM))
print("NP  -- The result was: %f" % a)


