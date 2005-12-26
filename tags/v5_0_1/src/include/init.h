# Init function
#
# $Id: init.h,v 1.1 2005-12-03 19:37:28 oops Exp $
#

init_service() {
	__chkchain=0

	o_echo $"  * Reset Basic Table"
	if [ ${_testmode} -eq 0 ]; then
		${c_iptables} -Z
		__chkchain=$?
	fi
	o_echo -n $"    Reset Chain Counter "
	print_result ${__chkchain}
	if [ ${_testmode} -eq 0 ]; then
		${c_iptables} -P INPUT ACCEPT
		__chkchain=$?
	fi
	o_echo -n $"    Accept All INPUT Chain"
	print_result ${__chkchain}
	if [ ${_testmode} -eq 0 ]; then
		${c_iptables} -F INPUT
		__chkchain=$?
	fi
	o_echo -n $"    Remove rule of INPUT Chain"
	print_result ${__chkchain}
	if [ ${_testmode} -eq 0 ]; then
		${c_iptables} -P OUTPUT ACCEPT
		__chkchain=$?
	fi
	o_echo -n $"    Accept All OUTPUT Chain"
	print_result ${__chkchain}
	if [ ${_testmode} -eq 0 ]; then
		${c_iptables} -F FORWARD
		__chkchain=$?
	fi
	o_echo -n $"    Remove rule of FORWARD Chain"
	print_result ${__chkchain}
	if [ ${_testmode} -eq 0 ]; then
		${c_iptables} -P FORWARD ACCEPT
		__chkchain=$?
	fi
	o_echo -n $"    Accept All FORWARD Chain"
	print_result ${__chkchain}
	if [ ${_testmode} -eq 0 ]; then
		${c_iptables} -F OUTPUT
		__chkchain=$?
	fi
	o_echo -n $"    Remove rule of OUTPUT Chain"
	print_result ${__chkchain}

	o_echo
	o_echo $"  * Reset NAT Table"
	if [ ${_testmode} -eq 0 ]; then
		${c_iptables} -t nat -Z
		__chkchain=$?
	fi
	o_echo -n $"    Reset NAT Tabel Counter"
	print_result ${__chkchain}
	if [ ${_testmode} -eq 0 ]; then
		${c_iptables} -t nat -P PREROUTING ACCEPT
		__chkchain=$?
	fi
	o_echo -n $"    Accept All PREROUTING Chain"
	print_result ${__chkchain}
	if [ ${_testmode} -eq 0 ]; then
		${c_iptables} -t nat -F PREROUTING
		__chkchain=$?
	fi
	o_echo -n $"    Remove rule of PREROUTING"
	print_result ${__chkchain}
	if [ ${_testmode} -eq 0 ]; then
		${c_iptables} -t nat -P POSTROUTING ACCEPT
		__chkchain=$?
	fi
	o_echo -n $"    Accept All POSTROUTING Chain"
	print_result ${__chkchain}
	if [ ${_testmode} -eq 0 ]; then
		${c_iptables} -t nat -F POSTROUTING
		__chkchain=$?
	fi
	o_echo -n $"    Remove rule of POSTROUTING"
	print_result ${__chkchain}

	o_echo
	o_echo $"  * Reset Mangle Table"
	if [ ${_testmode} -eq 0 ]; then
		${c_iptables} -t mangle -Z
		__chkchain=$?
	fi
	o_echo -n $"    Reset MANGLE Tabel Counter"
	print_result ${__chkchain}

	if [ ${_testmode} -eq 0 ]; then
		${c_iptables} -t mangle -P PREROUTING ACCEPT
		__chkchain=$?
	fi
	o_echo -n $"    Accept All PREROUTING Chain"
	print_result ${__chkchain}

	if [ ${_testmode} -eq 0 ]; then
		${c_iptables} -t mangle -F PREROUTING
		__chkchain=$?
	fi
	o_echo -n $"    Remove rule of PREROUTING"
	print_result ${__chkchain}

	if [ ${_testmode} -eq 0 ]; then
		${c_iptables} -t mangle -P OUTPUT ACCEPT
		__chkchain=$?
	fi
	o_echo -n $"    Accept All OUTPUT Chain"
	print_result ${__chkchain}

	if [ ${_testmode} -eq 0 ]; then
		${c_iptables} -t mangle -F OUTPUT
		__chkchain=$?
	fi
	o_echo -n $"    Remove rule of OUTPUT"
	print_result ${__chkchain}

	# 커널 2.4.18 이상에서 작동
	if [ "${extend_mangle}" = "1" ]; then
		if [ ${_testmode} -eq 0 ]; then
			${c_iptables} -t mangle -P INPUT ACCEPT
			__chkchain=$?
		fi
		o_echo -n $"    Accept All INPUT Chain"
		print_result ${__chkchain}

		if [ ${_testmode} -eq 0 ]; then
			${c_iptables} -t mangle -F INPUT
			__chkchain=$?
		fi
		o_echo -n $"    Remove rule of INPUT"
		print_result ${__chkchain}

		if [ ${_testmode} -eq 0 ]; then
			${c_iptables} -t mangle -P FORWARD ACCEPT
			__chkchain=$?
		fi
		o_echo -n $"    Accept All FORWARD Chain"
		print_result ${__chkchain}

		if [ ${_testmode} -eq 0 ]; then
			${c_iptables} -t mangle -F FORWARD
			__chkchain=$?
		fi
		o_echo -n $"    Remove rule of FORWARD"
		print_result ${__chkchain}

		if [ ${_testmode} -eq 0 ]; then
			${c_iptables} -t mangle -P POSTROUTING ACCEPT
			__chkchain=$?
		fi
		o_echo -n $"    Accept All POSTROUTING Chain"
		print_result ${__chkchain}

		if [ ${_testmode} -eq 0 ]; then
			${c_iptables} -t mangle -F POSTROUTING
			__chkchain=$?
		fi
		o_echo -n $"    Remove rule of POSTROUTING"
		print_result ${__chkchain}
	fi
}

