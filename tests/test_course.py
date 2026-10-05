"""Loopback integration tests. Python is a test driver; all course implementations are C."""
from concurrent.futures import ThreadPoolExecutor
from contextlib import contextmanager
from pathlib import Path
import hashlib
import os
import select
import signal
import socket
import struct
import subprocess
import tempfile
import time
import unittest
ROOT = Path(__file__).resolve().parents[1]
BIN = ROOT / 'build'
def port(family=socket.AF_INET, kind=socket.SOCK_STREAM):
    with socket.socket(family, kind) as s:
        s.bind(('::1' if family == socket.AF_INET6 else '127.0.0.1', 0))
        return s.getsockname()[1]
def run(name, *args, input=None):
    return subprocess.run([str(BIN/name), *map(str,args)],input=input,capture_output=True,timeout=12)
def exact(s, count):
    data = bytearray()
    while len(data) < count:
        more = s.recv(count-len(data))
        if not more: raise AssertionError('unexpected EOF')
        data.extend(more)
    return bytes(data)
def dial(p, host='127.0.0.1'):
    return socket.create_connection((host,p),timeout=5)
@contextmanager
def server(name, p, *args):
    # Never probe a one-shot server: its readiness line does not consume a client.
    process = subprocess.Popen([str(BIN/name),str(p),*map(str,args)],
        stdin=subprocess.PIPE,stdout=subprocess.PIPE,stderr=subprocess.PIPE,start_new_session=True)
    try:
        ready,_,_ = select.select([process.stdout,process.stderr],[],[],5)
        if not ready: raise AssertionError(f'{name}: no startup message')
        line = ready[0].readline()
        if process.poll() is not None: raise AssertionError(f'{name} failed: {line!r}')
        yield process
    finally:
        # Include forked children in cleanup even when a test assertion fails.
        try: os.killpg(process.pid,signal.SIGTERM)
        except ProcessLookupError: pass
        try: process.communicate(timeout=3)
        except subprocess.TimeoutExpired:
            os.killpg(process.pid,signal.SIGKILL); process.communicate()
