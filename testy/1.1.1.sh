#!/bin/bash
cp client.cfg client.cfg.old
./rso-client
if test `diff client.cfg client.cfg.old | grep serversCount= | wc -l` -eq 2;
then
	echo ok
else
	echo nie ok
	exit 1
fi

