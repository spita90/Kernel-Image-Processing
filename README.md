# Kernel-Image-Processing
This tool allows to compare image blurring times and speedups that you can obtain using all your CPU cores.
The software divides the input image in as many sections as there are selected threads, and applies a kernel convolutional mask to all pixels.
It also returns computing times, so you can compare Single-Thread times with Multi-Thread ones.

It is made with Qt Creator and C++11 Threads.
In the files "Spitaleri..." you will find explanatory docs (italian and english).
