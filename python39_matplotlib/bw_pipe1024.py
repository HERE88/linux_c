import matplotlib.pyplot as plt

y1=[202.890,353.20,430.753,695.940,833.398,1021.053,956.790]
x1=[1024,2048,4096,8192,16384,32768,65536]

y2=[207.722,297.157,369.987,430.433,485.955,443.706,441.369]

y3=[272.769,380.853,474.277,440.086,416.815,382.759,258.821]

plt.plot(x1,y1,label='AF_UNIX',linewidth=1,color='r',marker='o',markersize=5)
plt.plot(x1,y2,label='TCP',linewidth=1,color='b',marker='D',markersize=5)
plt.plot(x1,y3,label='PIPE',linewidth=1,color='g',marker='^',markersize=5)

plt.xlabel('message size(bytes)')
plt.ylabel('bandwidth\n(MB/s)')
plt.title('Bandwidth for various types of message passing')

plt.xticks(x1, rotation=270, fontsize=6)
plt.legend()
plt.show()
