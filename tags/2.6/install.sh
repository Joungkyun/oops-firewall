#!/bin/sh

# version numbering
VER="2.6"

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

ETCDIR="/etc/oops_firewall"
DOCDIR="/usr/doc/oops_firewall-${VER}"

mkdir ${ETCDIR}
install -m 700 ./oops_firewall-${VER} /sbin/oops_firewall
install -m 644 ./filter.conf ${ETCDIR}/filter.conf
install -m 644 ./masq.conf ${ETCDIR}/masq.conf
install -m 644 ./forward.conf ${ETCDIR}/forward.conf

mkdir -p ${DOCDIR}
install -m 644 ./COPYING ${DOCDIR}/COPYING
install -m 644 ./COPYING.ko ${DOCDIR}/COPYING.ko
install -m 644 ./CREDIT ${DOCDIR}/CREDIT
install -m 644 ./Changelog ${DOCDIR}/Changelog
install -m 644 ./INSTALL ${DOCDIR}/INSTALL
install -m 644 ./README ${DOCDIR}/README
install -m 644 ./README.SORIBADA ${DOCDIR}/README.SORIBADA
install -m 755 ./uninstall.sh ${DOCDIR}/uninstall.sh
install -m 755 ./oops_firewall.init ${DOCDIR}/oops_firewall.init

echo "mkdir ${ETCDIR}" >> ${DOCDIR}/install.log
echo "install -m 700 ./oops_firewall-${VER} /sbin/oops_firewall" >> ${DOCDIR}/install.log
echo "install -m 644 ./filter.conf ${ETCDIR}/filter.conf" >> ${DOCDIR}/install.log
echo "install -m 644 ./masq.conf ${ETCDIR}/masq.conf" >> ${DOCDIR}/install.log
echo "install -m 644 ./forward.conf ${ETCDIR}/forward.conf" >> ${DOCDIR}/install.log
echo "" >> ${DOCDIR}/install.log
echo "mkdir -p ${DOCDIR}" >> ${DOCDIR}/install.log
echo "install -m 644 ./COPYING ${DOCDIR}/COPYING" >> ${DOCDIR}/install.log
echo "install -m 644 ./COPYING.ko ${DOCDIR}/COPYING.ko" >> ${DOCDIR}/install.log
echo "install -m 644 ./CREDIT ${DOCDIR}/CREDIT" >> ${DOCDIR}/install.log
echo "install -m 644 ./Changelog ${DOCDIR}/Changelog" >> ${DOCDIR}/install.log
echo "install -m 644 ./INSTALL ${DOCDIR}/INSTALL" >> ${DOCDIR}/install.log
echo "install -m 644 ./README ${DOCDIR}/README" >> ${DOCDIR}/install.log
echo "install -m 644 ./README.SORIBADA ${DOCDIR}/README.SORIBADA" >> ${DOCDIR}/install.log
echo "install -m 755 ./uninstall.sh ${DOCDIR}/uninstall.sh" >> ${DOCDIR}/install.log
echo "install -m 755 ./oops_firewall.init ${DOCDIR}/oops_firewall.init" >> ${DOCDIR}/install.log

exit 0
