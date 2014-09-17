2048 OS

=======
A tiny OS that runs a graphical version of the game 2048. This OS is run inside
virtual machines in lieu of showing "Operating system not found" message
on the Nutanix virtualization platform.

It is released by Nutanix under permissive open source license for educational
purposes. It demonstrates the bare minimum required for booting a x86 processor
from real mode into protected mode, while initializing basic devices like
framebuffer, keyboard, and a timer.

Build and run:
1. Build tools needed: make, gcc, python, python-png
2. run: make
3. dd boot.bin into the first sectors of a (virtual) hdd, then boot from it.

Future work:
1. Make it bootable from floppy disk as well.
2. Add animation.

Credits:
- 2048 game was created by Gabriele Cirulli. Play the orignal version at
  http://gabrielecirulli.github.io/2048/
- The idea for running a game in a tiny OS comes from Carnegie Mellon
  University's Operating Systems class taught by Professor Dave Eckhardt.
  Every single semester, Project 1 for this class asks students to create a
  bare metal x86 OS that runs some game. It just happened that the game for
  Fall '14 is also 2048, but that is pure coincidence.
- Enhanced 2048 graphics are created by Nutanix's awesome design team.

