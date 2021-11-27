#!/usr/bin/env python3

import json
import socket
from .logging import LOGGER
from rich.console import Console

# Global console used for nicely printing
console = Console()

# Disabled: The port on which the KM listens on
# KEY_MANAGER_PORT = 1337

# The maximum number of clients waiting
MAX_QUEUED_CONNECTIONS = 10

# Bind all network interfaces available
ALL_INTERFACES = ""

# ECB mode of operation
MODE_ECB = "ECB"

# CTR mode of operation
MODE_XXX = "CTR"

# Available node modes
NODE_MODE_KEY_SERVER = "server"
NODE_MODE_PEER_NODE = "peer"

# Secret environment variables
SHARED_KEY_ENV_VAR = "Moy4TUu7lR4KU475P7OuwRjBbP3Kkz"
KEY_SERVER_FIRST_KEY_ENV_VAR = "te5ZHIfoAz0vtw7YHgDAEBwea9dIh2"
KEY_SERVER_SECOND_KEY_ENV_VAR = "Eq6foZUj2GuAryGdzBLxTCbbd5mBup"

# Maximum packet size in bytes
MAX_PACKET_SIZE = 1024  # 1 KiB

# Types
TYPE_RESPONSE = "RESPONSE"
TYPE_REQUEST = "REQUEST"

# Request types
REQUEST_FIRST_KEY = "REQ_KEY_1"
REQUEST_SECOND_KEY = "REQ_KEY_2"

# Response types
RESPONSE_SYN_ACK = "NODE_B_SYN_ACK"
RESPONSE_SEND_FILE = "RES_SEND_FILE"

# Block size (in bytes i.e 1 byte = 8 bits)
AES_BLOCK_SIZE = 16

# The initialization vector
# AES_INITIALIZATION_VECTOR = bytearray(uuid.uuid4().hex[:8], encoding='utf-8')
AES_INITIALIZATION_VECTOR = bytearray(b'91433ba9')


def banner():
    """Display the banner.

    C'mon, life is too short to use boring text, let's spice it up!
    """
    # Credits: http://www.ascii-art.de/ascii/pqr/rockets.txt
    print(r"""
                                                   ,:
                                                 ,' |
                                                /   :
                                             --'   /
                                             \/ />/
                                             / <//_\
                                          __/   /
                                          )'-. /
                                          ./  :\
                                           /.' '
                                         '/'
                                         +
                                        '
                                      `.
                                  .-"-
                                 (    |
                              . .-'  '.
                             ( (.   )8:
                         .'    / (_  )
                          _. :(.   )8P  `
                      .  (  `-' (  `.   .
                       .  :  (   .a8a)
                      /_`( "a `a. )"'
                  (  (/  .  ' )=='
                 (   (    )  .8"   +
                   (`'8a.( _(   (
                ..-. `8P    ) `  )  +
              -'   (      -ab:  )
            '    _  `    (8P"Ya
          _(    (    )b  -`.  ) +
         ( 8)  ( _.aP" _a   \( \   *
       +  )/    (8P   (88    )  )
          (a:f   "     `"       `
    """)


def beautify(data: dict):
    """Print JSON data beautifully."""
    LOGGER.info("Received transmission:")
    console.print(json.dumps(data, indent=4))


def send_to_node(connection: tuple, data: dict) -> None:
    """Establish a connection and send data to a node."""
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    try:
        sock.connect(connection)
        sock.sendall(json.dumps(data).encode("utf-8"))
    except ConnectionRefusedError:
        LOGGER.error("Could not reach node. I told you no crying afterwards.")
    else:
        LOGGER.info("Transmission completed successfully.")


def receive_from_node(connection: tuple) -> dict:
    """Establish a connection and send data to a node."""
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    data = {}
    try:
        sock.connect(connection)
        data = json.loads(sock.recv(MAX_PACKET_SIZE).decode("utf-8"))
    except ConnectionRefusedError:
        LOGGER.error("Could not reach node. I told you no crying afterwards.")
    else:
        LOGGER.info("Transmission completed successfully.")

    return data
