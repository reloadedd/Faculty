import discord
from .helpers import attach_embed_info_and_send, get_roles
from .constants import Color
from . import pprint


# Defines a custom Select containing colour options
# that the user can choose. The callback function
# of this class is called when the user changes their choice
class Dropdown(discord.ui.Select):
    def __init__(self):
        self.roles, self.colored_roles = get_roles()

        emojis = ('🔐', '🔫', '🛰', '💻', '👾', '👑', '🎓', '👔')
        descriptions = (
            'Welcome to Saint Tropez',
            'So... You managed to survive?',
            'You\'re almost done. Can\'t believe!',
            'Oh boy, you really like pain.',
            'Impressive, mic drop!',
            'Jaw drop.',
            'Congrats! You did it!',
            'Bonjour! Nous avons préparer pour aujourd\'hui...'
        )

        options = [
            discord.SelectOption(label=role[0],
                                 description=descriptions[i],
                                 emoji=emojis[i])
            for i, role in enumerate(self.colored_roles)
        ]

        # The placeholder is what will be shown when no option is chosen
        # The min and max values indicate we can only pick one of the three
        # options.
        # The `options` parameter defines the dropdown options.
        # We defined this above
        super().__init__(placeholder='Pick your poison...',
                         min_values=1,
                         max_values=1,
                         options=options)

    async def callback(self, interaction: discord.Interaction):
        # Use the interaction object to send a response message containing
        # the user's favourite colour or choice. The self object refers to the
        # Select object, and the values attribute gets a list of the user's
        # selected options. We only want the first one.
        role = discord.utils.get(interaction.guild.roles, name=self.values[0])

        for current_role in interaction.user.roles:
            if current_role in [
                discord.utils.get(interaction.guild.roles, name=i)
                for i, _ in self.colored_roles
            ]:
                await interaction.user.remove_roles(
                    current_role,
                    reason='Already owns another role from the same category'
                )
                pprint(f'[blue]INFO[/blue]\tRemoved role '
                       f'[bold]{current_role}[/bold] for user '
                       f'[italic]{interaction.user}[/italic]')

        if role:
            await interaction.user.add_roles(role,
                                             reason='Chosen from Dropdown')

            await attach_embed_info_and_send(
                interaction,
                f'You chose *{self.values[0]}*. Great!',
                Color.GREEN.value,
                ephemeral=True
            )
        else:
            await attach_embed_info_and_send(
                interaction,
                f'The role *{self.values[0]}* does not exist on the server.',
                Color.RED.value,
                ephemeral=True
            )


class DropdownView(discord.ui.View):
    def __init__(self):
        super().__init__()

        # Adds the dropdown to our view object.
        self.add_item(Dropdown())
