# SmallShell-Smash

A Unix shell is a command-line interpreter or shell that provides a command-line user interface for Unix-like operating systems https://en.wikipedia.org/wiki/Unix_shell


Smash, is a Small Linux shell, which will support only a subset of its commands.

## Installation:
```
make
./smash 
```

## Smash commands:

**Change Prompt Command:**
```
chprompt <new-prompt>
```
**Showpid Command:**
```
showpid
```
**PWD Command:**
```
pwd
```
**Change Directory Command:**
```
cd <new-path>
```
**Jobs Command:**
```
jobs
```
**Kill Command:**
```
kill -<signum> <job-id>
```
**Foreground Command:**
```
fg <job-id>
```
**Background Command:**
```
bg <job-id>
```
**Quit Command:**
```
quit [kill]
```
**External Commands:**
```
<command> [arguments]
```
**IO Redirection:**
```
command > output-file
```
**Pipes:**
```
command1 | [&] command2
```
**Copy Command:**
```
cp <old-file-path> <new-file-path>
```
**Signals:**
```
Ctrl-Z and Ctrl-C
```


## Example:
```
smash> chprompt new_prompt
new_prompt> chprompt
smash> showpid
smash pid is 17031
smash> pwd
/home/Gil/smash/
smash> cd /home/Gil
smash> pwd
/home/Gil
smash> sleep 100&
smash> sleep 200&
[1] sleep 100& : 17032 2 secs
[2] sleep 200& : 17033 1 secs
kill -9 1
signal number 9 was sent to pid 17032
smash> showpid > pid.txt
smash> cat pid.txt
smash pid is 17031
smash> cp pid.txt file.txt
smash: pid.txt was copied to file.txt
smash> sleep 1000
^Csmash: got ctrl-C
smash: process 17034 was killed
smash> quit kill
smash: sending SIGKILL signal to 2 jobs:
17032: sleep 100&
17033: sleep 200&
```
