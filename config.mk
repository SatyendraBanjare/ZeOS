#-------------------------
# TARGET
#-------------------------

TARGET := x86_64

#-------------------------
# Source Files
#-------------------------

#--------------------------------------------------------------------------------------------------
# TOOLS 
#
# specify the cross-compiler and assembler sources download them from https://toolchains.bootlin.com/
# Simply pass the path to those files
#--------------------------------------------------------------------------------------------------

TOOLCHAIN := ~/opt/x86-64-tool/bin/x86_64-linux
QEMU := qemu-system

CC := $(TOOLCHAIN)-gcc
LD := $(TOOLCHAIN)-ld
AS := nasm
AR := $(TOOLCHAIN)-ar
QEMU := $(QEMU)-$(TARGET)

#------------------------
# Flags
#------------------------

CCFLAGS		:= -std=gnu11 -I$(DIR_INCLUDE) -Qn -g \
		   -m64 -mno-red-zone -mno-mmx -mfpmath=sse -masm=intel \
		   -ffreestanding -fno-asynchronous-unwind-tables \
		   -Wall -Wextra -Wpedantic

LDFLAGS		:= -g -nostdlib -m64 -mno-red-zone -ffreestanding -lgcc \
		   -z max-page-size=0x1000


#####################
# Display colors
#####################
BLUE		:= \033[1;34m
YELLOW		:= \033[1;32m
NORMAL		:= \033[0m

SUCCESS := $(GREEN)[success]$(NORMAL)

