#!/bin/sh

# version numbering
VER="3.2.0"

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

# check of kernel version
KERCHK=$(uname -r | sed -e 's/2.4.\([0-9]\{1,3\}\).*/\1/g')
if [ "${KERCHK}" -gt 17 ]; then
  KERNEL_UP=1
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

if [ "${KERNEL_UP}" = 1 ]; then
  /sbin/iptables -t mangle -P POSTROUTING ACCEPT
  /sbin/iptables -t mangle -F POSTROUTING
  /sbin/iptables -t mangle -P INPUT ACCEPT
  /sbin/iptables -t mangle -F INPUT
  /sbin/iptables -t mangle -P FORWARD ACCEPT
  /sbin/iptables -t mangle -F FORWARD
fi

rm -rf /usr/sbin/oops-firewall
rm -rf /usr/doc/oops-firewall-${VER}

exit 0
