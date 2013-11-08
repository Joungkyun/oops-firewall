# Rule function
#
# $Id$
#

# {{{ add_all_rule
add_all_rule() {
	for i in INPUT OUTPUT
	do
		[ "${i}" = "INPUT" ] && redir="i" || redir="o"

		[ $_verbose -eq 1 ] && {
			printf "  * iptables -A %-6s -%s lo -j ACCEPT\n" ${i} $redir
		}
		[ "${_testmode}" = 0 ] && \
			${c_iptables} -A ${i} -${redir} lo -j ACCEPT
	done

	for dv in ${devlist}
	do
		WordToUpper ${dv} UPPERDEV

		if [ "${ALLOWSELF}" = 1 ]; then
			# subnet 전체 열기
			var_SN="varSN=\${${UPPERDEV}_SUBNET}"
			var_NT="varNT=\${${UPPERDEV}_NET}"

			eval ${var_SN}
			eval ${var_NT}

			if [ -n "${varNT}" -a -n "${varSN}" ]; then
				for intf in INPUT OUTPUT
				do
					[ "${intf}" = "INPUT" ] && redir="s" || redir="d"
					[ $_verbose -eq 1 ] && {
						printf "  * iptables -A %-6s -%s %s/%s -j ACCEPT\n" "${intf}" "${redir}" "${varNT}" "${varSN}"
					}
					[ "${_testmode}" = 0 ] && \
						${c_iptables} -A ${intf} -${redir} ${varNT}/${varSN} -j ACCEPT
				done
			fi
		else
			# 자신의 IP 만 열기
			var_IP="varIP=\${${UPPERDEV}_IPADDR}"
			eval ${var_IP}

			if [ -n "${varIP}" ]; then
				for intf in INPUT OUTPUT
				do
					[ "${intf}" = "INPUT" ] && redir="s" || redir="d"
					#o_echo "  * iptables -A ${pintf} ${redir} ${varIP} -j ACCEPT"
					[ $_verbose -eq 1 ] && {
						printf "  * iptables -A %-6s -%s %-15s -j ACCEPT\n" "${intf}" "${redir}" "${varIP}"
					}
					[ "${_testmode}" = 0 ] && \
						${c_iptables} -A ${intf} -${redir} ${varIP} -j ACCEPT
				done
			fi
		fi
	done

	if [ "${ALLOWALL}" != "" ] ; then
		for _values in ${ALLOWALL}
		do
			iprange_set ${_values} values
			iprange_check ${values}
			rangechk=$?

			for intf in INPUT OUTPUT
			do
				[ "${intf}" = "INPUT" ] && pintf="INPUT " || pintf="OUTPUT"
				if [ ${rangechk} -eq 1 ]; then
					r_mode="-p all -m iprange"
					[ "${intf}" = "INPUT" ] && redir="${r_mode} --src-range" || redir="${r_mode} --dst-range"
				else
					[ "${intf}" = "INPUT" ] && redir="-s" || redir="-d"
				fi
				o_echo "  * iptables -A ${pintf} ${redir} ${values} -j ACCEPT"
				[ "${_testmode}" = 0 ] && \
					${c_iptables} -A ${intf} ${redir} ${values} -j ACCEPT
			done
		done
	fi
}
# }}}

