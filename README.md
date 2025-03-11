# FreeMyXe

by Emma / InvoxiPlayGames

Xbox 360 executable designed to apply freedom-unlocking patches from xeBuild
to the kernel and hypervisor at runtime. For use with
[Xbox360BadUpdate](https://github.com/grimdoomer/Xbox360BadUpdate).

Nowhere near finished or ready or stable or anything, but might be good enough
for some.

> [!NOTE]
> This is not, and never will be, a "softmod" or CFW - all patches are temporary
> until you reboot!

> [!CAUTION]
> **DO NOT** run any homebrew that modifies any file on or installs anything to
> the NAND, and **DO NOT** try to flash a modified NAND. **YOU WILL BRICK YOUR
> CONSOLE!**

> [!WARNING] 
> Completely disconnect from Xbox Live and the internet before running BadUpdate
> and do NOT reconnect, and do NOT run homebrew with an Xbox Live-enabled profile.
> **You could risk a ban!**

## Features

* Displays CPU key front-and-center. Write that down!
* Allows unsigned content (homebrew, like XeXMenu) to be launched and loaded
  from the dashboard.
* Lets homebrew access the hypervisor and CPU key as they would be able to on
  a modded console. (useful for Simple 360 NAND Flasher, XeLLLaunch, etc)
* Allows more homebrew be able to be launched without erroring back to
  dashboard.
* Fixes the ring of light / controller syncing bug after using BadUpdate.
* Allows launching `xell-1f.bin` from the `BadUpdatePayload` folder.
* Patches USB controller authentication check (a-la "UsbdSecPatch") to let
  certain compatible controllers and adapters work. That's for you, Mario.

## TODO

* Actual patch loader
* Daaaashlaaaaaaunch?

## Credits

* ikari's original freeBOOT
* cOz et al. for xeBuild 17559 patches and Dashlaunch
  (see also https://www.xbins.org/nfo.php?file=xboxnfo2430.nfo,
  https://www.xbins.org/nfo.php?file=xboxnfo2431.nfo)
* RGLoader

(let me know if I got anything wrong!)
