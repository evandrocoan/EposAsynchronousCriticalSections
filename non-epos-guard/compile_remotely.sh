#!/bin/sh

pushd `dirname $0` > /dev/null
SCRIPT_FOLDER_PATH=`pwd`
popd > /dev/null

# EPOS_COMPILER_MACHINE_PASS=admin123
# EPOS_COMPILER_MACHINE_ADDRESS=linux@127.0.0.1
TARGET_DIRECTORY='~/OperatingSystems/NonEpos'

# Just ensures the directory is created
sshpass -p $EPOS_COMPILER_MACHINE_PASS ssh $EPOS_COMPILER_MACHINE_ADDRESS mkdir -p $TARGET_DIRECTORY

# Get the application name
APPLICATION=$1


# The command which will actually send the files
SYNCHRONIZER_COMMAND="sshpass -p $EPOS_COMPILER_MACHINE_PASS rsync -rvu $SCRIPT_FOLDER_PATH/* $EPOS_COMPILER_MACHINE_ADDRESS:$TARGET_DIRECTORY"

# Send the initial files
eval $SYNCHRONIZER_COMMAND

# REMOTE_COMMAND_TO_RUN="cd /home/evandro.coan/OperatingSystems/TeachingEpos;
# REMOTE_COMMAND_TO_RUN="PATH=/usr/local/ia32/gcc-7.2.0/bin:\$PATH;
REMOTE_COMMAND_TO_RUN="
cd $TARGET_DIRECTORY;
printf '\nThe current directory is:\n'; pwd;
printf '\nThe current path is: %s\n' \$PATH;
printf 'Running the command: g++ -ggdb -O0 -o test $APPLICATION -m32 --std=c++11  -lpthread && ./test;\n';
g++ -o test $APPLICATION -lpthread -m32 && ./test;"

sshpass -p $EPOS_COMPILER_MACHINE_PASS ssh $EPOS_COMPILER_MACHINE_ADDRESS $REMOTE_COMMAND_TO_RUN;

