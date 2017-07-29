%define _unpackaged_files_terminate_build 0
Summary: Individaul Firewall with IPTABLES
Name: oops-firewall
Version: 7.0.3
Release: 1
Epoch: 46
License: GPL
Group: Applications/Security
URL: http://oops.org/?t=lecture&sb=firewall&n=1
Source0: ftp://mirror.oops.org/pub/oops/oops-firewall/%{name}-%{version}.tar.bz2
BuildRoot: /var/tmp/%{name}-root
Requires: iptables perl sh-utils fileutils
Conflicts: oops_firewall
BuildArchitectures: noarch

%description
It keeps your network system basically secure for external access
with iptables package in KERNEL v2.4 or v2.6

%package doc
Summary: OOPS Firewall Documentation
Group: Documentation

%description doc
Documentation for oops firewall


%prep
%setup -q

./configure --prefix=/usr \
	--bindir=%{_sbindir} \
	--confdir=%{_sysconfdir}/oops-firewall \
	--includedir=%{_includedir}/oops-firewall \
	--shareddir=%{_datadir} \
%if 0%{?rhel} >= 7 || 0%{?fedora} >= 17
	--systemdunitdir=%{_unitdir} \
%else
	--initdir=%{_sysconfdir}/rc.d/init.d \
%endif
	--destdir=%{buildroot} \
#	--langenv=ko

%build
%{__make}

%install
if [ "%{buildroot}" != "/" -a -d "%{buildroot}" ]; then
  %{__rm} -rf %{buildroot}
fi
%{__mkdir_p} %{buildroot}

%{__make} DESTDIR=%{buildroot} install

%{__mkdir_p} %{buildroot}%{_mandir}/man8
%{__install} -m644 doc/%{name}.8 %{buildroot}%{_mandir}/man8/%{name}.8

%post
%if 0%{?rhel} >= 7 || 0%{?fedora} >= 17
%systemd_post oops-firewall.service
if [ $1 = 1 ]; then
	/usr/bin/systemctl enable oops-firewall
fi
%else
/sbin/chkconfig --add %{name}
if [ $1 = 1 ]; then
  /sbin/chkconfig --level 35 %{name} on
fi
%endif

%pre
if [ -f /etc/init.d/iptables ]; then
	echo "oops-firewall finds /etc/init.d/iptables init script"
	echo "ipatbles init script is conflicted by oops-firewall"
	echo "oops-firewall will remove iptables on chkconfig list"
	echo "If you want to revoke iptables init script, use follow commands"
	echo
	echo "chkconfig --add iptables"
	echo "chkconfig --level 345 iptables on"

	/sbin/chkconfig --del iptables
fi

%preun
%if 0%{?rhel} >= 7 || 0%{?fedora} >= 17
%systemd_preun oops-firewall.service
%else
if [ $1 = 0 ]; then
	/sbin/chkconfig --del %{name}
