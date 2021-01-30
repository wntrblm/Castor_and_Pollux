# Copyright (c) 2021 Alethea Katherine Flowers.
# Published under the standard MIT License.
# Full text available at: https://opensource.org/licenses/MIT

"""
Wrapper for talking to the Siglent SDS 1104X-E over VISA.

SCPI & Programming reference: https://storage.googleapis.com/files.winterbloom.com/docs/Programming%20Guide%20PG%2001%20E%2002%20C.pdf
"""

import time

import pyvisa.errors
from wintertools import log


class Oscilloscope:
    RESOURCE_NAME = "USB0::0xF4EC::0xEE38::SDSMMEBD3R6070::INSTR"
    TIMEOUT = 10 * 1000

    def __init__(self, resource_manager):
        self._connect(resource_manager)

    def _connect(self, resource_manager):
        try:
            resource = resource_manager.open_resource(self.RESOURCE_NAME)
        except pyvisa.errors.VisaIOError as exc:
            log.error("Couldn't connect to oscilloscope", exc=exc)
        resource.timeout = self.TIMEOUT
        self.port = resource
        # Don't send command headers in responses, just the result.
        self.port.write("chdr off")

    def close(self):
        self.port.close()

    def reset(self):
        self.port.write("*rst")
        # *opc? should block until the device is ready, but it doesn't, so just sleep.
        time.sleep(5)

    def enable_bandwidth_limit(self):
        self.port.write("BWL C1,ON,C2,ON,C3,ON,C4,ON")

    def enable_channel(self, channel: str):
        self.port.write(f"{channel}:trace on")

    def disable_channel(self, channel: str):
        self.port.write(f"{channel}:trace off")

    def set_vertical_division(self, channel: str, volts: float):
        self.port.write(f"{channel}:vdiv {volts}")

    def set_vertical_offset(self, channel: str, volts: float):
        self.port.write(f"{channel}:ofst {volts}")

    def set_time_division(self, value: str):
        self.port.write(f"tdiv {value}")

    def enable_cursors(self):
        self.port.write("cursor_measure manual")

    def set_vertical_cursor(self, trace: str, ref: float, dif: float):
        self.port.write(f"{trace}:cursor_set VREF,{ref}V,VDIF,{dif}V")

    def get_frequency(self):
        self.port.write("cymometer?")
        output = self.port.read_raw().decode("utf-8")
        return float(output)

    def get_peak_to_peak(self, trace: str):
        self.port.write(f"{trace}:parameter_value? PKPK")
        try:
            return float(self.port.read_raw().decode("utf-8").split(",")[-1])
        except ValueError:
            return 0

    def set_trigger_level(self, trig_source: str, trig_level: float):
        self.port.write(f"{trig_source}:trig_level {trig_level}V")
