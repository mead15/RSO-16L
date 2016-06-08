#!/bin/bash
# wczytywane są po kolei linie z konfiguracji, sekcja serwera to po kolei: [serverN], ip=adresIp, port=Port.
# Jeśli napotkamy na linię z "port", to adres ip serwera jest w poprzedniej linii. Zmienna nr służy do
# przeskoczenia pierwszego serwera

./rso-client
serwer=""
ip=""
nr=1
cat client.cfg | while read x
do
if printf -- '%s' $x | egrep -q -- "port"
then
	if test $nr -eq 2;
	then

		echo "Wylacz serwer o $ip i wcisnij Enter"
		read _ </dev/tty
		#odpalenie testu 1.1.1.sh - powinna się zmienić lista serwerów (wypadnie 1 wpis o odłączonym)
		if ./1.1.1.sh
		then
			echo "test 1.1.2 zaliczony"
		else
			echo "test 1.1.2 niezaliczony"
		fi
		exit 0
	else
		nr=`expr $nr + 1`
	fi
else
	serwer=$ip
	ip=$x
fi
done
