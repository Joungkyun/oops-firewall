#!/bin/sh

# version numbering
VER="2.7"

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

rm -rf /sbin/oops_firewall
rm -rf /usr/doc/oops_firewall-${VER}

exit 0
