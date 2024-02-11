import struct

NAME_SIZE = 255
FILE_SIZE = 255
CLIENT_ID_SIZE = 16


class ResponsePayload:
    def __init__(self, clientID):
        self.clientID = clientID

    def pack(self):
        return struct.pack(f"<{CLIENT_ID_SIZE}s", bytes.fromhex(self.clientID))


class RegistrationSuccess(ResponsePayload):
    def __init__(self, clientID):
        super().__init__(clientID)

    def pack(self):
        return super().pack()

    def __str__(self):
        return f"ResponsePayload(ClientID: {self.clientID})"


class RegistrationFail(ResponsePayload):
    def __init__(self):
        super().__init__(None)

    def pack(self):
        return


class ReceivedPublicKey(ResponsePayload):
    def __init__(self, clientID, aesKey):
        super().__init__(clientID)
        self.aesKey = aesKey

    def pack(self):
        return super().pack() + struct.pack(f"<{len(self.aesKey)}s", self.aesKey)


class FileReceivedWithValidCRC(ResponsePayload):
    def __init__(self, clientID, contentSize, fileName, ckSum):
        super().__init__(clientID)
        self.contentSize = contentSize
        self.fileName = fileName
        self.ckSum = ckSum

    def pack(self):
        super().pack()


class MessageReceived(ResponsePayload):
    def __init__(self, clientID):
        super().__init__(clientID)

    def pack(self):
        return super().pack()


class ReConnectAccepted(ResponsePayload):
    def __init__(self, clientID, aesKey):
        super().__init__(clientID)
        self.aesKey = aesKey

    def pack(self):
        return super().pack() + struct.pack(f"<{len(self.aesKey)}s", self.aesKey)


class ReConnectDenied(ResponsePayload):
    def __init__(self, clientID):
        super().__init__(clientID)

    def pack(self):
        return super().pack()


class ErrorResponse(ResponsePayload):
    def __init__(self):
        super().__init__(None)

    def pack(self):
        return