init_kernel() {
	__kpchk=0

	#if [ -f "/proc/sys/net/ipv4/ip_forward" ]; then
	#	if [ ${_testmode} -eq 0 ]; then
	#		echo 0 > /proc/sys/net/ipv4/ip_forward
	#		__kpchk=$?
	#	fi
	#	o_echo -n $"  * Deny IP Forward"
	#	print_result ${__kpchk}
	#fi

	# TCP Syncookies 를 사용할수 있게 하기 위해
	if [ -f "/proc/sys/net/ipv4/tcp_syncookies" ]; then
		if [ ${_testmode} -eq 0 ]; then
			echo 1 > /proc/sys/net/ipv4/tcp_syncookies
			__kpchk=$?
		fi
		o_echo -n $"  * Allow to use TCP SYNCOOKIES"
		print_result ${__kpchk}
	fi


	# RFC1812에 따른 IP spoof 방지를 위한 설정(커널 2.2 이상 버전)
	# 아래에서 ip 스푸핑 해당 항목 참고
	for pfile in /proc/sys/net/ipv4/conf/*/rp_filter
	do
		if [ -f "${pfile}" ]; then
			dev=$(echo ${pfile} | ${c_sed} -e 's!/proc/sys/net/ipv4/conf/\([^/]\+\)/rp_filter!\1!g')
			if [ ${_testmode} -eq 0 ]; then
				echo 1 > $pfile
				__kpchk=$?
			fi
			o_echo -n $"  * CHK IP SPOOFING ${dev}"
			print_result ${__kpchk}
		fi
	done

	# 정의되지 않은 에러 메시지를 막음
	if [ -f "/proc/sys/net/ipv4/icmp_ignore_bogus_error_responses" ]; then
		if [ ${_testmode} -eq 0 ]; then
			echo 1 > /proc/sys/net/ipv4/icmp_ignore_bogus_error_responses
			__kpchk=$?
		fi
		o_echo -n $"  * Deny Invalid MSG"
		print_result ${__kpchk}
	fi

	# ip 주소를 스푸핑한다고 예상되는 경우 로그에 기록하기
	#if [ -f "/proc/sys/net/ipv4/conf/all/log_martians" ]; then
	#	if [ ${_testmode} -eq 0 ]; then
	#		echo 1 > /proc/sys/net/ipv4/conf/all/log_martians
	#		__kpchk=$?
	#	fi
	#	o_echo -n $"  * Logged Spoofing check"
	#	print_result ${__kpchk}
	#fi


	# 브로드캐스트, 멀티캐스트 주소에 ICMP 메시지 보내는것 막기
	# "smurf" 공격 방지용
	# 아래에서 해당 조항 참고
	# 커널 2.2 이상에 해당
	if [ -f "/proc/sys/net/ipv4/icmp_echo_ignore_broadcasts" ]; then
		if [ ${_testmode} -eq 0 ]; then
			echo 1 > /proc/sys/net/ipv4/icmp_echo_ignore_broadcasts
			__kpchk=$?
		fi
		o_echo -n $"  * Deny Ping at BroadCast"
		print_result ${__kpchk}
	fi
}

init_invalid() {
	for v in INPUT OUTPUT
	do
		__logprefix="ANY Packet Refuse"
		[ "${v}" = "OUTPUT" ] && __logprefix="${v} ${__logprefix}"

		[ ${USE_LOG} -eq 1 ] && {
			o_echo "  * iptables -A ${v} -m stat --state ${_nI} \\"
			o_echo "             ${_logformat} \\"
			o_echo "             --log-prefix \"${__logprefix}\""
			${c_iptables} -A ${v} -m state --state ${_nI} ${_logformat} --log-prefix "${__logprefix}"
		}
		o_echo "  * iptables -A ${v} -m state --state ${_nI} -j DROP"
		[ ${_testmode} -eq 0 ] && ${c_iptables} -A ${v} -m state --state ${_nI} -j DROP
	done
}

init_deny() {
	# 모든 서비스를 거부
	o_echo "  * Reject All Syn Packet"
	[ ${USE_LOG} -eq 1 ] && {
		o_echo "    iptables -A INPUT -p tcp --syn ${_logformat} --log-prefix 'SYN Refuse'"
		[ ${_testmode} -eq 0 ] && \
			${c_iptables} -A INPUT -p tcp --syn ${_logformat} --log-prefix 'SYN Refuse'
	}
	o_echo "    iptables -A INPUT -p tcp --syn -j REJECT"
	[ ${_testmode} -eq 0 ] && ${c_iptables} -A INPUT -p tcp --syn -j REJECT

	o_echo "  * Drop All TCP packet"
	[ "${USE_LOG}" = "1" ] && {
		o_echo "    iptables -A INPUT -p tcp --dport ${_allport} ${_logformat} --log-prefix 'TCP Refuse'"
		[ ${_testmode} -eq 0 ] && \
			${c_iptables} -A INPUT -p tcp --dport ${_allport} ${_logformat} --log-prefix 'TCP Refuse'
	}
	o_echo "    iptables -A INPUT -p tcp --dport ${_allport} -j DROP"
	[ ${_testmode} -eq 0 ] && ${c_iptables} -A INPUT -p tcp --dport ${_allport} -j DROP

	o_echo "  * Drop All UDP packet"
	[ "${USE_LOG}" = "1" ] && {
		o_echo "    iptables -A INPUT -p udp --dport ${_allport} ${_logformat} --log-prefix 'UDP Refuse'"
		[ ${_testmode} -eq 0 ] && \
			${c_iptables} -A INPUT -p udp --dport ${_allport} ${_logformat} --log-prefix 'UDP Refuse'
	}
	o_echo "    iptables -A INPUT -p udp --dport ${_allport} -j DROP"
	[ ${_testmode} -eq 0 ] && ${c_iptables} -A INPUT -p udp --dport ${_allport} -j DROP

	o_echo "  * Prevented internal ping and traceroute request"
	# PING 을 막음
	[ "${USE_LOG}" = "1" ] && {
		o_echo "    iptables -A INPUT -p icmp --icmp-type echo-request \\"
		o_echo "             ${_logformat} \\"
		o_echo "             --log-prefix 'PING request Refuse'"
		o_echo "    iptables -A OUTPUT -p icmp --icmp-type time-exceeded \\"
		o_echo "             ${_logformat} \\"
		o_echo "             --log-prefix 'Traceroute Refuse'"
		[ ${_testmode} -eq 0 ] && {
			${c_iptables} -A INPUT -p icmp --icmp-type echo-request ${_logformat} \
						--log-prefix 'PING request Refuse'
			${c_iptables} -A OUTPUT -p icmp --icmp-type time-exceeded ${_logformat} \
						--log-prefix 'Traceroute Refuse'
		}
	}
	o_echo "    iptables -A INPUT -p icmp --icmp-type echo-request -j REJECT"
	o_echo "    iptables -A OUTPUT -p icmp --icmp-type time-exceeded -j REJECT"
	[ ${_testmode} -eq 0 ] && {
		${c_iptables} -A INPUT -p icmp --icmp-type echo-request -j REJECT
		${c_iptables} -A OUTPUT -p icmp --icmp-type time-exceeded -j REJECT
	}
}
