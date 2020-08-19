# ch8
This is yet another Chip-8 interpreter with SuperChip instructions support.

## Building on Linux
* Requirements
  * SDL2 library <https://wiki.libsdl.org/Installation#Linux.2FUnix>
  * GCC, Clang or TCC compiler
  * make
* type `make`


## TODO
* Add XO-Chip support
* Add debugger
* Check on Windows

## Technical specifications
* 4 KiB RAM
* 128x64 pixels in SuperChip8 mode / 64x32 pixels in Chip8 mode
* 16 8-bit registers
* 16 keys on keyboard
* 16 level stack
* 1 16-bit address register

## ROMs

I have a small collection of ROMs that I like.
You can download more ROMs here:

<http://devernay.free.fr/hacks/chip8/GAMES.zip>

<http://devernay.free.fr/hacks/chip8/SGAMES.zip>

Many real cool games also available here. Note that some of them
could be written according to XO-Chip specification that currenly not supported.

<https://github.com/JohnEarnest/chip8Archive/tree/master/roms>

 
## Keyboard

Original Chip8 had hexadecimal keyboard
```
+---------------+
| 1 | 2 | 3 | C |
+---+---+---+---+
| 4 | 5 | 6 | D |
+---+---+---+---+
| 7 | 8 | 9 | E |
+---+---+---+---+
| A | 0 | B | F |
+---------------+
```
ch8 maps this keyboard on following layout
```
+---------------+
| 1 | 2 | 3 | 4 |
+---+---+---+---+
| Q | W | E | R |
+---+---+---+---+
| A | S | D | F |
+---+---+---+---+
| Z | X | C | V |
+---------------+
```
## Screenshots

### Outlaw
<p align="center">
  <img src="https://imgur.com/g2Q9yJh.png">
</p>

### CAR
<p align="center">
  <img src="https://imgur.com/I8quAUi.png">
</p>

### DVN8
<p align="center">
  <img src="https://imgur.com/gV9m0ir.png">
  <img src="https://imgur.com/ltuKxtJ.png">
  <img src="https://imgur.com/twX3ukB.png">
</p>  

### Space Invaders
<p align="center">
  <img src="https://imgur.com/zy6yhEt.png">
  <img src="https://imgur.com/hIMnM89.png">
</p>

## Sources used
[1] Cowgod's Chip-8 Technical Reference v.1.0

<http://devernay.free.fr/hacks/chip8/C8TECH10.HTM>

[2] How to write an emulator (CHIP-8 interpreter)

<http://www.multigesture.net/articles/how-to-write-an-emulator-chip-8-interpreter/>

[3] r/EmuDev

<https://www.reddit.com/r/EmuDev/>

[4] Notes on Chip-8 and SuperChip implementation

<http://devernay.free.fr/hacks/chip8/schip.txt>

[5] Some useful information about SuperChip

https://courses.ece.cornell.edu/ece5990/ECE5725_Fall2016_Projects/Lab_group_bcp39_rk534/cpu.html

[6] Useful information about all possible quirks

https://github.com/Chromatophore/HP48-Superchip

[7] Cowgod's opcode list

http://devernay.free.fr/hacks/chip8/chip8.html
