#!/usr/bin/env python3

from os import getenv
from enum import Enum, unique
from dotenv import load_dotenv

# Locked commands - cannot use them
LOCKED_COMMANDS = ['genesis', 'singularity']
# LOCKED_COMMANDS = []

# Load environment variables from env file
load_dotenv()
TOKEN = getenv('DISCORD_TOKEN')


@unique
class Color(Enum):
    """Enum used to define colors used by the bot."""
    RED = 0xFF0000
    GREEN = 0x00FF00
    CYAN = 0x008B8B


QUOTES = (
    "Don't hate me, hate that code."
    "\n\t― Vedant Access",
    "Time is what determines security. With enough time nothing is unhackable."
    "\n\t― Aniekee Tochukwu Ezekiel",
    "Why should I apologize for being a HACKER? Has anyone ever apologized "
    "for turning me into one?\n\t― Harsh Mohan",
    "No technology that's connected to the internet is unhackable."
    "\n\t― Abhijit Naskar, The Gospel of Technology"
)

BANNER = """
                ._,.
           "..-..pf.
          -L   ..#'
        .+_L  ."]#
        ,'j' .+.j`                 -'.__..,.,p.
       _~ #..<..0.                 .J-.``..._f.
      .7..#_.. _f.                .....-..,`4'
      ;` ,#j.  T'      ..         ..J....,'.j`
     .` .."^.,-0.,,,,yMMMMM,.    ,-.J...+`.j@
    .'.`...' .yMMMMM0M@^=`""g.. .'..J..".'.jH
    j' .'1`  q'^)@@#"^".`"='BNg_...,]_)'...0-
   .T ...I. j"    .'..+,_.'3#MMM0MggCBf....F.
   j/.+'.{..+       `^~'-^~~\"\"\"\"'\"\"\"?'"``'1`
   .... .y.}                  `.._-:`_...jf
   g-.  .Lg'                 ..,..'-....,'.
  .'.   .Y^                  .....',].._f
  ......-f.                 .-,,.,.-:--&`
                            .`...'..`_J`
                            .~......'#'
                            '..,,.,_]`     The bot is ready.
                            .L..`..``.
"""
