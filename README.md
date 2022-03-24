# Raymarching-Render
A simple raymarching renderer making use of ffmpeg as well as toojpeg to generate a .mp4 video file.

This is done by loading a 2d array of usigned chars [numberOfFrames][Width * height * numberOfColorChanels] and yes, I am aware this uses a lot of memory but I'm working on a better solution just wait a little :-) . Following this a directory 'tmp/' is added in which the Frames will be stored until they get merged together.
This array is then used to store the color information for the Frame, using Multithreading. Here I use numberOfCores - 1 cores as toojpeg does not support multithreading and I save the generated Frames parallel using the last thread.

Finally ffmpeg is used to merge the Frames into the video and the folder 'tmp/' is deleted, Frames included.
