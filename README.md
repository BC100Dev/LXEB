# LXEB
LXEB stands for "LinuX Exam Browser" and is a rewrite of the well-known SEB, short for
"Safe Exam Browser". I decided to take it up a notch and rewrite it in C++ with Qt6
instead. Considering that SEB is also 
[open-source](https://github.com/SafeExamBrowser/seb-win-refactoring), I will be taking
major source code from there and get AI (specifically Claude) to help me rewrite the
necessary C# parts into C++. No I'm not going to vibe-code the hell out of LXEB because
knowing AI and the current technology, along with the risks of it writing Security
modules, it is well known to write exploitable code. I will be taking double looks.
Maybe even triple looks, just to be safe.

LXEB will also deliver the ability to put other Exam Browsing Providers through the APIs,
which means that the native tools that allows the Lockdown on Linux-based Desktop systems.
Integration through the APIs should be a seamless way of porting existing and newer ones
into LXEB itself. The application itself will take care of registering things properly,
along with the needed protocols and such. Yes, SEB has the URL protocol of `sebs://`. I
know it because my school uses Moodle and with exams that require SEB, the URL does say
with that protocol.

For easy maintenance, I will be putting the external parts of both of the codebases
(SEB for Windows, SEB Server) into the `External` path (referenced as Git submodules),
making my C# exploration a bit easier... and somewhat shittier because hello CLion 😂

---

## Temporary Archival
Considering my recent movements into closed-source development, I am temporarily
archiving this project. It is mainly to refocus and switch the active priorities, as
managing a thousand projects gets tiring. I already got OsintgramCXX over here, along
with AnlinxOS, tDEX, OpenMDJ, VPN Profile Manager and many others too. Another reason is
to slow the hype down because I strongly believe that I may or may not have overhyped my
own projects that I have listed. This also includes another dead project of mine, 
[aboawt](https://github.com/BC100Dev/aboawt). Anyway, right now, I will be focusing on
developing [novihook.net](https://novihook.net) (don't worry about the certificates, I
will deal with them on the public release, considering that Plesk decided to bitch around
with their own certificates. Damn 💀). Novihook will end up being a better Webhook
service. This shared notice will be put on all the relevant / active repos that I deem
to be in the priorities.

Not all hope is lost. Once I get Novihook finished, along with another desktop app into
commercial use (in an attempt to make some money), I will unarchive these repos. However,
in order to keep my focus alive, I will be making polls, as to what you guys think I
should focus on first after the first two (Novihook, I.T.) gets released. Basically,
instead of battling up multiple repos and trying to get the primary focus on, I will
progress into making the first one and make a public release. I would assume that
considering the amount of stars on LXEB, y'all probably want me to finish LXEB first
(aka. this one) before the others.

Up until then... let's cook! 😎

---

**WARNING**: If you try to run LXEB as-is, you will get locked out. The tests of locking
people out in VT10 (aka. performing Ctrl+Alt+F10) has been achieved. Since there is
nothing to render in that compositor, you will get locked out due to lack of
functionality. As a last resort, I wrote a few lines that allows SysRq executions. This
means to free yourself, you will have to perform the REISUB (Raw, tErminate, kIll, Sync,
Unmount, reBoot) using Alt + SysReq key combinations. Each letter has to be typed out
individually. This means that you have to press Alt+SysRq+R and so on. Until I get
something properly rendered, I will deal with the rest, so that you no longer get
forcefully locked in. For now, I am declaring this as a temporary warning, until the
window rendering part is finished.

**WIP**: Please note that LXEB is still a work in progress, so there won't be any current
usage / documentation on how to compile or how to get it running, considering the fact that
it is currently nowhere of actually getting a browser running. The best thing I could
really do is try to integrate a WebView renderer, but that's it, really. I also cannot
promise any timelines, considering that my primary focuses are
[OsintgramCXX](https://github.com/BC100Dev/OsintgramCXX) and
[AnlinxOS](https://github.com/BC100Dev/AnlinxOS). My primary focus, as of now, is on my
final exams, which also delays things further, considering that I am still an apprentice.
If I had to estimate a rough release date, it would be somewhere in 2027 and 2028. And
yes, I can confirm that once I am finished with this whole apprenticeship thing, I will
switch focus and finally start making proper progress on LXEB. This means that the wait
is slowly getting to a stop because from September or so, I will be grinding hard on this
native port.

## Why Rewrite
Considering that the developers behind SEB do not have any plans to develop a port for
Linux desktops, I decided to make a native port for Linux. This will allow schools to
finally remove the enforcement on Windows or macOS, just to get Safe Exam Browser running.
Linux users would usually have to use a school computer or have to use workarounds, just
to get SEB working. Considering that my school also uses Moodle, I'm integrating SEB
support right into the main codebase of this project.

Oh, and I know that the Linux Desktop Market share is still low, but considering that
I'm a part of that Linux Desktop Userbase, I do like to use Safe Exam Browser on my
machine. Yes, I tried using `wine`. It whined even harder.

## Work & Redesign Choices
Originally, LXEB was designed to be SEB-only compatible. However, considering that there
are other lockdown browsers, like the Respondus Lockdown Browser, I decided to try and
make the ability of making exam-based browser runnable for Linux by integrating the
APIs. This basically means that if developers behind Respondus or other exam-based
browsers want to expand for Linux, the LXEB APIs are the best for the call. No, this
still does not mean that anyone trying to mod it up because it will still include
Security Protocols. I see you, the people that love to reverse-engineer programs and
possibly exploit it.

## Codebase components
Instead of trying to fit up all SEB components into one single codebase, I decided to
split them up into these specific ones:

- **[LXEB_Main](Sources/LXEB_Main)**: The main executable of LXEB itself. This will be the
  main control for all the other components
- **[LXEB_API](Sources/LXEB_API)**: The API that components like SEB and other Secure
  Browser Providers can adapt upon
- **[Safe Exam Browser Integration](Sources/SEB_Integration)**: Integration of
  SEB itself into LXEB, using the LXEB APIs