# Forward rule function
#
# $Id: forward.h,v 1.6 2007-03-28 11:59:07 oops Exp $
#

add_forward_init() {
	[ -n "${TCP_FORWARD_TO}" ] && __finit=0 || __finit=1
	[ -n "${TCP_FORWARD_TO_S}" ] && __finit=0
	[ -n "${UDP_FORWARD_TO}" ] && __finit=0
	[ -n "${UDP_FORWARD_TO_S}" ] &&  __finit=0
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

	o_echo $"    Deprecated Direction Check"
	if [ -n "${FORWARD_MASTER}" ]; then
		o_echo -n "    * "
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
		}
	done

	for i in TCP_FORWARD_TO_S TCP_FORWARD_TO UDP_FORWARD_TO_S UDP_FORWARD_TO
	do
		[ -n "$(echo ${i} | $c_grep TCP)" ] && _fp="tcp" || _fp="udp"
		[ -n "$(echo ${i} | $c_grep _S)" ] && _fs=1 || _fs=0
		eval "_fv=\$${i}"

		case "${i}" in
			TCP*) proto="tcp";;
			UDP*) proto="udp";;
		esac

		for v in $_fv
		do
			_chk=0
			echo ${v} | {
				if [ $_fs -eq 0 ]; then
					IFS=':' read lports raddr rports
					laddr=${FIREWALL_WAN}
					ladd_dev=1

					[ -z "${lports}" ] && _chk=1
					[ -z "${raddr}" ] && _chk=1
					[ -z "${rports}" ] && _chk=1
				else
					IFS=':' read laddr lports raddr rports
					ladd_dev=0
					[ -z "${laddr}" ] && _chk=1
					[ -z "${lports}" ] && _chk=1
					[ -z "${raddr}" ] && _chk=1
					[ -z "${rports}" ] && _chk=1
				fi

				if [ $_chk -eq 1 ]; then
					echo "  * ${i} => Wrong Configuration Value : ${v}"
					continue
				fi

				for _laddr in ${laddr}
				do
					[ $ladd_dev -eq 1 ] && f_target="-i" || f_target="-d"
					o_echo "  * iptables -t nat -A PREROUTING -p ${proto} ${f_target} ${_laddr} \\"
					o_echo "             --dport ${lports} -j DNAT --to ${raddr}:${rports}"
					[ "${_testmode}" -eq 0 ] && \
						${c_iptables} -t nat -A PREROUTING -p ${proto} ${f_target} ${_laddr} \
									--dport ${lports} -j DNAT --to ${raddr}:${rports}
				done
			}
		done
	done
}
