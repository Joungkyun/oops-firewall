#!/bin/sh

# version numbering
VER="2.7"

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

rm -rf /sbin/oops_firewall
rm -rf /usr/doc/oops_firewall-${VER}

exit 0
