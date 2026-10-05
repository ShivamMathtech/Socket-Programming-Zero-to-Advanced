"""Small local functional concurrency exercise; prints measurements from your run only."""
import argparse
import socket
import time
from concurrent.futures import ThreadPoolExecutor
parser = argparse.ArgumentParser()
parser.add_argument('--port',type=int,default=9000)
parser.add_argument('--clients',type=int,default=8)
args=parser.parse_args()
if not 1<=args.clients<=32: parser.error('choose 1..32 clients for this local lab')
def exchange(index):
    payload=(f'client {index}\n'.encode())*128
    with socket.create_connection(('127.0.0.1',args.port),timeout=5) as s:
        start=time.perf_counter();s.sendall(payload);reply=b''
        while len(reply)<len(payload):
            part=s.recv(len(payload)-len(reply))
            if not part: raise RuntimeError('truncated echo')
            reply+=part
        if reply!=payload: raise RuntimeError('incorrect echo')
        return (time.perf_counter()-start)*1000
with ThreadPoolExecutor(max_workers=args.clients) as executor:
    elapsed=list(executor.map(exchange,range(args.clients)))
print(f'{len(elapsed)} correct replies; local mean roundtrip={sum(elapsed)/len(elapsed):.3f} ms')
print('This includes client scheduling; it is not a production throughput benchmark.')
