# Pandash

Pandash is a simple shell written in c. It can handle all basic tasks that any other interactive shell can handle.

## Installation

Just make sure you have gcc and make installed, then simply run:

```bash
make all
```
For installation:
```bash
sudo make install
```

## Usage
After runing just type "help".
```bash
WELCOME TO PANDASH, A SIMPLE SHELL WRITTEN IN C
BUILT IN COMMANDS:
	help:
		IT WILL PRINT THIS PROMT
	cd [path?]:
		IT WILL CHANGE CWD TO 'PATH' OR $HOME
	exit:
		IT WILL TERMINATE THE CURRENT SHELL
	pandash_talk [YOUR_ID] [THE_OTHER_SHELL_ID]: (ID can be anything)
		GOES TO CHAT MODE AND YOU CAN CHAT WITH ONE OTHER PANDASH SHELL
```

## About
If you are willing to run a script - like the example i have provided - you can run "pandash scriptName" and for interactive mode just run "pandash".

You can also communicate with other pandash shells using "pandash_talk" built-in command, it will take you to chat mode and you can start sending messages, just make sure your provided IDs are not identical. For example if your id is COOL and the other shell's ID is COOLER your should run "pandash_talk COOL COOLER" and the other shell must run "pandash_talk COOLER COOL" (you choose your ID by typing this command)!

### More details
Each command you type to execute is handled by a child process and can be terminated using Cntrl+c. Piped commands are handled in the same manner but they exchange their stdin and stdout.

Named pipes, aka FIFO, is used for implementing communication functionality and a child process is responsible for handling "chat mode" so you can use Cntrl+c to exit from "chat mode".

Hope you enjoy! Use at your own risk :)
