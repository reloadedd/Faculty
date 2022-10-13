#!/usr/bin/env python3

import re
import random
import discord
from functools import wraps
from urllib import parse, request

# Local imports
from .constants import Color, QUOTES, LOCKED_COMMANDS, HOF_LOGO


#######
# --=== Helper Functions ===--
#######
async def attach_embed_info_and_send(interaction: discord.Interaction,
                                     message: str = None,
                                     color: Color = None,
                                     **kwargs):
    _pop = lambda arg: kwargs.pop(arg) if kwargs.get(arg) else None
    # Parsing custom args
    custom_args = {
        'IS_DEFERRED': _pop('arg__is_deferred')
    }

    embed = discord.Embed(description=message, colour=color)
    embed.set_thumbnail(url=f'{HOF_LOGO}')
    embed.set_footer(text=random.choice(QUOTES))

    if not custom_args['IS_DEFERRED']:
        await interaction.response.send_message(embed=embed, **kwargs)
    else:
        await interaction.edit_original_response(embed=embed, **kwargs)


def get_roles():
    groups = [
        f'{letter}{number}' for letter in (
            'A', 'B', 'E') for number in range(1, 7)
    ]
    roles = [f'{year}{group}' for year in range(1, 4) for group in groups]

    roles.extend(
        [
            f'{year}{master}' for year in range(1, 3) for master in
            ('MSAII', 'MISS', 'MOC', 'MLC', 'MSI', 'MSD')
        ])

    colored = [
        ('1ST YEAR', discord.Colour(value=0x1abc9c)),
        ('2ND YEAR', discord.Colour(value=0x32cd32)),
        ('3RD YEAR', discord.Colour(value=0x6a5aef)),
        ('1ST MASTER', discord.Colour(value=0xffd700)),
        ('2ND MASTER', discord.Colour(value=0x00ffff)),
        ('PHD', discord.Colour(value=0xff9400)),
        ('ALUMNI', discord.Colour(value=0xff0000)),
        ('TEACHER', discord.Colour(value=0xa84300))
    ]

    return roles, colored


def find_youtube_video(search_query):
    query_string = parse.urlencode({'search_query': search_query})
    html_content = request.urlopen(
        f'https://www.youtube.com/results?{query_string}'
    )
    search_content = html_content.read().decode()
    search_results = re.findall(r'/watch\?v=[a-zA-Z0-9_-]{11}', search_content)
    return f'https://www.youtube.com{search_results[0]}'


#######
# --=== Decorators ===--
#######
def check_if_locked_up(func):
    @wraps(func)
    async def wrapper(*args):
        ctx = args[0]
        if ctx.command.name in LOCKED_COMMANDS:
            message = f'Umm...nope! That command is locked up.'
            await attach_embed_info_and_send(ctx,
                                             message,
                                             color=Color.RED.value)
        else:
            await func(*args)

    return wrapper
