#!/usr/bin/env python3

import os
import json
import time
import socket
import threading
from . import config
from uuid import uuid4
from .logging import LOGGER
from rich.table import Table
from .CustomAES import CustomAES
from base64 import b64encode, b64decode
from .GenericMultiThreadedServer import GenericMultiThreadedServer


class PeerNode(GenericMultiThreadedServer):
    """Class which describes a simple node."""
    def __init__(self, big_k: str, connection: tuple) -> None:
        """Initialization.

        :param big_k: the globally shared key, known by all nodes and the KM.
        :param iv (optional): the Initialization Vector (IV).
        """
        self.iv = config.AES_INITIALIZATION_VECTOR
        self.big_k = big_k
        self.connection = connection
        # Generate an unique UUID v4
        self.uuid = uuid4()

        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        try:
            self.sock.connect(self.connection)
        except ConnectionRefusedError:
            LOGGER.critical(
                "Unable to reach the Key Manager Server Node. Aborting...")
            exit(-1)

        data = {
            "key": self.big_k,
            "handler": self.__handler
        }

        super().__init__(data)

    def __send_response(self, data: dict, where: socket.socket) -> None:
        response = {
            "uuid": str(self.uuid)
        }
        response.update(data)

        where.sendall(json.dumps(response).encode("utf-8"))

    def __handler(self, connection) -> None:
        """Code to be executed for each node that connects."""
        data_node_a = json.loads(
            connection.recv(config.MAX_PACKET_SIZE).decode('utf-8')
        )
        config.beautify(data_node_a)

        self.__send_response(
            {
                "mode": data_node_a["mode"],
                "request": config.REQUEST_FIRST_KEY if
                data_node_a["mode"] == config.MODE_ECB else
                config.REQUEST_SECOND_KEY
            }, self.sock)

        LOGGER.info("Waiting for the key from Key Manager...")
        if data := json.loads(
                self.sock.recv(config.MAX_PACKET_SIZE).decode("utf-8")):
            config.beautify(data)

        key_master_aes = CustomAES(self.big_k, config.MODE_ECB)
        key = key_master_aes.decrypt(
            bytearray(b64decode(data["key"]))
        ).decode("utf-8")
        LOGGER.info(f"Decoded key: '{key}'")

        LOGGER.info(f"Sending acknowledgement message to "
                    f"{data_node_a['uuid']}...")
        self.__send_response(
            {
                "response": config.RESPONSE_SYN_ACK
            }, connection)

        node_aes = None
        if data_node_a["mode"] == config.MODE_ECB:
            node_aes = CustomAES(key, config.MODE_ECB)
        elif data_node_a["mode"] == config.MODE_XXX:
            node_aes = CustomAES(key, config.MODE_XXX, self.iv)

        LOGGER.info(f"Waiting for transmission from {data_node_a['uuid']}...")
        full_data = []
        while data := connection.recv(config.MAX_PACKET_SIZE).decode("utf-8"):
            full_data.append(data)

        full_data = json.loads(''.join(full_data))
        config.beautify(full_data)

        LOGGER.info(f"Saving '{full_data['filename']}_decrypted' on disk...")
        with open(f"{full_data['filename']}_decrypted", "wb") as f:
            f.write(node_aes.decrypt(bytearray(
                b64decode(full_data['content'])))
            )

        LOGGER.info("Bye!")
        os._exit(0)

    def __start_peer_node_listener(self) -> None:
        """Wrapper over GenericMultiThreadedServer.start_server() method."""
        super().start_server("Beep, boop! Peer Node launched on the orbit!")

    def menu(self):
        table = Table(show_header=True, header_style="bold magenta")
        table.add_column("Option Number", style="dim", width=12,
                         justify="right")
        table.add_column("Node Type", justify="right")
        table.add_row(
            "1",
            "Node A"
        )
        table.add_row(
            "2",
            "Node B"
        )

        config.console.print(table)
        LOGGER.info(f"Which node do you want to be?")
        node_num = input(">>> ")

        while node_num not in ("1", "2"):
            LOGGER.warn("Choose between 1 or 2!")
            LOGGER.info(f"Which node do you want to be?")
            node_num = input(">>> ")
        LOGGER.info(f"You are now the Node {'A' if node_num == '1' else 'B'}!")
        
        if node_num == "1":
            LOGGER.warn("Make sure that you type the following answers "
                        "correctly. No crying afterwards!")
            LOGGER.info("Enter Node B's IP address [default localhost]:")
            ip = input(">>> ")
            ip = "127.0.0.1" if ip else ip
            LOGGER.info("Enter Node B's port number:")
            port = input(">>> ")
    
            while True:
                try:
                    port = int(port)
                    break
                except ValueError:
                    LOGGER.error(f"Does '{port}' look like a number to you?")
                    LOGGER.info("Enter Node B's port number:")
                    port = input(">>> ")
    
            LOGGER.info(f"Which encryption mode do you want to use? "
                        f"[ECB or CTR]")
            mode = input(">>> ").upper()
            
            while mode not in ("ECB", "CTR"):
                LOGGER.warn("Choose between ECB or CTR!")
                LOGGER.info(
                    f"Which encryption mode do you want to use? [ECB or CTR]")
                mode = input(">>> ").upper()

            # Create the socket to communicate with Node B
            node_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            try:
                node_sock.connect((ip, port))
            except ConnectionRefusedError:
                LOGGER.error(
                    "Could not reach node. I told you no crying afterwards.")

            LOGGER.info(f"Informing Node B that we are using AES 128 bit in "
                        f"{mode} mode...")
            self.__send_response(
                {
                    "mode": mode
                }, node_sock
            )

            LOGGER.info(f"Informing the Key Manager that we are using AES 128 "
                        f"bit in {mode} mode and request the corresponding "
                        f"key...")
            self.__send_response(
                {
                    "mode": mode,
                    "request": config.REQUEST_FIRST_KEY if
                    mode == config.MODE_ECB else config.REQUEST_SECOND_KEY
                }, self.sock)

            LOGGER.info("Waiting for the key from Key Manager...")
            if data := json.loads(
                    self.sock.recv(config.MAX_PACKET_SIZE).decode("utf-8")):
                config.beautify(data)

            key_master_aes = CustomAES(self.big_k, config.MODE_ECB)
            key = key_master_aes.decrypt(
                bytearray(b64decode(data["key"]))
            ).decode("utf-8")
            LOGGER.info(f"Decoded key: '{key}'")

            if data := json.loads(
                    node_sock.recv(config.MAX_PACKET_SIZE).decode("utf-8")):
                config.beautify(data)

            node_aes = None
            if mode == config.MODE_ECB:
                node_aes = CustomAES(key, config.MODE_ECB)
            elif mode == config.MODE_XXX:
                node_aes = CustomAES(key, config.MODE_XXX, self.iv)

            # Send an encrypted file to Node B
            with open("sample3.txt") as f:
                self.__send_response(
                    {
                        "type": config.TYPE_RESPONSE,
                        "response": config.RESPONSE_SEND_FILE,
                        "filename": "sample3.txt",
                        "content": b64encode(
                            node_aes.encrypt(f.read())
                        ).decode("utf-8")
                    }, node_sock
                )

            LOGGER.info("Bye!")
            os._exit(0)
        else:
            LOGGER.info("Waiting for Node A's message...")

    def run(self):
        """Wrapper over internal method needed to run the peer node."""
        LOGGER.info(f"Preparing for launch...")
        LOGGER.info(f"Assigned UUID '{self.uuid}' to this node!")

        t1 = threading.Thread(target=self.__start_peer_node_listener)
        t2 = threading.Thread(target=self.menu)

        t1.start()
        time.sleep(0.01)
        t2.start()

        t1.join()
        t2.join()
