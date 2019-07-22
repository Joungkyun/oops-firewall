# Forward rule function
#
# $Id$
#

add_forward_init() {
	[ -n "${TCP_FORWARD_TO}" ] && __finit=0 || __finit=1
	[ -n "${TCP_FORWARD_TO_S}" ] && __finit=0
	[ -n "${TCP_FORWARD_TO_SS}" ] && __finit=0
	[ -n "${UDP_FORWARD_TO}" ] && __finit=0
	[ -n "${UDP_FORWARD_TO_S}" ] &&  __finit=0
	[ -n "${UDP_FORWARD_TO_SS}" ] &&  __finit=0
	[ -n "${ALL_FORWARD_TO}" ] && __finit=0

	# if exists masq device (or have several ethernet device),
	# set outgoing ip address on nic address that set masq device
	#if [ "${MASQ_USED}" = "0" ]; then
	#	if [ -n "${FORWARD_MASTER}" ]; then
	#		WordToUpper ${FORWARD_MASTER} FORWARD_MASTER
	#		eval "INCOM_ADDR=\"\$${FORWARD_MASTER}_IPADDR\""
	#		[ -z "${INCOM_ADDR}" ] && INCOM_ADDR=${ETH0_IPADDR}
	#	else
	#		INCOM_ADDR=${ETH0_IPADDR}
	#	fi
	#else
	#	INCOM_ADDR=${MASQ_IPADDR}
	#fi

	o_echo $"  * Deprecated Direction Check"
	if [ -n "${FORWARD_MASTER}" ]; then
		o_echo -n "    ==> "
		print_color $"FORWARD_MASTER is Deprecated." red
		o_echo
	fi
	o_echo

	[ $__finit -eq 0 ] && FORWARD_USED=1 || FORWARD_USED=0
	export FORWARD_USED

	return $__finit
}

add_forward_rule() {
	add_forward_init
	[ $? -ne 0 ] && return 0

	for v in ${ALL_FORWARD_TO}
	do
		echo ${v} | {
			IFS=':' read dest target
			o_echo "  * iptables -t nat -A PREROUTING -d ${dest}  -j DNAT --to ${target}"
			[ "${_testmode}" = 0 ] && \
				${c_iptables} -t nat -A PREROUTING -d ${dest} -j DNAT --to ${target}

			# For Bridge mode
			if [ "${BRIDGE_USED}" -ne 0 ]; then
				o_echo "  * iptables -A FORWARD -s ${dest} -d ${target} -j ACCEPT"
				[ "${_testmode}" = 0 ] && \
					${c_iptables} -A FORWARD -s ${dest} -d ${target} -j ACCEPT
			fi
		}
	done

	for i in TCP_FORWARD_TO_SS TCP_FORWARD_TO_S TCP_FORWARD_TO UDP_FORWARD_TO_S UDP_FORWARD_TO_SS UDP_FORWARD_TO
	do
		[ -n "$(echo ${i} | $c_grep TCP)" ] && _fp="tcp" || _fp="udp"

		_fs=0
		[ -n "$(echo ${i} | $c_grep '_S$')" ] && _fs=1
		[ -n "$(echo ${i} | $c_grep '_SS$')" ] && _fs=2

		eval "_fv=\$${i}"

		case "${i}" in
			TCP*) proto="tcp";;
			UDP*) proto="udp";;
		esac

		for v in $_fv
		do
			local _chk=0
			local srcf=
			local rangemod=

			echo ${v} | {
				if [ $_fs -eq 2 ]; then
					IFS=':' read src laddr lports raddr rports
					ladd_dev=0
					[ -z "${src}" ] && _chk=1
					[ -z "${laddr}" ] && _chk=1
					[ -z "${lports}" ] && _chk=1
					[ -z "${raddr}" ] && _chk=1
					[ -z "${rports}" ] && _chk=1

					iprange_set "${src}" src
					iprange_check "${src}"
					range_chk=$?

					if [ $range_chk -eq 1 ]; then
						range_mod=" -m iprange"
						srcf="${range_mod} --src-range ${src}"
					else
						srcf=" -s ${src}"
					fi
				elif [ $_fs -eq 1 ]; then
					IFS=':' read laddr lports raddr rports
					ladd_dev=0
					[ -z "${laddr}" ] && _chk=1
					[ -z "${lports}" ] && _chk=1
					[ -z "${raddr}" ] && _chk=1
					[ -z "${rports}" ] && _chk=1
				else
					IFS=':' read lports raddr rports
					laddr=${FIREWALL_WAN}
					ladd_dev=1
					[ -z "${lports}" ] && _chk=1
					[ -z "${raddr}" ] && _chk=1
					[ -z "${rports}" ] && _chk=1
				fi

				if [ $_chk -eq 1 ]; then
					echo $"  * ${i} => Wrong Configuration Value : ${v}"
					continue
				fi

				port_set "" "${lports}" lports

				for _laddr in ${laddr}
				do
					[ $ladd_dev -eq 1 ] && f_target="-i" || f_target="-d"

					if [ $ladd_dev -eq 0 ]; then
						iprange_set "${_laddr}" _laddr
						iprange_check "${_laddr}"
						range_chk=$?

						[ ${range_chk} -eq 1 -a -z "${range_mode}" ] && range_mod="-m iprange "
						[ ${range_chk} -eq 1 ] && f_target="${range_mod}--dst-range"
					fi

					o_echo "  * iptables -t nat -A PREROUTING -p ${proto}${srcf} \\"
					o_echo "             ${f_target} ${_laddr} \\"
					o_echo "             --dport ${lports} -j DNAT --to ${raddr}:${rports}"
					[ "${_testmode}" -eq 0 ] && \
						${c_iptables} -t nat -A PREROUTING -p ${proto}${srcf} ${f_target} ${_laddr} \
									--dport ${lports} -j DNAT --to ${raddr}:${rports}
				done
			}
		done
	done
}

#
# Local variables:
# tab-width: 4
# c-basic-offset: 4
# End:
# vim: filetype=sh noet sw=4 ts=4 fdm=marker
# vim<600: noet sw=4 ts=4:
#
