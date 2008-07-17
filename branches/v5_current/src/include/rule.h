# Rule function
#
# $Id: rule.h,v 1.2.2.2 2008-07-17 18:49:06 oops Exp $
#

add_named_port() {
	o_echo "    iptables -A INPUT -p udp --sport 53 -j ACCEPT"
	[ ${_testmode} -eq 0 ] && \
		${c_iptables} -A INPUT -p udp --sport 53 -j ACCEPT
}

# 1st argument : port
# 2st argument : incoming/outgoing
#
add_ftp_rule() {
	__port=${1}
	___type=${2}
	___host="${3}"
	__port=${__port=0}
	__prange=
	__ftpchk=0

	[ "$___type" = "incoming" ] && ___pname="dport" || ___pname="sport"

	__prange=$(echo ${__port} | ${c_grep} "-")
	if [ -n "${__prange}" ]; then
		__prange=$(echo ${__port} | ${c_sed} -e 's/-/ /g')
		__sport=$(echo ${__prange} | ${c_awk} '{print $1}')
		__eport=$(echo ${__prange} | ${c_awk} '{print $2}')

		__sport=${__sport=0}
		__eport=${__eport=0}

		[ ${__sport} -lt 21 -a ${__eport} -gt 21 ] && __ftpchk=1
	else
		[ $__port -eq 21 ] && __ftpchk=1
	fi

	# open tcp port 20 for ftp active mode
	if [ $__ftpchk -eq 1 ]; then
		o_echo "    iptables -A INPUT${___host} -p tcp --${___pname} 20 -m state --state ${_nE},${_nR} -j ACCEPT"
		[ ${_testmode} -eq 0 ] && \
			${c_iptables} -A INPUT${___host} -p tcp --${___pname} 20 -m state --state ${_nE},${_nR} -j ACCEPT
		o_echo "    iptables -A INPUT${___host} -p tcp --sport ${_userport} --dport ${_userport} \\"
		o_echo "             -m state --state ${_nE},${_nR} -j ACCEPT"
		[ ${_testmode} -eq 0 ] && \
			${c_iptables} -A INPUT${___host} -p tcp --sport ${_userport} --dport ${_userport} \
						-m state --state ${_nE},${_nR} -j ACCEPT
	fi
}

add_all_rule() {
	o_echo "  * iptables -A INPUT -i lo -j ACCEPT"
	[ "${_testmode}" = 0 ] && \
		${c_iptables} -A INPUT -i lo -j ACCEPT

	for dv in ${devlist}
	do
		i=$(echo ${dv} | ${c_sed} 's/eth//g')

		if [ "${ALLOWSELF}" = 1 ]; then
			# subnet 전체 열기
			var_SN="varSN=\${ETH${i}_SUBNET}"
			var_NT="varNT=\${ETH${i}_NET}"

			eval ${var_SN}
			eval ${var_NT}

			if [ -n "${varNT}" -a -n "${varSN}" ]; then
				o_echo "  * iptables -A INPUT -s ${varNT}/${varSN} -j ACCEPT"
				[ "${_testmode}" = 0 ] && \
					${c_iptables} -A INPUT -s ${varNT}/${varSN} -j ACCEPT
			fi
		else
			# 자신의 IP 만 열기
			var_IP="varIP=\${ETH${i}_IPADDR}"
			eval ${var_IP}

			if [ -n "${varIP}" ]; then
				o_echo "  * iptables -A INPUT -s ${varIP} -j ACCEPT"
				[ "${_testmode}" = 0 ] && \
					${c_iptables} -A INPUT -s ${varIP} -j ACCEPT
			fi
		fi
	done

	if [ "${ALLOWALL}" != "" ] ; then
		for _values in ${ALLOWALL}
		do
			iprange_set ${_values} values
			iprange_check ${values}
			rangechk=$?

			[ ${rangechk} -eq 1 ] && redir="-p all -m iprange --src-range" || redir="-s"
			o_echo "  * iptables -A INPUT ${redir} ${values} -j ACCEPT"
			[ "${_testmode}" = 0 ] && \
				${c_iptables} -A INPUT ${redir} ${values} -j ACCEPT
		done
	fi
}

