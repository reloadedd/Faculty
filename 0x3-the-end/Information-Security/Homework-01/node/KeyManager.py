#!/usr/bin/env python3

import json
from . import config
from uuid import uuid4
from .logging import LOGGER
from base64 import b64encode
from .CustomAES import CustomAES
from .GenericMultiThreadedServer import GenericMultiThreadedServer


class KeyManager(GenericMultiThreadedServer):
    """Class which describes the Key Manager node."""

    def __init__(self, keys: tuple) -> None:
        """Initialization.

        :param keys: a tuple containing all three keys.
        """
        self.big_k = keys[0]
        self.key_1 = keys[1]
        self.key_2 = keys[2]
        self.iv = config.AES_INITIALIZATION_VECTOR

        # Generate an unique UUID v4
        self.uuid = uuid4()

        data = {
            "key": self.big_k,
            "handler": self.__handler
        }

        super().__init__(data)

    def __handler(self, connection) -> None:
        """Code to be executed for each node that connects."""
        data = json.loads(
            connection.recv(config.MAX_PACKET_SIZE).decode('utf-8')
        )
        config.beautify(data)

        # Create the encryptor
        # Simply encrypt a single block using ECB - it's pure AES 'cause we
        # don't encrypt multiple blocks. Eventually we will encrypt another
        # block full of padding if the key size is 128 bits.
        aes = CustomAES(self.big_k, config.MODE_ECB)
        key = b""
        if data["mode"] == config.MODE_ECB:
            key = aes.encrypt(self.key_1)
        elif data["mode"] == config.MODE_XXX:
            key = aes.encrypt(self.key_2)

        data = {
            "uuid": str(self.uuid),
            "mode": data["mode"],
            "key": b64encode(key).decode("utf-8")
        }

        connection.sendall(json.dumps(data).encode("utf-8"))

    def __start_key_manager_server(self) -> None:
        """Wrapper over GenericMultiThreadedServer.start_server() method."""
        super().start_server("The Key Manager server is ready for takeoff!")

    def run(self):
        """Wrapper over internal method needed to run the server node."""
        LOGGER.info(f"Preparing for launch...")
        LOGGER.info(f"Assigned UUID '{self.uuid}' to this node!")

        self.__start_key_manager_server()
