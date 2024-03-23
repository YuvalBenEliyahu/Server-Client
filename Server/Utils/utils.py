from Crypto.Cipher import PKCS1_OAEP, AES
from Crypto.PublicKey import RSA
from Crypto.Random import get_random_bytes
from Crypto.Util.Padding import unpad
import hashlib
import os

from Utils.cksum import memcrc

DEFAULT_PORT_NUMBER = 1256


def get_port():
    port = DEFAULT_PORT_NUMBER
    try:
        with open('port.info', 'r') as file:
            port = file.readline().strip()
            port = int(port)
            return port
    except FileNotFoundError:
        print("File port.info not found. Using default port 1256.")
    except ValueError:
        print("Invalid port in port.info file. Using default port 1256.")
    finally:
        return port


def generateAESKey():
    return get_random_bytes(32)  # 256 bits key


def encryptAESKey(publicKey, aesKey):
    rsaKey = RSA.import_key(publicKey)
    cipher = PKCS1_OAEP.new(rsaKey)
    return cipher.encrypt(aesKey)


def decrypt(ciphertext, key):
    # Create AES key
    cipher = AES.new(key, AES.MODE_CBC, iv=bytes(16))
    # Decrypt the ciphertext
    plaintext_bytes = cipher.decrypt(ciphertext)
    # Unpad the plaintext bytes (if padding was used during encryption)
    plaintext = unpad(plaintext_bytes, AES.block_size).decode('utf-8')
    return plaintext


def writeToFile(fileName, clientID, content):
    # Create directory for user
    if not os.path.exists(f"Files/{clientID}"):
        os.makedirs(f"Files/{clientID}")
    # Writing file
    with open(f"Files/{clientID}/{fileName}", 'wb') as f:
        f.write(content.encode('utf-8'))


def calculate_checksum(fileName, clientID):
    try:
        buffer = open(f"Files/{clientID}/{fileName}", 'rb').read()
        return memcrc(buffer)
    except IOError:
        print("Unable to open input file", fileName)
        exit(-1)
    except Exception as err:
        print("Error processing the file", err)
        exit(-1)
