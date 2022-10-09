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

    async def on_guild_join(self, guild: discord.Guild):
        try:
            general = list(filter(lambda x: x.name == 'general',
                                  guild.text_channels))[0]
        except IndexError:
            general = guild.text_channels[0]

        if general and general.permissions_for(guild.me).send_messages:
            await general.send(
                'Hai noroc la toti baietii, fetele si fanii care face '
                'banii :money_with_wings:',
            )

            embed = discord.Embed()
            embed.set_image(url=constants.JOIN_GIF_URL)
            await general.send(embed=embed)
