# Start here — install, build, run

This is a C course, not a Python web application. The simplest supported setup is
Ubuntu Linux. Windows users run the same Linux tools inside WSL2.

## Windows setup

1. Open **PowerShell as Administrator**. Run:

   ```powershell
   wsl --install -d Ubuntu
   ```

2. Restart Windows if requested, then open **Ubuntu** from the Start menu.
   Complete the first-run Linux username/password setup. Password characters do
   not appear while you type.
3. In PowerShell, inspect the installed distributions:

   ```powershell
   wsl --list --verbose
   ```

   The Ubuntu entry should use version 2. If necessary, substitute the exact
   distribution name from the list:

   ```powershell
   wsl --set-version Ubuntu 2
   ```

Installation prerequisites and troubleshooting are in
[Microsoft's WSL guide](https://learn.microsoft.com/en-us/windows/wsl/install).
WSL installation requires supported Windows and enabled virtualization; managed
computers may need their administrator to enable these features.

## Open the course

Download and extract the ZIP with Windows Explorer. Copy the extracted
`socket-programming-zero-to-advanced` folder into the Ubuntu home directory for
convenient builds. For example, in Ubuntu (replace `YourName` with the Windows
account folder):

```bash
cp -r /mnt/c/Users/YourName/Downloads/socket-programming-zero-to-advanced ~/socket-programming-zero-to-advanced
cd ~/socket-programming-zero-to-advanced
```

If extraction created an extra outer folder, copy the inner folder containing
`Makefile` and `README.md`. To check your location:

```bash
pwd
ls
```

On native Ubuntu, use the file manager to extract the archive, open a terminal in
that folder and continue below. Paths containing spaces must be enclosed in
quotes. Do not type the placeholder `YourName` unchanged.

## Install the basic tools

These commands run in **Ubuntu**, not PowerShell:

```bash
sudo apt update
sudo apt install -y build-essential python3
cc --version
make --version
python3 --version
make -j2
```

The last command compiles 31 executables into `build/`. C11 and POSIX.1-2008 are
used. `-Wall -Wextra -Wpedantic -Werror` makes compiler warnings fail the build.
The course was verified with GCC 13 on Ubuntu; see [VERIFICATION.md](VERIFICATION.md)
for the actual recorded environment and checks.

## Your first two-terminal experiment

In **Ubuntu terminal A**, from the repository root:

```bash
./build/tcp_server 9000
```

It prints a listening message and waits. This pause is expected: `accept()` is
waiting for a client. Leave the terminal open.

Open **Ubuntu terminal B** in the same distribution. Change to the same folder:

```bash
cd ~/socket-programming-zero-to-advanced
./build/tcp_client 127.0.0.1 9000 "Hello, sockets!"
```

The client prints `Hello, sockets!`. The server exits after this connection ends.
Run the server again before repeating. `127.0.0.1` means this Linux networking
environment; both processes must use the same port.

For a server that accepts clients repeatedly:

```bash
./build/iterative_server 9000
```

Press Ctrl-C to stop a running server. Ctrl-D means end of terminal input in the
chat and framed-client examples. In a WSL terminal, use Ctrl-Shift-C/Ctrl-Shift-V
for copying/pasting if the terminal reserves Ctrl-C for interruption.

## Verify your installation

```bash
make test
make check-docs
```

The tests start their own loopback servers on temporary ports and stop them
again. An IPv6 test may be skipped if the local host has IPv6 disabled. Passing
here demonstrates local correctness; it does not measure Internet reliability.

Optional debugging tools:

```bash
sudo apt install -y iproute2 iputils-ping net-tools lsof tcpdump strace curl
```

Wireshark is optional; command-line packet capture is sufficient for the labs.
Use the Ubuntu loopback interface `lo` for traffic between the course examples.

## First-run troubleshooting

| What you see | What to do |
|---|---|
| `make: command not found` | Install `build-essential` inside Ubuntu. |
| `No rule to make target` / missing Makefile | Change to the extracted repository root. |
| `./build/...: No such file` | Run `make`; these Linux binaries are created locally. |
| `Connection refused` | Start the server first and match its address/port. |
| `Address already in use` | Stop your previous server or choose another port in both terminals. |
| No output after starting a server | Usually correct: it is waiting for a client. |
| No broadcast reply under WSL | Broadcast depends on virtual networking; use the manual UDP lab. |
| `epoll` headers unavailable | Use Ubuntu/WSL2; the full build is Linux-specific. |

Next: [Chapter 00](00-prerequisites/README.md). All subsequent shell commands
assume the repository root unless explicitly stated otherwise.
