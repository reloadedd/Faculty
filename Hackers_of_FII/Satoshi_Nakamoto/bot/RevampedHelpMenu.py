#!/usr/bin/env python3

from discord.ext import commands

# Local imports
from .constants import Color
from .helpers import attach_embed_info_and_send


# Credits: https://stackoverflow.com/a/65773077
class RevampedHelpMenu(commands.MinimalHelpCommand):
    async def send_pages(self):
        destination = self.get_destination()
        for page in self.paginator.pages:
            await attach_embed_info_and_send(destination,
                                             page,
                                             Color.CYAN.value)
