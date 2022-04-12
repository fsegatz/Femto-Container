# name of the example application
APPLICATION = bpf_tutorial

# Default is to build for the "native" target. Allows overriding
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../RIOT

# Include the bpf module
USEMODULE += bpf
USEMODULE += ztimer_usec
USEMODULE += xtimer

# Use the compiled increment.bin as binary object in the built
BLOBS += container/system/system.bin


# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 0

include $(RIOTBASE)/Makefile.include
