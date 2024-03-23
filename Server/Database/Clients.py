from Database.Database import DatabaseManager

database = DatabaseManager()


class Clients:
    def __init__(self):
        self.clients = {}

    def get_client(self, clientID):
        # if client in DB but not in RAM
        if clientID in self.clients and database.IsClientIDExists(clientID):
            client = self.clients[clientID]
            if client.name is None:
                client.name = database.GetClientName(clientID)
            if client.publicKey is None:
                client.publicKey = database.GetClientPublicKey(clientID)
            if client.aesKey is None:
                client.aesKey = database.GetClientAESKeyByID(clientID)

        elif clientID not in self.clients and database.IsClientIDExists(clientID):
            name = database.GetClientName(clientID)
            publicKey = database.GetClientPublicKey(clientID)
            aesKey = database.GetClientAESKeyByID(clientID)
            self.add_client(name, clientID, publicKey, aesKey)

        if clientID in self.clients:
            return self.clients[clientID]
        else:
            return None

    def add_client(self, name, clientID, publicKey=None, aesKey=None, files=None):
        self.clients[clientID] = Client(name, clientID, publicKey, aesKey, files)

    def set_client_name(self, name, clientID):
        self.clients[clientID].name = name

    def set_client_publicKey(self, clientID, publicKey):
        self.clients[clientID].publicKey = publicKey

    def set_client_aesKey(self, clientID, aesKey):
        self.clients[clientID].aesKey = aesKey

    def set_client_file(self, clientID, filename, file):
        self.clients[clientID].files[filename] = file


class Client:
    def __init__(self, name, clientID, publicKey, aesKey, files=None):
        self.name = name
        self.clientID = clientID
        self.publicKey = publicKey
        self.aesKey = aesKey
        if files is None:
            self.files = {}
        else:
            self.files = files
