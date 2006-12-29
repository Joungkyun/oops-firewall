%define _unpackaged_files_terminate_build 0
Summary: Individaul Firewall with IPTABLES
Summary(ko): IPTABLES 를 이용한 개별 방화벽
Name: oops-firewall
Version: 6.0.0
Release: 1
Epoch: 30
Copyright: GPL
Group: Applications/Security
URL: http://oops.org/?t=lecture&sb=firewall&n=1
Source0: ftp://mirror.oops.org/pub/oops/oops-firewall/%{name}-%{version}.tar.bz2
BuildRoot: /var/tmp/%{name}-root
Requires: iptables perl sh-utils fileutils bridge-utils
Conflicts: oops_firewall
BuildArchitectures: noarch

Packager: JoungKyun. Kim <http://oops.org>
Vendor:   OOPS Development ORG

%description
It keeps your network system basically secure for external access
with iptables package in KERNEL v2.4 or v2.6

In version 6.0, support Bridge firewall

%description -l ko
커널 2.4 부터 지원하는 NETFILTER 기능을 이용하여 원격 공격으로
부터 시스템을 안전하게 막기 위하여 사용을 할수있다.

6.0 부터는 Bridge 방화벽을 지원한다.

%package doc
Summary: OOPS Firewall Documentation
Summary(ko): OOPS Firewall 관련 문서
Group: Documentation

%description doc
Documentation for oops firewall

%description doc -l ko
방화벽 관련 문서들

%prep
%setup -q

./configure --prefix=/usr \
	--bindir=/usr/sbin \
	--confdir=/etc/oops-firewall \
	--includedir=/usr/include/oops-firewall \
	--shareddir=/usr/share \
	--initdir=/etc/rc.d/init.d \
	--destdir=$RPM_BUILD_ROOT

%build
make

%install
if [ -d $RPM_BUILD_ROOT ]; then
  rm -rf $RPM_BUILD_ROOT
fi
mkdir -p $RPM_BUILD_ROOT

make DESTDIR=$RPM_BUILD_ROOT install

%post
/sbin/chkconfig --add %{name}
if [ $1 = 0 ]; then
  /sbin/chkconfig --level 35 %{name} on
fi

%preun
if [ $1 = 0 ]; then
   /sbin/chkconfig --del %{name}
