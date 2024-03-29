#!/usr/bin/env python3
import asyncio
import discord
import coloredlogs
import logging
from discord import app_commands

# Local imports
from bot.constants import Color, TOKEN
from bot.helpers import attach_embed_info_and_send, get_roles, \
    check_if_locked_up
from keep_alive import keep_alive
from bot.slash_client import SlashClient
from bot.dropdown import DropdownView
from bot import pprint

coloredlogs.install()
logger = logging.getLogger('discord')
logger.setLevel(logging.INFO)
logging.getLogger('discord.http').setLevel(logging.INFO)
BOT = SlashClient()


@BOT.tree.command(name='assign')
async def assign(interaction: discord.Interaction):
    """Display a dropdown which lets you choose your role."""

    # Create the view containing our dropdown
    view = DropdownView()

    # Sending a message containing our view
    await interaction.response.send_message(
        'Choose your academic actual status:', view=view, ephemeral=True
    )


@BOT.tree.command(name='genesis',
                  description='Create all roles - Authorized access only.')
@app_commands.checks.has_permissions(administrator=True)
@check_if_locked_up
async def create_all_roles(interaction: discord.Interaction):
    """Create all roles."""
    _, colored_roles = get_roles()

    await interaction.response.defer(thinking=True)

    guild = interaction.guild
    for role in colored_roles:
        await guild.create_role(
            name=role[0],
            color=role[1],
            hoist=True,
            mentionable=True,
            reason='Genesis',
            permissions=
            discord.Permissions(
                view_channel=True,
                manage_emojis_and_stickers=True,
                create_instant_invite=True,
                change_nickname=True,
                connect=True,
                speak=True,
                stream=True,
                use_voice_activation=True,
                request_to_speak=True) |
            discord.Permissions.text()
        )

    colored_roles = [role[0] for role in colored_roles]
    await attach_embed_info_and_send(interaction,
                                     f'Created roles: {colored_roles}',
                                     Color.CYAN.value,
                                     arg__is_deferred=True)


@BOT.tree.command(name='singularity',
                  description='Remove all roles created at genesis - '
                              'Authorized access only.')
@app_commands.checks.has_permissions(administrator=True)
@check_if_locked_up
async def activate_singularity(interaction: discord.Interaction):
    """Remove all roles created at genesis."""
    await interaction.response.defer(thinking=True)
    roles = [role_name[0] for role_name in get_roles()[1]]
    guild = interaction.guild
    success = True

    not_deleted = []
    for role_name in roles:
        role = discord.utils.get(guild.roles, name=role_name)
        if role:
            try:
                await role.delete()
            except discord.Forbidden:
                success = False
        else:
            not_deleted.append(role_name)

    if success:
        await attach_embed_info_and_send(interaction,
                                         'All roles have been destroyed.',
                                         Color.GREEN.value,
                                         arg__is_deferred=True)
    else:
        await attach_embed_info_and_send(
            interaction,
            f"The following roles couldn't be deleted: {not_deleted}",
            Color.RED.value,
            arg__is_deferred=True
        )


@create_all_roles.error
@activate_singularity.error
async def error_handler(interaction: discord.Interaction,
                        error: app_commands.AppCommandError):
    if isinstance(error, app_commands.MissingPermissions):
        await attach_embed_info_and_send(
            interaction,
            'Could not execute command because of insufficient '
            'permissions.',
            Color.RED.value,
            ephemeral=True
        )


@BOT.tree.command(name='create')
async def create(interaction: discord.Interaction, channel_name: str) -> None:
    """Create channel for a CTF event."""
    overwrites = {
        interaction.guild.default_role: discord.PermissionOverwrite(
            view_channel=True
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


@BOT.tree.command(name='join')
async def join(interaction: discord.Interaction, channel: str):
    """Join the specified CTF channel."""
    _channel = discord.utils.get(interaction.guild.channels,
                                 name=channel)

    if not _channel:
        await attach_embed_info_and_send(
            interaction,
            f'Invalid channel name.',
            Color.RED.value,
            ephemeral=True
        )
        return

    await _channel.set_permissions(
        interaction.user,
        overwrite=discord.PermissionOverwrite(
            add_reactions=True,
            stream=True,
            read_messages=True,
            send_messages=True,
            send_tts_messages=True,
            embed_links=True,
            attach_files=True,
            read_message_history=True,
            mention_everyone=True,
            external_emojis=True,
            create_public_threads=True,
            external_stickers=True,
            send_messages_in_threads=True
        )
    )

    await attach_embed_info_and_send(
        interaction,
        f'Joined channel **{channel}**. Welcome on board!',
        Color.GREEN.value,
        ephemeral=True
    )


@BOT.tree.command(name='leave')
async def leave(interaction: discord.Interaction, channel: str):
    """Leave the specified CTF channel."""
    _channel = discord.utils.get(interaction.guild.channels,
                                 name=channel)

    if not _channel:
        await attach_embed_info_and_send(
            interaction,
            f'Invalid channel name.',
            Color.RED.value,
            ephemeral=True
        )
        return

    await _channel.set_permissions(
        interaction.user,
        overwrite=discord.PermissionOverwrite(
            add_reactions=False,
            stream=False,
            read_messages=False,
            send_messages=False,
            send_tts_messages=False,
            embed_links=False,
            attach_files=False,
            read_message_history=False,
            mention_everyone=False,
            external_emojis=False,
            create_public_threads=False,
            external_stickers=False,
            send_messages_in_threads=False
        )
    )

    await attach_embed_info_and_send(
        interaction,
        f'Left channel **{channel}**. Aurevoir!',
        Color.GREEN.value,
        ephemeral=True
    )


# Start a webserver in another thread in order to keep the bot alive
keep_alive()


async def main():
    async with BOT:
        await BOT.start(TOKEN)


if __name__ == '__main__':
    try:
        asyncio.run(main())
    except KeyboardInterrupt:
        pprint('\n[blue]INFO[/blue]\tSee ya soon, baby.')
