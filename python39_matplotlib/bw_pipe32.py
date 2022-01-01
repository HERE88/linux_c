import matplotlib.pyplot as plt

y1=[7.725,15.087,29.918,57.965,112.131,201.191]
x1=[32,64,128,256,512,1024]

y2=[10.306,20.778,40.179,75.555,130.217,210.129]

y3=[12.597,24.569,48.787,93.762,173.159,264.963]

plt.plot(x1,y1,label='AF_UNIX',linewidth=1,color='r',marker='o',markersize=5)
plt.plot(x1,y2,label='TCP',linewidth=1,color='b',marker='D',markersize=5)
plt.plot(x1,y3,label='PIPE',linewidth=1,color='g',marker='^',markersize=5)

plt.xlabel('message size(bytes)')
plt.ylabel('bandwidth\n(MB/s)')
plt.title('Bandwidth for various types of message passing')

plt.xticks(x1, rotation=270, fontsize=6)
plt.legend()
plt.show()
