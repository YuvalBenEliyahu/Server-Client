import base64

from Protocol.Request.Request import Request
from Protocol.Request.RequestPayloadCode import RequestPayloadCode
from Protocol.Response.Response import Response
from Protocol.Response.ResponsePayload import *
from Protocol.Response.ResponsePayloadCode import ResponsePayloadCode
from database.database import DatabaseManager
from Crypto.Cipher import AES, PKCS1_OAEP
from Crypto.Random import get_random_bytes

import uuid

database = DatabaseManager()


def protocolHandler(data):
    request = Request(data)
    print(str(request))
    request_code_map = {
        RequestPayloadCode.REQUEST_REGISTRATION.value: registrationHandler,
        RequestPayloadCode.REQUEST_PUBLIC_KEY.value: RequestPublicKey,
        RequestPayloadCode.REQUEST_RE_CONNECT.value: RequestReConnect,
        # RequestPayloadCode.REQUEST_SEND_FILE.value: RequestFile,
        # RequestPayloadCode.REQUEST_CRC_VALID.value: RequestCRC,
        # RequestPayloadCode.REQUEST_CRC_NOT_VALID_RE_SEND.value: RequestCRC,
        # RequestPayloadCode.REQUEST_CRC_NOT_VALID_STOP.value: RequestCRC,
    }
    responseHandler = request_code_map[request.header.code]
    return responseHandler(request)


def registrationHandler(request):
    username = request.payload.name
    if database.IsUsernameExists(username):
        payload = RegistrationFail()
        return Response(ResponsePayloadCode.RESPONSE_REGISTER_FAIL, payload).pack()
    clientID = uuid.uuid4().hex
    database.AddNewClient(clientID, username)
    payload = RegistrationSuccess(clientID)
    return Response(ResponsePayloadCode.RESPONSE_REGISTER_SUCCESS.value, payload).pack()


def RequestPublicKey(request):
    username = request.payload.name
    if not database.IsUsernameExists(username):
        return errorResponse()
    publicKey = request.payload.publicKey
    encryptedAESKey = createEncryptedAESKey(publicKey)
    clientID = database.GetClientIDByName(username)
    database.UpdateClientKeysByName(username, publicKey, encryptedAESKey)
    payload = ReceivedPublicKey(clientID, encryptedAESKey)
    return Response(ResponsePayloadCode.RESPONSE_RECEIVED_PUBLIC_KEY.value, payload).pack()


def RequestReConnect(request):
    username = request.payload.name

    if not database.IsUsernameExists(username):
        return ErrorResponse()

    clientID = database.GetClientIDByName(username)

    if database.GetClientAESKeyByID(clientID) is None:
        payload = ReConnectDenied(clientID)
        return Response(ResponsePayloadCode.RESPONSE_RE_CONNECT_DENIED.value, payload).pack()

    publicKey = request.payload.publicKey
    encryptedAESKey = createEncryptedAESKey(publicKey)
    payload = ReConnectAccepted(clientID, encryptedAESKey)
    return Response(ResponsePayloadCode.RESPONSE_RE_CONNECT_ACCEPTED.value, payload).pack()


def createEncryptedAESKey(publicKey):
    aesKey = get_random_bytes(32)
    cipherRSA = PKCS1_OAEP.new(publicKey)
    return cipherRSA.encrypt(aesKey)


def errorResponse():
    payload = ErrorResponse()
    return Response(ResponsePayloadCode.RESPONSE_ERROR.value, payload).pack()
