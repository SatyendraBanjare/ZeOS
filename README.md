# ZeOS
Operating system Hobby Project 

## How To(s)
- Install required Cross Compiler.
- run `make` to build zeos.iso
- run `make run` to run the Zeos OS.
- run `make clean` to clean all unnecessary files

## Progress :
- [x] GDT setup
- [x] IDT & ISR setup
- [ ] Terminal 
  + [ ] Terminal Scrolling
  + [ ] Clear Screen Command (implement shortcut if possible)
  + [ ] Implement delete function for deleting backways
  + [ ] Implement C-A and C-E (endofline)
  + [ ] Implement <- | -> arrows !
- [ ] basic keyboard input
- [ ] Heap
- [ ] InitRD
- [ ] VFS
  + [ ] Read- Write Capabilities
  + [ ] Directory Traversal ??
- [ ] Scheduling
- [ ] User_Mode
- [ ] PCI ??
- [ ] Connectivity
  + [ ] basic Algos
  + [ ] Protocol Implementation
- [ ] Programs
  + [ ] Alarm
  + [ ] Mini REPL-interpreter
  + [ ] Calculator


### Resources

- https://www.cs.bham.ac.uk/~exr/lectures/opsys/10_11/lectures/os-dev.pdf
- https://github.com/cfenollosa/os-tutorial
- https://littleosbook.github.io/
- https://wiki.osdev.org/
- http://www.jamesmolloy.co.uk/tutorial_html
- https://www.cs.utah.edu/flux/oskit/examples.html
- http://os.inf.tu-dresden.de/fiasco/

File System : 
- https://wiki.osdev.org/VFS 
- http://e2fsprogs.sourceforge.net/ext2intro.html
- https://www.nongnu.org/ext2-doc/ext2.html

Using Grub :
- https://www.dedoimedo.com/computers/grub-2.html
- https://blog.fpmurphy.com/2010/06/grub2-modules.html
- https://wiki.osdev.org/Writing_GRUB_Modules
