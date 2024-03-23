import sqlite3
from datetime import datetime


class DatabaseManager:
    def __init__(self, db_filename="defensive.db"):
        self.conn = sqlite3.connect(db_filename)
        self.cursor = self.conn.cursor()
        # Create clients table if not exists
        self.cursor.execute('''
            CREATE TABLE IF NOT EXISTS clients (
                ClientID TEXT PRIMARY KEY,
                Name TEXT NOT NULL,
                PublicKey TEXT,
                LastSeen TEXT,
                AESKey TEXT
            )
        ''')

        # Create files table if not exists
        self.cursor.execute('''
            CREATE TABLE IF NOT EXISTS files (
                ClientID TEXT,
                FileName TEXT NOT NULL,
                FilePath TEXT NOT NULL,
                Verified BOOLEAN,
                FOREIGN KEY (ClientID) REFERENCES clients(ClientID)
            )
        ''')

        self.conn.commit()

    def __del__(self):
        # Close the connection when the object is deleted
        self.conn.close()

    def IsClientIDExists(self, ClientID):
        self.cursor.execute('''
            SELECT COUNT(*)
            FROM clients
            WHERE ClientID = ?
        ''', (ClientID,))

        result = self.cursor.fetchone()
        return result[0] > 0 if result else False

    def AddNewClient(self, ClientID, ClientName, PublicKey=None, AESKey=None):
        # Get the current timestamp for LastSeen
        last_seen = str(datetime.now())

        # Insert the new client into the clients table
        self.cursor.execute('''
            INSERT INTO clients (ClientID, Name, PublicKey, LastSeen, AESKey)
            VALUES (?, ?, ?, ?, ?)
        ''', (ClientID, ClientName, PublicKey, last_seen, AESKey))

        self.conn.commit()

    def UpdateClientKeys(self, ClientID, PublicKey=None, AESKey=None):
        # Update the public key and AES key for the specified client
        self.cursor.execute('''
            UPDATE clients
            SET PublicKey = ?, AESKey = ?, LastSeen = ?
            WHERE ClientID = ?
        ''', (PublicKey, AESKey, str(datetime.now()), ClientID))

        self.conn.commit()

    def AddFile(self, ClientID, FileName, FilePath, Verified=False):
        # Insert the file into the files table
        self.cursor.execute('''
            INSERT INTO files (ClientID, FileName, FilePath, Verified)
            VALUES (?, ?, ?, ?)
        ''', (ClientID, FileName, FilePath, Verified))

        self.conn.commit()

    def GetClientName(self, ClientID):
        self.cursor.execute('''
            SELECT Name FROM clients
            WHERE ClientID = ?
        ''', (ClientID,))
        result = self.cursor.fetchone()
        return result[0] if result else None

    def SetClientName(self, ClientID, NewName):
        self.cursor.execute('''
            UPDATE clients
            SET Name = ?
            WHERE ClientID = ?
        ''', (NewName, ClientID))
        self.conn.commit()

    def GetClientPublicKey(self, ClientID):
        self.cursor.execute('''
            SELECT PublicKey FROM clients
            WHERE ClientID = ?
        ''', (ClientID,))
        result = self.cursor.fetchone()
        return result[0] if result else None

    def SetClientPublicKey(self, ClientID, NewPublicKey):
        self.cursor.execute('''
            UPDATE clients
            SET PublicKey = ?
            WHERE ClientID = ?
        ''', (NewPublicKey, ClientID))
        self.conn.commit()

    def GetClientAESKeyByID(self, ClientID):
        self.cursor.execute('''
            SELECT AESKey FROM clients
            WHERE ClientID = ?
        ''', (ClientID,))
        result = self.cursor.fetchone()
        return result[0] if result else None

    def SetClientAESKey(self, ClientID, NewAESKey):
        self.cursor.execute('''
            UPDATE clients
            SET AESKey = ?
            WHERE ClientID = ?
        ''', (NewAESKey, ClientID))
        self.conn.commit()

    def GetFileName(self, ClientID):
        self.cursor.execute('''
            SELECT FileName FROM files
            WHERE ClientID = ?
        ''', (ClientID,))
        result = self.cursor.fetchone()
        return result[0] if result else None

    def SetFileName(self, ClientID, NewName):
        self.cursor.execute('''
            UPDATE files
            SET FileName = ?
            WHERE ClientID = ?
        ''', (NewName, ClientID))
        self.conn.commit()

    def GetFilePath(self, ClientID):
        self.cursor.execute('''
            SELECT FilePath FROM files
            WHERE ClientID = ?
        ''', (ClientID,))
        result = self.cursor.fetchone()
        return result[0] if result else None

    def SetFilePath(self, ClientID, NewPath):
        self.cursor.execute('''
            UPDATE files
            SET FilePath = ?
            WHERE ClientID = ?
        ''', (NewPath, ClientID))
        self.conn.commit()

    def GetFileVerified(self, ClientID):
        self.cursor.execute('''
            SELECT Verified FROM files
            WHERE ClientID = ?
        ''', (ClientID,))
        result = self.cursor.fetchone()
        return bool(result[0]) if result else None

    def SetFileVerified(self, ClientID, NewVerified):
        self.cursor.execute('''
            UPDATE files
            SET Verified = ?
            WHERE ClientID = ?
        ''', (NewVerified, ClientID))
        self.conn.commit()