# {{{ add port_rule
# 1st argument   : incoming/outgoing
# 2st argument ~ : option directives
#
add_port_rule() {
	local type=$1; shift
	local types=$*
	local constate="-m state --state"

	local proto
	local ment
	local perhost

	[ "$type" = "incoming" ] && table="INPUT" || table="OUTPUT"

	for i in ${types}
	do
		chk="$(echo ${i} | $c_grep TCP)"
		[ -n "${chk}" ] && proto="tcp" || proto="udp"
		[ -n "${chk}" ] && ment="TCP" || ment="UDP"

		chk="$(echo ${i} | $c_grep HOSTPERPORT)"
		[ -n "${chk}" ] && perhost=1 || perhost=0
		[ -n "${chk}" ] && ment="${ment} per HOST"

		eval "_values=\$${i}"

		o_echo $"  * ${ment} service"

		for v in $_values
		do
			echo ${v} | {
				if [ ${perhost} -eq 1 ]; then
					IFS=':' read _hosts oport tconnect
					iprange_set ${_hosts} rhosts
					[ "${table}" = "OUTPUT" ] && hosts=" -d" || hosts=" -s"
					hosts="${hosts} ${rhosts}"
				else
					IFS=':' read oport tconnect
				fi

				[ "${tconnect}" = "" ] &&
					constate="${constate} ${_nN}" || \
					constate="${constate} ${tconnect}"

				port_set "" ${oport} oport
				if [ ${perhost} -eq 1 ]; then
					iprange_check ${rhosts}
					rangechk=$?

					if [ ${rangechk} -eq 1 ]; then
						[ "${table}" = "INPUT" ] && target="src-range" || target="dst-range"
						[ $_verbose -eq 1 ] && {
							printf "    iptables -A %-7s -p %-4s -m iprange --%s %s --dport %-5s %s -j ACCEPT\n" \
								"${table}" "${proto}" "${target}" "${rhosts}" "${oport}" "${constate}"
						}
						[ ${_testmode} -eq 0 ] && \
							${c_iptables} -A ${table} -p ${proto} -m iprange --${target} ${rhosts} \
										--dport ${oport} ${constate} -j ACCEPT
					else
						[ $_verbose -eq 1 ] && {
							printf "    iptables -A %-7s%s -p %-4s --dport %-5s %s -j ACCEPT\n" \
								"${table}" "${hosts}" "${proto}" "${oport}" "${constate}"
						}
						[ ${_testmode} -eq 0 ] && \
							${c_iptables} -A ${table}${hosts} -p ${proto} --dport ${oport} ${constate} -j ACCEPT
					fi
				else
					[ $_verbose -eq 1 ] && {
						printf "    iptables -A %-7s -p %-4s --dport %-5s %s -j ACCEPT\n" \
							"${table}" "${proto}" "${oport}" "${constate}"
					}
					[ ${_testmode} -eq 0 ] && \
						${c_iptables} -A ${table} -p ${proto} --dport ${oport} ${constate} -j ACCEPT
				fi
			}
		done

		o_echo
	done
}
# }}}

