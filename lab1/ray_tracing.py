import numpy as np
import matplotlib.pyplot as plt
import matplotlib.collections as mc

segments = [[(0, 0), (-10, 17.3205)], [(-10, 17.3205), (10, 17.3205)], [(0, 0), (10, 17.3205)]]
prev = (0, 0)
with open('out.txt', 'r') as f:
    lines = f.readlines()
    for line in lines:
        inter = tuple(map(float, line.split(",")))
        segments.append([prev, inter])
        prev = inter

coll = mc.LineCollection(segments)
fig, ax = plt.subplots()
ax.add_collection(coll)
ax.margins(0.1)
plt.show()
