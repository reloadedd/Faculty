#!/usr/bin/env python3
########################################################
# The following file has been taken from official      #
# `rich` module documentation.                         #
########################################################

import logging
from rich.logging import RichHandler

FORMAT = "%(message)s"
logging.basicConfig(
    level="NOTSET", format=FORMAT, datefmt="[%X]", handlers=[RichHandler()]
)

LOGGER = logging.getLogger("rich")
