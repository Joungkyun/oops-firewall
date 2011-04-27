# Init function
#
# $Id: init.h,v 1.2 2006-12-29 05:45:17 oops Exp $
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

	# Ŀ�� 2.4.18 �̻󿡼� �۵�
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

	# TCP Syncookies �� ����Ҽ� �ְ� �ϱ� ����
	if [ -f "/proc/sys/net/ipv4/tcp_syncookies" ]; then
		if [ ${_testmode} -eq 0 ]; then
			echo 1 > /proc/sys/net/ipv4/tcp_syncookies
			__kpchk=$?
		fi
		o_echo -n $"  * Allow to use TCP SYNCOOKIES"
		print_result ${__kpchk}
	fi


	# RFC1812�� ���� IP spoof ������ ���� ����(Ŀ�� 2.2 �̻� ����)
	# �Ʒ����� ip ��Ǫ�� �ش� �׸� ����
	_pfile="/proc/sys/net/ipv4/conf/*/rp_filter"
	for pfile in $_pfile
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

	# ���ǵ��� ���� ���� �޽����� ����
	if [ -f "/proc/sys/net/ipv4/icmp_ignore_bogus_error_responses" ]; then
		if [ ${_testmode} -eq 0 ]; then
			echo 1 > /proc/sys/net/ipv4/icmp_ignore_bogus_error_responses
			__kpchk=$?
		fi
		o_echo -n $"  * Deny Invalid MSG"
		print_result ${__kpchk}
	fi

	# ip �ּҸ� ��Ǫ���Ѵٰ� ����Ǵ� ��� �α׿� ����ϱ�
	#if [ -f "/proc/sys/net/ipv4/conf/all/log_martians" ]; then
	#	if [ ${_testmode} -eq 0 ]; then
	#		echo 1 > /proc/sys/net/ipv4/conf/all/log_martians
	#		__kpchk=$?
	#	fi
	#	o_echo -n $"  * Logged Spoofing check"
	#	print_result ${__kpchk}
	#fi


	# ��ε�ĳ��Ʈ, ��Ƽĳ��Ʈ �ּҿ� ICMP �޽��� �����°� ����
	# "smurf" ���� ������
	# �Ʒ����� �ش� ���� ����
	# Ŀ�� 2.2 �̻� �ش�
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
	_tables="INPUT OUTPUT"
	[ ${BRIDGE_USED} -eq 1 ] && _tables="${_tables} FORWARD"

	for v in $_tables
	do
		case "$v" in
			INPUT) _pv="INPUT  ";;
			OUTPUT) _pv="OUTPUT ";;
			*) _pv="${v}"
		esac
		if [ "${v}" != "FORWARD" ]; then
			__logprefix="${v} ANY Packet Refuse"
		else
			__logprefix="Bridge ANY Packet Refuse"
		fi

		[ ${USE_LOG} -eq 1 ] && {
			o_echo "  * iptables -A ${_pv} -m stat --state ${_nI} \\"
			o_echo "             ${_logformat} \\"
			o_echo "             --log-prefix \"${__logprefix}\""
			${c_iptables} -A ${v} -m state --state ${_nI} ${_logformat} --log-prefix "${__logprefix}"
		}
		o_echo "  * iptables -A ${_pv} -m state --state ${_nI} -j DROP"
		[ ${_testmode} -eq 0 ] && ${c_iptables} -A ${v} -m state --state ${_nI} -j DROP
	done
}

