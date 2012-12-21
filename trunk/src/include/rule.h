# Rule function
#
# $Id$
#

# {{{ all_all_rule
add_all_rule() {
	for i in INPUT OUTPUT
	do
		[ "${i}" = "INPUT" ] && redir="i" || redir="o"

		printf "  * iptables -A %-6s -%s lo -j ACCEPT\n" ${i} $redir
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
					printf "  * iptables -A %-6s -%s %s/%s -j ACCEPT\n" "${intf}" "${redir}" "${varNT}" "${VARSN}"
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
					printf "  * iptables -A %-6s -%s %-15s -j ACCEPT\n" "${intf}" "${redir}" "${varIP}"
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

	if [ ${BRIDGE_USED} -eq 1 -a -n "${BR_ALLOWALL}" ]; then
		for _values in ${BR_ALLOWALL}
		do
			iprange_set ${_values} values
			iprange_check ${values}
			rangechk=$?

			if [ ${rangechk} -eq 1 ]; then
				r_mode="-p all -m iprange "
				o_echo "  * iptables -A FORWARD -d ${BRG0_NETPX} ${r_mode} --src-range ${values} -j ACCEPT"
				o_echo "  * iptables -A FORWARD -s ${BRG0_NETPX} ${r_mode} --dst-range ${values} -j ACCEPT"
				if [ "${_testmode}" = 0 ]; then
					${c_iptables} -A FORWARD -d ${BRG0_NETPX} ${r_mode} --src-range ${values} -j ACCEPT
					${c_iptables} -A FORWARD -s ${BRG0_NETPX} ${r_mode} --dst-range ${values} -j ACCEPT
				fi
			else
				o_echo "  * iptables -A FORWARD -d ${BRG0_NETPX} -s ${values} -j ACCEPT"
				o_echo "  * iptables -A FORWARD -s ${BRG0_NETPX} -d ${values} -j ACCEPT"
				if [ "${_testmode}" = 0 ]; then
					${c_iptables} -A FORWARD -d ${BRG0_NETPX} -s ${values} -j ACCEPT
					${c_iptables} -A FORWARD -s ${BRG0_NETPX} -d ${values} -j ACCEPT
				fi
			fi
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

				oport=$(echo "${oport}" | ${c_sed} -e 's/-/:/g')
				if [ ${perhost} -eq 1 ]; then
					iprange_check ${rhosts}
					rangechk=$?

					if [ ${rangechk} -eq 1 ]; then
						printf "    iptables -A %-7s -p %-4s -m iprange --dst-range %s --dport %-5s %s -j ACCEPT\n" \
							"${table}" "${proto}" "${rhosts}" "${oport}" "${constate}"
						[ ${_testmode} -eq 0 ] && \
							${c_iptables} -A ${table} -p ${proto} -m iprange --dst-range ${rhosts} \
										--dport ${oport} ${constate} -j ACCEPT
					else
						printf "    iptables -A %-7s%s -p %-4s --dport %-5s %s -j ACCEPT\n" \
							"${table}" "${hosts}" "${proto}" "${oport}" "${constate}"
						[ ${_testmode} -eq 0 ] && \
							${c_iptables} -A ${table}${hosts} -p ${proto} --dport ${oport} ${constate} -j ACCEPT
					fi
				else
					printf "    iptables -A %-7s -p %-4s --dport %-5s %s -j ACCEPT\n" \
							"${table}" "${proto}" "${oport}" "${constate}"
					[ ${_testmode} -eq 0 ] && \
						${c_iptables} -A ${table} -p ${proto} --dport ${oport} ${constate} -j ACCEPT
				fi
			}
		done

		o_echo
	done
}
# }}}

