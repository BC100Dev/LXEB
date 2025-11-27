# LXEB
LXEB stands for "Linux Exam Browser" and is a rewrite of SEB (Safe Exam Browser) for Linux.
However, instead of rewriting it in C# that SEB is also written in (and open source), I
decided to take over and rewrite it in C++ and Qt6, while also using Linux-native
components. In basic words, it is a cohesive rewrite of SEB that should bring the
functionality to Linux, no matter what desktop environment and what window manager you
use.

Documentation will follow, once I get the codebase stable enough, and when things will
reach a stable way of compiling.

**WIP**: Please note that LXEB is still a work in progress, so there won't be any current
usage / documentation on how to compile or how to get it running, considering the fact that
it is currently nowhere of actually getting a browser running. The best thing I could
really do is try to integrate a WebView renderer, but that's it, really. I also cannot
promise any timelines, considering that my primary focuses are
[OsintgramCXX](https://github.com/BC100Dev/OsintgramCXX) and
[AnlinxOS](https://github.com/BC100Dev/AnlinxOS).

## Why Rewrite
Originally, with how Linux's market share is still low and developers behind SEB refusing
to make a port for Linux, along with schools forcing the use of Windows or macOS for SEB
usage, Linux users are either forced to use school computers or have to use workarounds.
To combat this, I decided to make LXEB itself and make it work with the structure of SEB
itself… especially that my own school uses Moodle, and Moodle supports SEB, so I'm
integrating SEB support right into the main codebase of this project.

## Redesign choices
Originally, LXEB was designed to be only compatible with SEB. However, considering that
there are other lockdown browsers (like the Respondus LockDown Browser), I decided to
try and de-glue the code that I had before (even if there was little) and allow the
modularity for other Browser protocols. No, this still does not mean for anyone in trying
to mod them up, I will include Security protocols that developers can use for self-checks.

Also, since yall really hope that I finish this project (24 stars, 1 fork and 4 people are
watching, gawh-damn Google AI search, wtf did you do :skull:), I will try to get some
proper progress finally starting up. No promises though... 2069 :neutral_face:

## Codebase components
Instead of trying to fit up all SEB components into one single codebase, I decided to
split them up into these specific ones:

- **[LXEB_Main](Sources/LXEB_Main)**: The main executable of LXEB itself. This will be the
  main control for all the other components
- **[LXEB_API](Sources/LXEB_API)**: The API that components like SEB and other Secure
  Browser Providers can adapt upon
- **[Safe Exam Browser Integration](Sources/LXEB_SafeExamBrowserModule)**: Integration of
  SEB itself into LXEB, using the LXEB APIs