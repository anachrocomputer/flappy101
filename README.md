![Static Badge](https://img.shields.io/badge/CPU-6502-green "CPU:6502")
![Static Badge](https://img.shields.io/badge/BOARD-Compukit%20UK101-green "BOARD:Compukit UK101")
![Static Badge](https://img.shields.io/badge/MiniLD-50-green "MiniLD:50")

# Flappy 101 #

A simple game for the Compukit UK101.
Originally written for Ludum Dare Mini-LD \#50: DEMAKE in March 2014.

## Ludum Dare ##

I wrote this program for Ludum Dare's MiniLD \#50:

https://web.archive.org/web/20140410105604/http://www.ludumdare.com/compo/2014/03/07/mini-ludum-dare-50-announcement

## Implementation ##

The main loop of the game runs in BASIC, but the BASIC program
calls a machine-code subroutine via the USR() function.
The machine-code part of the game handles the movement of the
obstacles across the UK101's screen.

## Screenshots ##

![Instructions](flappy2.png "Instructions")

![Game play](flappy3.png "Game play")

![Game over](flappy1.png "Game over")
