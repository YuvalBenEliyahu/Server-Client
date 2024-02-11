import sys

from Protocol.Response.ResponseHeader import ResponseHeader
from Protocol.Response.ResponsePayload import *
from Protocol.Response.ResponsePayloadCode import ResponsePayloadCode

VERSION = 3


class Response:

    def __init__(self, payloadCode, payload):
        try:
            self.header = ResponseHeader(VERSION, payloadCode, sys.getsizeof(payload))
            self.payload = payload
        except Exception as e:
            raise Exception(f"Error unpacking request: {e}")

    def pack(self):
        try:
            if self.header.code in [ResponsePayloadCode.RESPONSE_ERROR, ResponsePayloadCode.RESPONSE_REGISTER_FAIL]:
                return self.header.pack()
            return self.header.pack() + self.payload.pack()
        except (Exception, struct.error) as e:
            raise Exception(f"Error unpacking request: {e}")

    def __str__(self):
        return f"Response({self.header}, {self.payload})"
