#!/bin/perl -w


$fileExist = -e "./history.o";
if ( $fileExist ) {
    unlink "./history.o";
}

system ("g++ -static -Oz -Wall -z now history.cpp -o history.o")
