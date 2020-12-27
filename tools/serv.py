#!/usr/bin/env python

import socket
import time
import struct
import crc8
TCP_IP = '127.0.0.1'
TCP_PORT = 5556
BUFFER_SIZE = 20  # Normally 1024, but we want fast response
ACK_MESSAGE = [0x11, 0x2, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xff]
REPLY_MESSAGE = [0x15, 0x4, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xff]

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.bind((TCP_IP, TCP_PORT))
s.listen(1)
print("waiting for connection")
conn, addr = s.accept()
print("user accepted")

conn.send(bytearray([ord('A'),ord('A'),ord('A'),ord('A'),ord('A'),ord('A'),ord('A'),ord('A'),ord('A'),ord('A'),ord('A'),ord('A')]) )  # echo
time.sleep(1)

print("Waiting for pakets...")

while True:
  hash = crc8.crc8()

  control_bytes = conn.recv(4)
  control_bytes = struct.unpack("<I", control_bytes)[0]
  if control_bytes != 0x123456ff:
    conn.close();
    exit()
  print( "Packet received..." )
  size = conn.recv(2)
  size = struct.unpack("<H", size)[0]
  print( "Packet size: {}".format(hex(size)) )
  packet = conn.recv( size )
  # decode packet to human readable form :)
  decoded_packet = []
  for i in range(size):
    decoded_packet.append(packet[i])

  print( "Packet data: {}".format(decoded_packet) )
  packet_type = packet[1]
  if(packet_type not in [0x1, 0x3, 0x15]):
    print("Packet {} not recognize".format(packet_type))
    continue

  # copy seq number
  ACK_MESSAGE[2] = REPLY_MESSAGE[2] = packet[2]
  # copy uid
  ACK_MESSAGE[3:15] = REPLY_MESSAGE[3:15] = packet[3:15]
  
  if packet_type == 0x1:
    print("Packet PING")
    # set packet for which will be send ack
    ACK_MESSAGE[15] = packet_type
    # set crc
    hash.update(bytearray(ACK_MESSAGE[:-1]))
    ACK_MESSAGE[16] = hash.digest()[0]
    print("Sending ACK... {}".format(ACK_MESSAGE))
    conn.send(bytearray(ACK_MESSAGE))
  elif packet_type == 0x3:
    print("Packet STATUS REQUEST")
    # set logic addres
    REPLY_MESSAGE[15] = 0xca
    # set signal
    REPLY_MESSAGE[16] = 0x40;
    # set uptime
    REPLY_MESSAGE[17] = REPLY_MESSAGE[18] = REPLY_MESSAGE[19] = 0x1;
    # set crc
    hash.update(bytearray(REPLY_MESSAGE[:-1]))
    REPLY_MESSAGE[20] = hash.digest()[0]
    print("Sending REPLY... {}".format(REPLY_MESSAGE))
    conn.send(bytearray(REPLY_MESSAGE))
  elif packet_type == 0x15:
    print("Packet SET CHANNEL")
    # set crc
    REPLY_MESSAGE[15] = 0x15
    ACK_MESSAGE[15] = 0x15
    hash.update(bytearray(ACK_MESSAGE[:-1]))
    ACK_MESSAGE[16] = hash.digest()[0]
    print("Sending ACK... {}".format(ACK_MESSAGE))
    conn.send(bytearray(ACK_MESSAGE))

conn.close()

