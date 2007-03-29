# Rule function
#
# $Id: rule.h,v 1.6 2007-03-29 17:53:30 oops Exp $
#

add_named_port() {
	n_mode=${1}

	if [ "${n_mode}" != "BRIDGE" ]; then
		o_echo "    iptables -A INPUT   -p udp --sport 53 -j ACCEPT"
		[ ${_testmode} -eq 0 ] && \
			${c_iptables} -A INPUT   -p udp --sport 53 -j ACCEPT
	else
		o_echo "    iptables -A FORWARD -s ${BRG0_NETPX} -p udp --dport 53 -j ACCEPT"
		o_echo "    iptables -A FORWARD -d ${BRG0_NETPX} -p udp --sport 53 -j ACCEPT"
		[ ${_testmode} -eq 0 ] && {
			${c_iptables} -A FORWARD -s ${BRG0_NETPX} -p udp --dport 53 -j ACCEPT
			${c_iptables} -A FORWARD -d ${BRG0_NETPX} -p udp --sport 53 -j ACCEPT
		}
	fi
}

# 1st argument : port
# 2st argument : incoming/outgoing
#
add_ftp_rule() {
	f_port=${1}
	f_type=${2}
	f_host="${3}"
	f_port=${f_port=0}
	f_prange=
	f_ftpchk=0
	f_table="INPUT"


	if [ -n "$(echo ${f_type} | ${c_grep} 'BR_' 2> /dev/null)" ]; then
		f_table="FORWARD"
		f_type="$(echo ${f_type} | ${c_sed} 's/BR_//g')"
	fi

	case "${f_table}" in
		INPUT)  fp_table="INPUT  ";;
		OUTPUT) fp_table="OUTPUT ";;
		*)      fp_table="${f_table}"
	esac

	f_prange=$(echo ${f_port} | ${c_grep} "-")
	if [ -n "${f_prange}" ]; then
		f_prange=$(echo ${f_port} | ${c_sed} -e 's/-/ /g')
		__sport=$(echo ${f_prange} | ${c_awk} '{print $1}')
		__eport=$(echo ${f_prange} | ${c_awk} '{print $2}')

		__sport=${__sport=0}
		__eport=${__eport=0}

		[ ${__sport} -lt 21 -a ${__eport} -gt 21 ] && f_ftpchk=1
	else
		[ $f_port -eq 21 ] && f_ftpchk=1
	fi

	if [ $f_ftpchk -eq 1 ]; then
		if [ "${f_table}" != "FORWARD" ]; then
			[ "${f_type}" = "incoming" ] && f_tmpfile="${_tmpifile}" || f_tmpfile="${_tmpofile}"
		else
			[ "${f_type}" = "incoming" ] && f_tmpfile="${_passiveif}" || f_tmpfile="${_passiveof}"
		fi

		[ -f "${f_tmpfile}" ] && return

		[ "${f_type}" = "incoming" ] && f_pname1="--dport" || f_pname1="--sport"
		[ "${f_type}" = "incoming" ] && f_pname2="--sport" || f_pname2="--dport"

		if [ "${f_table}" = "FORWARD" ]; then
			f_pname1="--dport"
			f_pname2="--sport"
			f_tr1=" -s ${BRG0_NETPX}"
			f_tr2=" -d ${BRG0_NETPX}"
		fi

		# open tcp port 20 for ftp active mode
		o_echo "    iptables -A ${fp_table}${f_tr1} -p tcp ${f_pname1} 20 -m state --state ${_nE},${_nR} -j ACCEPT"
		[ ${_testmode} -eq 0 ] && \
			${c_iptables} -A ${f_table}${f_tr1} -p tcp ${f_pname1} 20 -m state --state ${_nE},${_nR} -j ACCEPT
		if [ "${f_table}" = "FORWARD" ]; then
			o_echo "    iptables -A ${fp_table}${f_tr2} -p tcp ${f_pname2} 20 -m state --state ${_nE},${_nR} -j ACCEPT"
			[ ${_testmode} -eq 0 ] && \
				${c_iptables} -A ${f_table}${f_tr2} -p tcp ${f_pname2} 20 -m state --state ${_nE},${_nR} -j ACCEPT
		else
			o_echo "    iptables -A OUTPUT ${f_tr2} -p tcp ${f_pname2} 20 -m state --state ${_nE},${_nR} -j ACCEPT"
			[ ${_testmode} -eq 0 ] && \
				${c_iptables} -A OUTPUT ${f_tr2} -p tcp ${f_pname2} 20 -m state --state ${_nE},${_nR} -j ACCEPT
		fi

		# open unprivileges tcp port for ftp passive mode
		o_echo "    iptables -A ${fp_table} -p tcp --sport ${_userport} --dport ${_userport} \\"
		o_echo "             -m state --state ${_nE},${_nR} -j ACCEPT"
		[ ${_testmode} -eq 0 ] && \
			${c_iptables} -A ${f_table} -p tcp --sport ${_userport} --dport ${_userport} \
						-m state --state ${_nE},${_nR} -j ACCEPT

		if [ "${f_table}" != "FORWARD" ]; then
			o_echo "    iptables -A OUTPUT  -p tcp --sport ${_userport} --dport ${_userport} \\"
			o_echo "             -m state --state ${_nN} -j ACCEPT"
			[ ${_testmode} -eq 0 ] && \
				${c_iptables} -A OUTPUT -p tcp --sport ${_userport} --dport ${_userport} \
							-m state --state ${_nN} -j ACCEPT
		fi

		touch "${f_tmpfile}"
	fi
}

