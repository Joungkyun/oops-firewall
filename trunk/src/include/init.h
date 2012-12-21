# Init function
#
# $Id$
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
	_tables="INPUT OUTPUT"
	[ ${BRIDGE_USED} -eq 1 ] && _tables="${_tables} FORWARD"

	for v in $_tables
	do
		if [ "${v}" != "FORWARD" ]; then
			__logprefix="${v} ANY Packet Refuse"
		else
			__logprefix="Bridge ANY Packet Refuse"
		fi

		[ ${USE_LOG} -eq 1 ] && {
			o_echo "  * iptables -A ${v} -m stat --state ${_nI} \\"
			o_echo "             ${_logformat} \\"
			o_echo "             --log-prefix \"${__logprefix}\""
			${c_iptables} -A ${v} -m state --state ${_nI} ${_logformat} --log-prefix "${__logprefix}"
		}
		printf "  * iptables -A %-7s -m state --state ${_nI} -j DROP\n" "${v}"
		[ ${_testmode} -eq 0 ] && ${c_iptables} -A ${v} -m state --state ${_nI} -j DROP
	done
}

init_default_rule() {
	_tables="INPUT OUTPUT"
	[ ${BRIDGE_USED} -eq 1 ] && _tables="${_tables} FORWARD"

	# ESTABLISHED 세션과 RELATE 세션을 항상 허용한다.
	# FTP session의 경우 nf_conntrack_ftp 모듈이 올라와 있어야
	# 작동한다.
	for v in $_tables
	do
		printf "  * iptables -A %-7s -m state --state ${_nE},${_nR} -j ACCEPT\n" "${v}"
		[ ${_testmode} -eq 0 ] && ${c_iptables} -A ${v} -m state --state ${_nE},${_nR} -j ACCEPT
	done

	# 외부 DNS 질의를 허용한다.
	o_echo "  * iptables -A OUTPUT  -p udp --dport 53 -m state --state ${_nN} -j ACCEPT"
	[ ${_testmode} -eq 0 ] && ${c_iptables} -A OUTPUT -p udp --dport 53 -m state --state ${_nN} -j ACCEPT

	[ ${BRIDGE_USED} -eq 0 ] && {
		for dv in ${BRIDGE_INTERNAL}
		do
			o_echo "  * iptables -A FORWARD -p udp ! -d ${dv} --dport 53 -j ACCEPT"
			[ ${_testmode} -eq 0 ] && ${c_iptables} -A FORWARD -p udp ! -d ${dv} --dport 53 -j ACCEPT
		done
	}
}

init_deny() {
	_tables="INPUT OUTPUT"
	[ ${BRIDGE_USED} -eq 1 ] && _tables="${_tables} FORWARD"

	o_echo
	o_echo $"  * Drop All TCP packet"
	for _tb in ${_tables}
	do
		case ${_tb} in
			INPUT) _tbv="Inbound" ;;
			OUTPUT) _tbv="Outbound" ;;
			*) _tbv="Bridging" ;;
		esac

		[ "${USE_LOG}" = "1" ] && {
			printf "    iptables -A %-7s -p tcp ${_logformat} \\" ${_tb}
			printf "             --log-prefix 'Drop %-8s TCP request'\n" ${_tbv}
			[ ${_testmode} -eq 0 ] && \
				${c_iptables} -A ${_tb} -p tcp ${_logformat} \
							--log-prefix "Drop ${_tbv} TCP request refuse"
		}
		printf "    iptables -A %-7s -p tcp  -j DROP\n" ${_tb}
		[ ${_testmode} -eq 0 ] && ${c_iptables} -A ${_tb} -p tcp -j DROP
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

		case ${_tb} in
			INPUT) _tbv="Inbound" ;;
			OUTPUT) _tbv="Outbound" ;;
			*) _tbv="Bridging" ;;
		esac

		[ "${USE_LOG}" = "1" ] && {
			printf "    iptables -A %-7s -p udp ${_logformat} \\" ${_tb}
			printf "             --log-prefix 'Drop %-8s UDP request'\n" ${_tbv}
			[ ${_testmode} -eq 0 ] && \
				${c_iptables} -A ${_tb} -p udp ${_logformat} --log-prefix "Drop ${_tbv} UDP Refuse"
		}
		printf "    iptables -A %-7s -p udp  -j DROP\n" ${_tb}
		[ ${_testmode} -eq 0 ] && ${c_iptables} -A ${_tb} -p udp -j DROP
	done

	o_echo
	o_echo $"  * Rejcet all ICMP packet"

	for _tb in ${_tables}
	do
		case ${_tb} in
			INPUT) _tbv="Inbound" ;;
			OUTPUT) _tbv="Outbound" ;;
			*) _tbv="Bridging" ;;
		esac

		# ICMP block
		[ "{USE_LOG}" = "1" ] && {
			printf "    iptables -A %-7s -p icmp ${_logformat} \\" ${_tb}
			printf "             --log-prefix '%-8s ICMP request refuse'\n" ${_tbv}

			[ ${_testmode} -eq 0 ] && \
				${c_iptables} -A ${_tb} -p icmp ${_logformat} \
							--log-prefix "${_tbv} ICMP request refuse"
		}

		printf "    iptables -A %-7s -p icmp -j REJECT\n" ${_tb}
		[ ${_testmode} -eq 0 ] && {
			${c_iptables} -A ${_tb} -p icmp -j REJECT
		}
	done
}

#
# Local variables:
# tab-width: 4
# c-basic-offset: 4
# End:
# vim: set filetype=sh noet sw=4 ts=4 fdm=marker:
# vim<600: noet sw=4 ts=4:
#
