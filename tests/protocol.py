import socket, time

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

s.connect(('localhost', 5000))
s.send(bytes([0x01]))

data = s.recv(1024)
if len(data) > 0 and data[0] == 0x01:
	code = data[1:].decode()
	print("Received: " + code)
	response = bytes([0x01]) + code.encode()
	s.send(response)
time.sleep(2)
s.send(bytes([0x10]))
data = s.recv(1024)

s.close()