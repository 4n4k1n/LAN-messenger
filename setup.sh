#!/bin/bash
git clone https://github.com/4n4k1n/local-network-messenger.git ~/local-network-messenger

if [ $# -eq 0 ]; then
    read -p "Enter IP address: " ip_address
else
    ip_address=$1
fi

make -C ~/local-network-messenger
~/local-network-messenger/client $ip_address
rm -rf ~/local-network-messenger