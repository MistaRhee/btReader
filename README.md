Baka-Tsuki Reader for PC (Something which I'm doing in my spare time)
=====================================================================

tl;dr this is just an app which will read the BT site and download novels so
people can access offline. Just something to make life easier I guess? Was
suggested to me by NoOne2246 (find him on git as well). Also, this will only be
for English TLs only, sorry, but I just can't find the effort to do any more
then what NoOne2246 wanted of me.

After I've finished with this, I'll hand over ownership to NoOne2246 (or at
least offer it first) and if he doesn't want it, it will be posted on the
Baka-Tsuki IRC/Facebook Page/Forums for someone to look after this lost and
lonely project, which I will abandon because it will drive me insane if I have
to hardcode all the exceptions there are in the website (for example
Hikaru ga Chikyuu ni Itakoro...... as a title should not have the extra periods
after it.................................. and some URLs have them, others
don't. Yeah, that stuff)


-> NOTE: This version is the "lazy man's" version without a proper reader. If
you wish to have a nice reader attached to this which doesn't launch a web-browser
make it yourself. Also, Windows will always remain inferior to unix, since I
can't really test on Windows too well.

Modding/Modifications that can be made
--------------------------------------

So, when designing this, I have tried to allow as much flexibility in the
interface as possible. This is the main cause of me using XML to store the data
about graphics as much as possible. So basically, there will be three main XML
files which need changing: manifest.db (yes, it's actually an XML file),
system/user.profile and system/profile.xml. "system/user.profile"

"system/user.profile" is the most common file that will be edited (if you're
looking into modding this stuff). This file will contain most of the options
needed to make this thing work (nothing much else I guess). All extra resources
or data MUST be added to manifest.db (which is an XML file), otherwise the
program will refuse to work (most of the time)

"system/profile.xml" is the only XML file which won't force you to
manually edit it (it's the result of the settings page) and it will store your
precious settings. You can do nifty things to it (like set the text colour or
the background colour) which I don't really want to put into the settings menu
because I want it to be simple (not out of laziness).

There are quite a few restrictions in the modding though. There MUST BE certain
elements, (such as settings, settings-selected etc..) to ensure that the user
is able to have access to all of the functions, and since everything is on ONE
SCREEN (i.e. there will be no changes in windows) these things must be readily
available. 


License and stuff
-----------------
(Note: I'm surprised that people actually are reading this. Have fun wasting
these seconds of time which you are reading this note and realising that this
note is actually pointless and is not worth NOTICING)

    btReader - An unofficial reader for the website
    Copyright (C) 2015  Justin Huang and Christopher Ho

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

Not that I really mind if you take stuff from here... just be nice and you
know, acknowledge me a bit. Maybe even donate money to charity if you really
feel guilty about taking the intellectual property from a kid off the internet
without paying.

Compiling
---------

I'm not even there yet, but

    $ make
    $ make clean

is usually how this works. MAKE SURE YOU CLEAN (YOU DON'T WANT TO BE A DIRTY
PERSON When it actually works, it'll only be tested under GNU/Linux. Should 
work for Windows with a few tweaks I guess.

Dependencies
------------

- SDL2
- SDL_Image
- SDL_TTF
- libCurl (Tested on v7.40.0, but should work on later versions)

Maybe more stuff later, but I'll be adding to it. No funny business here. Sorry
to disappoint.

Not So Dependencies
-------------------

I found this pretty nifty library for parsing XML,

http://www.applied-mathematics.net/tools/xmlParser.html

Pretty handy. Would rate 10/10.
