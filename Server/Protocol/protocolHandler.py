from Protocol.Request.Request import Request
from Protocol.Request.RequestPayloadCode import RequestPayloadCode
from Protocol.Response.Response import Response
from Protocol.Response.ResponsePayload import *
from Protocol.Response.ResponsePayloadCode import ResponsePayloadCode
from Utils.utils import *
from Database.Clients import Clients
from Database.Database import DatabaseManager

import uuid

database = DatabaseManager()
clients = Clients()


def protocolHandler(data):
    request = Request(data)
    request_code_map = {
        RequestPayloadCode.REQUEST_REGISTRATION.value: registrationHandler,
        RequestPayloadCode.REQUEST_PUBLIC_KEY.value: RequestPublicKey,
        RequestPayloadCode.REQUEST_RE_CONNECT.value: RequestReConnect,
        RequestPayloadCode.REQUEST_SEND_FILE.value: RequestFile,
        # RequestPayloadCode.REQUEST_CRC_VALID.value: RequestCRC,
        # RequestPayloadCode.REQUEST_CRC_NOT_VALID_RE_SEND.value: RequestCRC,
        # RequestPayloadCode.REQUEST_CRC_NOT_VALID_STOP.value: RequestCRC,
    }
    responseHandler = request_code_map[request.header.code]
    response = responseHandler(request)
    return response


def registrationHandler(request):
    clientID = uuid.uuid4().hex
    while database.IsClientIDExists(clientID):
        clientID = uuid.uuid4().hex
    database.AddNewClient(clientID, request.payload.name)
    payload = RegistrationSuccess(clientID)
    return Response(ResponsePayloadCode.RESPONSE_REGISTER_SUCCESS.value, payload).pack()


def RequestPublicKey(request):
    clientID = request.header.clientID
    if not database.IsClientIDExists(clientID):
        return ErrorResponse()
    publicKey = request.payload.publicKey
    aesKey = generateAESKey()
    encryptedAESKey = encryptAESKey(publicKey, aesKey)
    database.UpdateClientKeys(clientID, publicKey, aesKey)
    payload = ReceivedPublicKey(clientID, encryptedAESKey)
    return Response(ResponsePayloadCode.RESPONSE_RECEIVED_PUBLIC_KEY.value, payload).pack()


def RequestReConnect(request):
    clientID = request.header.clientID
    if not database.IsClientIDExists(clientID):
        return ErrorResponse()

    if database.GetClientAESKeyByID(clientID) is None:
        payload = ReConnectDenied(clientID)
        return Response(ResponsePayloadCode.RESPONSE_RE_CONNECT_DENIED.value, payload).pack()

    publicKey = database.GetClientPublicKey(clientID)
    aesKey = generateAESKey()
    encryptedAESKey = encryptAESKey(publicKey, aesKey)
    database.UpdateClientKeys(clientID, publicKey, aesKey)
    payload = ReConnectAccepted(clientID, encryptedAESKey)
    return Response(ResponsePayloadCode.RESPONSE_RE_CONNECT_ACCEPTED.value, payload).pack()


def RequestFile(request):
    clientID = request.header.clientID
    if not database.IsClientIDExists(clientID):
        return ErrorResponse()
    aesKey = database.GetClientAESKeyByID(clientID)
    fileName = request.payload.fileName
    client = clients.get_client(clientID)  # Getting client from RAM
    decryptedFile = decrypt(request.payload.messageContent, aesKey)
    # The file is stored as dictionary with fileName:[packets]
    if fileName not in client.files:
        # Initialize an empty dictionary for this file
        client.files[fileName] = {}
    client.files[fileName][request.payload.packetNumber] = decryptedFile
    if len(client.files[fileName]) == request.payload.totalPackets:
        writeToFile(fileName, clientID, client.files[fileName])


def errorResponse():
    payload = ErrorResponse()
    return Response(ResponsePayloadCode.RESPONSE_ERROR.value, payload).pack()
