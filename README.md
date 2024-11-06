# LXEB
LXEB, as it stands for, "LinuX Exam Browser", is a Safe Exam Browser equivalent for
Linux systems. However, instead of porting the C# code into Linux equivalents, I
decided to go the lengthy route, and make it completely rewritten in C++, with Qt,
and its specific Linux counterparts.

As of now, there are no documentations, but I'll try to document the parts on what
it includes because I like documenting it to the lengths, where no one wants to
even read in the first place.

- Application: The application on itself that Client and Server possibly communicates
  with.
- Browser: Powered with Qt, the browser itself.
- Client: the main executable for Clients.
- Communication: possibly Network related
- Configuration: the default SEB configuration port
- ConfigurationJSON: a newly made JSON counterpart for this specific port
- Core: Application's main core implementation
- I18n: Internationalization for multi-lingual support
- LinuxApi: The connectors that make certain connections work with Linux
- Lockdown: yes, the lockdown feature in itself
- Logging: Logging related features
- Monitoring: Monitoring related features
- Proctoring: A library that disallows cheating during exams
- ResetUtility: An executable to reset the configurations
- Runtime: Application's Executable Runtime
- Server: Server executable that allows the hosting of SEB/LXEB servers
- Service: LXEBs Service to monitor the system with higher privileges than the
  current user running LXEB
- SystemComponents: Components related to the system configurations, including VM
  checks
- UserInterface: Powered with Qt, powering up the main user interface

Along with that, there is the `Installer` that you can find. It is responsible for
installing, modifying and uninstalling LXEB itself.