# VirusBlinksGame

Current Gameplay:

1 player
18 blinks recommended

Intitial state is all white.

To start the game, double click any blink.  This becomes the virus.  The additional blinks will become play pieces.
When you click a play piece that is connected to others of the same color, they will damage the virus.
- 1 blink = actually heals the virus 1
- 2 blinks = 0 damage, but useful for cycling colors
- 3-5 blinks = 1 damage
- 6+ blinks = 2 damage

The virus will continue to strengthen and if it reaches 18 health it is game over.  The virus starts at 13 health.  If the virus loses all health, the player wins.

Win screen = pulsing white blinks
Lose screen = pulsing red blinks
Game can be restarted on either end game screen by double clicking any blink to create a new virus

Once I get this basic gameplay down, I will most likely go more towards my original game plan listed below

# Original game planning notes:
Working Title: 
Virus

Players:
1?

How to Win:
Destroy the virus/ghost/whatever it is.  Once the last light is out, the player wins.

Lose Condition:
If the virus is fully blue with 6 lights, it will then start to blink.  If the virus strengthens again as it is blinking, the game is over and the player has lost.

Game Play:
Match 3+ colors to charge your laser.  When you do, depending on how many are matched, you gain more charges.  Click the laser to fire it.  When the laser is fired, it will go towards the enemy, destroying all tiles in the way and damaging the virus.  If a 2 or 1 is instead used, the virus regenerates.  If time runs out, the virus regenerates.  If the virus is pressed, it will recreate the board and regenerate.  The virus starts blue with 1 light lit.  The virus will transition from blue to white to red showing its level of damage.  Within those levels, there are 6 lights.  Lights going out shows its transition to the lower levels.  Once the last light goes out, the blinks will all blink white to show the game is over and you have won.  If the virus wins, the blinks will all blink red to show you lost.

Game Play Details:
Laser takes 10 charges to fire.
3 matches = 1 charge
4 matches = 3 charges
5 matches = 7 charges
6 matches = 10 charges  

0 charge = 20fpm
1 charge = 10fpm
9 charge = 1fpm
>=10 charge = solid

Layout: 
18+ blinks.  

Pieces:
1 laser and 1 enemy.  The remaining make up the board.

Colors:
blue - 3rd tier virus health
white - 2nd tier virus health
red - 1st tier virus health
magenta - laser
orange - tile color 1
yellow - tile color 2
cyan - tile color 3
green - tile color 4

Difficulty:
Level 1: Timer 30 seconds

Setup Steps:
When the game is in the "waiting to play" phase, double tap a piece to make it the laser.  Long press a piece to create the virus and populate the board.

Game Flow:
Once the game begins, a round timer begins.  There is no indicator for this.  The laser will begin to pulse slowly. The laser can be moved around after the game begins but must be attached as matches occur.  Otherwise the laser will never know it is getting credit. (Or maybe the virus should know about this state) As 3+ matches occur, the laser starts to pulse faster.  If a match of 6+ occurs, the laser is instantly charged. When the laser becomes a solid color, it is ready to be fired.  When a laser is fired, it will go towards the virus, wiping out every tile on the way and damaging the virus.  Win condition is checked as the virus takes damage.  Lose condition is checked when a match of 2 or less is executed or the timer expires or when the virus is pressed.  Timer refreshes whenever the laser is fired or when the timer expires.

Resetting Game:
Long press the virus tile.  This can be done during the gameplay or after win/lose condition.

Communication:

16 values per message type

000  xxx - reset messages
     000 - game reset
     001 - clear lasers
     010 - clear viruses

001  xxx - matching messages

010  xxx - laser messages
     0-3 - boost amount
     
100  xxx - reserved