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

rm -f /etc/oops_firewall/oops_firewall.conf
rm -rf /sbin/oops_firewall
rm -rf /usr/doc/oops_firewall-2.1

exit 0
