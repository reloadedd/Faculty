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
    "hacker nobody knows you.\n\t― Rishabh Surya",

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

HOF_LOGO = 'https://hackers-of-fii.s3.us-east-1.amazonaws.com/HoF_grayscale.' \
           'png?response-content-disposition=inline&X-Amz-Security-Token=' \
           'IQoJb3JpZ2luX2VjEPP%2F%2F%2F%2F%2F%2F%2F%2F%2F%2FwEaDGV1LWNlbnRy' \
           'YWwtMSJHMEUCIQD%2FH6i%2FZD1QeXVLKhddqu00PgWZGOY0EWoV8uVUyu%2Fxng' \
           'IgOBFMa9PIwCnOCHtBlJ28%2Fk2pf79RZjjfGBIsTp1QINUqlgMIvP%2F%2F%2F' \
           '%2F%2F%2F%2F%2F%2F%2FARACGgwzMTY0MTIwNTEzNzAiDAYN4Zx3UOmwpGjjFS' \
           'rqAieAJ6lbQ9b%2FoEHbS4mfIESt8K%2BFKnQXTm93175g2Q3ytFhpp9xw18osr' \
           'mGSPSLFddJzIEMtHuLllqnSE4igFbWRXHPAFSd%2BKS%2B6EHSWM%2BMPs%2BKAv' \
           'W3Tx7kQAC07NuuCw8Vep3iP%2FT%2Fjw6ofya6q9KUHF24edQxYP1Eo1z%2B4b6T' \
           'gD3KJQLePiVIRQyLT%2BFgfCR23Hm8A8EqG3Lx8Vhogq6hdF6%2FAa7nPKXvj%2B' \
           'vYMQpqUktA%2FZVrYSt7rlMog9E4YVPocjnSyugUE7jb%2Fbcoy3xdd4aBRC0TFF' \
           'diSxYzVpOwGjmmYF8L79mveaRIjXh9xgKGXpCvx7JYIOX0iFZULtmcyXukQHV6xF' \
           'h9xNqjIGWn4wb0UBG0o0syAkG8Kv1Ksmx7eN%2FnzEvp%2Bnr6exwQE3U95FXONj' \
           '4uea0WiWccz5AqpX0f3TCJLSmAhDkvKOy2vkfaAcBbShorpNjm3l4dl7CA59eDZ1' \
           '06kinduMJnAjY0GOrMCyige4CeaWigwU4wCpeCE2%2ByT3CO2YTfkXEw6am5njFb' \
           'F3G7NEzXT1v0eaGLDq%2B%2F4MoIjmkjpfDBzKZxwM9hEFyeZFZZuDDBTE39ClTV' \
           '27Kxg2UzvLbjvB9avLOO2qU9X9xVZV2W0LnIJTYMhgqpom9KvLcXxstNHeGaHwq' \
           'C%2BkZMPDbVxgG9rFbtIJBeLtUlPNkz6CUm%2BRw%2Fr1ipFJLsyrGHdS0qclwD' \
           'cPRdIMJzXX9TgQ5tG1cdKUIf4%2FxLAmWqZ31umv7OsGpMZ5lpqB5Fh8bxXwOvUj' \
           '05FcR9ASk92vWo4lOTkWnJ9MmYsEciVYH020hQK9QUpLvdFTX9luuzo5ByxJGANf' \
           'dIEa1UjL8rDvFHy2wVU%2BPqhfdN5189Dg%2BOEh%2FLZ0r0E0T0GwLe0KpfDbhG' \
           'M3A%3D%3D&X-Amz-Algorithm=AWS4-HMAC-SHA256&X-Amz-Date=20211128T' \
           '105745Z&X-Amz-SignedHeaders=host&X-Amz-Expires=300&X-Amz-Credent' \
           'ial=ASIAUTK5AX6VADOBGWGQ%2F20211128%2Fus-east-1%2Fs3%2Faws4_' \
           'request&X-Amz-Signature=451c9569193225bf5646e0bc665efc29098570d9' \
           '8a0df5936daa5c86fbece7c4'
