#!/usr/bin/env python3

import os
import argparse
from node import config
from node.logging import LOGGER
from node.PeerNode import PeerNode
from node.KeyManager import KeyManager

########################################################
SHARED_KEY_ENV_VAR = None
########################################################


def get_secrets(mode: str):
    """Get the secrets stored in environment variables."""
    keys = []

    if not (shared_key := os.getenv(config.SHARED_KEY_ENV_VAR)):
        LOGGER.error("Shared key environment variable not set. Please set "
                     "it and run again.")
        LOGGER.info(f"The name of the environment variable is: "
                    f"{config.SHARED_KEY_ENV_VAR}")
        exit(-1)
    keys.append(shared_key)

    if mode == config.NODE_MODE_KEY_SERVER:
        if not (key_1 := os.getenv(config.KEY_SERVER_FIRST_KEY_ENV_VAR)):
            LOGGER.error("Shared key environment variable not set. Please set "
                         "it and run again.")
            LOGGER.info(f"The name of the environment variable is: "
                        f"{config.KEY_SERVER_FIRST_KEY_ENV_VAR}")
            exit(-1)

        if not (key_2 := os.getenv(config.KEY_SERVER_SECOND_KEY_ENV_VAR)):
            LOGGER.error("Shared key environment variable not set. Please set "
                         "it and run again.")
            LOGGER.info(f"The name of the environment variable is: "
                        f"{config.KEY_SERVER_SECOND_KEY_ENV_VAR}")
            exit(-1)
        keys.extend([key_1, key_2])
    return tuple(keys)


def run_peer(shared_key: str, connection: tuple) -> None:
    """Run a Peer Node.

    :param shared_key: A global key known by all participants.
    :param connection: A tuple containing the IP address and the port of the
                        Key Manager.
    """
    peer_node = PeerNode(shared_key, connection)
    peer_node.run()


def run_server(shared_key: tuple) -> None:
    """Run a Key Manager Server Node.

    :param shared_key: A global key known by all participants.
    """
    key_manager = KeyManager(shared_key)
    key_manager.run()


def main():
    """The main driver of the program."""
    parser = argparse.ArgumentParser(description="Cheap TLS replica by "
                                                 "Ionuț Roșca")
    parser.add_argument("-m",
                        "--mode",
                        dest="mode",
                        choices=[config.NODE_MODE_PEER_NODE,
                                 config.NODE_MODE_KEY_SERVER],
                        help="The mode in which this node will be run")
    parser.add_argument("--server-ip",
                        type=str,
                        dest="server_ip",
                        help="IP address of the other peer node")
    parser.add_argument("--server-port",
                        type=int,
                        dest="server_port",
                        help="Port of the other peer node")
    args = parser.parse_args()

    if not args.mode:
        LOGGER.critical("You must decide whether this node will be a Key "
                        "Manager or a Peer Node.")
        parser.print_usage()
        exit(-1)
    elif args.mode == config.NODE_MODE_PEER_NODE and \
            not (args.server_ip and args.server_port):
        LOGGER.critical("All required IPs and ports must be specified.")
        parser.print_usage()
        exit(-1)

    keys = get_secrets(args.mode)

    # I bet you weren't expecting this
    config.banner()

    if args.mode == config.NODE_MODE_KEY_SERVER:
        run_server(keys)
    if args.mode == config.NODE_MODE_PEER_NODE:
        run_peer(keys[0], (args.server_ip, args.server_port))


if __name__ == '__main__':
    try:
        main()
    except KeyboardInterrupt:
        exit(0)
