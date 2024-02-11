import struct


class ResponseHeader:

    def __init__(self, version, code, payloadSize):
        self.version = version
        self.code = code
        self.payloadSize = payloadSize

    def pack(self):
        return struct.pack("<BHL", self.version, self.code, self.payloadSize)

    def __str__(self):
        return f"ResponseHeader(version: {self.version}, code: {self.code}, payloadSize: {self.payloadSize})"
