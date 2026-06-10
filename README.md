# TNFS Commander

A Norton Commander-style two-panel file manager for 16-bit DOS, built around the [TNFS](http://fujinet.online/tnfs) protocol and the [mTCP](http://www.brutman.com/mTCP/) networking stack.

![TNFS Commander screenshot placeholder](docs/screenshot.png)

## What it does

TNFS Commander gives you a classic dual-panel interface on a DOS machine:

- **Left panel** — your local filesystem. Browse drives and directories, see file sizes.
- **Right panel** — a remote TNFS server over UDP. Navigate directories just like a local drive.

It is designed for retro computers running MS-DOS or compatible, including 8088/8086 machines.

## Requirements

### To build
- [Open Watcom](https://github.com/open-watcom/open-watcom-v2) C/C++ compiler (`wpp`, `wcc`, `wlink`)
- [mTCP](http://www.brutman.com/mTCP/) source tree (expected at `../mTCP/`)

### To run
- MS-DOS or compatible operating system
- A packet driver for your network card
- mTCP configured (`MTCPCFG` environment variable pointing to your `mtcp.cfg`)
- A TNFS server to connect to (e.g. [FujiNet](http://fujinet.online) or a local `tnfsd` instance)

## Building

```
make
```

The compiled executable ends up in `build/tnfscmdr.exe`.

## Configuration

Copy `TNFSCMDR.CFG` to the same directory as `TNFSCMDR.EXE` on your DOS machine and edit it to add your TNFS servers:

```ini
[server]
name=FujiNet
server=tnfs.fujinet.online
mount=/
port=16384

[server]
name=Local
server=192.168.1.10
mount=/msdos
port=16384
```

## Usage

```
TNFSCMDR.EXE
```

### Key bindings

| Key | Action |
|-----|--------|
| `Tab` | Switch between left and right panel |
| `Arrow keys` | Move cursor / scroll |
| `Enter` | Open directory |
| `D` | Select local drive |
| `S` | Select / switch TNFS server |
| `Esc` | Quit |

## Features

- Two-panel layout, 80×25 text mode
- Real local directory listing with `..` navigation
- TNFS remote directory listing and navigation
- Directories sorted before files, both alphabetically
- File sizes shown in bytes with thousands separator (1.234.567)
- Always-visible proportional scrollbars (up to 64 items per panel)
- Current directory shown below each panel header
- Drive selection dialog showing all available local drives
- Server selection dialog with current server pre-selected
- Working directory saved and restored on exit
- Ctrl+C fully ignored — exit cleanly with Esc

## Project structure

```
src/        C/C++ source files
include/    Header files
build/      Compiled output (not in git)
makefile    Build rules
TNFSCMDR.CFG  Example configuration
```

## Dependencies

- `mTCP` — TCP/IP stack for DOS by Michael Brutman (not included, referenced via `../mTCP/`)
- `tnfs.c` / `tnfs.h` — TNFS protocol implementation

## License

MIT
