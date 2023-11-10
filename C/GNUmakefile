################################################################################
################################################################################
#	GNUmakefile
################################################################################
################################################################################

CFLAGS = -Wall -O -ggdb -g3 -I/opt/ASI_linux_mac_SDK/include

# Only two architectures are considered...  aarch64 (armv8) and x86_64 (x64).

ifeq ($(shell uname -m), x86_64)
LDFLAGS = -L/opt/ASI_linux_mac_SDK/lib/x64
else ifeq ($(shell uname -m), aarch64)
LDFLAGS = -L/opt/ASI_linux_mac_SDK/lib/armv8
else
$(error 'uname -m' must be either x86_64 or armv8!)
endif

all: picapture

picapture: picapture.c
	gcc $(CFLAGS) $(LDFLAGS) -o $@ $< -lASICamera2

clean:
	rm -f *~ picapture
