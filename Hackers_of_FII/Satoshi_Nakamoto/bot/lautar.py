import discord
import asyncio
import youtube_dl
from discord.ext import commands
from discord import app_commands

# Local imports
from . import helpers
from . import pprint

# Suppress noise about console usage from errors
youtube_dl.utils.bug_reports_message = lambda: ''


ytdl_format_options = {
    'format': 'bestaudio/best',
    'outtmpl': '%(extractor)s-%(id)s-%(title)s.%(ext)s',
    'restrictfilenames': True,
    'noplaylist': True,
    'nocheckcertificate': True,
    'ignoreerrors': False,
    'logtostderr': False,
    'quiet': True,
    'no_warnings': True,
    'default_search': 'auto',
    # Bind to IPv4 since IPv6 addresses cause issues sometimes
    # (taken from web, I guess I believe the comment)
    'source_address': '0.0.0.0'
}

ffmpeg_options = {
    'options': '-vn',
}

ytdl = youtube_dl.YoutubeDL(ytdl_format_options)


class YTDLSource(discord.PCMVolumeTransformer):
    def __init__(self, source, *, data, volume=0.5):
        super().__init__(source, volume)

        self.data = data

        self.title = data.get('title')
        self.url = data.get('url')

    @classmethod
    async def from_url(cls, url, *, loop=None, stream=False):
        loop = loop or asyncio.get_event_loop()
        data = await loop.run_in_executor(None,
                                          lambda: ytdl.extract_info(
                                              url,
                                              download=not stream
                                          ))

        if 'entries' in data:
            # take first item from a playlist
            data = data['entries'][0]

        filename = data['url'] if stream else ytdl.prepare_filename(data)
        return cls(
            discord.FFmpegPCMAudio(filename, **ffmpeg_options),
            data=data
        )


class Lautar(commands.GroupCog,
             name='lautare',
             description="Adu micu' și vizoru', lăutaru-ți sparge difuzoru'"):
    def __init__(self, bot: commands.Bot) -> None:
        self.bot = bot
        self.voice_client = None
        super().__init__()

    @app_commands.command()
    async def join(self,
                   interaction: discord.Interaction,
                   channel: discord.VoiceChannel) -> None:
        """Joins a voice channel"""
        if self.voice_client is not None:
            return await self.voice_client.move_to(channel)

        await channel.connect()

    @app_commands.command()
    async def canta(self,
                    interaction: discord.Interaction,
                    query: str) -> None:
        """Lăutaru' cântă cu dedicație sau link dă pă YT, SoundCloud etc."""
        await interaction.response.defer(thinking=True)

        if 'https://' not in query:
            query = helpers.find_youtube_video(query)

        player = await YTDLSource.from_url(query,
                                           loop=self.bot.loop,
                                           stream=True)

        if self.voice_client is None:
            if interaction.user.voice:
                self.voice_client = \
                    await interaction.user.voice.channel.connect()
            else:
                await interaction.response.send_message(
                    "You are not connected to a voice channel.",
                    ephemeral=True
                )
                pprint(f"[red]ERROR[/red]\t"
                       f"{interaction.user} is not connected to a voice "
                       f"channel.")
                return
        elif self.voice_client.is_playing():
            self.voice_client.stop()

        try:
            self.voice_client.play(
                player,
                after=lambda e: pprint(f'[red]ERROR[/red]\tPlayer error: '
                                       f'{e}')
                if e else None
            )
        except discord.errors.ClientException:
            return

        await interaction.edit_original_response(
            content=f'Now playing: {player.title}'
        )

    @app_commands.command()
    async def volume(self,
                     interaction: discord.Interaction,
                     volume: int) -> None:
        """Changes the player's volume"""
        if self.voice_client is None:
            return await interaction.response.send_message(
                "Not connected to a voice channel."
            )

        self.voice_client.source.volume = volume / 100
        await interaction.response.send_message(f"Changed volume to {volume}%")

    @app_commands.command(name='resume', description='Resumes the song')
    async def resume(self, interaction: discord.Interaction):
        if self.voice_client.is_paused():
            await self.voice_client.resume()
        else:
            await interaction.response.send_message(
                "The bot was not playing anything before this. "
                "Use play_song command")

    @app_commands.command(name='pause', description='Pause the song')
    async def pause(self, interaction: discord.Interaction):
        if self.voice_client.is_playing():
            await self.voice_client.pause()
        else:
            await interaction.response.send_message(
                "The bot is not playing anything at the moment."
            )

    @app_commands.command(name='afara',
                          description='Dă lăutaru\' afara de pe canal')
    async def afara(self, interaction: discord.Interaction) -> None:
        """Stops and disconnects the bot from voice"""
        await self.voice_client.disconnect()
        await interaction.response.send_message('Gata, boss. Am iesit!')