# 1st argument   : incoming/outgoing
# 2st argument ~ : option directives
#
add_port_rule() {
	__type=$1
	shift;
	__types=$*

	for i in ${__types}
	do
		if [ -n "$(echo ${i} | grep TCP)" ]; then
			__proto="tcp"
			__ment="TCP"
		else
			__proto="udp"
			__ment="UDP"
		fi

		[ -n "$(echo ${i} | grep HOSTPERPORT)" ] && __host=1 || __host=0
		[ "${__type}" = "incoming" ] && __pname="--dport" || __pname="--sport"

		eval "_values=\$${i}"

		[ $__host -eq 0 ] && o_echo $"  * ${__ment} service"

		for v in $_values
		do
			echo ${v} | {
				if [ ${__host} -eq 1 ]; then
					IFS=':' read _hosts oport
					iprange_set ${_hosts} c_hosts
					hosts=" -s ${c_hosts}"
				else
					IFS=':' read oport tconnect
					hosts=""
				fi

				if [ "${__proto}" = "tcp" ]; then
					[ "${tconnect}" = "" ] && tconnect=${_nE}
					[ "${__type}" = "incoming" ] && tconnect="${_nN},${_nE}"
					t_connect="-m state --state ${tconnect}"
					add_ftp_rule ${oport} ${__type} "${hosts}"
				else
					t_connect=""
				fi

				oport=$(echo "${oport}" | ${c_sed} -e 's/-/:/g')
				if [ ${__host} -eq 1 ]; then
					iprange_check ${c_hosts}
					rangechk=$?

					if [ ${rangechk} -eq 1 ]; then
						o_echo "    iptables -A INPUT -p ${__proto} -m iprange --src-range ${c_hosts} --dport ${oport} ${t_connect}  -j ACCEPT"
						[ ${_testmode} -eq 0 ] && \
							${c_iptables} -A INPUT -p ${__proto} -m iprange --src-range ${c_hosts} --dport ${oport} ${t_connect}  -j ACCEPT
					else
						o_echo "    iptables -A INPUT${hosts} -p ${__proto} --dport ${oport} ${t_connect}  -j ACCEPT"
						[ ${_testmode} -eq 0 ] && \
							${c_iptables} -A INPUT${hosts} -p ${__proto} --dport ${oport} ${t_connect}  -j ACCEPT
					fi
				else
					o_echo "    iptables -A INPUT${hosts} -p ${__proto} ${__pname} ${oport} ${t_connect} -j ACCEPT"
					[ ${_testmode} -eq 0 ] && \
						${c_iptables} -A INPUT${hosts} -p ${__proto} ${__pname} ${oport} ${t_connect} -j ACCEPT
				fi
			}
		done

		o_echo
	done
}

add_icmp_host() {
	for i in $*
	do
		if [ -n "$(echo ${i} } | grep PING)" ]; then
			__ct="ping"
			__ctype="echo-request"
			__chain="INPUT"
		else
			__ct="traceroute"
			__ctype="time-exceeded"
			__chain="OUTPUT"
		fi

		eval "__ivalue=\$${i}"

		o_echo $"    ==> for ${__ct} service"
		for _v in ${__ivalue}
		do
			iprange_set ${_v} v
			iprange_check ${v}
			rangechk=$?

			if [ ${rangechk} -eq 1 ]; then
				o_echo "    iptables -A ${__chain} -p icmp --icmp-type ${__ctype} -m iprange --src-range ${v} -j ACCEPT"
				${c_iptables} -A ${__chain} -p icmp --icmp-type ${__ctype} -m iprange --src-range ${v} -j ACCEPT
				[ "$__ct" = "traceroute" ] && {
					o_echo "    iptables -A INPUT -p udp -m iprange --src-range ${v} --dport 33434:33525 -j ACCEPT"
					${c_iptables} -A ${__chain} -p udp -m iprange --src-range ${v} --dport 33434:33525 -j ACCEPT
				}
			else
				o_echo "    iptables -A ${__chain} -s ${v} -p icmp --icmp-type ${__ctype} -j ACCEPT"
				${c_iptables} -A ${__chain} -s ${v} -p icmp --icmp-type ${__ctype} -j ACCEPT
				[ "$__ct" = "traceroute" ] && {
					o_echo "    iptables -A INPUT -s ${v} -p udp --dport 33434:33525 -j ACCEPT"
					${c_iptables} -A ${__chain} -s ${v} -p udp --dport 33434:33525 -j ACCEPT
				}
			fi
		done
		o_echo
	done
}

add_tos_rule() {
	[ -z "${USE_TOS}" ] && return 1

	# defined tos chain
	TOS_TABLE="OUTPUT"
	[ ${extend_mangle} -eq 1 ] && TOS_TABLE="INPUT OUTPUT"
	[ ${MASQ_USED} -eq 1 -a ${extend_mangle} -eq 1 ]  && \
		TOS_TABLE="${TOS_TABLE} POSTROUTING"
	[ ${FORWARD_USED} -eq 1 ] && TOS_TABLE="${TOS_TABLE} PREROUTING"
	if [ ${MASQ_USED} -eq 1 -o ${FORWARD_USED} -eq 1 ]; then
		[ ${extend_mangle} -eq 1 ] && TOS_TABLE="${TOS_TABLE} FORWARD"
	fi

	# configure tos
	for i in ${TOS_TABLE}
	do
		o_echo
		o_echo $"  = Tos configuration in ${i} chain"
		for v in ${USE_TOS}
		do
			echo ${v} | {
				IFS=':' read port tosv
				[ "${tport}" = "" ] && tport="dport"
				o_echo "    * iptables -t mangle -A ${i} -p tcp --dport ${port} -j TOS --set-tos ${tosv}"
				o_echo "    * iptables -t mangle -A ${i} -p tcp --sport ${port} -j TOS --set-tos ${tosv}"
				if [ "${_testmode}" = 0 ]; then
					${c_iptables} -t mangle -A ${i} -p tcp --dport ${port} -j TOS --set-tos ${tosv}
					${c_iptables} -t mangle -A ${i} -p tcp --sport ${port} -j TOS --set-tos ${tosv}
				fi
			}
		done
	done

	return 0
}
