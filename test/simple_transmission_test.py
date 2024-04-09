import unittest
import serial
from os import system, getenv
from threading import Thread
import time
import logging
import sys

SMALL_UART_DEVICE = getenv('SMALL_UART_DEVICE')
BIG_UART_DEVICE = getenv('BIG_UART_DEVICE')

logger = logging.getLogger()
try:
    logger.level = getattr(logging, getenv('LOG_LEVEL', 'INFO').upper())
except AttributeError:
    logger.level = logging.INFO

stream_handler = logging.StreamHandler(sys.stdout)
logger.addHandler(stream_handler)

class MCU(object):
    def __init__(self, name, port, startTime, testCase):
        self.startTime = startTime
        self.testCase = testCase

        self.port = port
        self.serial = serial.Serial(port=port, baudrate=115200, bytesize=8, timeout=1, stopbits=serial.STOPBITS_ONE)
        self.name = name
        self.linesLimit = 200
        self._setUp()

        self._read_data_in_background()

    def _setUp(self):
        self.ready = False
        self.lines = []

    def reset(self):
        self._setUp()
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

    def write(self, data):
        logger.debug('%0.4f: %s %5s: %s' % (time.time()-self.startTime, '>', self.name, data))
        self.serial.write(data)

    def _read_data_in_background(self):
        self.running = True
        def _thread_main(self):
            while self.running:
                data = self.serial.readline()
                if data:
                    self.lines.append(data)
                    logger.debug('%0.4f: %s %5s: %s' % (time.time()-self.startTime, '<', self.name, data))
                    self.testCase.assertLessEqual(len(self.lines), self.linesLimit, f'Too much lines from UART, limit is {self.linesLimit}')
            self.serial.close()

        self._thread=Thread(target=_thread_main,args=(self,))
        self._thread.start()



class SimpleTransmissionTest(unittest.TestCase):
    def setUp(self):
        self.startTime = time.time()

        self.big = MCU('big', BIG_UART_DEVICE, self.startTime, self)
        self.big.reset()

        self.small = MCU('small', SMALL_UART_DEVICE, self.startTime, self)
        self.small.reset()

        self.big.wait_for_string(b'Ready')
        self.small.wait_for_string(b'Ready')

        return super().setUp()

    def tearDown(self):
        self.small.close()
        self.big.close()
        return super().tearDown()

    def test_simply_pass_string_over_spi(self):
        self.big.write(b"QWERTY\r\n")
        self.assertIsNotNone(self.small.wait_for_string(b"SPI: < QWERTY"), 'MCU should print QWERTY')


if __name__ == '__main__':
    unittest.main()
