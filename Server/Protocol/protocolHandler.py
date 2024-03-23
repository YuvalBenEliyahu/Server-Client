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
    try:
        request_code_map = {
            RequestPayloadCode.REQUEST_REGISTRATION.value: registrationHandler,
            RequestPayloadCode.REQUEST_PUBLIC_KEY.value: RequestPublicKey,
            RequestPayloadCode.REQUEST_RE_CONNECT.value: RequestReConnect,
            RequestPayloadCode.REQUEST_SEND_FILE.value: RequestFile,
            RequestPayloadCode.REQUEST_CRC_VALID.value: RequestCRCValid,
            RequestPayloadCode.REQUEST_CRC_NOT_VALID_RE_SEND.value: RequestCRCReSend,
            RequestPayloadCode.REQUEST_CRC_NOT_VALID_STOP.value: RequestCRCInvalidStop,
        }
        responseHandler = request_code_map[request.header.code]
        response = responseHandler(request)
        return response
    except Exception as e:
        print(f"An error occurred: {e}")
        return errorResponse(request)


def registrationHandler(request):
    clientID = uuid.uuid4().hex
    while database.IsClientIDExists(clientID):
        clientID = uuid.uuid4().hex
    database.AddNewClient(clientID, request.payload.name)
    clients.add_client(request.payload.name, clientID)
    payload = RegistrationSuccess(clientID)
    return Response(ResponsePayloadCode.RESPONSE_REGISTER_SUCCESS.value, payload).pack()


def RequestPublicKey(request):
    clientID = request.header.clientID
    if not database.IsClientIDExists(clientID):
        return ErrorResponse(request)
    publicKey = request.payload.publicKey
    aesKey = generateAESKey()
    encryptedAESKey = encryptAESKey(publicKey, aesKey)
    updateDatabaseAndRAM(clientID, aesKey, publicKey)
    payload = ReceivedPublicKey(clientID, encryptedAESKey)
    return Response(ResponsePayloadCode.RESPONSE_RECEIVED_PUBLIC_KEY.value, payload).pack()


def RequestReConnect(request):
    clientID = request.header.clientID
    if not database.IsClientIDExists(clientID) or not database.GetClientPublicKey(clientID):
        payload = ReConnectDenied(clientID)
        return Response(ResponsePayloadCode.RESPONSE_RE_CONNECT_DENIED.value, payload).pack()
    clients.get_client(clientID)  # get client to RAM incase it is not there
    publicKey = database.GetClientPublicKey(clientID)
    aesKey = generateAESKey()
    encryptedAESKey = encryptAESKey(publicKey, aesKey)
    updateDatabaseAndRAM(clientID, aesKey, publicKey)
    payload = ReConnectAccepted(clientID, encryptedAESKey)
    return Response(ResponsePayloadCode.RESPONSE_RE_CONNECT_ACCEPTED.value, payload).pack()


def RequestFile(request):
    clientID = request.header.clientID
    client = clients.get_client(clientID)  # Getting client from RAM
    if client is None:
        return ErrorResponse(request)
    aesKey = client.aesKey
    fileName = request.payload.fileName
    # The file is stored as dictionary with fileName:[packets]
    if fileName not in client.files:
        # Initialize an empty dictionary for this file
        client.files[fileName] = {}
    client.files[fileName][request.payload.packetNumber] = request.payload.messageContent
    if len(client.files[fileName]) == request.payload.totalPackets:
        concatenated_ciphertext = b''.join(client.files[fileName].values())
        decryptedFile = decrypt(concatenated_ciphertext, aesKey)
        writeToFile(fileName, clientID, decryptedFile)
        ckSum = calculate_checksum(fileName, clientID)
        payload = FileReceivedWithValidCRC(clientID, len(decryptedFile), fileName, ckSum)
        return Response(ResponsePayloadCode.RESPONSE_FILE_RECEIVED_WITH_VALID_CRC.value, payload).pack()


def RequestCRCValid(request):
    clientID = request.header.clientID
    if not database.IsClientIDExists(clientID):
        return ErrorResponse(request)
    fileName = request.payload.fileName
    client = clients.get_client(clientID)
    if fileName not in client.files:
        return ErrorResponse(request)
    database.AddFile(clientID, fileName, f"Files/{clientID}/{fileName}", True)
    payload = MessageReceived(clientID)
    return Response(ResponsePayloadCode.RESPONSE_MSG_RECEIVED.value, payload).pack()


def RequestCRCInvalidStop(request):
    print("CRC invalid stopped with invalid ")
    clientID = request.header.clientID
    payload = MessageReceived(clientID)
    return Response(ResponsePayloadCode.RESPONSE_MSG_RECEIVED.value, payload).pack()


def RequestCRCReSend(request):
    print("CRC invalid, resend")


def errorResponse(request):
    clientID = request.header.clientID
    payload = ErrorResponse(clientID)
    return Response(ResponsePayloadCode.RESPONSE_ERROR.value, payload).pack()


def updateDatabaseAndRAM(clientID, aesKey, publicKey):
    database.UpdateClientKeys(clientID, publicKey, aesKey)
    clients.set_client_publicKey(clientID, publicKey)
    clients.set_client_aesKey(clientID, aesKey)
