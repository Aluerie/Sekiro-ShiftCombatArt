# 💃 Sekiro mod - Rebind Combat Art to Shift modifier

* Did you ever press Deflect + Attack buttons together by accident in a game where you constantly spam Deflect and Attack buttons?.. which results in you stupidly standing here slashing Mortal Draw for 2 seconds.
* Did you ever watch your keyboard logs to find out that that one time you actually didn't fuck it up and you had 0.03 seconds between un-pressing Deflect and pressing Attack down?..

My answer to both of these questions is **"Yes"**. I've done a lot of accidental/unwanted combat arts. This mod tries to provide a solution to this problem.

## 🤔 What does this mod actually do?

* When you hold `Shift` down - it equips the Mortal Draw;
* When you let `Shift` up - it unequips the Mortal Draw;

Simply put, now you need to hold `Shift` down to be able to use the Mortal Draw. This way you can never accidentally press it.

So now you need to press 3 keys combo: Block+Attack while holding Shift down. Bonus trick to reduce to 2 keys: bind your Block keyboard button to Shift in game settings. This way you only need to press Shift+Attack since the game will start blocking for you automatically.

Funnily enough, with bonus trick the mod makes Sekiro *combat art* work in same fashion as Katanas' *weapon art* in Elden Ring/Dark Souls 3. I mean "Unsheathe"-like skills where you hold your skill button (and character takes ready-stance) into pressing light attack that releases the art like Moonveil/Frayed Blade/Uchigatana/etc. With my keybinds Wolf first goes into blocking stance (the ready-stance) and then we press light attack to release the Mortal Draw. Video Showcase (notice how combat art's small katana icon in the corner comes/fades away dynamically):

<https://github.com/Aluerie/Sekiro-ShiftCombatArt/assets/33165440/a9c09fb1-488a-4c8d-91e0-f62af9a5087f>

## 🔬 Installation

* Go to Releases tab, grab the archive.
    * If you want to use the latest version from master branch then go to `ShiftArt/ForRelease` and take all the content.
    * PS. Yes, currently, I'm super noob and the script won't run without all these images. Idk how to un-spaghetti the code to stop loading them. Any help is appreciated.
* Paste it into the same folder with `sekiro.exe`
* If you get prompted to replace `dinput8.dll` (because you use Mod Engine or something):
    * Rename `dinput8.dll` to something like `shiftart.dll`
    * Chain load with mod engine - edit `modengine.ini` according to instructions in the comments of that file.
    * for other `dinput8.dll`-using mods refer to their chain-loading practices.
* Ready. Now you can run Sekiro and try out holding shift into pressing Block+Attack buttons.

## 👐 Credits

As you can notice from forking note, I found that most of needed code-base already exists in other mod: [tmsrise/Sekiro-Weapon-Wheel](https://github.com/tmsrise/Sekiro-Weapon-Wheel) so all credits to them.

Also, I'm extremely noob in C++ myself. This mod probably can be done in 100x less lines, but I just took tmsrise's mod and edited a few lines for my own desires. So I still have all the code that loads in weapon wheel, it's just not fully used. Sorry about that.

So anyway, if anybody can help - please, I beg you, help me and teach me.

## 🗞️ ToDo

### 🧪 Soon^tm

* Remove all unnecessary references to stuff from the original mod like images, fonts, GUI, Wheel, Wheel settings, prosthetic tools

### 🌈 Dream tier todo list

* Allow customizing the keybind
* Allow customizing the combat art
* I don't know what happens when you don't have the Mortal Draw available yet.
* Publish at nexusmods

### ⚠️ Disclaimer

This disclaimer should probably be mentioned above.

* I tried to fork two similar projects: Weapon Wheel and HotkeySystem. This repo is a fork of Weapon Wheel.
* Currently, I seem to get better results with this repo (both repos have animation bug but this one meets it less frequently)

The related issues:

* issue for the bug in HotkeySystem Fork Repo: <https://github.com/Aluerie/SekiroHotkeySystem/issues/1>
* issue for the bug in this repo - <https://github.com/Aluerie/Sekiro-ShiftCombatArt/issues/1>
