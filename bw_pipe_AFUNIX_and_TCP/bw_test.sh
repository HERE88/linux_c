#!/bin/sh
echo "test for AFUNIX 1024:"
/data/log/bw_pipeAFUNIX 50 500 1024 > /data/log/1.txt

echo "test for AFUNIX 2048:"
/data/log/bw_pipeAFUNIX 50 500 2048 >> /data/log/1.txt

echo "test for AFUNIX 4096:"
/data/log/bw_pipeAFUNIX 50 500 4096 >> /data/log/1.txt

echo "test for AFUNIX 8192:"
/data/log/bw_pipeAFUNIX 50 500 8192 >> /data/log/1.txt

echo "test for AFUNIX 16384:"
/data/log/bw_pipeAFUNIX 50 500 16384 >> /data/log/1.txt

echo "test for AFUNIX 32768:"
/data/log/bw_pipeAFUNIX 50 500 32768 >> /data/log/1.txt

echo "test for AFUNIX 65536:"
/data/log/bw_pipeAFUNIX 50 500 65536 >> /data/log/1.txt

echo ""

echo "test for TCP 1024:"
/data/log/bw_pipeTCP 50 500 1024 >> /data/log/1.txt

echo "test for TCP 2048:"
/data/log/bw_pipeTCP 50 500 2048 >> /data/log/1.txt

echo "test for TCP 4096:"
/data/log/bw_pipeTCP 50 500 4096 >> /data/log/1.txt

echo "test for TCP 8192:"
/data/log/bw_pipeTCP 50 500 8192 >> /data/log/1.txt

echo "test for TCP 16384:"
/data/log/bw_pipeTCP 50 500 16384 >> /data/log/1.txt

echo "test for TCP 32768:"
/data/log/bw_pipeTCP 50 500 32768 >> /data/log/1.txt

echo "test for TCP 65536:"
/data/log/bw_pipeTCP 50 500 65536 >> /data/log/1.txt

echo ""

echo "test for PIPE 1024:"
/data/log/bw_pipe 50 500 1024 >> /data/log/1.txt

echo "test for TCP 2048:"
/data/log/bw_pipe 50 500 2048 >> /data/log/1.txt

echo "test for TCP 4096:"
/data/log/bw_pipe 50 500 4096 >> /data/log/1.txt

echo "test for TCP 8192:"
/data/log/bw_pipe 50 500 8192 >> /data/log/1.txt

echo "test for TCP 16384:"
/data/log/bw_pipe 50 500 16384 >> /data/log/1.txt

echo "test for TCP 32768:"
/data/log/bw_pipe 50 500 32768 >> /data/log/1.txt

echo "test for TCP 65536:"
/data/log/bw_pipe 50 500 65536 >> /data/log/1.txt
