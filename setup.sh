#!/bin/bash

git clone https://github.com/4n4k1n/local-network-messenger.git ~/local-network-messenger

read -p "Enter IP address: " ip_address

make -C ~/local-network-messenger

~/local-network-messenger/client $ip_address

rm -rf ~/local-network-messenger 
