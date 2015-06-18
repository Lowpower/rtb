#! /bin/sh
killall -9 boomer
killall -9 spawn-fcgi

if [ "$1" = "kill" ];then
  exit 0
fi

SPAWNFCGI=/usr/local/bin/spawn-fcgi
EXEC=./boomer

sleep 1

$SPAWNFCGI -p 12395 -F 1 -- $EXEC
