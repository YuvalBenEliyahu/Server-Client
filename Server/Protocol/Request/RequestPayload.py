import struct

NAME_SIZE = 255
FILE_SIZE = 255
PUBLIC_KEY_SIZE = 160


class RequestRegistration:
    def __init__(self, data):
        self.name = None
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
        self.name = None
        self.publicKey = None
        self.unpack(data)

    def unpack(self, data):
        if not data:
            raise Exception("Error unpacking request: empty payload")

        nameData = data[:NAME_SIZE]
        self.name = str(struct.unpack(f"<{NAME_SIZE}s", nameData)[0].partition(b'\0')[0].decode('utf-8'))
        keyData = data[NAME_SIZE:NAME_SIZE + PUBLIC_KEY_SIZE]
        self.publicKey = struct.unpack(f"<{PUBLIC_KEY_SIZE}s", keyData)[0]

    def __str__(self):
        return f"RequestPayload(name: {self.name} publicKey: {self.publicKey})"


class RequestReConnect:
    def __init__(self, data):
        self.name = b""
        self.unpack(data)

    def unpack(self, data):
        nameData = data[:NAME_SIZE]
        self.name = str(struct.unpack(f"<{NAME_SIZE}s", nameData)[0].partition(b'\0')[0].decode('utf-8'))

    def __str__(self):
        return f"RequestPayload(name: {self.name})"


class RequestFile:
    def __init__(self, data):
        self.contentSize = b""
        self.origFileSize = b""
        self.packetNumber = b""
        self.totalPackets = b""
        self.fileName = b""
        self.messageContent = b""
        self.unpack(data)

    def unpack(self, data):
        self.contentSize, self.origFileSize, self.packetNumber, self.totalPackets, self.fileName = struct.unpack(
            '<IIHH255s', data[:267])

        self.fileName = self.fileName.partition(b'\0')[0].decode('utf-8')
        # The rest of the data is the message content
        messageContent = data[267:]
        print(messageContent)
        print(len(messageContent))
        self.messageContent = struct.unpack(f"<{len(messageContent)}s", messageContent)[0]

    def __str__(self):
        return (f"RequestPayload(contentSize: {self.contentSize} origFileSize: {self.origFileSize}"
                f" packetNumber: {self.packetNumber + 1} totalPackets: {self.totalPackets} fileName: {self.fileName} messageContent: {self.messageContent})")


class RequestCRC:
    def __init__(self, data):
        self.fileName = b""
        self.unpack(data)

    def unpack(self, data):
        fileData = data[:FILE_SIZE]
        self.fileName = str(struct.unpack(f"<{FILE_SIZE}s", fileData)[0].partition(b'\0')[0].decode('utf-8'))

    def __str__(self):
        return f"RequestPayload(fileName: {self.fileName})"
