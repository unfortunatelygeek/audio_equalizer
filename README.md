# Real-Time Audio Equalizer with FR Modelling

## Problem Statement:
Design and implement a real-time audio equalizer on an embedded system using C programming. 
The goal is to create an audio equalizer that allows users to adjust the gain levels of specific frequency bands to enhance audio quality based on their preferences. 
The equalizer should leverage mathematical modeling techniques to design the frequency response characteristics of each equalizer band. 

## Solution

### Theory
A peaking equalizer filter allows you to increase or decrease the level of frequencies around a central point. It is also known as a parametric equalizer section. The level remains unchanged for frequencies far away from the boosted or cut area. This makes it convenient to combine multiple peaking equalizer sections together in a series configuration.

#### The Analog Transfer Function:
![image](https://github.com/unfortunatelygeek/audio_equalizer/assets/118061722/efe61b50-1d62-4097-87a9-e0b37cbeca9f) 
Where,   g is gain (in the code as gain_lin)
		     Wc is the cutoff frequency in radians/s
		     Q is the Quality Factor

Source: Phil's Lab YouTube Channel

#### The Analog Transfer Function: Tustin's Method
![image](https://github.com/unfortunatelygeek/audio_equalizer/assets/118061722/7bd13cd2-372d-45d0-aa3c-c2b8d3cdb75a)

Source: MIT OCWare

## Further References/Readings:
1. Phil's Lab: https://www.youtube.com/@PhilsLab
2. The EQ CookBook: https://www.w3.org/TR/audio-eq-cookbook/
3. Pre-warping For Discretisation: https://dsp.stackexchange.com/questions/64025/frequency-prewarping-of-a-bilinear-transform-tustin-transform


