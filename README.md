# 💃 Sekiro mod - Rebind Combat Art to Shift modifier

* Did you ever press Deflect + Attack buttons together by accident in a game where you constantly spam Deflect and Attack buttons?.. which results in you stupidly standing here slashing Mortal Draw for 2 seconds?
* Did you ever watch your keyboard logs to find out that that one time you actually didn't fuck it up and you had 0.03 seconds between un-pressing Deflect and pressing Attack down?..

My answer to both of these questions is **"Yes"**. I've done a lot of accidental/unwanted combat arts. This mod tries to provide a solution to this problem.

## 🤔 What does this mod actually do?

* When you hold `Shift` down - it equips the Mortal Draw;
* When you let `Shift` up - it unequips the Mortal Draw;

> [!WARNING]
> Currently, only Mortal Draw is supported and there is no choosing menu like in forked mod.

Simply put, with this mod you need to hold `Shift` down to be able to use the Mortal Draw. This way you can never accidentally press it.

So now you need to press 3 keys combo: Block+Attack while holding `Shift` down.

> [!NOTE]
> **Bonus trick** to reduce this to just 2 keys: bind your Block keyboard button to `Shift` in game settings. This way you only need to press `Shift`+Attack since the game is going to start blocking automatically.

Funnily enough, with bonus trick the mod makes Sekiro *combat art* work in same fashion as Katanas' *weapon art* in Elden Ring/Dark Souls 3. I mean "Unsheathe"-like skills where you hold your skill button (and character takes ready-stance) into pressing light attack that releases the art like Moonveil/Frayed Blade/Uchigatana/etc. With my keybinds Wolf first goes into blocking stance (the ready-stance) and then we press light attack to release the Mortal Draw.

Video Showcase (notice how combat art's small katana icon in the corner comes/fades away dynamically):

<https://github.com/Aluerie/Sekiro-ShiftCombatArt/assets/33165440/a9c09fb1-488a-4c8d-91e0-f62af9a5087f>

## 🔬 Installation

1. Go to Releases tab, grab the archive.
    * If you want to use the latest version from master branch then go to `ShiftArt/ForRelease` and take the `.dll` and `ini` files.
2. Extract the archive, copy `shift_art.dll`, `shift_art.ini` and paste them into the same folder with `sekiro.exe`
3. Now we need to secure `.dll` file loading. There are two ways about it.
    1. Chain-loading - load `dll` files in a chain matter. Thus, let some other mod load my `dll`.
       * Most likely, you already have [Mod Engine](https://www.nexusmods.com/sekiro/mods/6). If not, install it. It's a must have for 95% mods for Sekiro 😀 anyway;
       * In this case, edit `modengine.ini`: change the line `chainDInput8DLLPath=""` to `chainDInput8DLLPath="\shift_art.dll"`
       * Now we have a following chain: modengine's `dinput8.dll` ➡️chain-loads `shift_art.dll`
           * If there is nothing else needed or no problems then you are ready to go ✅.
       * However, if you already have some other mod here then refer to its instructions about chain-loading and load `shift_art.dll` with it (if it's supported by that other mod).
    2. Bare `dinput8.dll`
        * if for some reason my mod is the only mod you ever needed then:
        * Rename `shift_art.dll` into `dinput8.dll`. Sekiro will automatically load it when launching.
4. Bonus: if you want to chain-load with my mod then use `shift_art.ini` and put `"chainDllName=othermod.dll"` in here.
    * Note, that unlike `modengine.ini` - there is no need for `\` before the file name.

## 👐 Credits

As you can notice from forking note, I found that most of needed code-base already exists in other mod: [tmsrise/Sekiro-Weapon-Wheel](https://github.com/tmsrise/Sekiro-Weapon-Wheel) so all credits to them.

Also, I'm extremely noob in C++ myself. This mod probably can be done in 100x less lines, but I just took tmsrise's mod and edited a few lines for my own desires. So I still have all the code that loads in weapon wheel, it's just not fully used. Sorry about that.

So anyway, if anybody can help - please, I beg you, help me and teach me.

## 🗞️ ToDo

### 🧪 Soon^tm

* Nothing really

### 🌈 Dream tier todo list

* Clean code further up - we have a lot of references to functions from WeaponWheel that are not used.
    * variables like settings
    * prosthetic switch left-over
    * wheel slots amount
    * some gui hooks and rendering
    * mouse input catching
    * etc
* Allow customizing the keybind (including controller users)
* Allow customizing the combat art
* I don't know what happens when you don't have the Mortal Draw available yet.
* Publish at nexusmods
* Learn C++ and actually improve the code (comment at least)

### ⚠️ Disclaimer

* I tried to fork two similar projects:
    * Weapon Wheel - current repo.
    * HotkeySystem - [Aluerie/SekiroHotkeySystem](https://github.com/Aluerie/SekiroHotkeySystem)
* But both repos have the same annoying animation bug (that will kill you once or twice!).
    * However, it occurs less when using the mod provided with this repo.