class CourseTests(unittest.TestCase):
    def test_01_local_demos(self):
        for name,expected in [('byte_order',b'wire=23 28'),('socket_info',b'SOCK_STREAM'),
                              ('fork_demo',b'parent value=7'),('thread_demo',b'shared value=99'),
                              ('counter',b'400000'),('socket_options',b'SO_KEEPALIVE=1'),
                              ('receive_timeout',b'timed out'),('broken_pipe',b'EPIPE')]:
            with self.subTest(program=name):
                result=run(name); self.assertEqual(result.returncode,0,result.stderr); self.assertIn(expected,result.stdout)
    def test_02_standalone_echo(self):
        p=port()
        with server('tcp_server',p) as process:
            result=run('tcp_client','127.0.0.1',p,'hello C sockets')
            self.assertEqual(result.returncode,0,result.stderr)
            self.assertEqual(result.stdout,b'hello C sockets\n'); self.assertEqual(process.wait(timeout=3),0)
    def test_03_raw_echo_variants(self):
        payload=bytes(range(256))*40
        for name in ['iterative_server','fork_server','thread_server','select_server','poll_server','epoll_server']:
            with self.subTest(server=name):
                p=port()
                with server(name,p):
                    for _ in range(2):
                        with dial(p) as s:
                            s.sendall(payload); s.shutdown(socket.SHUT_WR)
                            self.assertEqual(exact(s,len(payload)),payload); self.assertEqual(s.recv(1),b'')
    def test_04_concurrent_echo_and_idle_peer(self):
        def exchange(p,index):
            data=bytes([index])*4000
            with dial(p) as s: s.sendall(data); return exact(s,len(data))==data
        for name in ['fork_server','thread_server','select_server','poll_server','epoll_server']:
            with self.subTest(server=name):
                p=port()
                with server(name,p),dial(p) as idle:
                    with ThreadPoolExecutor(max_workers=8) as pool:
                        self.assertTrue(all(pool.map(lambda i:exchange(p,i),range(16))))
    def test_05_event_loop_slow_reader(self):
        for name in ['select_server','poll_server','epoll_server']:
            with self.subTest(server=name):
                p=port()
                with server(name,p),dial(p) as slow:
                    slow.setsockopt(socket.SOL_SOCKET,socket.SO_RCVBUF,1024)
                    slow.setblocking(False)
                    block=b'x'*65536
                    # Stop when the sender itself experiences backpressure; never block the test.
                    for _ in range(128):
                        try: slow.send(block)
                        except BlockingIOError: break
                    with dial(p) as normal:
                        normal.sendall(b'progress'); self.assertEqual(exact(normal,8),b'progress')
    def test_06_fragmented_coalesced_and_empty_frames(self):
        p=port()
        with server('frame_server',p),dial(p) as s:
            for payload in [b'abc\x00def',b'',bytes(range(256))*3]:
                wire=struct.pack('!I',len(payload))+payload
                for i in range(0,len(wire),3): s.sendall(wire[i:i+3])
                count=struct.unpack('!I',exact(s,4))[0]; self.assertEqual(exact(s,count),payload)
            s.sendall(struct.pack('!I',1)+b'A'+struct.pack('!I',1)+b'B')
            self.assertEqual(exact(s,10),b'\x00\x00\x00\x01A\x00\x00\x00\x01B')
    def test_07_frame_client_and_rejected_length(self):
        p=port()
        with server('frame_server',p):
            result=run('frame_client','127.0.0.1',p,input=b'first\nsecond\n')
            self.assertEqual(result.returncode,0,result.stderr); self.assertEqual(result.stdout,b'first\nsecond\n')
            with dial(p) as s:
                s.sendall(struct.pack('!I',2**24)); self.assertEqual(s.recv(1),b'')
            with dial(p) as s:
                s.sendall(b'\0\0'); s.shutdown(socket.SHUT_WR); self.assertEqual(s.recv(1),b'')
    def test_08_udp_echo_and_empty_datagram(self):
        p=port(kind=socket.SOCK_DGRAM)
        with server('udp_server',p):
            result=run('udp_client','127.0.0.1',p,'datagram')
            self.assertEqual(result.returncode,0,result.stderr); self.assertEqual(result.stdout,b'datagram\n')
            with socket.socket(socket.AF_INET,socket.SOCK_DGRAM) as s:
                s.settimeout(3)
                for data in [b'',bytes(range(256))*10]:
                    s.sendto(data,('127.0.0.1',p)); reply,source=s.recvfrom(65536)
                    self.assertEqual(reply,data); self.assertEqual(source[1],p)
    def test_09_broadcast_option_local_delivery(self):
        # Runtime path is checked with loopback unicast; actual LAN broadcast is a manual topology-specific lab.
        with socket.socket(socket.AF_INET,socket.SOCK_DGRAM) as s:
            s.bind(('127.0.0.1',0)); s.settimeout(3)
            result=run('udp_broadcast','127.0.0.1',s.getsockname()[1],'one datagram')
            self.assertEqual(result.returncode,0,result.stderr); self.assertEqual(s.recvfrom(2048)[0],b'one datagram')
    def test_10_nonblocking_connect(self):
        p=port()
        with server('iterative_server',p):
            self.assertEqual(run('connect_timeout','127.0.0.1',p).returncode,0)
        result=run('connect_timeout','127.0.0.1',p)
        self.assertNotEqual(result.returncode,0)
    def test_11_ipv6(self):
        try:p=port(socket.AF_INET6)
        except OSError as error:self.skipTest(f'IPv6 loopback unavailable: {error}')
        with server('ipv6_server',p):
            result=run('ipv6_client','::1',p,'IPv6 works')
            self.assertEqual(result.returncode,0,result.stderr); self.assertEqual(result.stdout,b'IPv6 works\n')
    def test_12_tcp_chat_server_duplex(self):
        p=port()
        with server('tcp_chat_server',p) as process,dial(p) as s:
            process.stdin.write(b'from server\n'); process.stdin.flush()
            self.assertEqual(exact(s,12),b'from server\n')
            s.sendall(b'from client\n')
            ready,_,_=select.select([process.stdout],[],[],3); self.assertTrue(ready)
            self.assertEqual(process.stdout.readline(),b'from client\n')
            s.shutdown(socket.SHUT_WR); self.assertEqual(process.wait(timeout=3),0)
    def test_13_tcp_chat_client(self):
        p=port()
        with server('iterative_server',p):
            result=run('tcp_chat_client','127.0.0.1',p,input=b'chat client\n')
            self.assertEqual(result.returncode,0,result.stderr); self.assertEqual(result.stdout,b'chat client\n')
    def test_14_udp_chat_peer(self):
        p=port(kind=socket.SOCK_DGRAM)
        with socket.socket(socket.AF_INET,socket.SOCK_DGRAM) as remote:
            remote.bind(('127.0.0.1',0)); remote.settimeout(3)
            with server('udp_chat',p,'127.0.0.1',remote.getsockname()[1]) as process:
                process.stdin.write(b'hello UDP\n'); process.stdin.flush()
                data,addr=remote.recvfrom(2048); self.assertEqual(data,b'hello UDP\n')
                remote.sendto(b'reply UDP\n',addr)
                ready,_,_=select.select([process.stdout],[],[],3); self.assertTrue(ready)
                self.assertEqual(process.stdout.readline(),b'reply UDP\n')
    def test_15_multi_chat_split_lines(self):
        p=port()
        with server('multi_chat',p),dial(p) as a,dial(p) as b:
            # Roundtrip from B establishes that the server accepted both peers.
            b.sendall(b'ready\n')
            def line(s):
                data=b''
                while not data.endswith(b'\n'): data+=exact(s,1)
                return data
            self.assertIn(b'ready\n',line(b)); self.assertIn(b'ready\n',line(a))
            a.sendall(b'hel'); a.sendall(b'lo\nworld\n')
            for s in (a,b):
                self.assertIn(b'hello\n',line(s)); self.assertIn(b'world\n',line(s))
    def test_16_multi_chat_oversize_line(self):
        p=port()
        with server('multi_chat',p),dial(p) as s:
            s.sendall(b'x'*513)
            self.assertEqual(s.recv(1),b'')
    def test_17_file_transfer_binary_and_empty(self):
        with tempfile.TemporaryDirectory() as directory:
            root=Path(directory)
            for data in [b'',bytes(range(256))*400]:
                source=root/'input.bin'; dest=root/'output.bin'; source.write_bytes(data); p=port()
                with server('file_receiver',p,dest) as process:
                    result=run('file_sender','127.0.0.1',p,source)
                    self.assertEqual(result.returncode,0,result.stderr); self.assertEqual(process.wait(timeout=3),0)
                self.assertEqual(hashlib.sha256(dest.read_bytes()).digest(),hashlib.sha256(data).digest()); dest.unlink()
    def test_18_file_transfer_refuses_overwrite(self):
        with tempfile.TemporaryDirectory() as d:
            path=Path(d)/'existing';path.write_bytes(b'keep me')
            self.assertNotEqual(run('file_receiver',port(),path).returncode,0)
            self.assertEqual(path.read_bytes(),b'keep me')
    def test_19_file_transfer_truncation_cleanup(self):
        with tempfile.TemporaryDirectory() as d:
            path=Path(d)/'partial'; p=port()
            with server('file_receiver',p,path) as process,dial(p) as s:
                s.sendall(struct.pack('!I',100)+b'short');s.shutdown(socket.SHUT_WR)
                self.assertNotEqual(process.wait(timeout=3),0)
            self.assertFalse(path.exists())
    def test_20_http_routes_head_and_errors(self):
        p=port()
        def request(data):
            with dial(p) as s:
                s.sendall(data); response=b''
                while True:
                    part=s.recv(16384)
                    if not part: return response
                    response+=part
        with server('http_server',p):
            self.assertIn(b'200 OK',request(b'GET / HTTP/1.1\r\nHost: localhost\r\n\r\n'))
            self.assertTrue(request(b'GET /health HTTP/1.1\r\n\r\n').endswith(b'ok\n'))
            head=request(b'HEAD / HTTP/1.1\r\n\r\n');self.assertTrue(head.endswith(b'\r\n\r\n'))
            self.assertIn(b'404 Not Found',request(b'GET /../etc/passwd HTTP/1.1\r\n\r\n'))
            self.assertIn(b'405 Method Not Allowed',request(b'POST / HTTP/1.1\r\n\r\n'))
            self.assertIn(b'400 Bad Request',request(b'broken\r\n\r\n'))
            self.assertIn(b'431 Request Header',request(b'G'*8192))
    def test_21_http_fragmented_headers(self):
        p=port()
        with server('http_server',p),dial(p) as s:
            for chunk in [b'GET /he',b'alth HTTP/1.1\r\n',b'Host: localhost\r\n',b'\r\n']: s.sendall(chunk)
            data=b''
            while True:
                chunk=s.recv(4096)
                if not chunk: break
                data+=chunk
            self.assertTrue(data.endswith(b'ok\n'))
    def test_22_invalid_cli_arguments(self):
        for name in ['tcp_server','tcp_client','frame_server','frame_client','udp_server','udp_client',
                     'iterative_server','fork_server','thread_server','select_server','poll_server','epoll_server',
                     'connect_timeout','ipv6_server','ipv6_client','tcp_chat_server','tcp_chat_client','udp_chat',
                     'multi_chat','file_sender','file_receiver','http_server','udp_broadcast']:
            with self.subTest(program=name): self.assertNotEqual(run(name).returncode,0)
if __name__ == '__main__': unittest.main(verbosity=2)
