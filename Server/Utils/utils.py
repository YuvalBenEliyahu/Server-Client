import binascii

from Crypto.Cipher import PKCS1_OAEP, AES
from Crypto.PublicKey import RSA
from Crypto.Random import get_random_bytes
from Crypto.Util.Padding import unpad
import hashlib
import os

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
    print(plaintext)
    return plaintext


def writeToFile(fileName, clientID, packets):
    # Create directory for user
    if not os.path.exists(f"Files/{clientID}"):
        os.makedirs(f"Files/{clientID}")
    # Writing file
    with open(f"Files/{clientID}/{fileName}", 'w') as f:
        file_data = ''.join(packets[packetNumber] for packetNumber in sorted(packets.keys()))
        # Decode the byte sequence
        f.write(file_data)


def calculate_checksum(file_path):
    sha256_hash = hashlib.sha256()
    with open(file_path, "rb") as f:
        # Read and update hash string value in blocks of 4K
        for byte_block in iter(lambda: f.read(4096), b""):
            sha256_hash.update(byte_block)
        return sha256_hash.hexdigest()
