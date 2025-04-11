import matplotlib.pyplot as plt

f = open("./output/results.txt", "r")
resultx = []
resulty = []
with open("./output/results.txt") as fp:
    for line in fp:
        # print(line)
        n, minim = line.split()[0], min([float(x) for x in line.split()[1:]]);
        resultx.append(int((n.split("="))[1]))
        resulty.append(minim);

plt.plot(resultx, resulty)
plt.ylabel('ticks')
plt.xlabel('num of nops')
plt.show()