# {{{ add brport_rule
# 1st argument   : incoming/outgoing
# 2st argument ~ : option directives
#
add_brport_rule() {
	a_type=$1; shift
	a_types=$*

	[ ${BRIDGE_USED} -eq 0 ] && return

	for i in ${a_types}
	do
		if [ -n "$(echo ${i} | $c_grep TCP)" ]; then
			a_proto="tcp"
			a_ment="TCP"
		else
			a_proto="udp"
			a_ment="UDP"
		fi

		[ -n "$(echo ${i} | $c_grep HOSTPERPORT)" ] && a_host=1 || a_host=0
		[ $a_host -eq 1 ] && a_ment="${a_ment} per HOST"

		a_redir1="-s ${BRG0_NETPX}"
		a_redir2="-d ${BRG0_NETPX}"
		if [ "${a_type}" = "incoming" ]; then
			a_pname1="--sport"
			a_pname2="--dport"
		else
			a_pname1="--dport"
			a_pname2="--sport"
		fi

		eval "_values=\$${i}"

		o_echo $"  * Bridge ${a_ment} service"

		for v in $_values
		do
			# outgoing DNS 질의는 기본으로 제공하기 때문에 skip
			if [ "${a_type}" = "outgoing" -a "${a_proto}" = "udp" -a "${v}" = "53" ]; then
				continue;
			fi

			echo ${v} | {
				if [ ${a_host} -eq 1 ]; then
					IFS=':' read _hosts oport

					iprange_set ${_hosts} hosts
					iprange_check ${hosts}
					rangechk=$?

					if [ ${rangechk} -eq 1 ]; then
						hosts1=" --dst-range ${hosts}"
						hosts2=" --src-range ${hosts}"
					else
						hosts1=" -d ${hosts}"
						hosts2=" -s ${hosts}"
					fi
				else
					IFS=':' read oport tconnect
				fi

				if [ "${a_proto}" = "tcp" ]; then
					if [ "${tconnect}" = "" ]; then
					   tconnect1="${_nN},${_nE}"
					   tconnect2="${_nE}"
					fi
					if [ "${a_type}" = "incoming" ]; then
						tconnect1="${_nE}"
						tconnect2="${_nN},${_nE}"
					fi

					t_connect1="-m state --state ${tconnect1}"
					t_connect2="-m state --state ${tconnect2}"
				else
					t_connect1=""
					t_connect2=""
				fi

				oport=$(echo "${oport}" | ${c_sed} -e 's/-/:/g')
				if [ ${a_host} -eq 1 ]; then
					if [ ${rangechk} -eq 1 ]; then
						o_echo "    iptables -A FORWARD -p ${a_proto} -m iprange ${hosts1} ${a_pname2} ${oport} ${t_connect2} -j ACCEPT"
						o_echo "    iptables -A FORWARD -p ${a_proto} -m iprange ${hosts2} ${a_pname1} ${oport} ${t_connect1} -j ACCEPT"
						[ ${_testmode} -eq 0 ] && {
							${c_iptables} -A FORWARD -p ${a_proto} -m iprange ${hosts1} ${a_pname2} ${oport} ${t_connect2} -j ACCEPT
							${c_iptables} -A FORWARD -p ${a_proto} -m iprange ${hosts2} ${a_pname1} ${oport} ${t_connect1} -j ACCEPT
						}
					else
						o_echo "    iptables -A FORWARD${hosts1} -p ${a_proto} ${a_pname2} ${oport} ${t_connect2} -j ACCEPT"
						o_echo "    iptables -A FORWARD${hosts2} -p ${a_proto} ${a_pname1} ${oport} ${t_connect1} -j ACCEPT"
						[ ${_testmode} -eq 0 ] && {
							${c_iptables} -A FORWARD${hosts1} -p ${a_proto} ${a_pname2} ${oport} ${t_connect2} -j ACCEPT
							${c_iptables} -A FORWARD${hosts2} -p ${a_proto} ${a_pname1} ${oport} ${t_connect1} -j ACCEPT
						}
					fi
				else
					o_echo "    iptables -A FORWARD ${a_redir1} -p ${a_proto} ${a_pname1} ${oport} ${t_connect1} -j ACCEPT"
					o_echo "    iptables -A FORWARD ${a_redir2} -p ${a_proto} ${a_pname2} ${oport} ${t_connect2} -j ACCEPT"
					[ ${_testmode} -eq 0 ] && {
						${c_iptables} -A FORWARD ${a_redir1} -p ${a_proto} ${a_pname1} ${oport} ${t_connect1} -j ACCEPT
						${c_iptables} -A FORWARD ${a_redir2} -p ${a_proto} ${a_pname2} ${oport} ${t_connect2} -j ACCEPT
					}
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
				printf "      iptables -A %-7s -p icmp --icmp-type %-13s -m iprange --src-range %s -j ACCEPT\n" \
						"${table}" "${type}" "${v}"
				${c_iptables} -A ${table} -p icmp --icmp-type ${type} -m iprange --src-range ${v} -j ACCEPT
				[ "${ment}" = "traceroute" ] && {
					printf "      iptables -A %-7s -p udp -m iprange --src-range ${v} --dport 33434:33525 -j ACCEPT\n" \
							"${table}" "${v}"
					${c_iptables} -A ${table} -p udp -m iprange --src-range ${v} --dport 33434:33525 -j ACCEPT
				}
			else
				#o_echo "      iptables -A ${table} -s ${v} -p icmp --icmp-type ${type} -j ACCEPT"
				printf "      iptables -A %-7s -s %-15s -p icmp --icmp-type %-13s -j ACCEPT\n" \
						"${table}" "${v}" "${type}"
				${c_iptables} -A ${table} -s ${v} -p icmp --icmp-type ${type} -j ACCEPT
				[ "${ment}" = "traceroute" ] && {
					printf "      iptables -A %-7s -s %-15s -p udp --dport 33434:33525 -j ACCEPT\n" \
							"${table}" "${v}"
					${c_iptables} -A ${table} -s ${v} -p udp --dport 33434:33525 -j ACCEPT
				}
			fi
		done
		o_echo
	done
}
# }}}

# {{{ add_bricmp_host
add_bricmp_host() {
	[ ${BRIDGE_USED} -eq 0 ] && return

	for i in $*
	do
		if [ -n "$(echo ${i} | ${c_grep} PING)" ]; then
			i_ct="ping"
			i_ctype="echo-request"
		else
			i_ct="traceroute"
			i_ctype="time-exceeded"
		fi

		eval "i_ivalue=\$${i}"

		o_echo $"    ==> for Bridge ${i_ct} service"
		for v in ${i_ivalue}
		do
			iprange_check ${v}
			rangechk=$?

			if [ ${rangechk} -eq 1 ]; then
				o_echo "      iptables -A FORWARD -d ${BRG0_NETPX} -p icmp --icmp-type ${i_ctype} -m iprange --src-range ${v} -j ACCEPT"
				${c_iptables} -A FORWARD -d ${BRG0_NETPX} -p icmp --icmp-type ${i_ctype} -m iprange --src-range ${v} -j ACCEPT
				[ "$i_ct" = "traceroute" ] && {
					o_echo "      iptables -A FORWARD -d ${BRG0_NETPX} -p udp -m iprange --src-range ${v} --dport 33434:33525 -j ACCEPT"
					o_echo "      iptables -A FORWARD -s ${BRG0_NETPX} -p udp -m iprange --dst-range ${v} --sport 32767:33167 -j ACCEPT"
					${c_iptables} -A FORWARD -d ${BRG0_NETPX} -p udp -m iprange --src-range ${v} --dport 33434:33525 -j ACCEPT
					${c_iptables} -A FORWARD -s ${BRG0_NETPX} -p udp -m iprange --dst-range ${v} --sport 32767:33167 -j ACCEPT
				}
			else
				o_echo "      iptables -A FORWARD -d ${BRG0_NETPX} -s ${v} -p icmp --icmp-type ${i_ctype} -j ACCEPT"
				${c_iptables} -A FORWARD -d ${BRG0_NETPX} -s ${v} -p icmp --icmp-type ${i_ctype} -j ACCEPT
				[ "$i_ct" = "traceroute" ] && {
					o_echo "      iptables -A FORWARD -d ${BRG0_NETPX} -s ${v} -p udp --dport 33434:33525 -j ACCEPT"
					o_echo "      iptables -A FORWARD -s ${BRG0_NETPX} -d ${v} -p udp --sport 32767:33167 -j ACCEPT"
					${c_iptables} -A FORWARD -d ${BRG0_NETPX} -s ${v} -p udp --dport 33434:33525 -j ACCEPT
					${c_iptables} -A FORWARD -s ${BRG0_NETPX} -d ${v} -p udp --sport 32767:33167 -j ACCEPT
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
				port=$(echo "${port}" | ${c_sed} 's/-/:/g')
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
