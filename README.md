kitchenlight
============

An arduino based light controller for kitchen bench lights.

This project came about when one of our 20+ years old kitchen benchtop lights
broke down. Since Invector manufactures LED strips for light applications I
figured I could build a modern motion triggered controller.

Armed with an Arduino Uno and a Sweet Pea's LED Light shield I came up with
this piece of software. The ramp controller need some fixing up (I know it
is a mess) but otherwise it works pretty well.

Hardware:
For this project I used a prototype of the LED Shield which have its PWM inputs
on arduino pins 5 and 6. Currently only one LED driver is used but this will 
change shortly. For motion detection I used two Sweet Pea's PIR sensors who
are connected to arduino pins 7 and 8.

What does it do ?
Not much, but what it does is quite neat.
PIR sensor 1 is a room based trigger. It will detect whenever someone enters
the room (the kitchen in my case) and ramp the light smoothly up to a
pre defined level. We use this function as a sort of a guide light so that
when someone gets up in the night to go to the kitchen, the lights will go on
smoothly and really low not to hurt your eyes.
PIR sensor 2 is used as a close range trigger. It sits in the controller box
underneath the kithen top cabinet close to the LED strip. It is used to turn
the lights up to 100% by simply gesturing in front of it. In order for it to
work properly, not to be triggered by the cat sneezing in the vicinity, I
had to put a lot of opaque tape on top of it. I also turned the sensitivity
down a bit so that it wouldn't trigger accidentaly when someone is simply
working on the worktop.

Licenses:

The protothreads header files (lcaddrlabels.h  lc.h  lcswitch.h and pt.h)
are under the following license:

/*
 * Copyright (c) 2004-2005, Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

The rest of the files are under the following license:
/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <pontus.oldberg@invector.se> wrote this file. As long as you retain this
 * notice you can do whatever you want with this stuff. If we meet some day,
 * and you think this stuff is worth it, you can buy me a beer in return.
 * /Pontus Oldberg
 * ----------------------------------------------------------------------------
 */

