# OOPS-Firewall

## Introduction

oops-firewall is a very intuitive and easy Linux firewall.

By default, you can replace iptables init file or firewalld on RHEL / CentOS.

Although it targets __RHEL / CentOS___, it can be used with any distribution that can use iptables.

See the [README.debian](https://raw.githubusercontent.com/Joungkyun/oops-firewall/master/README.debian) documentation for use on ___Debian___ or ___Ubuntu___ distributions.

The oops-firewall project was first developed in 2001 and has been under maintenance for over 15 years. In addition, it has been used as a firewall, NAT, and bridge server in enterprise-class sites such as Neowiz and Ticket monster in Republic of Korea, and it is also [recommended to be used](http://www.kisa.or.kr/uploadfile/201603/201603181350595503.pdf) by trusted institutions such as Republic of Korea's KISA (Korea Internet & Security Agency).

## License

Copyright &copy; 2019 JoungKyun.Kim All Rights Reserved.<br>
And follows GPL v2

## Installation

### RHEL / CentOS

```bash
[root@host ~]# rpmbuild -tb oops-filrewal-7.0.4.tar.bz2
[root@host ~]# yum localinstall /root/rpmbuild/RPMS/noarch/oops-firewall-7.0.4-1.noarch.rpm
```

### Debian / Ubuntu

See also [README.debian](https://raw.githubusercontent.com/Joungkyun/oops-firewall/master/README.debian)

### Other distributions

```bash
[root@host ~]# tar xvfpj oops-firewall-7.0.4.tar.bz2
[root@host ~]# cd oops-firewall-7.0.4
[root@host ~]# ./configure --prefix=/opt/oops-firewall
[root@host ~]# make
[root@host ~]# make install
```

See the [INSTALL documentation](doc/INSTALL) for details.

## Usage

Please refer to the following documents. Sorry, but Korean.

* https://oops.org/?t=lecture&s=firewall
* https://joungkyun.gitbooks.io/annyung-3-user-guide/chapter2/chapter2-1-firewall/
