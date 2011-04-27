#!/bin/sh

# version numbering
VER="3.0.1"

# Root user의 권한으로 작동시켜야 함
#
if [ "$UID" != "0" ]; then
  if [ "$LANG" = "ko" ]; then
    echo "에러 - Root 의 권한으로 작동해야 합니다."
  else
    echo "U are not super user!"
  fi
  exit 1
fi

/sbin/iptables -Z
/sbin/iptables -P INPUT ACCEPT
/sbin/iptables -F
/sbin/iptables -t nat -Z
/sbin/iptables -t nat -P PREROUTING ACCEPT
/sbin/iptables -t nat -F 
/sbin/iptables -t nat -P POSTROUTING ACCEPT
/sbin/iptables -t nat -F 
/sbin/iptables -t mangle -Z 
/sbin/iptables -t mangle -P PREROUTING ACCEPT
/sbin/iptables -t mangle -F PREROUTING
/sbin/iptables -t mangle -P OUTPUT ACCEPT
/sbin/iptables -t mangle -F OUTPUT

rm -rf /sbin/oops_firewall
rm -rf /usr/doc/oops_firewall-${VER}

exit 0
