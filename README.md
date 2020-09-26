### How Do I Install & Uninstall?

To install, cd into the source folder, then `sudo make install`.

To uninstall, cd into the source folder, then `sudo make uninstall`.

### How Do I Use This?

```
 Usage: cppm [-d DELAY] [-sr SPAWN_RATE] [-l MAX_LENGTH] [-m MIN_LENGTH] [-b] [-B][-r] [-R] [-h] [-hc COLOR] [-bc COLOR] [-s] [-c COLOR] [-cs CHARACTER_SETS] [-f]

 -d:  delay (0 - 10) delay before screen update
 -sr: spawn rate (0 - 10) of rows
 -l:  length (0 - 50) row length
 -m:  minimal length (0 - 49) minimal row length
 -b:  random bold characters
 -B:  every character is bold
 -r:  every character's color is rainbow
 -R:  every character's background color and foreground color is reversed
 -h:  print usage
 -hc: head color
 -bc: background color
 -s:  random speed
 -c:  set matrix color
 -f:  character flashes to different character
 -cs: set character sets

 Colors: black, red, yellow, green, blue, magenta, cyan, white, none

 Character Sets:
 a: lower case alphabet
 A: upper case alphabet
 d: 0-9 numbers
 s: common symbols
 j: half-width japanese kana
 g: greek symbols
 S: symbols from matrix movie

 Examples:
 cppm -cs SSsaaAA
 cppm -cs SSgjjj -b
 cppm -r -B
```

### What Is "cppm"

"cppm" stands for "c++ matrix".

cppm gives you a cool matrix screen from the movies. You can use a combination of different characters, including japanese half-width kana. Their is a lot of room for customization, you can change the colors, the delay, the spawn rate, and much more.

Here are some screenshots!

![japanese](https://github.com/static-startup/cppm/blob/master/images/japanese.png)
![ascii](https://github.com/static-startup/cppm/blob/master/images/ascii.png)
![wacky](https://github.com/static-startup/cppm/blob/master/images/wacky.png)
![classic](https://github.com/static-startup/cppm/blob/master/images/classic.png)