fi

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root)
%attr(755,root,root) /etc/rc.d/init.d/oops-firewall
%attr(755,root,root) /usr/sbin/oops-firewall
/usr/include/oops-firewall/*.h
/usr/include/oops-firewall/*.sed
/usr/share/locale/ko/LC_MESSAGES/oops-firewall.mo
%config(noreplace) /etc/oops-firewall/application.conf
%config(noreplace) /etc/oops-firewall/bridge.conf
%config(noreplace) /etc/oops-firewall/filter.conf
%config(noreplace) /etc/oops-firewall/interface.conf
%config(noreplace) /etc/oops-firewall/masq.conf
%config(noreplace) /etc/oops-firewall/forward.conf
%config(noreplace) /etc/oops-firewall/tos.conf
%config(noreplace) /etc/oops-firewall/user.conf
%config(noreplace) /etc/oops-firewall/modules.list
%dir /usr/include/oops-firewall

%files doc
%doc doc/README doc/README.SORIBADA doc/README.DIALPAD doc/CONFIG_SYNTAX
%doc CREDIT COPYING Changelog

%changelog
* Fri Dec 29 2006 JoungKyun.Kim <http://oops.org> 30:6.0.0-1
- update 6.0.1

* Tue Dec 26 2005 JoungKyun Kim <http://oops.org> 14:5.0.1-1
- update 5.0.1

* Sun Dec  4 2005 JoungKyun Kim <http://oops.org> 13:5.0.0-1
- update 5.0.0

* Sun Nov 14 2004 JoungKyun Kim <http://oops.org> 12:4.2.1-1
- update 4.2.1

* Wed Aug 26 2004 JoungKyun Kim <http://oops.org> 10:4.2.0-1
- update 4.2.0

* Wed Aug 18 2004 JoungKyun Kim <http://oops.org> 9:4.1.0-1
- update 4.1.0
- support kernel 2.6

* Wed Aug  4 2004 JoungKyun Kim <http://oops.org> 8:4.0.5-1
- update 4.0.5

* Thu Feb 26 2004 JoungKyun Kim <http://oops.org> 4.0.4-1
- update 4.0.4

* Thu Dec 11 2003 JoungKyun Kim <http://oops.org> 4.0.3-1
- removed ip_forward and log_martians configuration on proc

* Sat Aug 16 2003 JoungKyun Kim <http://oops.org> 4.0.2-1
- fixed ftp active mode access problem
- fixed mis load module in nat mode

* Fri Aug  5 2003 JoungKyun Kim <http://oops.org> 4.0.1-2
- fixed update filter.conf problem

* Fri Aug  1 2003 JoungKyun Kim <http://oops.org> 4.0.1-1
- fixed builtin module error on ins_mod function
- added i18N text in oops-firewall.h

* Fri Aug  1 2003 JoungKyun Kim <http://oops.org> 4.0.0-1
- fixed typo
- added -n option on init status mode
- support port range with '-' charactor
- added module admin code
- supported i18N

* Sat Jan 18 2003 JoungKyun Kim <http://oops.org> 3.2.3-2
- fixed typo

* Mon Dec 16 2002 JoungKyun Kim <http://oops.org> 3.2.3
- modified init file for redhat kernel

* Mon Dec  9 2002 JoungKyun Kim <http://oops.org> 3.2.2
- removed udp state option

* Fri Jun 14 2002 JoungKyun Kim <http://oops.org> 3.2.1
- fixed IFS bug

* Wed Feb 27 2002 JoungKyun Kim <http://oops.org> 3.2.0
- changed package name to oops-firewall from oops_firewall
- supported kernel 2.4.18
- removed Dial Pad configuration
- merged client.conf with filter.conf
- added tos.conf user.conf
- added README.DIALPAD
- update README.SORIBADA
- update README.STRING

* Mon Feb 25 2002 JoungKyun Kim <http://oops.org> 3.1.2
- added check of ethernet device that eth0 don't exist
  and eth1 exists in ethernet device exsits one card
  on system
- fixed bug of refuse OUTPUT packet with put log logic

* Thu Nov 15 2001 JoungKyun Kim <http://oops.org>
- fixed ftp connection in serveral ethernet device

* Tue Nov 13 2001 JoungKyun Kim <http://oops.org>
- update 3.1.0
- added TCPMSS target For MASQ and ADSL
- added string check for MASQ packet
- modifed to use sport and dport in string check
- moved architecture to noarch from i686

* Fri Oct 26 2001 JoungKyun Kim <http://oops.org>
- patch version 3.0.2
- patched udp states at client udp service
- typo log messages
- added rule that protected output invalied packet
- separated display of firewall state as tables
- modified README.SORIBADA

* Tue Oct 16 2001 JoungKyun Kim <http://oops.org>
- patch version 3.0.1
- change var name STRINT to STRING
- patched wrong udp command

* Thu Oct 11 2001 JoungKyun Kim <http://oops.org>
- major upgrade 3.0
- separated daemon service and client service
- closed all port (change from 1-1024 to 1-65535)
- controled network state (NEW,ESTABLISHED,RELATED,INVALID)

* Wed Sep 19 2001 JoungKyun Kim <http://oops.org>
- update version 2.9
- added TOS rule

* Sun Aug 19 2001 JoungKyun Kim <http://oops.org>
- patched rmmod command option

* Mon Aug 13 2001 JoungKyun Kim <http://oops.org>
- update version 2.8
- prefix ftp access problem
- added log function

* Sat Aug 11 2001 JoungKyun Kim <http://oops.org>
- update version 2.7
- modified ftp client configuration
- added iptable string match rule (require iptables include ipt_string)

* Mon Aug  6 2001 JoungKyun Kim <http://oops.org>
- update version 2.6
- fixed ftp access problems
- added option -h
- modified deny rule

* Wed Jun 27 2001 JoungKyun Kim <http://oops.org>
- update version 2.5
- fixed iptables path in oops_firewal script
- added zero count option

* Tue May 01 2001 JoungKyun Kim <http://oops.org>
- update version 2.4
- modified udp forward table

* Wed Apr 26 2001 JoungKyun, Kim <http://oops.org>
- update version 2.3
- adjusted privite ip address in masq
- modified forwarding chains

* Wed Apr 25 2001 JoungKyun, Kim <http://oops.org>
- update version 2.2
- added masq start address (masq.conf)

* Tue Apr  3 2001 JoungKyun, Kim <http://oops.org>
- update version 2.1
- added msql configurations
- added port forward configurations

* Tue Mar 21 2001 JoungKyun, Kim <http://oops.org>
- update version 2.0
- based on kernel v2.4 and require iptables package
- Can't use in kernel 2.2.x and Don't used ipchains pacakge

* Thu Aug 17 2000 JoungKyun, Kim <http://oops.org>
- added in DNS entry

* Wed Aug 16 2000 JoungKyun, Kim <http://oops.org>
- packaging 1.0