# {{{ add_icmp_host
add_icmp_host() {
	local ment
	local type
	local table

	for i in $*
	do
		if [ -n "$(echo ${i} | ${c_grep} PING)" ]; then
			ment="ping"
			type="echo-request"
			table="INPUT"
		else
			ment="traceroute"
			type="time-exceeded"
			table="OUTPUT"
		fi

		eval "i_ivalue=\$${i}"

		o_echo $"    ==> for ${ment} service"
		for _v in ${i_ivalue}
		do
			iprange_set ${_v} v
			iprange_check ${v}
			rangechk=$?

			if [ ${rangechk} -eq 1 ]; then
				[ $_verbose -eq 1 ] && {
					printf "      iptables -A %-7s -p icmp --icmp-type %-13s -m iprange --src-range %s -j ACCEPT\n" \
						"${table}" "${type}" "${v}"
				}

				[ "${_testmode}" = 0 ] &&  \
					${c_iptables} -A ${table} -p icmp --icmp-type ${type} -m iprange --src-range ${v} -j ACCEPT

				[ "${ment}" = "traceroute" ] && {
					[ $_verbose -eq 1 ] && {
						printf "      iptables -A %-7s -p icmp --icmp-type port-unreachable -m iprange --src-range ${v} -j ACCEPT\n" \
							"OUTPUT" "${v}"
						# traceroute default port
						printf "      iptables -A %-7s -p udp  -m iprange --src-range ${v} --dport 33434:33525 -j ACCEPT\n" \
							"INPUT" "${v}"
						# tracepath default port
						printf "      iptables -A %-7s -p udp  -m iprange --src-range ${v} --dport 44444:44624 -j ACCEPT\n" \
							"INPUT" "${v}"
					}
					[ "${_testmode}" = 0 ] && {
						${c_iptables} -A OUTPUT -p icmp --icmp-type port-unreachable -m iprange --src-range ${v} -j ACCEPT
						${c_iptables} -A INPUT -p udp -m iprange --src-range ${v} --dport 33434:33525 -j ACCEPT
						${c_iptables} -A INPUT -p udp -m iprange --src-range ${v} --dport 44444:44624 -j ACCEPT
					}
				}
			else
				#o_echo "      iptables -A ${table} -s ${v} -p icmp --icmp-type ${type} -j ACCEPT"
				[ $_verbose -eq 1 ] && {
					printf "      iptables -A %-7s -s %-15s -p icmp --icmp-type %-16s -j ACCEPT\n" \
						"${table}" "${v}" "${type}"
				}

				[ "${_testmode}" = 0 ] &&  \
					${c_iptables} -A ${table} -s ${v} -p icmp --icmp-type ${type} -j ACCEPT

				[ "${ment}" = "traceroute" ] && {
					[ $_verbose -eq 1 ] && {
						printf "      iptables -A %-7s -s %-15s -p icmp --icmp-type port-unreachable -j ACCEPT\n" \
							"OUTPUT" "${v}"
						# traceroute default port
						printf "      iptables -A %-7s -s %-15s -p udp  --dport 33434:33525 -j ACCEPT\n" \
							"INPUT" "${v}"
						# tracepath default port
						printf "      iptables -A %-7s -s %-15s -p udp  --dport 44444:44624 -j ACCEPT\n" \
							"INPUT" "${v}"
					}

					[ "${_testmode}" = 0 ] && {
						${c_iptables} -A OUTPUT -s ${v} -p icmp --icmp-type port-unreachable -j ACCEPT
						${c_iptables} -A INPUT -s ${v} -p udp --dport 33434:33525 -j ACCEPT
						${c_iptables} -A INPUT -s ${v} -p udp --dport 44444:44624 -j ACCEPT
					}
				}
			fi
		done
		o_echo
	done
}
# }}}

# {{{ add_tos_rule
add_tos_rule() {
	[ -z "${USE_TOS}" ] && return 1

	# defined tos chain
	TOS_TABLE="OUTPUT"
	[ ${extend_mangle} -eq 1 ] && TOS_TABLE="INPUT OUTPUT"
	[ ${BRIDGE_USED} -eq 1 -a ${extend_mangle} -eq 1 ] && \
		TOS_TABLE="${TOS_TABLE} FORWARD"
	[ ${MASQ_USED} -eq 1 -a ${extend_mangle} -eq 1 ]  && \
		TOS_TABLE="${TOS_TABLE} POSTROUTING"
	[ ${FORWARD_USED} -eq 1 -a ${extend_mangle} -eq 1 ] && \
		TOS_TABLE="${TOS_TABLE} PREROUTING"

	# configure tos
	for i in ${TOS_TABLE}
	do
		o_echo
		o_echo $"  * Tos configuration in ${i} chain"
		for v in ${USE_TOS}
		do
			echo ${v} | {
				IFS=':' read port tosv
				port_set "" "${port}" port
				o_echo "    iptables -t mangle -A ${i} -p tcp --dport ${port} -j TOS --set-tos ${tosv}"
				o_echo "    iptables -t mangle -A ${i} -p tcp --sport ${port} -j TOS --set-tos ${tosv}"
				if [ "${_testmode}" = 0 ]; then
					${c_iptables} -t mangle -A ${i} -p tcp --dport ${port} -j TOS --set-tos ${tosv}
					${c_iptables} -t mangle -A ${i} -p tcp --sport ${port} -j TOS --set-tos ${tosv}
				fi
			}
		done
	done

	return 0
}
# }}}

#
# Local variables:
# tab-width: 4
# c-basic-offset: 4
# End:
# vim: set filetype=sh noet sw=4 ts=4 fdm=marker:
# vim<600: noet sw=4 ts=4:
#
