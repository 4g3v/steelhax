# steelhax
Steelhax is a secondary entrypoint for the 3DS game `Steel Diver: Sub Wars`, available for free on the shop. It allows you to get homebrew apps/userland code execution on your 3DS. Thanks to the work of [@zoogie](https://github.com/zoogie) it turned into a primary entrypoint.

It basically works as follows: 
- Get control of the PC register by exploiting a vulnerability in the savedata parsing
- Use rop to load a different ropchain from the SD card (Allows for easier updating)
- This ropchain then loops through the code of the game to find a unique value which allows us to defeat PASLR
- Use [gspwn](https://www.3dbrew.org/wiki/3DS_System_Flaws#:~:text=GSP%20module%20does%20not%20validate%20addresses) to load code from the SD card and actually get code execution
- This code then loads the [otherapp payload](https://smealum.github.io/3ds/#otherapp) from the SD card, uses [gspwn](https://www.3dbrew.org/wiki/3DS_System_Flaws#:~:text=GSP%20module%20does%20not%20validate%20addresses) again which then loads [the homebrew launcher](https://smealum.github.io/3ds/).