add_all_rule() {
	for i in INPUT OUTPUT
	do
		if [ "${i}" = "INPUT" ]; then
			i="INPUT "
			redir="i"
		else
			redir="o"
		fi
		o_echo "  * iptables -A ${i} -${redir} lo -j ACCEPT"
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
				o_echo "  * iptables -A INPUT  -s ${varNT}/${varSN} -j ACCEPT"
				[ "${_testmode}" = 0 ] && \
					${c_iptables} -A INPUT -s ${varNT}/${varSN} -j ACCEPT
			fi
		else
			# 자신의 IP 만 열기
			var_IP="varIP=\${${UPPERDEV}_IPADDR}"
			eval ${var_IP}

			if [ -n "${varIP}" ]; then
				o_echo "  * iptables -A INPUT  -s ${varIP} -j ACCEPT"
				[ "${_testmode}" = 0 ] && \
					${c_iptables} -A INPUT -s ${varIP} -j ACCEPT
			fi
		fi
	done

	if [ "${ALLOWALL}" != "" ] ; then
		for values in ${ALLOWALL}
		do
			o_echo "  * iptables -A INPUT  -s ${values} -j ACCEPT"
			[ "${_testmode}" = 0 ] && \
				${c_iptables} -A INPUT -s ${values} -j ACCEPT
		done
	fi

	if [ ${BRIDGE_USED} -eq 1 -a -n "${BR_ALLOWALL}" ]; then
		for values in ${BR_ALLOWALL}
		do
			o_echo "  * iptables -A FORWARD -d ${BRG0_NETPX} -s ${values} -j ACCEPT"
			o_echo "  * iptables -A FORWARD -s ${BRG0_NETPX} -d ${values} -j ACCEPT"
			if [ "${_testmode}" = 0 ]; then
				${c_iptables} -A FORWARD -d ${BRG0_NETPX} -s ${values} -j ACCEPT
				${c_iptables} -A FORWARD -s ${BRG0_NETPX} -d ${values} -j ACCEPT
			fi
		done
	fi
}

