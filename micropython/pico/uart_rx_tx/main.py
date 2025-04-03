from pio_rx_tx import SerialRxTx
ser = SerialRxTx(20,21)
while True:
    val = ser.readline()
    print(val + "\n")
    ser.put(val+"\n")