init_deny() {
	_tables="INPUT"
	[ ${BRIDGE_USED} -eq 1 ] && _tables="${_tables} FORWARD"

	# ��� ���񽺸� �ź�
	o_echo $"  * Reject All Syn Packet"
	for _tb in OUTPUT ${_tables}
	do
		case "$_tb" in
			INPUT) _ptb="INPUT  ";;
			OUTPUT) _ptb="OUTPUT ";;
			*) _ptb="$_tb"
		esac
		[ ${USE_LOG} -eq 1 ] && {
			o_echo "    iptables -A ${_ptb} -p tcp --syn ${_logformat} --log-prefix 'SYN Refuse'"
			[ ${_testmode} -eq 0 ] && \
				${c_iptables} -A ${_tb} -p tcp --syn ${_logformat} --log-prefix 'SYN Refuse'
		}
		o_echo "    iptables -A ${_ptb} -p tcp --syn -j REJECT"
		[ ${_testmode} -eq 0 ] && ${c_iptables} -A ${_tb} -p tcp --syn -j REJECT
	done

	o_echo
	o_echo $"  * Drop All TCP packet"
	for _tb in ${_tables}
	do
		case "$_tb" in
			INPUT) _ptb="INPUT  ";;
			OUTPUT) _ptb="OUTPUT ";;
			*) _ptb="$_tb"
		esac
		[ "${USE_LOG}" = "1" ] && {
			o_echo "    iptables -A ${_ptb} -p tcp --dport ${_allport} ${_logformat} --log-prefix 'TCP Refuse'"
			[ ${_testmode} -eq 0 ] && \
				${c_iptables} -A ${_tb} -p tcp --dport ${_allport} ${_logformat} --log-prefix 'TCP Refuse'
		}
		o_echo "    iptables -A ${_ptb} -p tcp --dport ${_allport} -j DROP"
		[ ${_testmode} -eq 0 ] && ${c_iptables} -A ${_tb} -p tcp --dport ${_allport} -j DROP
	done

	o_echo
	o_echo $"  * Drop All UDP packet"
	for _tb in ${_tables}
	do
		case "$_tb" in
			INPUT) _ptb="INPUT  ";;
			OUTPUT) _ptb="OUTPUT ";;
			*) _ptb="$_tb"
		esac
		if [ "${_tb}" = "FORWARD" ]; then
			[ "${USE_LOG}" = "1" ] && {
				o_echo "    iptables -A ${_ptb} -o ${BRIDGE_NAME} -p udp --dport 33434:33523 ${_logformat} --log-prefix 'UDP Traceroute'"
				o_echo "    iptables -A ${_ptb} -i ${BRIDGE_NAME} -p udp --sport 32767:33167 ${_logformat} --log-prefix 'UDP Traceroute'"
				if [ ${_testmode} -eq 0 ]; then
					${c_iptables} -A ${_tb} -o ${BRIDGE_NAME} -p udp --dport 33434:33523 ${_logformat} --log-prefix 'UDP Traceroute'
					${c_iptables} -A ${_tb} -i ${BRIDGE_NAME} -p udp --sport 32767:33167 ${_logformat} --log-prefix 'UDP Traceroute'
				fi
			}
			o_echo "    iptables -A ${_ptb} -o ${BRIDGE_NAME} -p udp --dport 33434:33523 -j ACCEPT"
			o_echo "    iptables -A ${_ptb} -i ${BRIDGE_NAME} -p udp --sport 32767:33167 -j ACCEPT"
			if [ ${_testmode} -eq 0 ]; then
				${c_iptables} -A ${_tb} -o ${BRIDGE_NAME}  -p udp --dport 33434:33523 -j ACCEPT 
				${c_iptables} -A ${_tb} -i ${BRIDGE_NAME}  -p udp --sport 32767:33167 -j ACCEPT 
			fi
		fi

		[ "${USE_LOG}" = "1" ] && {
			o_echo "    iptables -A ${_ptb} -p udp --dport ${_allport} ${_logformat} --log-prefix 'UDP Refuse'"
			[ ${_testmode} -eq 0 ] && \
				${c_iptables} -A ${_tb} -p udp --dport ${_allport} ${_logformat} --log-prefix 'UDP Refuse'
		}
		o_echo "    iptables -A ${_ptb} -p udp --dport ${_allport} -j DROP"
		[ ${_testmode} -eq 0 ] && ${c_iptables} -A ${_tb} -p udp --dport ${_allport} -j DROP
	done

	o_echo
	o_echo $"  * Prevented internal ping and traceroute request"

	_tables="INPUT OUTPUT"
	[ ${BRIDGE_USED} -eq 1 ] && _tables="${_tables} FORWARD"

	# PING �� ����
	[ "${USE_LOG}" = "1" ] && {
		o_echo "    iptables -A INPUT   -p icmp --icmp-type echo-request \\"
		o_echo "             ${_logformat} \\"
		o_echo "             --log-prefix 'PING request Refuse'"
		o_echo "    iptables -A OUTPUT  -p icmp --icmp-type time-exceeded \\"
		o_echo "             ${_logformat} \\"
		o_echo "             --log-prefix 'Traceroute Refuse'"
		if [ ${BRIDGE_USED} -eq 1 ]; then
			o_echo "    iptables -A FORWARD -o ${BRIDGE_NAME} -p icmp --icmp-type echo-request \\"
			o_echo "             ${_logformat} \\"
			o_echo "             --log-prefix 'Traceroute Refuse'"
			o_echo "    iptables -A FORWARD -i ${BRIDGE_NAME} -p icmp --icmp-type time-exceeded \\"
			o_echo "             ${_logformat} \\"
			o_echo "             --log-prefix 'Traceroute Refuse'"
		fi
		[ ${_testmode} -eq 0 ] && {
			${c_iptables} -A INPUT -p icmp --icmp-type echo-request ${_logformat} \
						--log-prefix 'PING request Refuse'
			${c_iptables} -A OUTPUT -p icmp --icmp-type time-exceeded ${_logformat} \
						--log-prefix 'Traceroute Refuse'
			if [ ${BRIDGE_USED} -eq 1 ]; then
				${c_iptables} -A FORWARD -o ${BRIDGE_NAME} -p icmp --icmp-type echo-request ${_logformat} \
							--log-prefix 'PING request Refuse'
				${c_iptables} -A FORWARD -i ${BRIDGE_NAME} -p icmp --icmp-type time-exceeded ${_logformat} \
							--log-prefix 'Traceroute Refuse'
			fi
		}
	}
	o_echo "    iptables -A INPUT   -p icmp --icmp-type echo-request -j REJECT"
	o_echo "    iptables -A OUTPUT  -p icmp --icmp-type time-exceeded -j REJECT"
	if [ ${BRIDGE_USED} -eq 1 ]; then
		o_echo "    iptables -A FORWARD -o ${BRIDGE_NAME} -p icmp --icmp-type echo-request -j ACCEPT"
		o_echo "    iptables -A FORWARD -i ${BRIDGE_NAME} -p icmp --icmp-type time-exceeded -j ACCEPT"
		o_echo "    iptables -A FORWARD -i ${BRIDGE_NAME} -p icmp --icmp-type echo-request -j REJECT"
		o_echo "    iptables -A FORWARD -o ${BRIDGE_NAME} -p icmp --icmp-type time-exceeded -j REJECT"
	fi
	[ ${_testmode} -eq 0 ] && {
		${c_iptables} -A INPUT -p icmp --icmp-type echo-request -j REJECT
		${c_iptables} -A OUTPUT -p icmp --icmp-type time-exceeded -j REJECT
		if [ ${BRIDGE_USED} -eq 1 ]; then
			${c_iptables} -A FORWARD -o ${BRIDGE_NAME} -p icmp --icmp-type echo-request -j ACCEPT
 			${c_iptables} -A FORWARD -i ${BRIDGE_NAME} -p icmp --icmp-type time-exceeded -j ACCEPT
 			${c_iptables} -A FORWARD -i ${BRIDGE_NAME} -p icmp --icmp-type echo-request -j REJECT
 			${c_iptables} -A FORWARD -o ${BRIDGE_NAME} -p icmp --icmp-type time-exceeded -j REJECT
		fi
	}
}