# 1st argument   : incoming/outgoing
# 2st argument ~ : option directives
#
add_port_rule() {
	a_table="INPUT"
	a_type=$1; shift
	a_types=$*

	case "${a_table}" in
		INPUT)  ap_table="INPUT  ";;
		OUTPUT) ap_table="OUTPUT ";;
		*)      ap_table="${a_table}"
	esac

	for i in ${a_types}
	do
		if [ -n "$(echo ${i} | $c_grep TCP)" ]; then
			a_proto="tcp"
			a_ment="TCP"
		else
			a_proto="udp"
			a_ment="UDP"
		fi

		_outted=0
		if [ -n "$(echo ${i} | $c_grep HOSTPERPORT)" ]; then
			a_ment="${a_ment} per HOST"
			a_host=1
		else
			a_host=0
		fi

		a_List="${FIREWALL_WAN}"
		if [ "${a_type}" = "incoming" ]; then
			a_pname="--dport"
		else
			a_pname="--sport"
			[ "${a_proto}" = "tcp" ] && _outted=1 || _outted=0
		fi

		eval "_values=\$${i}"

		o_echo $"  * ${a_ment} service"

		for v in $_values
		do
			# outgoing DNS 질으는 기본으로 제공하기 때문에 skip
			if [ "${a_type}" = "outgoing" -a "${a_proto}" = "udp" -a "${v}" = "53" ]; then
				continue;
			fi

			echo ${v} | {
				if [ ${a_host} -eq 1 ]; then
					IFS=':' read hosts oport
					hosts=" -s ${hosts}"
				else
					IFS=':' read oport tconnect
				fi

				if [ "${a_proto}" = "tcp" ]; then
					[ "${tconnect}" = "" ] && tconnect=${_nE}
					[ "${a_type}" = "incoming" ] && tconnect="${_nN},${_nE}"

					t_connect="-m state --state ${tconnect}"
					add_ftp_rule ${oport} ${a_type} "${hosts}"
				fi

				oport=$(echo "${oport}" | ${c_sed} -e 's/-/:/g')
				if [ ${a_host} -eq 1 -a "${a_proto}" = "tcp" ]; then
					o_echo "    iptables -A ${ap_table}${hosts} -p ${a_proto} ${a_pname} ${oport} ${t_connect} -j ACCEPT"
					[ ${_testmode} -eq 0 ] && \
						${c_iptables} -A ${a_table}${hosts} -p ${a_proto} ${a_pname} ${oport} ${t_connect} -j ACCEPT
				else
					for a_int in ${a_List}
					do
						a_intrule="-i ${a_int}"

						o_echo "    iptables -A ${ap_table} ${a_intrule} -p ${a_proto} ${a_pname} ${oport} ${t_connect} -j ACCEPT"
						[ $_outted -eq 1 ] && \
							o_echo "    iptables -A OUTPUT  -o ${a_int} -p ${a_proto} --dport ${oport} -m state --state NEW -j ACCEPT"
						[ ${_testmode} -eq 0 ] && \
							${c_iptables} -A ${a_table} ${a_intrule} -p ${a_proto} ${a_pname} ${oport} ${t_connect} -j ACCEPT
						[ ${_testmode} -eq 0 -a $_outted -eq 1 ] && \
							${c_iptables} -A OUTPUT -o ${a_int} -p ${a_proto} --dport ${oport} -m state --state NEW -j ACCEPT
					done
				fi
			}
		done

		o_echo
	done
}

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
					IFS=':' read hosts oport
					hosts1=" -d ${hosts}"
					hosts2=" -s ${hosts}"
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
					add_ftp_rule ${oport} BR_${a_type} "${hosts}"
				fi

				oport=$(echo "${oport}" | ${c_sed} -e 's/-/:/g')
				if [ ${a_host} -eq 1 ]; then
					if [ "${a_prot}" = "tcp" ]; then
						o_echo "    iptables -A FORWARD${hosts} -p ${a_proto} ${a_pname1} ${oport} ${t_connect1} -j ACCEPT"
						o_echo "    iptables -A FORWARD${hosts} -p ${a_proto} ${a_pname2} ${oport} ${t_connect2} -j ACCEPT"
						[ ${_testmode} -eq 0 ] && {
							${c_iptables} -A FORWARD${hosts} -p ${a_proto} ${a_pname1} ${oport} ${t_connect1} -j ACCEPT
							${c_iptables} -A FORWARD${hosts} -p ${a_proto} ${a_pname2} ${oport} ${t_connect2} -j ACCEPT
						}
					else
						o_echo "    iptables -A FORWARD${hosts1} -p ${a_proto} ${a_pname1} ${oport} -j ACCEPT"
						o_echo "    iptables -A FORWARD${hosts2} -p ${a_proto} ${a_pname2} ${oport} -j ACCEPT"
						[ ${_testmode} -eq 0 ] && {
							${c_iptables} -A FORWARD${hosts1} -p ${a_proto} ${a_pname1} ${oport} -j ACCEPT
							${c_iptables} -A FORWARD${hosts2} -p ${a_proto} ${a_pname2} ${oport} -j ACCEPT
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

add_icmp_host() {
	for i in $*
	do
		if [ -n "$(echo ${i} | ${c_grep} PING)" ]; then
			i_ct="ping"
			i_ctype="echo-request"
			i_chain="INPUT"
			i_pchain="INPUT  "
		else
			i_ct="traceroute"
			i_ctype="time-exceeded"
			i_chain="OUTPUT"
			i_pchain="OUTPUT "
		fi

		eval "i_ivalue=\$${i}"

		o_echo $"    ==> for ${i_ct} service"
		for v in ${i_ivalue}
		do
			o_echo "    iptables -A ${i_pchain} -s ${v} -p icmp --icmp-type ${i_ctype} -j ACCEPT"
			${c_iptables} -A ${i_chain} -s ${v} -p icmp --icmp-type ${i_ctype} -j ACCEPT
			[ "$i_ct" = "traceroute" ] && {
				o_echo "    iptables -A ${i_pchain} -s ${v} -p udp --dport 33434:33525 -j ACCEPT"
				${c_iptables} -A ${i_chain} -s ${v} -p udp --dport 33434:33525 -j ACCEPT
			}
		done
		o_echo
	done
}

add_bricmp_host() {
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
			o_echo "      iptables -A FORWARD -d ${BRG0_NETPX} -s ${v} -p icmp --icmp-type ${i_ctype} -j ACCEPT"
			${c_iptables} -A FORWARD -d ${BRG0_NETPX} -s ${v} -p icmp --icmp-type ${i_ctype} -j ACCEPT
			[ "$i_ct" = "traceroute" ] && {
				o_echo "      iptables -A FORWARD -d ${BRG0_NETPX} -s ${v} -p udp --dport 33434:33525 -j ACCEPT"
				o_echo "      iptables -A FORWARD -s ${BRG0_NETPX} -d ${v} -p udp --sport 32767:33167 -j ACCEPT"
				${c_iptables} -A FORWARD -d ${BRG0_NETPX} -s ${v} -p udp --dport 33434:33525 -j ACCEPT
				${c_iptables} -A FORWARD -s ${BRG0_NETPX} -d ${v} -p udp --sport 32767:33167 -j ACCEPT
			}
		done
		o_echo
	done
}

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
		o_echo $"  = Tos configuration in ${i} chain"
		for v in ${USE_TOS}
		do
			echo ${v} | {
				IFS=':' read port tosv
				port=$(echo "${port}" | ${c_sed} 's/-/:/g')
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
