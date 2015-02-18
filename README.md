# LPC8xx_Flash_EEPROM

Example LPC8xx project to show how the 64 byte page flash in-application programming can be used to emulate 'EEPROM' (electrically erasable read-only-memory) function found in other MCUs.

This can be used, for example, to preserve settings or network node
addresses across power cycles. 

Currently compiles to just under 4KiB (without space optimization) so it should work with all the LPC8xx range of MCUs including the LPC810. I'm using LPCXpresso IDE version 7.6.2 to build the project.

I've got a more detailed write up on my blog:
http://jdesbonnet.blogspot.ie/2015/02/nxp-lpc8xx-eeprom-emulation.html

![screen grab](https://github.com/jdesbonnet/LPC8xx_Flash_EEPROM/blob/master/doc/Flash_EEPROM_screengrab_annotated.png)

##Acknowledgements

This demo uses iap_driver.c code from an NXP example:
http://www.lpcware.com/content/forum/eeprom-emulation 

Thanks to @markt and @Chris Stratton on electronics.stackexchange.com
for tips on how best to allocate space in flash memory to use as an
EEPROM bank. Ref thread:
http://electronics.stackexchange.com/questions/153424/how-to-automatically-allocate-a-page-of-flash-for-in-application-storage-of-sett


Enjoy,

Joe Desbonnet
jdesbonnet@gmail.com

18 Feb 2015.

