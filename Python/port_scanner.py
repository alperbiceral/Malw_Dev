import socket
import threading
import queue

target_ip = "192.168.1.36"
q = queue.Queue()

for i in range(1, 65000):
    q.put(i)

def scan():
    while not q.empty():
        port = q.get() # get the next queue value
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            try:
                s.connect((target_ip, port))
                print(f"Port {port} is open")
            except:
                print(f"Port {port} is closed")
                pass

        q.task_done()

# set threads
for i in range(30):
    # if the scan function would have parameters, the code will be threading.Thread(target=scan, args=[param1, param2], daemon=True)
    t = threading.Thread(target=scan, daemon=True)
    t.start()

q.join() # don't pass this line until all the queues are done
print("finished")