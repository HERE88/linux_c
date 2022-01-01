import matplotlib.pyplot as plt
y1=[36.143,41.325,121.139]
x1=['PIPE','AF_UNIX','TCP']

plt.bar(x1,y1,width=0.4)

plt.ylabel('Latency(microseconds)')
plt.title('Latency to exchange a 1-byte message \nusing various forms of IPC')

plt.show()