#!/bin/sh


# Import the helper functions.
. ./timer_calculator.sh


# The application name or path to run
APPLICATION_PATH=$1

# Removed the '/app/blabla/' from the $1 argument name - https://regex101.com/r/rR0oM2/1
APPLICATION_PATH=$(echo $APPLICATION_PATH | sed -r "s/((.+\/)+)//")

# Removed the '.' from the $APPLICATION_PATH name.
APPLICATION_NAME=$(echo $APPLICATION_PATH | cut -d'.' -f 1)

# Get the application name
# qemu-system-i386 -smp 1 -m 262144k -nographic -no-reboot -fda  philosophers_dinner.img | tee philosophers_dinner.out
printf 'Running the command: make APPLICATION=$APPLICATION_NAME run\n'
make APPLICATION=$APPLICATION_NAME run

showTheElapsedSeconds "$0"

