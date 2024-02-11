import struct


class RequestHeader:
    CLIENT_ID_SIZE = 16
    HEADER_SIZE = 7

    def __init__(self, data):

        self.clientID = None
        self.version = None
        self.code = None
        self.payloadSize = None
        self.unpack(data)

    def unpack(self, data):
        if not data:
            raise Exception("Error unpacking request: empty header")
        self.clientID = struct.unpack(f"<{self.CLIENT_ID_SIZE}s", data[:self.CLIENT_ID_SIZE])[0]
        headerData = data[self.CLIENT_ID_SIZE:self.CLIENT_ID_SIZE + self.HEADER_SIZE]
        self.version, self.code, self.payloadSize = struct.unpack("<BHL", headerData)

    def __str__(self):
        return f"RequestHeader(clientID: {self.clientID}, version: {self.version}, code: {self.code}, payloadSize: {self.payloadSize})"
