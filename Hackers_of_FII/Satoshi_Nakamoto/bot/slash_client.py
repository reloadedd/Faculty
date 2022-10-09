import discord
from discord.ext import commands
from .lautar import Lautar
from . import constants
from . import pprint, Console


class SlashClient(commands.Bot):
    def __init__(self) -> None:
        super().__init__(command_prefix='/', intents=discord.Intents.all())

    async def setup_hook(self) -> None:
        await self.add_cog(Lautar(self), override=True)
        # self.tree.clear_commands(guild=None)
        self.tree.copy_global_to(guild=discord.Object(id=914146847164092487))
        await self.tree.sync()

    async def on_ready(self):
        pprint(constants.BANNER, style='bold red')
        Console().rule('[bold red]xxxxx')
        await self.change_presence(activity=discord.Game('with you'))

    async def on_member_join(self, member):
        guild = member.guild
        if guild.system_channel is not None:
            to_send = f'Welcome {member.mention} to {guild.name}!'
            await guild.system_channel.send(to_send)
