import struct

NAME_SIZE = 255
FILE_SIZE = 255
PUBLIC_KEY_SIZE = 160


class RequestRegistration:
    def __init__(self, data):
        self.name = b""
        self.unpack(data)

    def unpack(self, data):
        if not data:
            raise Exception("Error unpacking request: empty payload")
        nameData = data[:NAME_SIZE]
        self.name = str(struct.unpack(f"<{NAME_SIZE}s", nameData)[0].partition(b'\0')[0].decode('utf-8'))

    def __str__(self):
        return f"RequestPayload(name: {self.name})"


class RequestPublicKey:
    def __init__(self, data):
        self.name = b""
        self.publicKey = b""
        self.unpack(data)

    def unpack(self, data):
        if not data:
            raise Exception("Error unpacking request: empty payload")

        nameData = data[:NAME_SIZE]
        self.name = str(struct.unpack(f"<{NAME_SIZE}s", nameData)[0].partition(b'\0')[0].decode('utf-8'))
        keyData = data[NAME_SIZE:NAME_SIZE + PUBLIC_KEY_SIZE]
        self.publicKey = struct.unpack(f"<{PUBLIC_KEY_SIZE}s", keyData)[0]


class RequestReConnect:
    def __init__(self, data):
        self.name = b""
        self.unpack(data)

    def unpack(self, data):
        nameData = data[:NAME_SIZE]
        self.name = str(struct.unpack(f"<{NAME_SIZE}s", nameData)[0].partition(b'\0')[0].decode('utf-8'))


class RequestFile:
    def __init__(self, data):
        self.contentSize = b""
        self.OrigFileSize = b""
        self.PacketNumber = 0
        self.totalPackets = 0
        self.fileName = b""
        self.messageContent = b""

    def unpack(self, data):
        print("add")


class RequestCRC:
    def __init__(self, data):
        self.fileName = b""
        self.unpack(data)

    def unpack(self, data):
        fileData = data[:FILE_SIZE]
        self.fileName = str(struct.unpack(f"<{FILE_SIZE}s", fileData)[0].partition(b'\0')[0].decode('utf-8'))
