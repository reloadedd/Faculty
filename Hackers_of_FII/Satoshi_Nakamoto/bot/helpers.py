#!/usr/bin/env python3

import random
import discord
from functools import wraps
from discord.ext import commands

# Local imports
from .constants import Color, QUOTES, LOCKED_COMMANDS, HOF_LOGO


#######
# --=== Helper Functions ===--
#######
async def attach_embed_info_and_send(ctx: commands.Context = None,
                                     message: str = None,
                                     color: Color = None,
                                     **kwargs):
    embed = discord.Embed(description=message, colour=color)
    embed.set_thumbnail(url=f'{HOF_LOGO}')
    embed.set_footer(text=random.choice(QUOTES))
    # embed.set_footer(text="\tSatoshi Nakamoto, Hacker of FII", icon_url=url)

    await ctx.send(embed=embed, **kwargs)


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


def get_assign_help_menu():
    assign_help_menu = [
        'Assign the requested role to the user. Available roles:']
    roles, colored_roles = get_roles()
    colored_roles = [role[0] for role in colored_roles]
    for role in roles + colored_roles:
        if not ('YEAR' in role or 'MASTER' in role or 'TEACHER' in role):
            assign_help_menu.append(f'\n\t- {role}')
    return ''.join(assign_help_menu)


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
