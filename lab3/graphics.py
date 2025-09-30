import matplotlib.pyplot as plt

resultx = []
resulty = []
with open("./cur_graphics.txt") as fp:
    for line in fp:
        # print(line)
        if "NUM" in line:
            plt.title(line)
            continue
        n, misses = line.split()
        resultx.append(int(n))
        resulty.append(int(misses));

plt.plot(resultx, resulty)
plt.ylabel('branch misses')
plt.xlabel('pattern length')
plt.show()
