# TARGET
TARGET := x86_64

# TOOLS ::
# specify the cross-compiler and assembler sources
#	download them from https://toolchains.bootlin.com/
#	Simply pass the path to those files
CC := ~/opt/x86-i686-tool/bin/i686-buildroot-linux-gnu-gcc
LD := ~/opt/x86-i686-tool/bin/i686-buildroot-linux-gnu-ld
AS := nasm
AR := ~/opt/x86-64-tool/bin/x86_64-linux-ar
QEMU := qemu-system-x86_64

# Source Files

# FLAGS

# BUILD


#####################
# Display colors
#####################
BLUE		:= \033[1;34m
YELLOW		:= \033[1;32m
NORMAL		:= \033[0m

SUCCESS := $(GREEN)[success]$(NORMAL)

