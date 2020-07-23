#!/bin/bash
echo -n -e "\033[1;32mIniciando GKLog: logando em /dev/tty12.\033[0m\n"
exec /usr/bin/tail -f /var/log/syslog | /usr/bin/gklog - > /dev/tty12 &
