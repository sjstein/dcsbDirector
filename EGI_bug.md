# CDU EGI Display bug

During the development of dcsbDirector, an obscure bug was uncovered when navigating to the EGI system screen on the CDU

When the operator would choose the SYS display, then drill down to the EGI sub-screen, the DCS-BIOS stream would not 
send any more updates of the screen state. With the help of a (more experienced) developer, it was discovered that 
a lua file from Eagle Dynamics had a bug within. 

### NOTE 1: 
It is **NOT** recommended to modify this file as it is outside of the "user scope". Modify at your own peril.

Realize that this file may be overwritten/changed without warning by ED.

### NOTE 2: 
This screen (SYS -> EGI) is completely cosmetic AFAIK, so not having it available on your CDU display is likely
completely OK.

In any event, as of 27-July-2022 the file with the bug is located in:
~(DCS-Install-Directory)/Mods/aircraft/A-10C_2/Cockpit/Scripts/CDU/CDU.lua

The line in question is #114

Original: `set_page_name({CDU_MASTER,CDU_SYS,CDU_EGI}                        ,"CDU_EGI")`

change to: `set_page_name({CDU_MASTER,CDU_SYS,CDU_EGI1}                        ,"CDU_EGI")`

_This also shows up for the regular A-10, so the same idea holds for that aircraft_