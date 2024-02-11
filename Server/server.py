import selectors
import socket

from Utils import utils
from Protocol.protocolHandler import protocolHandler


class Server:
    PACKET_SIZE = 1024
    SERVER_VERSION = 3
    MAX_CONNECTIONS = 10

    def __init__(self):
        self.host = '127.0.0.1'
        self.port = utils.get_port()
        self.selector = selectors.DefaultSelector()

    def start(self):
        try:
            sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            sock.bind((self.host, self.port))
            sock.listen(Server.MAX_CONNECTIONS)
            sock.setblocking(False)
            self.selector.register(sock, selectors.EVENT_READ, self.accept)
            print(f"Server listening on {self.host}:{self.port}")
            while True:
                events = self.selector.select(timeout=None)
                for key, mask in events:
                    callback = key.data
                    callback(key.fileobj, mask)
        except Exception as e:
            print(f"An error occurred while listening on {self.host}:{self}: {e}")
            print("Server shutting down.")
        finally:
            self.selector.close()

    def accept(self, sock, mask):
        conn, addr = sock.accept()
        print('accepted', conn, 'from', addr)
        conn.setblocking(False)
        self.selector.register(conn, selectors.EVENT_READ, self.read)

    def read(self, conn, mask):
        try:
            data = conn.recv(Server.PACKET_SIZE)
            if data:
                response = protocolHandler(data)
                self.write(conn, response)
        except ConnectionResetError:
            print('Connection reset by peer')
        except Exception as e:
            self.write(conn, e)
        finally:
            self.selector.unregister(conn)
            conn.close()

    # Add a new method to handle writing to the connection without closing it immediately
    def write(self, conn, data):
        try:
            conn.send(data)
        except Exception as e:
            print(f"Error occurred while writing to connection: {e}")

