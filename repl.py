import serial

ser = serial.Serial("/dev/ttyUSB0", 9600, timeout=0.2)

try:
  while True:
    request = raw_input("req>  ").strip() + "\r\n"
    ser.write(request)
    response = ser.read(1024)
    for line in response.splitlines():
      print "resp> %s" % (line.strip())
    print
finally:
  ser.close()
