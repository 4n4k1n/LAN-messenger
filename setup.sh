#!/bin/bash
git clone https://github.com/4n4k1n/LAN-messenger.git ~/LAN-messenger

if [ $# -eq 0 ]; then
    read -p "Enter IP address: " ip_address
else
    ip_address=127.0.0.1
fi

make -C ~/LAN-messenger
~/LAN-messenger/client $ip_address
rm -rf ~/LAN-messenger