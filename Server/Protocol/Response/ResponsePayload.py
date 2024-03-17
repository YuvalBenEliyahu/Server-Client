import struct
import sys

NAME_SIZE = 255
FILE_SIZE = 255
CLIENT_ID_SIZE = 16


class ResponsePayload:
    def __init__(self, clientID):
        self.clientID = clientID

    def pack(self):
        return struct.pack(f"<{CLIENT_ID_SIZE}s", bytes.fromhex(self.clientID))

    def __str__(self):
        return f"ResponsePayload(clientID: {self.clientID})"


class RegistrationSuccess(ResponsePayload):
    def __init__(self, clientID):
        super().__init__(clientID)

    def pack(self):
        return super().pack()

    def __str__(self):
        return super().__str__()


class RegistrationFail(ResponsePayload):
    def __init__(self):
        super().__init__(None)

    def pack(self):
        return

    def __str__(self):
        return super().__str__()


class ReceivedPublicKey(ResponsePayload):
    def __init__(self, clientID, aesKey):
        super().__init__(clientID)
        self.aesKey = aesKey

    def pack(self):
        print(f"AES KEY SIZE : {len(self.aesKey)}")
        return super().pack() + struct.pack(f"<{sys.getsizeof(self.aesKey)}s", self.aesKey)

    def __str__(self):
        return super().__str__() + f"aesKey: {self.aesKey}"


class FileReceivedWithValidCRC(ResponsePayload):
    def __init__(self, clientID, contentSize, fileName, ckSum):
        super().__init__(clientID)
        self.contentSize = contentSize
        self.fileName = fileName
        self.ckSum = ckSum

    def pack(self):
        super().pack()

    def __str__(self):
        return super().__str__() + f"contentSize: {self.contentSize} fileName: {self.fileName} ckSum: {self.ckSum}"


class MessageReceived(ResponsePayload):
    def __init__(self, clientID):
        super().__init__(clientID)

    def pack(self):
        return super().pack()

    def __str__(self):
        return super().__str__()


class ReConnectAccepted(ResponsePayload):
    def __init__(self, clientID, aesKey):
        super().__init__(clientID)
        self.aesKey = aesKey

    def pack(self):
        return super().pack() + struct.pack(f"<{sys.getsizeof(self.aesKey)}s", self.aesKey)

    def __str__(self):
        return super(ReConnectAccepted, self).__str__() + f"aesKey: {self.aesKey}"


class ReConnectDenied(ResponsePayload):
    def __init__(self, clientID):
        super().__init__(clientID)

    def pack(self):
        return super().pack()

    def __str__(self):
        return super().__str__()


class ErrorResponse(ResponsePayload):
    def __init__(self):
        super().__init__(None)

    def pack(self):
        return
