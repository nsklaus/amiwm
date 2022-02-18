# settings
< amiwm >  by Marcus Comstedt (marcus@mc.pp.se)


## Legal notices

This program is distributed as FreeWare.  The copyright remains with
the author.  See the file LICENSE for more information.

Amiga and Workbench are registered trademarks of AMIGA International Inc.

## Intro

This is amiwm, an X window manager that tries to make your workstation
look like an Amiga�.  "Why?" you ask.  Because I wanted it to.  So there!

## Compiling etc.

For instructions on building and installing, read the file INSTALL.

## Configuration

amiwm reads the file .amiwmrc in the directory pointed to by $HOME
when starting.  If this file does not exist, it reads the file
system.amiwmrc instead.

The configuration file can contain any of the following statements:

```
FastQuit {yes|no}
```

Specifies whether amiwm should quit directly when the Quit menu item
is selected, rather than popping up a requester.
(on/off or true/false can be used instead of yes/no.)

```
SizeBorder {right|bottom|both|none}
```

Specifies which border should be enlarged when a sizegadget is present.

```
ForceMove {manual|auto|always}
```

Enables you to move windows outside the perimeter of the root window
when pressing shift, when trying to drag at least 25% of the window
ofscreen, or always, respectively.

```
IconDir "path"
```

Specifies a directory in which amiwm will look for icons.

```
DefaultIcon "name"
```

Gives the filename of the .info file to use as a default icon.
It is relative to the IconDir.

```
IconPalette {system|magicwb|schwartz|"filename"}
```

Selects either the Workbench� default palette, or the MagicWorkbench
standard palette for use with icons.  The third option is a 16 color
palette used on the "Eric Schwartz Productions CD Archive".  Alternatively,
the filename of a PPM file representing the palette to use can be used.

```
ScreenFont "fontname"
```

Selects a font to use for windowtitles etc.

```
IconFont "fontname"
```

Selects a font for icontitles.

{detailpen|blockpen|textpen|shinepen|shadowpen|fillpen|filltextpen|
	backgroundpen|highlighttextpen|bardetailpen|barblockpen|
	bartrimpen} "colorname"

Modifies the colour scheme for the window manager.

```
CustomIconsOnly {yes|no}
```

This prevent applications to display their own icons when in iconified state.
Only icons defined in amiwmrc for each apps will be used.
Because apps own icon are too different from one eachother.
They comes in various sizes and themes. Some icons will be 32x32,
while some other will be a 128x128 or even bigger .. By using this option,
You have the possibility to decide which icon should be used for each app.  
If no custom icons are defined at all, the def_tool.info will be used for 
all iconified apps. Format is: style { class "myClass" icon "myIcon.info" }
To find out a given program's class, use "xlsclients -l" to list all opened
applications, and then use "xprop -id <0x0000000>" to list a 
given app's properties. Use either WM_CLASS or WM_ICON_NAME strings in amiwmrc.

```
ShortLabelIcons {yes|no}
```

This limit the length of the text for iconified programs. For example, if this 
option is activated, an iconified program text will be limited to 8 chars + ".."
Use this option if you don't want iconified program text to be loong strings..

```
TitleBarClock {yes|no}
```

Enables a clock in the titlebar.  It displays the date and time.

```
TitleClockFormat [<number>] "time string"
```

This lets you choose a new format to display the Title Bar Clock.
The time string is formatted with the standard strftime() parameters.
The default is "%c".  It has been found that "%a %b %e %Y   %l:%M %p" works
well too.  Number is the update interval in seconds.  

```
ToolItem "name" "command" ["hotkey"]
```

Adds an item in the Tools menu with the specified name, which executes
the command when selected.  A string containing a single uppercase letter
may be specified as the third argument, making this letter a hotkey for the
item.

```
ToolItem Separator
```

Inserts a separator bar in the Tools menu.

```
ToolItem "name" { <tool item commands> }
```

Create ToolItem:s in a submenu with the specified name.  It is not legal
to create a submenu inside another submenu.

```
Screen [<number>] "name"
```

Create a new screen with the specified name.  It will be placed below all
earlier created screens.  To move a window between two screens, iconify it
and drag the icon over to the other screen.  If the second argument is given,
this screen is placed on the corresponding X screen.

```
ModulePath "path(:path...)"
```

Sets one or more directories where amiwm will look for module binaries.
The default is AMIWM_HOME.  Note that the module binaries cannot be
shared amongst different architectures.

```
Module "name" ["initstring"]
```

Start a module with the specified name.  If initstring is specified, it
is sent to the module.  There are currently two modules shipped with
amiwm; Background and Keyboard.  These are documented in the file
README.modules.  If a module is limited to a single screen, like the
Background module, the screen that was created last is used.

```
InterScreenGap number
```

Sets the size in number of pixels of the "video DMA off" area that appears
between screens when they are dragged.

```
AutoRaise {yes|no}
```

Selects wheteher windows will automatically be moved to the front when
they are activated.

```
Focus {followmouse|sloppy|clicktotype}
```

Sets the policy by which windows are given focus.  Followmouse is the
default and means that the window that contains the mouse pointer will
have focus.  Sloppy means that the window that had focus last will keep
it when the pointer is not over a window.  ClickToType is the original AmigaOS
policy in which you have to explicitly give focus to windows by clicking
in them.


## Troubleshooting

The most likely reason for the windowmanager to crash is if it
can't find its default icon, or if this is not accepted as an
.info file.  If this happens, amiwm _will_ dump core.  So make
sure that either 'make install' runs successfully (in which case
amiwm will know where its icon is), or that the file .amiwmrc
contains a correct specification of the icon's location.

Don't mail me bug reports just because amiwm prints "Bad Window" on
stderr once in a while.  Redirect it to /dev/null or live with it.  :)

