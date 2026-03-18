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

Documentation will follow, once I get the codebase stable enough, and when things will
reach a stable way of compiling.

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
- **[Safe Exam Browser Integration](Sources/LXEB_SafeExamBrowserModule)**: Integration of
  SEB itself into LXEB, using the LXEB APIs