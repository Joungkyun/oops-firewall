#!/bin/sh

# version numbering
VER="3.1.2"

# Root user�� �������� �۵����Ѿ� ��
#
if [ "$UID" != "0" ]; then
  if [ "$LANG" = "ko" ]; then
    echo "���� - Root �� �������� �۵��ؾ� �մϴ�."
  else
    echo "U are not super user!"
  fi
  exit 1
fi


/sbin/iptables -Z
/sbin/iptables -P INPUT ACCEPT
/sbin/iptables -F INPUT
/sbin/iptables -P FORWARD ACCEPT
/sbin/iptables -F FORWARD
/sbin/iptables -P OUTPUT ACCEPT
/sbin/iptables -F OUTPUT
/sbin/iptables -t nat -Z
/sbin/iptables -t nat -P PREROUTING ACCEPT
/sbin/iptables -t nat -F PREROUTING
/sbin/iptables -t nat -P POSTROUTING ACCEPT
/sbin/iptables -t nat -F POSTROUTING
/sbin/iptables -t mangle -Z
/sbin/iptables -t mangle -P PREROUTING ACCEPT
/sbin/iptables -t mangle -F PREROUTING
/sbin/iptables -t mangle -P OUTPUT ACCEPT
/sbin/iptables -t mangle -F OUTPUT


rm -rf /usr/sbin/oops_firewall
rm -rf /usr/doc/oops_firewall-${VER}

exit 0
