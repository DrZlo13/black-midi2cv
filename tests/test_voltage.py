#! /usr/bin/env python3

import matplotlib.pyplot as plt
import requests
import json
import logging
import colorlog
import sys

# import http.client
# http.client.HTTPConnection.debuglevel = 1

VIRTUAL_BENCH_NAME = "VB8012-30ADC18"
# SERVER_URL = "http://localhost:6123/"
SERVER_URL = "http://172.30.1.185:6123/"

"http://172.30.1.185:6123/api/v0/ping"

class App:
    def __init__(self):
        self.logger = logging.getLogger(__name__)

    def get_dmm_voltage(self, count=1, auto=True) -> float:
        """Get the voltage from the DMM."""
        url = f"{SERVER_URL}api/v0/dmm_voltage?bench_name={VIRTUAL_BENCH_NAME}&count={count}{ '&auto' if auto else ''}"
        response = requests.get(url)
        if response.status_code != 200:
            raise Exception(
                f"Failed to get DMM voltage. Status code: {response.status_code}"
            )

        measurements = response.json().get("data").get("measurements")
        measurements_len = len(measurements)
        measurements_sum = sum(measurements)
        measurements_avg = measurements_sum / measurements_len
        return measurements_avg

    def ping(self) -> bool:
        """Ping the server."""
        url = f"{SERVER_URL}api/v0/ping"
        response = requests.get(url)
        if response.status_code != 200:
            raise Exception(
                f"Failed to ping server. Status code: {response.status_code}"
            )

        self.logger.info(f"Server pinged successfully")

        return True

    def main(self):
        self.logger = logging.getLogger(__name__)
        log_level = logging.DEBUG
        self.logger.setLevel(log_level)

        if not self.logger.hasHandlers():
            handler = colorlog.StreamHandler(sys.stdout)
            handler.setLevel(log_level)
            formatter = colorlog.ColoredFormatter(
                "%(log_color)s%(asctime)s [%(levelname)s] %(message)s",
                log_colors={
                    "DEBUG": "cyan",
                    "INFO": "white",
                    "WARNING": "yellow",
                    "ERROR": "red",
                    "CRITICAL": "red,bg_white",
                },
            )
            handler.setFormatter(formatter)
            self.logger.addHandler(handler)

        requests_log = logging.getLogger("requests.packages.urllib3")
        requests_log.setLevel(logging.DEBUG)
        requests_log.propagate = True

        self.logger.info("Starting")

        one_volt = 1.0
        one_halftone = one_volt / 12
        one_semitone = one_halftone / 100

        self.logger.info(f"One volt: {one_volt} V")
        self.logger.info(f"One halftone: {one_halftone} V")
        self.logger.info(f"One semitone: {one_semitone} V")

        last_voltage = 0
        start_voltage = self.get_dmm_voltage(count=10)
        while True:
            try:
                voltage = self.get_dmm_voltage(count=10)
                diff = voltage - start_voltage
                diff_in_semitones = diff / one_semitone

                self.logger.info(
                    f"Voltage: {voltage}, diff: {diff}, diff_in_semitones: {diff_in_semitones}"
                )
                last_voltage = voltage
            except Exception as e:
                self.logger.error(f"Error: {e}")
            finally:
                pass


if __name__ == "__main__":
    app = App()
    app.main()
