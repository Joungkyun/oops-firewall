#!/bin/sh

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

mkdir /etc/oops_firewall
install -m 700 ./oops-firewall /sbin/oops_firewall
install -m 644 ./oops_firewall.conf /etc/oops_firewall/oops_firewall.conf

echo "install -m 700 ./oops-firewall /sbin/oops_firewall" >> install.log
echo "install -m 644 ./oops_firewall.conf /etc/oops_firewall/oops_firewall.conf" >> install.log

exit 0
