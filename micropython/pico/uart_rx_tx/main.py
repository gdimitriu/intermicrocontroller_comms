from pio_rx_tx import SerialRxTx
ser = SerialRxTx(20,21)
while True:
    val = ser.readline()
    ser.put(val)
