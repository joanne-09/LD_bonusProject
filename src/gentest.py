from random import *
d = dict()

f = open("test.txt", "w")

for i in range(800):
    x = randint(0, 4095)
    binary_x = bin(x)[2:].zfill(12)
    d[binary_x] = x

for v in d:
    f.write(v)
    f.write("\n")

f.close()