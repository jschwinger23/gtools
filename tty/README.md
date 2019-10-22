# gtools/tty

## 1. ptyfork.c

sshd from scratch

usage:

```bash
gcc ptyfork.c
tail -f output | nc -l [port] | ./a.out output
```
