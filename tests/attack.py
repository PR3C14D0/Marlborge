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
	stat = s.recv(1)
	if stat[0] != 0x0F:
		print("Error")
time.sleep(1)

data = s.recv(1024)
packet = data[0]
ipLength = data[1]
ipEnd = 2 + ipLength;
ip = data[2:ipEnd].decode()
port = data[ipEnd]
duration = data[ipEnd + 1]

print(data)

if packet == 0x02:
	print(ip)
	print(port)
	print(duration)

s.send(bytes([0x10]))
data = s.recv(1024)

s.close()