#!/bin/sh
echo "test for AFUNIX 32:"
/data/log/bw_pipeAFUNIX 50 100 32 > /data/log/2.txt

echo "test for AFUNIX 64:"
/data/log/bw_pipeAFUNIX 50 100 64 >> /data/log/2.txt

echo "test for AFUNIX 128:"
/data/log/bw_pipeAFUNIX 50 100 128 >> /data/log/2.txt

echo "test for AFUNIX 256:"
/data/log/bw_pipeAFUNIX 50 100 256 >> /data/log/2.txt

echo "test for AFUNIX 512:"
/data/log/bw_pipeAFUNIX 50 100 512 >> /data/log/2.txt

echo "test for AFUNIX 1024:"
/data/log/bw_pipeAFUNIX 50 100 1024 >> /data/log/2.txt

echo ""

echo "test for TCP 32:"
/data/log/bw_pipeTCP 50 100 32 >> /data/log/2.txt

echo "test for TCP 64:"
/data/log/bw_pipeTCP 50 100 64 >> /data/log/2.txt

echo "test for TCP 128:"
/data/log/bw_pipeTCP 50 100 128 >> /data/log/2.txt

echo "test for TCP 256:"
/data/log/bw_pipeTCP 50 100 256 >> /data/log/2.txt

echo "test for TCP 512:"
/data/log/bw_pipeTCP 50 100 512 >> /data/log/2.txt

echo "test for TCP 1024:"
/data/log/bw_pipeTCP 50 100 1024 >> /data/log/2.txt

echo ""

echo "test for PIPE 32:"
/data/log/bw_pipe 50 100 32 >> /data/log/2.txt

echo "test for PIPE 64:"
/data/log/bw_pipe 50 100 64 >> /data/log/2.txt

echo "test for PIPE 128:"
/data/log/bw_pipe 50 100 128 >> /data/log/2.txt

echo "test for PIPE 256:"
/data/log/bw_pipe 50 100 256 >> /data/log/2.txt

echo "test for PIPE 512:"
/data/log/bw_pipe 50 100 512 >> /data/log/2.txt

echo "test for PIPE 1024:"
/data/log/bw_pipe 50 100 1024 >> /data/log/2.txt