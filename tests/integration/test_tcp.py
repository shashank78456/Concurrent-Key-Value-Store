import socket
import time
import uuid
import unittest

HOST = "127.0.0.1"
PORT = "6379"

MAX_REQUEST_SIZE = 64*1024

def connect():
    client = socket.create_connection((HOST, PORT), timeout=5)
    client.settimeout(5)
    return client

def recv_line(client):
    data = bytearray()
    
    while(True):
        chunk = client.recv(1)
        
        if(not chunk):
            raise RuntimeError("Server closed connection unexpectedly")
    
        data.extend(chunk)
        
        if(chunk==b'\n'):
            break
        
    return bytes(data)

def unique_key(prefix):  
    return f"{prefix}_{uuid.uuid4().hex}"


class TCPBasicTests(unittest.TestCase):
    
    def setUp(self):
        self.client = connect()
    
    def tearDown(self):
        self.client.close()
        
    def test_set_get_delete(self):
        key = unique_key("basic")

        self.client.sendall(f"SET {key} Test\n".encode())
        self.assertEqual(recv_line(self.client), b"OK\n")

        self.client.sendall(f"GET {key}\n".encode())
        self.assertEqual(recv_line(self.client), b"Test\n")

        self.client.sendall(f"DELETE {key}\n".encode())
        self.assertEqual(recv_line(self.client), b"OK\n")

        self.client.sendall(f"GET {key}\n".encode())
        self.assertEqual(recv_line(self.client), b"NOT FOUND\n")

    def test_update_existing_key(self):
        key = unique_key("update")

        self.client.sendall(f"SET {key} first\n".encode())
        self.assertEqual(recv_line(self.client), b"OK\n")

        self.client.sendall(f"SET {key} second\n".encode())
        self.assertEqual(recv_line(self.client), b"OK\n")

        self.client.sendall(f"GET {key}\n".encode())
        self.assertEqual(recv_line(self.client), b"second\n")

    def test_get_missing_key(self):
        key = unique_key("missing")

        self.client.sendall(f"GET {key}\n".encode())
        self.assertEqual(recv_line(self.client), b"NOT FOUND\n")


class TCPFramingTests(unittest.TestCase):
    def setUp(self):
        self.client = connect()
    
    def tearDown(self):
        self.client.close()
    
    def test_fragmented_command(self):
        key = unique_key("fragmented")

        self.client.sendall(f"SET {key} ".encode())

        time.sleep(0.1)

        self.client.sendall(b"Hello World\n")

        self.assertEqual(recv_line(self.client), b"OK\n")

        self.client.sendall(f"GET {key}\n".encode())

        self.assertEqual(
            recv_line(self.client),
            b"Hello World\n"
        )

    def test_multiple_commands(self):
        key = unique_key("pipeline")

        requests = (
            f"SET {key} one\n"
            f"GET {key}\n"
            f"DELETE {key}\n"
            f"GET {key}\n"
        )

        self.client.sendall(requests.encode())

        self.assertEqual(recv_line(self.client), b"OK\n")
        self.assertEqual(recv_line(self.client), b"one\n")
        self.assertEqual(recv_line(self.client), b"OK\n")
        self.assertEqual(recv_line(self.client), b"NOT FOUND\n")

    def test_crlf(self):
        key = unique_key("crlf")

        requests = (
            f"SET {key} Hello World\r\n"
            f"GET {key}\r\n"
        )

        self.client.sendall(requests.encode())

        self.assertEqual(recv_line(self.client), b"OK\n")
        self.assertEqual(recv_line(self.client), b"Hello World\n")


class TCPClientTests(unittest.TestCase):
    def test_eight_simultaneous_clients(self):
        clients = []

        try:
            for _ in range(8):
                clients.append(connect())

            keys = []

            for i, client in enumerate(clients):
                key = unique_key(f"client_{i}")
                keys.append(key)

                client.sendall(f"SET {key} value_{i}\n".encode())

            for client in clients:
                self.assertEqual(recv_line(client), b"OK\n")

            for i, client in enumerate(clients):
                client.sendall(f"GET {keys[i]}\n".encode())

            for i, client in enumerate(clients):
                self.assertEqual(
                    recv_line(client),
                    f"value_{i}\n".encode()
                )

        finally:
            for client in clients:
                client.close()

    def test_ninth_client_is_rejected(self):
        clients = []
        ninth_client = None

        try:
            for _ in range(8):
                clients.append(connect())

            time.sleep(0.2)

            ninth_client = connect()
            ninth_client.settimeout(3)

            self.assertEqual(ninth_client.recv(1), b'')

        finally:
            if ninth_client is not None:
                ninth_client.close()

            for client in clients:
                client.close()

    def test_server_after_client_disconnect(self):
        client = connect()
        client.close()

        client = connect()

        try:
            key = unique_key("disconnect")

            client.sendall(f"SET {key} working\n".encode())

            self.assertEqual(
                recv_line(client),
                b"OK\n"
            )

        finally:
            client.close()


class TCPErrorTests(unittest.TestCase):
    
    def setUp(self):
        self.client = connect()

    def tearDown(self):
        self.client.close()

    def test_invalid_commands(self):
        self.client.sendall(b"GET name extra\n")
        self.assertEqual(recv_line(self.client), b"ERROR\n")

        self.client.sendall(b"DELETE name extra\n")
        self.assertEqual(recv_line(self.client), b"ERROR\n")

        self.client.sendall(b"UNKNOWN name\n")
        self.assertEqual(recv_line(self.client), b"ERROR\n")

    def test_reject_oversized_request(self):
        request = b'x' * (MAX_REQUEST_SIZE + 1)

        try:
            self.client.sendall(request)
        except OSError:
            pass

        self.assertEqual(
            recv_line(self.client),
            b"ERROR\n"
        )

    
if __name__ == "__main__":
    unittest.main(verbosity=2)