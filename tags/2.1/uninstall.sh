#!/bin/sh

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

rm -f /etc/oops_firewall/oops_firewall.conf
rm -rf /sbin/oops_firewall
rm -rf /usr/doc/oops_firewall-2.1

exit 0
