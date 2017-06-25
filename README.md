# TypexOS

Minimal OS Kernel written in C and Assembler

# Run

## 1. Assemble loader.s
```
nasm -f elf -o loader.o loader.s
```

## 2. Compile kernel.c
```
i586-elf-gcc -o kernel.o -c kernel.c -Wall -Wextra -Werror \ -nostdlib -nostartfiles -nodefaultlibs
```

## 3. Link liker.ld
```
i586-elf-ld -T linker.ld -o kernel.bin loader.o kernel.o
```

## 4. Create pad file
´´´
del pad
fsutil file createnew pad 750
´´´

## 5. Create floppy image
´´´
copy \b stage1+stage2+pad+kernel.bin floppy.img
´´´

## 6. Run Bochs
´´´
bochs.lnk
´´´

## 7. In Bochs:
´´´
kernel 200+18
boot
´´´