fi
%endif

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root)
%if 0%{?rhel} >= 7 || 0%{?fedora} >= 17
%{_unitdir}/oops-firewall.service
%attr(755,root,root) %{_sbindir}/init.d/oops-firewall
%else
%attr(755,root,root) %{_sysconfdir}/rc.d/init.d/oops-firewall
%endif
%attr(755,root,root) %{_sbindir}/oops-firewall
%{_includedir}/oops-firewall/*.h
%{_includedir}/oops-firewall/*.sed
%{_datadir}/locale/ko/LC_MESSAGES/oops-firewall.mo
%config(noreplace) %{_sysconfdir}/oops-firewall/application.conf
%config(noreplace) %{_sysconfdir}/oops-firewall/bridge.conf
%config(noreplace) %{_sysconfdir}/oops-firewall/filter.conf
%config(noreplace) %{_sysconfdir}/oops-firewall/interface.conf
%config(noreplace) %{_sysconfdir}/oops-firewall/masq.conf
%config(noreplace) %{_sysconfdir}/oops-firewall/forward.conf
%config(noreplace) %{_sysconfdir}/oops-firewall/tos.conf
%config(noreplace) %{_sysconfdir}/oops-firewall/user.conf
%config(noreplace) %{_sysconfdir}/oops-firewall/modules.list
%{_mandir}/man8/%{name}.8*
%dir %{_includedir}/oops-firewall

%files doc
%doc doc/README doc/CONFIG_SYNTAX doc/ko
%doc CREDIT COPYING Changelog

%changelog
* Sat Jul 29 2017 JoungKyun.Kim <hostmaster@oops.org> 47:7.0.3-1
- release 7.0.3
- close snmp output udp port on default
- open ntp output udp port on default
- fixed can't get device information
- enhanced systemd control

* Thu Jan 22 2015 JoungKyun.Kim <hostmaster@oops.org> 47:7.0.2-1
- release 7.0.2
- support systemd on RHEL > 6, Fedora > 16

* Sat Nov  9 2013 JoungKyun.Kim <hostmaster@oops.org> 46:7.0.1-1
- update 7.0.1
- support i18n configuration with langenv option of configure
- fixed missing subnet no  ALLOWSELF verbose msg
- fixed iprange function for wrong source or destination
  on add_port_rule API

* Fri Jan  4 2013 JoungKyun.Kim <hostmaster@oops.org> 46:7.0.0-1
- update 7.0.0

* Tue Mar 28 2012 JoungKyun.Kim <hostmaster@oops.org> 46:6.2.9-1
- update 6.2.9

* Tue Mar 27 2012 JoungKyun.Kim <hostmaster@oops.org> 45:6.2.8-1
- update 6.2.8

* Fri Jan 14 2011 JoungKyun.Kim <hostmaster@oops.org> 44:6.2.7-1
- update 6.2.7

* Fri May  2 2009 JoungKyun.Kim <hostmaster@oops.org> 43:6.2.6-1
- update 6.2.6

* Fri Jul 18 2008 JoungKyun.Kim <hostmaster@oops.org> 42:6.2.4-1
- update 6.2.4

* Thu Jan 10 2008 JoungKyun.Kim <hostmaster@oops.org> 41:6.2.3-1
- update 6.2.3

* Tue Dec 18 2007 JoungKyun.Kim <hostmaster@oops.org> 40:6.2.2-1
- update 6.2.2

* Sun May 20 2007 JoungKyun.Kim <hostmaster@oops.org> 39:6.2.1-1
- update 6.2.1

* Tue Apr 10 2007 JoungKyun.Kim <hostmaster@oops.org> 38:6.2.0-1
- update 6.2.0

* Tue Apr 10 2007 JoungKyun.Kim <hostmaster@oops.org> 37:6.1.3-1
- update 6.1.3

* Sun Apr  8 2007 JoungKyun.Kim <hostmaster@oops.org> 36:6.1.2-1
- update 6.1.2
- complete deprecated direction support stop on MASQ configuration
- fixed wrong masquearding operation when use bridge
- add OUTPUT rule on ALLOWALL

* Fri Mar 30 2007 JoungKyun.Kim <hostmaster@oops.org> 35:6.1.1-1
- update 6.1.1
- add netfilter module option control fucntion

* Fri Mar 30 2007 JoungKyun.Kim <hostmaster@oops.org> 34:6.1.0-1
- update 6.1.0
- utf8 support

* Sun Mar 25 2007 JoungKyun.Kim <hostmaster@oops.org> 33:6.0.3-1
- update 6.0.3

* Sat Jan 27 2007 JoungKyun.Kim <hostmaster@oops.org> 32:6.0.2-1
- update 6.0.2

* Tue Jan  2 2007 JoungKyun.Kim <hostmaster@oops.org> 31:6.0.1-1
- update 6.0.1

* Fri Dec 29 2006 JoungKyun.Kim <hostmaster@oops.org> 30:6.0.0-1
- update 6.0.0

* Tue Dec 26 2005 JoungKyun Kim <hostmaster@oops.org> 14:5.0.1-1
- update 5.0.1

* Sun Dec  4 2005 JoungKyun Kim <hostmaster@oops.org> 13:5.0.0-1
- update 5.0.0

* Sun Nov 14 2004 JoungKyun Kim <hostmaster@oops.org> 12:4.2.1-1
- update 4.2.1

* Wed Aug 26 2004 JoungKyun Kim <hostmaster@oops.org> 10:4.2.0-1
- update 4.2.0

* Wed Aug 18 2004 JoungKyun Kim <hostmaster@oops.org> 9:4.1.0-1
- update 4.1.0
- support kernel 2.6

* Wed Aug  4 2004 JoungKyun Kim <hostmaster@oops.org> 8:4.0.5-1
- update 4.0.5

* Thu Feb 26 2004 JoungKyun Kim <hostmaster@oops.org> 4.0.4-1
- update 4.0.4

* Thu Dec 11 2003 JoungKyun Kim <hostmaster@oops.org> 4.0.3-1
- removed ip_forward and log_martians configuration on proc

* Sat Aug 16 2003 JoungKyun Kim <hostmaster@oops.org> 4.0.2-1
- fixed ftp active mode access problem
- fixed mis load module in nat mode

* Fri Aug  5 2003 JoungKyun Kim <hostmaster@oops.org> 4.0.1-2
- fixed update filter.conf problem

* Fri Aug  1 2003 JoungKyun Kim <hostmaster@oops.org> 4.0.1-1
- fixed builtin module error on ins_mod function
- added i18N text in oops-firewall.h

* Fri Aug  1 2003 JoungKyun Kim <hostmaster@oops.org> 4.0.0-1
- fixed typo
- added -n option on init status mode
- support port range with '-' charactor
- added module admin code
- supported i18N

* Sat Jan 18 2003 JoungKyun Kim <hostmaster@oops.org> 3.2.3-2
- fixed typo

* Mon Dec 16 2002 JoungKyun Kim <hostmaster@oops.org> 3.2.3
- modified init file for redhat kernel

* Mon Dec  9 2002 JoungKyun Kim <hostmaster@oops.org> 3.2.2
- removed udp state option

* Fri Jun 14 2002 JoungKyun Kim <hostmaster@oops.org> 3.2.1
- fixed IFS bug

* Wed Feb 27 2002 JoungKyun Kim <hostmaster@oops.org> 3.2.0
- changed package name to oops-firewall from oops_firewall
- supported kernel 2.4.18
- removed Dial Pad configuration
- merged client.conf with filter.conf
- added tos.conf user.conf
- added README.DIALPAD
- update README.SORIBADA
- update README.STRING

* Mon Feb 25 2002 JoungKyun Kim <hostmaster@oops.org> 3.1.2
- added check of ethernet device that eth0 don't exist
  and eth1 exists in ethernet device exsits one card
  on system
- fixed bug of refuse OUTPUT packet with put log logic

* Thu Nov 15 2001 JoungKyun Kim <hostmaster@oops.org>
- fixed ftp connection in serveral ethernet device

* Tue Nov 13 2001 JoungKyun Kim <hostmaster@oops.org>
- update 3.1.0
- added TCPMSS target For MASQ and ADSL
- added string check for MASQ packet
- modifed to use sport and dport in string check
- moved architecture to noarch from i686

* Fri Oct 26 2001 JoungKyun Kim <hostmaster@oops.org>
- patch version 3.0.2
- patched udp states at client udp service
- typo log messages
- added rule that protected output invalied packet
- separated display of firewall state as tables
- modified README.SORIBADA

* Tue Oct 16 2001 JoungKyun Kim <hostmaster@oops.org>
- patch version 3.0.1
- change var name STRINT to STRING
- patched wrong udp command

* Thu Oct 11 2001 JoungKyun Kim <hostmaster@oops.org>
- major upgrade 3.0
- separated daemon service and client service
- closed all port (change from 1-1024 to 1-65535)
- controled network state (NEW,ESTABLISHED,RELATED,INVALID)

* Wed Sep 19 2001 JoungKyun Kim <hostmaster@oops.org>
- update version 2.9
- added TOS rule

* Sun Aug 19 2001 JoungKyun Kim <hostmaster@oops.org>
- patched rmmod command option

* Mon Aug 13 2001 JoungKyun Kim <hostmaster@oops.org>
- update version 2.8
- prefix ftp access problem
- added log function

* Sat Aug 11 2001 JoungKyun Kim <hostmaster@oops.org>
- update version 2.7
- modified ftp client configuration
- added iptable string match rule (require iptables include ipt_string)

* Mon Aug  6 2001 JoungKyun Kim <hostmaster@oops.org>
- update version 2.6
- fixed ftp access problems
- added option -h
- modified deny rule

* Wed Jun 27 2001 JoungKyun Kim <hostmaster@oops.org>
- update version 2.5
- fixed iptables path in oops_firewal script
- added zero count option

* Tue May 01 2001 JoungKyun Kim <hostmaster@oops.org>
- update version 2.4
- modified udp forward table

* Wed Apr 26 2001 JoungKyun, Kim <hostmaster@oops.org>
- update version 2.3
- adjusted privite ip address in masq
- modified forwarding chains

* Wed Apr 25 2001 JoungKyun, Kim <hostmaster@oops.org>
- update version 2.2
- added masq start address (masq.conf)

* Tue Apr  3 2001 JoungKyun, Kim <hostmaster@oops.org>
- update version 2.1
- added msql configurations
- added port forward configurations

* Tue Mar 21 2001 JoungKyun, Kim <hostmaster@oops.org>
- update version 2.0
- based on kernel v2.4 and require iptables package
- Can't use in kernel 2.2.x and Don't used ipchains pacakge

* Thu Aug 17 2000 JoungKyun, Kim <hostmaster@oops.org>
- added in DNS entry

* Wed Aug 16 2000 JoungKyun, Kim <hostmaster@oops.org>
- packaging 1.0

