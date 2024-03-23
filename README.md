# 💃 Sekiro mod - Rebind Combat Art to Shift

* Did you ever press Deflect + Attack buttons together by accident in a game where you constantly spam Deflect and Attack buttons?.. which results in you stupidly standing here slashing Mortal Draw for 2 seconds.
* Did you ever watch your keyboard logs to find out that that one time you actually didn't fuck it up and you had 0.03 seconds between un-pressing Deflect and pressing Attack down?

My answer to both of these questions is "Yes". I do a lot of accidental/unwanted combat arts. Hence, let's try to find a solution.

## 🤔 What does this mod do ?

Quite simple:

* When you hold Shift down - it equips Mortal Draw;
* When you let Shift up - it unequips Mortal Draw;

This way you can never accidentally press it since we need to hold a "modifier" key to be able to cast Mortal Draw.

So now you need to do Block+Attack while holding Shift down when you want to do a Mortal Draw. Bonus trick: in-game bind your block keyboard button to Shift: this way you only need to press Shift+Attack button in order to trigger Mortal Draw.

Funnily enough, this way it starts working in same fashion as Katanas' skill in Elden Ring/Dark Souls 3. I'm talking about "Unsheathe"-like weapon arts where you hold your skill button into pressing light attack. With my keybinds Wolf first goes into blocking stance and then we press light attack to release Mortal Draw. Very similar!

## 🔬 Installation

* Go to Releases, grab the archive.
* Paste it into the same folder with `sekiro.exe`
* If you get prompted to replace `dinput8.dll` (because you use Mod Engine or something):
    * Rename `dinput8.dll` to something like `shiftart.dll`
    * Chain load with mod engine - edit `modengine.ini`.
    * for other `dinput8.dll`-using mods refer to their chain-loading practices.
* Ready.

## 👐 Credits

As you can notice from forking note, I found that most of code-base already exists in other mod: [tmsrise/Sekiro-Weapon-Wheel](https://github.com/tmsrise/Sekiro-Weapon-Wheel) so all credits to them.

Also, I'm extremely noob in C++ myself. This mod probably can be done in 100x less lines and optimized, but I just took tmsrise's mod and added a few lines for my own desires. So I still have all the code that loads in weapon wheel, it just isn't fully used. Sorry about that.

So anyway, if anybody can help - please, I beg you, help me and teach me.

## 🗞️ ToDo

* Remove all unnecessary references to
