#!/bin/bash
cp client.cfg client.cfg.old
./rso-client
if test `diff client.cfg client.cfg.old | grep serversCount= | wc -l` -eq 2;
then
	echo ok
	exit 0
else
	echo nie ok
	exit 1
fi

