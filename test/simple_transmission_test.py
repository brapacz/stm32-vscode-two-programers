import unittest
import serial
from os import system
from threading import Thread
import time

SMALL_UART = '/dev/ttyUSB2'
BIG_UART = '/dev/ttyUSB0'

class MCU(object):
    def __init__(self, name, port):
        self.port = port
        self.serial = serial.Serial(port=port, baudrate=115200, bytesize=8, timeout=1, stopbits=serial.STOPBITS_ONE)
        self.name = name

        self.ready = False
        self.lines = []

        self._read_data_in_background()


    def reset(self):
        self.ready = False
        self.lines = []

        system(f'cd {self.name} && make reset 2> /dev/null > /dev/null')

    def close(self):
        self.running = False
        self._thread.join()

    def wait_for_string(self, string, timeout=1):
        start = time.time()
        while time.time() - start <= timeout:
            for line in self.lines:
                if line.startswith(string):
                    return line

    def _read_data_in_background(self):
        self.running = True
        def _thread_main(self):
            while self.running:
                line = self.serial.readline()
                if line:
                    self.lines.append(line)
            self.serial.close()

        self._thread=Thread(target=_thread_main,args=(self,))
        self._thread.start()



class SimpleTransmissionTest(unittest.TestCase):
    def setUp(self) :
        self.big = MCU('big', BIG_UART)
        self.big.reset()

        self.small = MCU('small', SMALL_UART)
        self.small.reset()

        self.big.wait_for_string(b'Ready')
        self.small.wait_for_string(b'Ready')

        return super().setUp()

    def tearDown(self):
        self.small.close()
        self.big.close()

        result = self._outcome.result
        ok = all(test != self for test, text in result.errors + result.failures)
        if not ok:
            print()
            print('SMALL:')
            for line in self.small.lines:
                print(line)
            print()
            print('BIG:')
            for line in self.big.lines:
                print(line)
            print()

        return super().tearDown()

    def test_simply_pass_string_over_spi(self):
        self.big.serial.write(b"QWERTY\r\n")
        self.assertIsNotNone(self.small.wait_for_string(b"SPI: < QWERTY"), 'MCU should print QWERTY')


if __name__ == '__main__':
    unittest.main()
