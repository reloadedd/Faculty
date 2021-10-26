#!/usr/bin/env python3

import socket
import threading
from . import config
from .logging import LOGGER


class GenericMultiThreadedServer:
    def __init__(self, data: dict) -> None:
        """Initialization."""
        self.big_k = data["key"]
        self.__handler_function = data["handler"]

        self.server_socket = None

    def __enable_processing_loop(self) -> None:
        thread_count = 0
        while True:
            client, address = self.server_socket.accept()
            LOGGER.info(f'{address[0]}:{address[1]} connected!\t'
                        f'[{thread_count}]')
            threading.Thread(target=self.__handler_function,
                             args=(client,)).start()
            thread_count += 1

    def start_server(self, welcome_message: str) -> None:
        """Setup the Key Manager server."""
        self.server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.server_socket.setsockopt(socket.SOL_SOCKET,
                                      socket.SO_REUSEADDR,
                                      1)

        try:
            self.server_socket.bind(
                (config.ALL_INTERFACES, 0))
        except socket.error as err:
            LOGGER.error(err)
            exit(-1)

        self.server_socket.listen(config.MAX_QUEUED_CONNECTIONS)

        LOGGER.info(f"Opening port {self.server_socket.getsockname()[1]} for "
                    f"communication..")
        LOGGER.info(welcome_message)
        self.__enable_processing_loop()
        self.server_socket.close()
