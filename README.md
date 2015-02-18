# LPC8xx_Flash_EEPROM

Example LPC8xx project to show how the 64 byte page flash in-application programming can be used to emulate 'EEPROM' (electrically erasable read-only-memory) function found in other MCUs.

This can be used, for example, to preserve settings or network node
addresses across power cycles. 

Currently compiles to just under 4KiB (without space optimization) so it should work with all the LPC8xx range of MCUs including the LPC810. I'm uming LPCXpresso version 7.6.2 to build the project.

Enjoy,

Joe Desbonnet
jdesbonnet@gmail.com


