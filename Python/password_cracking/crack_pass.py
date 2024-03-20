import hashlib

hashed_passwords = []
with open("breached_creds.txt", "r") as f:
    content = f.read()
    hashed_passwords = content.split("\n")

pass_dict = {}
for p in hashed_passwords:
    pass_dict[p.split(":")[0]] = p.split(":")[1]

with open("common.txt", "r") as f:
    content = f.read()
    passwords = content.split("\n")
    for password in passwords:
        common_password = hashlib.sha256(password.encode("utf-8")).hexdigest()
        for username in pass_dict:
            if pass_dict[username] == common_password:
                print(username, password, pass_dict[username])