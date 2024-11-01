# SortUniqWc
This C program demonstrates basic concurrent programming by creating a pipeline of three commands: `sort`, `uniq`, and `wc -l`. Each command runs in a separate process, with pipes used to pass data between them. This project shows the use of `fork()`, `pipe()`, `execlp()`, and `dup2()` to emulate Unix-style pipelines.
