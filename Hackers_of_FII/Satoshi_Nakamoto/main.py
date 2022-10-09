#!/usr/bin/env python3
import asyncio
import discord
from discord.ext import commands

# Local imports
from bot.constants import Color, TOKEN
from bot.helpers import attach_embed_info_and_send, get_roles,\
    check_if_locked_up
from keep_alive import keep_alive
from bot.slash_client import SlashClient
from bot.dropdown import DropdownView
from bot import pprint

BOT = SlashClient()


@BOT.tree.command(name='assign')
async def assign(interaction):
    """Sends a message with our dropdown containing colours"""

    # Create the view containing our dropdown
    view = DropdownView()

    # Sending a message containing our view
    await interaction.response.send_message(
        'Choose your academic actual status:', view=view, ephemeral=True
    )


@BOT.tree.command(name='genesis',
                  description='Create all roles - Authorized access only.')
@commands.has_permissions(manage_roles=True)
@check_if_locked_up
async def create_all_roles(interaction: discord.Interaction):
    """Create all roles."""
    roles, colored_roles = get_roles()

    await attach_embed_info_and_send(interaction,
                                     'Working on it...',
                                     Color.GREEN.value,
                                     delete_after=60)

    guild = interaction.guild
    for role in roles:
        await guild.create_role(name=role, reason='Genesis')

    for role in colored_roles:
        await guild.create_role(name=role[0],
                                color=role[1],
                                hoist=True,
                                mentionable=True,
                                reason='Genesis')

    colored_roles = [role[0] for role in colored_roles]
    await attach_embed_info_and_send(interaction,
                                     f'Roles: {roles + colored_roles}',
                                     Color.CYAN.value)


@BOT.tree.command(name='singularity',
                  description='Remove all roles created at genesis - '
                              'Authorized access only.')
@commands.has_permissions(manage_roles=True)
@check_if_locked_up
async def activate_singularity(interaction: discord.Interaction):
    """Remove all roles created at genesis."""
    roles = get_roles()[0] + [role_name[0] for role_name in get_roles()[1]]
    guild = interaction.guild
    success = True

    await attach_embed_info_and_send(interaction,
                                     'Working on it...',
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
        await attach_embed_info_and_send(interaction,
                                         'All roles have been destroyed.',
                                         Color.GREEN.value)
    else:
        await attach_embed_info_and_send(interaction,
                                         'Unable to destroy all roles.',
                                         Color.RED.value)


@BOT.tree.command(name='create')
async def create(interaction: discord.Interaction, channel_name: str) -> None:
    """Create channel for a CTF event."""
    overwrites = {
        interaction.guild.default_role: discord.PermissionOverwrite(
            view_channel=False
        )
    }

    if not len(list(
        filter(lambda x: x.name == 'CTFs',
               interaction.guild.categories)
    )):
        pprint('[blue]INFO[/blue]\tThe category [bold]CTFs[/bold] has not been'
               ' created. Creating it now...')
        await interaction.guild.create_category(
            name='CTFs',
            reason='Created by the Discord Bot',
            overwrites=overwrites
        )

    await interaction.guild.create_text_channel(
        channel_name,
        reason=f'Requested by {interaction.user}',
        category=list(
            filter(lambda x: x.name == 'CTFs',
                   interaction.guild.categories)
        )[0]
    )

    await attach_embed_info_and_send(
        interaction,
        f'Channel **{channel_name}** has been created.',
        Color.GREEN.value
    )


@BOT.tree.command(name='join', description='Joins the specified CTF channel.')
async def join(interaction: discord.Interaction, channel: discord.TextChannel):
    """Join the specified CTF channel."""
    pass


# Start a webserver in another thread in order to keep the bot alive
keep_alive()


async def main():
    async with BOT:
        await BOT.start(TOKEN)


asyncio.run(main())
