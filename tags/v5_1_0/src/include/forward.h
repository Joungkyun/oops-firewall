# Forward rule function
#
# $Id: forward.h,v 1.2.2.1 2008-07-19 06:58:10 oops Exp $
#

add_forward_init() {
	[ -n "${TCP_FORWARD_TO}" ] && __finit=0 || __finit=1
	[ -n  "${UDP_FORWARD_TO_S}" ] && __finit=0
	[ -n "${UDP_FORWARD_TO}" ] && __finit=0
	[ -n "${UDP_FORWARD_TO_S}" ] &&  __finit=0
	[ -n "${ALL_FORWARD_TO}" ] && __finit=0

	# if exists masq device (or have several ethernet device),
	# set outgoing ip address on nic address that set masq device
	if [ "${MASQ_USED}" = "0" ]; then
		if [ -n "${FORWARD_MASTER}" ]; then
			WordToUpper ${FORWARD_MASTER} FORWARD_MASTER
			eval "INCOM_ADDR=\"\$${FORWARD_MASTER}_IPADDR\""
			[ -z "${INCOM_ADDR}" ] && INCOM_ADDR=${ETH0_IPADDR}
		else
			INCOM_ADDR=${ETH0_IPADDR}
		fi
	else
		INCOM_ADDR=${MASQ_IPADDR}
	fi

	[ $__finit -eq 0 ] && FORWARD_USED=1 || FORWARD_USED=0
	export FORWARD_USED

	return $__finit
}

add_forward_rule() {
	add_forward_init
	[ $? -ne 0 ] && return 0

	for i in TCP_FORWARD_TO TCP_FORWARD_TO_S UDP_FORWARD_TO UDP_FORWARD_TO_S
	do
		[ -n "$(echo ${i} | grep TCP)" ] && _fp="tcp" || _fp="udp"
		[ -n "$(echo ${i} | grep _S)" ] && _fs=1 || _fs=0
		eval "_fv=\$${i}"

		case "${i}" in
			TCP*) proto="tcp";;
			UDP*) proto="udp";;
		esac

		for v in $_fv
		do
			echo ${v} | {
				if [ $_fs -eq 0 ]; then
					IFS=':' read lports raddr rports
					laddr=${INCOM_ADDR}
				else
					IFS=':' read laddr lports raddr rports
				fi

				o_echo "  * iptables -t nat -A PREROUTING -p ${proto} -d ${laddr} \\"
				o_echo "             --dport ${lports} -j DNAT --to ${raddr}:${rports}"
				[ "${_testmode}" -eq 0 ] && \
					${c_iptables} -t nat -A PREROUTING -p ${proto} -d ${laddr} \
								--dport ${lports} -j DNAT --to ${raddr}:${rports}
			}
		done
	done

	for v in ${ALL_FORWARD_TO}
	do
		echo ${v} | {
			IFS=':' read dest target
			o_echo "  * iptables -t nat -A PREROUTING -d ${dest}  -j DNAT --to ${target}"
			[ "${_testmode}" = 0 ] && \
				${c_iptables} -t nat -A PREROUTING -d ${dest} -j DNAT --to ${target}
		}
	done
}
