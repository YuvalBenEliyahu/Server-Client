from Protocol.Request.RequestHeader import RequestHeader
from Protocol.Request.RequestPayloadCode import RequestPayloadCode
from Protocol.Request.RequestPayload import *


class Request:
    HEADER_SIZE = 23

    def __init__(self, data):
        self.header = None
        self.payload = None
        self.unpack(data)

    def unpack(self, data):
        try:
            self.header = RequestHeader(data[:self.HEADER_SIZE])
            self.unpack_payload(data[self.HEADER_SIZE:])
            print(self.__str__())
        except (Exception, struct.error) as e:
            raise Exception(f"Error unpacking request: {e}")

    def unpack_payload(self, data):
        payloadCode = self.header.code

        payload_mapping = {
            RequestPayloadCode.REQUEST_REGISTRATION.value: RequestRegistration,
            RequestPayloadCode.REQUEST_PUBLIC_KEY.value: RequestPublicKey,
            RequestPayloadCode.REQUEST_RE_CONNECT.value: RequestReConnect,
            RequestPayloadCode.REQUEST_SEND_FILE.value: RequestFile,
            RequestPayloadCode.REQUEST_CRC_VALID.value: RequestCRC,
            RequestPayloadCode.REQUEST_CRC_NOT_VALID_RE_SEND.value: RequestCRC,
            RequestPayloadCode.REQUEST_CRC_NOT_VALID_STOP.value: RequestCRC,
        }
        if payloadCode not in payload_mapping:
            raise Exception(f"Request code {payloadCode} not recognized")
        payloadHandler = payload_mapping[payloadCode]
        self.payload = payloadHandler(data)

    def __str__(self):
        return f"Request({self.header}, {self.payload})"
