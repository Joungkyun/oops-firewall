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

mkdir /etc/oops_firewall
install -m 700 ./oops-firewall /sbin/oops_firewall
install -m 644 ./oops_firewall.conf /etc/oops_firewall/oops_firewall.conf

echo "install -m 700 ./oops-firewall /sbin/oops_firewall" >> install.log
echo "install -m 644 ./oops_firewall.conf /etc/oops_firewall/oops_firewall.conf" >> install.log

exit 0
