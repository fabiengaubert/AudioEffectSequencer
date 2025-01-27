# audio effect sequencer

This is the archival of a working audio effect sequencer I developped several years ago on an ATmega2560 8-bit MCU.
Such a platform is a really good teacher of hardware/firmware mechanisms. Because the architecture is so simple, and the datasheet so self-explainatory, everything can be understood down to the register.

The project included an ADC, DAC, buttons, potentiometers, LEDs and multiplexers. It provides a simple bit-crusher effect that can be controlled by potentiometers, whose movements can be recorded as automation and automatically replayed by the system (a well known feature in music production, called automation or parameter lock).
