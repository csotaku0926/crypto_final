import matplotlib.pyplot as plt
import numpy as np

with open("test.txt", "r") as f:
    datas = f.readlines()

Bs = []
rates = []
for data in datas[:500]:
    n, q, b, dec_rate = data.split(" ")
    n, q, b, dec_rate = int(n), int(q), int(b), int(dec_rate[:-1])
    Bs.append(b)
    rates.append(dec_rate)

plt.plot(Bs, rates, 'r+')

plt.xticks(np.arange(0, len(rates)+1, 100))
plt.yticks(np.arange(20, 120, 20))
plt.xlabel("Error Bound")
plt.ylabel("Decryption Success Rate (%)")

plt.savefig("figs/plot.png")
plt.show()