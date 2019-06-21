# Unix-like-FileSystem
Course project for Operating System in Tongji University.
## Requirements
- Microsoft Visual C++ >= 11.0
## Usage
- Before use for the first time, run `fformat` to generate `MyDisk.img`.
```
fformat
```
- Run `cd` command to change the directory.
```
cd [dir]
```
- Run `mkdir` command to make a directory.
```
mkdir [dir]
```
- To list all files of the current directory, run:
```
ls
``` 
- Run `fopen` to open a file. If success, the file id `fd` will be provided.
```
fopen [dir]
```
- Run `fcreate` to create a file.
```
fcreate [dir]
```
- Run `fclose` to close a file.
```
fclose [fd]
```
- To read bytes of a file, run:
```
fread [fd] [nbytes]
```
- To write bytes to a file, run:
```
fwrite [fd] [nbytes] [string]
```
- To relocate the read-write pointer of a file, run:
```
fseek [fd] [offset] [ptrname]
```
- Run `fdelete` to delete a file:
```
fdelete [dir]
```
- To copy a file from Windows to the disk, run:
```
fmount [dir1] [dir2]
```
- To quit the file system, run:
```
quit
```


