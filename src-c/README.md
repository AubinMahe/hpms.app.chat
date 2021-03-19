# YACA in std11 C #

**Y**et **A**nother **C**hat **A**pplication... :-)

## Commands ##

This program read its standard input for commands and print on the standard output the messages received from the network.

Commands are:
* debug, argument is `true` or `false`
* join, argument are `pseudo` a string, `mcast` an IP v4 address in range 239.0.0.0/8 and `port` greater then 1024
* send, argument is a text: the sender identifier, pseudo and text are published on the network to the group
* leave, without argument: leave the group
* shutdown, without argument: leave the group, free the resources, ends the program

## Outputs ##

The printed messages are three lines each:
* A unique network identifier
* The end-user pseudo
* The text sent

## Design ##

The main thread is dedicated to commands parsing accordingly with the grammar `<single word command> <comma> <arguments comma separated>`

A second thread will be created on `join` command dedicated to network operations.
