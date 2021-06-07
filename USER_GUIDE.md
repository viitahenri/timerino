# Here is a guide to the functions and operations of the timer:

The unit has two modes of operation, Linear and F/stop. There are four functions within each mode, corresponding to the A, B, C and D keys. Pressing the function select button when the unit is not running or the enlarger on will toggle between the Linear and F/stop modes. Each function (A, B, C, and D) is similar to its function in the opposite mode. Therefore, when the mode select button is pressed, the unit will keep the same letter function, but in the opposite mode. When any mode or function is changed, the unit writes the new selection to non-volatile memory and will power up next time in the same mode.

# Linear mode

In this mode there are 4 functions, selectable with the keypad using the keys (A, B, C, D). 

## Function A (Setup}:
This mode controls the timer as a simple enlarger switch, for focusing and setup. The "Start" button is pressed and the enlarger turns on. A subsequent press and the enlarger is turned off. The mode can be used to focus or for positioning of the paper and easel. In this mode are also some setup functions:

- You can enable / disable the sound by pressing 0. The setting is saved and remembered after power is switched off.
- It is possible to select the channel with the keys 1 to 9. Each function/mode combination keeps track of the last times set and used. These memories are per channel. The time settings and channel selection are NOT saved to non-volatile memory and are zeroed and resset to Channel 1 after a reset or power cycle.

## Function B (Stopwatch):
This mode turns on the enlarger at the press of the start button, simultaneously counting the time elapsed with an acoustic
signal every second. A subsequent press of the button stops counting and turns off the enlarger and halts the count. Pressing the button
again restarts operation. The time count is reset by simultaneously pressing the * and # keys on the keypad. You
can not enter time with the keypad in this mode.

## Function C (Countdown):
This mode allows one to enter a time via the numeric keypad (simultaneously pressing the * and # keys on the keypad performs a reset). Pressing the start button turns on the enlarger and simultaneously counts down starting from the time set with an acoustic signal every second. A new press of the button turns off the enlarger and pauses the timer. Pressing the button again restarts operation. When the time reaches 0 the enlarger turns off and the timer is reset to the time initially chosen.

## Function D (Countdown with initial enlarger focus)
This function is identical to the C function, but the first press of the button turns on the enlarger but does not start the counting of
the time. A subsequent press of the button turns off the enlarger. The third press turns on the enlarger and also begins the
countdown as for function C. The function is used to perform manual masking or burns that require precision placement. Ex: Set a time of 5 seconds for a burn, put the red filter in place, push the button, position the cardboard / hands / etc under the beam of light, push the button and the enlarger is turned off. Push again to finish the exposure.


# F-stop mode

In this mode there are 4 additional functions, selectable with the keypad using the keys (A, B, C, D):

## Function A (F/stop precision setting):
This mode runs the timer as a simple switch. The button is pressed and the enlarger turns on. A subsequent press and the
enlarger is turned off. The function can be used to focus or for the positioning of the easel/paper. This mode also selects the precision of f-stop units. Pressing the keys from 1 to 0 on the numeric keypad will select the precision that will be used in the functions B, C, and D in f-stop mode. The selectable accuracies are 1; 1/2; 1/3; 1/4; 1/6; 1/8; 1/12; 1/24; 1/32; 1/48 stops. This value is saved to EEPROM and restored after a power cycle or reset. The value is also displayed on the second line of the LCD while in the f/stop modes.

## Function B (Test Strip):
This mode allows making a test strip with increasing exposure steps in f-stop units (precision is chosen in function A). The base exposure is chosen through the keypad in seconds. The enlarger is started with the start button with the paper completely uncovered. After the base exposure and at each f/stop interval thereafter, there will be an acoustic signal of three short beeps and one long to indicate to cover a portion of the paper (test strip) with a piece of cardboard. One continues in this way until the coverage of the paper is complete. By pressing the start button, the enlarger is shut off and the timer stops counting. One can reset the time with the simultaneous pressing of the * and # keys of the keypad.

## Function C (Countdown with + or - exposure adjustment in f/stop units):
This mode allows one to enter a time via the numeric keypad (simultaneously pressing the * and # keys on the keypad performs a clear). The * key decreases the time by a stop or fraction thereof according to the set precision in function A. The # increases the time in a similar fashion. Pressing the start button then turns on the enlarger and begins the countdown, starting from the time set, with an acoustic signal every second. Another button press turns off the enlarger and pauses the timer. Pressing the button again restarts
operation. When the time reaches zero, the enlarger is turned off and the timer is reset to the time initially chosen. This function can be used to make test strips with modified f-stop exposures on the same paper; A different portion of a test print is uncovered and a complete exposure made with the desired increased or decreased time from the base exposure.

## Function D (Masking / burn f-stop):
This mode allows entering a time via the numeric keypad (simultaneously pressing the * and # keys on the keypad performs a clear). Once the time set, # sets the timer to carry out a burn, while * a mask, at the current f/stop precision setting.

Burn: Pressing the # key is increments the set time by one stop (or fraction thereof according to the set precision in function
A). Then, pressing the start button turns on the enlarger and begins part of the countdown. During this time only the part of the image to burn is exposed. At the end of the burn time the timer stops by switching off the enlarger, and then displays the time associated with the exposure of the entire image. Pressing the button turns on the enlarger and the countdown resumes to the end of the base time.

Mask: Starting from an entered base time, pressing the * key calculates the masking time according to the set precision in function A. Then, pressing the start button turns on the enlarger and at the same time the first part of the countdown. During this time the area of the image to be masked is covered. At the end of the mask time, the timer time stops by switching off the enlarger, and showing the remaining time for the entire image. The mask is removed and the button pressed again. The enlarger is switched on and the countdown of the balance of the base time completes and the enlarger is shut off.