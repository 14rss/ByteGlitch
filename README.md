# ByteGlitch

### A Simple Tool for The Intentional Corruption of Digital Files (namely images)

Can be hit or miss depending on your glitch algorithm and other settings. Tested most with TIFF and PNG files. The file compression and format will have the most effect on the look of the resulting glitch. Sometimes if the resulting glitched output isn't opening correctly in most image viewers, you will need to open an image editor and resave it. Paint.net works very nicely for this(https://www.getpaint.net) and has been able to open corrupted images that other tools couldn't. I recommend resaving the glitched output if you intend to use it for anything afterwards.

!["Much corrupt"](https://github.com/DeepDarkDweller/ByteGlitch/blob/main/glitched_example.png?raw=true)

TODO (maybe):
- [ ] Make the program specifically target areas of the file containing color data to avoid corrupting headers (would be format specific) 
- [ ] Make the UX a little smoother 
- [ ] Automate the process so that it could be used to glitch GIFs or Videos. 
- [ ] Come up with more interesting glitch algorithms (e.g. copy/pasting chunks of the file, palette shifts, etc.) 
- [ ] Display the image in a window while the corruption is visualized in real-time 
- [ ] Tell the user exactly which bytes were corrupted and how

