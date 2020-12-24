import socket
import struct
UDP_IP = "127.0.0.1"
UDP_PORT = 5555

sock = socket.socket(socket.AF_INET, # Internet
                     socket.SOCK_DGRAM) # UDP
sock.bind((UDP_IP, UDP_PORT))
counter =0
while True:

    code_ver,addr = sock.recvfrom(4)
    print(code_ver);
    size,addr = sock.recvfrom(2)
    left = struct.unpack("<H",size)[0]
    decoded_packet = []
    while left > 0 :
      print(left)
      data, addr = sock.recvfrom( left ) # buffer size is 1024 bytes
      for i in range(left):
        decoded_packet.append(hex(data[i]))
      left -= len(data)
    counter+=1  
    print( "received message: ", decoded_packet )

    print( "\nWAITING NEW DATA !!!!!! {} \n".format(counter) )
