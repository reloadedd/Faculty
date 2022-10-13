#!/usr/bin/env python3

from os import getenv
from enum import Enum, unique
from dotenv import load_dotenv

# Locked commands - cannot use them
# LOCKED_COMMANDS = ['genesis', 'singularity']
LOCKED_COMMANDS = []

# Load environment variables from env file
load_dotenv()
TOKEN = getenv('DISCORD_TOKEN')


@unique
class Color(Enum):
    """Enum used to define colors used by the bot."""
    RED = 0xFF0000
    GREEN = 0x00FF00
    CYAN = 0x008B8B


# Credits: https://www.goodreads.com/quotes/tag/hacking
QUOTES = (
    "Don't hate me, hate that code."
    "\n\t― Vedant Access",
    "Time is what determines security. With enough time nothing is unhackable."
    "\n\t― Aniekee Tochukwu Ezekiel",
    "Why should I apologize for being a HACKER? Has anyone ever apologized "
    "for turning me into one?\n\t― Harsh Mohan",
    "No technology that's connected to the internet is unhackable."
    "\n\t― Abhijit Naskar, The Gospel of Technology",
    "You can't hack your destiny, brute force...you need a back door, a side "
    "channel into Life.\n\t― Clyde Dsouza",
    "If you are a good hacker everybody knows you, But if you are a great "
    "hacker nobody knows you.\n\t― Rishabh Surya"
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
                            '..,,.,_]`     [bold red]INFO[/bold red]\t\
[cyan]The bot is ready. Phew, phew![/cyan]
                            .L..`..``.
"""

HOF_LOGO = 'https://hackers-of-fii.s3.amazonaws.com/HoF_grayscale.png'
JOIN_GIF_URL = 'https://hackers-of-fii.s3.amazonaws.com/bot_join.gif'

ENCODED_RESPONSES_PATH = './bot/messages.txt.encoded'
SURELY_NOT_RANDOM_RESPONSES_TO_MESSAGES = dict()

_lines = [
    line.strip() for line in open(ENCODED_RESPONSES_PATH, 'r').readlines()
]
for i in range(0, len(_lines), 2):
    SURELY_NOT_RANDOM_RESPONSES_TO_MESSAGES[_lines[i]] = _lines[i + 1]
