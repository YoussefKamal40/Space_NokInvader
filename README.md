# Space_NokInvader

Spaghetti code for space invader clone using slide resistance, limit switch, Nokia screen and STM32F407.

This fun Project includes ADC driver, SPI Driver and NOKIA5110 Driver.

The rest of the code is a spagetti code trying to apply multi threaded software and OOP concept using only C language on embedded target.

Timer in main.c was used to implement the second thread and another timer was used in ADC.c to trigger the injected mode ADC.

The main function is used to plot any changes in the screen objects, Rockets, enimys and player, and the timer in main.c is used to trigger the game FSM, scoring system, positioning system.

The FSM runs with frequency 60HZ.

The ADC conversion sampling rate is 120 sample per second.

Find here steps and demo to my two weeks of spontanious fun journy of programming this simple system into a fun game.
https://youtu.be/X-oamANJmAk

