from cryptography.fernet import Fernet
import os

with open("decryptor.key", "r+b") as key_file:
    key = key_file.read()
#os.chdir("C:\\")
for each_file in os.listdir():
    if each_file != "ransomware.py" and each_file != "decrypt.py" and each_file != "decryptor.key" and os.path.isfile(each_file):
        with open(each_file, "r+b") as f:
            content = f.read()
            decrypted_content = Fernet(key).decrypt(content)
            f.seek(0)
            f.write(decrypted_content)
            f.truncate()
