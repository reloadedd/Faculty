#!/usr/bin/env python3

import discord
from discord.utils import get
from discord.ext import commands

# Local imports
from bot.constants import Color, TOKEN, BANNER
from bot.helpers import attach_embed_info_and_send, get_roles,\
    check_if_locked_up, get_assign_help_menu
from bot.RevampedHelpMenu import RevampedHelpMenu
from keep_alive import keep_alive

intents = discord.Intents.all()
BOT = commands.Bot(command_prefix='>', intents=intents)
BOT.help_command = RevampedHelpMenu()


@BOT.event
async def on_ready():
    print(BANNER)


@BOT.command(name='genesis', help='Create all roles - Authorized access only.')
@commands.has_permissions(manage_roles=True)
@check_if_locked_up
async def create_all_roles(ctx: commands.Context):
    """Create all roles."""
    roles, colored_roles = get_roles()

    await attach_embed_info_and_send(ctx,
                                     "Working on it...",
                                     Color.GREEN.value,
                                     delete_after=60)

    guild = ctx.guild
    for role in roles:
        await guild.create_role(name=role, reason='Genesis')

    for role in colored_roles:
        await guild.create_role(name=role[0],
                                color=role[1],
                                hoist=True,
                                mentionable=True,
                                reason='Genesis')

    colored_roles = [role[0] for role in colored_roles]
    await attach_embed_info_and_send(ctx,
                                     f'Roles: {roles + colored_roles}',
                                     Color.CYAN.value)


@BOT.command(name='singularity', help="Remove all roles created at genesis - "
                                      "Authorized access only.")
@commands.has_permissions(manage_roles=True)
@check_if_locked_up
async def activate_singularity(ctx):
    """Remove all roles created at genesis."""
    roles = get_roles()[0] + [role_name[0] for role_name in get_roles()[1]]
    guild = ctx.guild
    success = True

    await attach_embed_info_and_send(ctx,
                                     "Working on it...",
                                     Color.GREEN.value,
                                     delete_after=60)

    for role_name in roles:
        role = discord.utils.get(guild.roles, name=role_name)
        if role:
            try:
                await role.delete()
            except discord.Forbidden:
                success = False
        else:
            success = False

    if success:
        await attach_embed_info_and_send(ctx,
                                         "All roles have been destroyed.",
                                         Color.GREEN.value)
    else:
        await attach_embed_info_and_send(ctx,
                                         "Unable to destroy all roles.",
                                         Color.RED.value)


@BOT.command(name='assign', help=get_assign_help_menu())
@check_if_locked_up
async def assign(ctx: commands.Context, *args):
    """Assign role(s) to the calling user.

    Note: Roles are overridden if called multiple times.
    """
    if len(args) != 1:
        message = f'Incorrect format. Example usage: ** ' \
                  f'{BOT.command_prefix}assign 3B5.**'

        await attach_embed_info_and_send(ctx, message, Color.RED.value)
    elif get(ctx.guild.roles, name=args[0].upper()):
        member = ctx.author
        group = discord.utils.get(ctx.guild.roles, name=args[0].upper())
        year = None

        if args[0].upper() == 'TEACHER':
            message = "Sorry, an admin has to manually assign a " \
                      "**TEACHER** role."
            await attach_embed_info_and_send(ctx,
                                             message,
                                             Color.RED.value)
            return

        if args[0].upper() not in ('ALUMNI', 'PHD', 'TEACHER'):
            colored_role = ''
            if args[0][0] in ('1', '2', '3'):
                if len(args[0]) == 3:
                    if args[0][0] == '1':
                        colored_role = '1ST YEAR'
                    if args[0][0] == '2':
                        colored_role = '2ND YEAR'
                    if args[0][0] == '3':
                        colored_role = '3RD YEAR'
                else:
                    if args[0][0] == '1':
                        colored_role = '1ST MASTER'
                    if args[0][0] == '2':
                        colored_role = '2ND MASTER'

            year = discord.utils.get(ctx.guild.roles, name=colored_role)

        # Remove pre-existing roles (overwrite)
        everyone = discord.utils.get(ctx.guild.roles, name='@everyone')
        roles = member.roles.copy()
        roles.remove(everyone)
        for role in roles:
            await member.remove_roles(role)

        # Add year and group roles
        if year is None:
            await member.add_roles(group)
        else:
            await member.add_roles(year, group)

        message = f'Successfully assigned role **{args[0].upper()}** to ' \
                  f'<@{ctx.author.id}>.'
        await attach_embed_info_and_send(ctx, message, color=Color.GREEN.value)
    else:
        message = f'Are you sure that **{args[0]}** is a valid role?'
        await attach_embed_info_and_send(ctx, message, Color.RED.value)


@BOT.command(name='create')
async def create(ctx: commands.Context, *args):
    """Create channel for a CTF event."""
    pass


@BOT.command(name='join', help='Join to the specified CTF channel.')
async def join(ctx: commands.Context):
    """Join a the specified CTF channel."""
    pass


# Start a webserver in another thread in order to keep the bot alive
keep_alive()

# Let's rock
BOT.run(TOKEN)
