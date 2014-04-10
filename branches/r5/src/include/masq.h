# Masq rule function
#
# $Id$
#

add_masq_init() {
	[ "${MASQ_USED}" = "0" ] && return 1

	o_echo
	ins_mod ip_nat_ftp
	o_echo

	WordToUpper ${MASQ_DEVICE} MASQ_DEVICE_UPPER

	eval "MASQ_IPADDR=\"\$${MASQ_DEVICE_UPPER}_IPADDR\""
  
	[ -z "${MASQ_CLIENT_DEVICE}" ] && MASQ_CLIENT_DEVICE=eth1
  
	# enabled packet forwarding
	[ -f "/proc/sys/net/ipv4/ip_forward" ] && \
		echo 1 > /proc/sys/net/ipv4/ip_forward

	return 0
}

masqStartCheck() {
  SCHKTMP=$1
  SCHKTMPNAME=$2

  SCHKTMP1=$(echo ${SCHKTMP} | ${c_awk} -F ':' '{print $1}')
  [ "${SCHKTMP1}" != "0" ] && return 0
  [ -z "${SCHKTMPNAME}" ] && SCHKTMPNAME=1 || eval "${SCHKTMPNAME}=1"
}

add_masq_rule() {

	# MSSQ 시나 접속 지연 현상을 향상 시키기 위한 옵션
	o_echo "  * ${c_iptables} -A FORWARD -p tcp --tcp-flags SYN,RST SYN -j TCPMSS --clamp-mss-to-pmtu"
	[ "${_testmode}" = 0 ] && \
		${c_iptables} -A FORWARD -p tcp --tcp-flags SYN,RST SYN -j TCPMSS --clamp-mss-to-pmtu

	add_masq_init
	[ $? -ne 0 ] && return 0

	# 외부로 나가는 출발점을 지정하도록 함. 값은 적용사설ip:출발지주소 의 형태를
	# 취하며 적용사설ip의 값이 0 일경우에는 everywhere 로 적용함.
	if [ -n "${MASQ_MATCH_START}" ]; then
		for values in ${MASQ_MATCH_START}
		do
			masqStartCheck ${values} MASQ_WHOLE_ADJ
  
			echo ${values} | {
				IFS=':' read pc output
				if [ "${pc}" = "0" ]; then
					o_echo "  * iptables -t nat -A POSTROUTING -o ${MASQ_DEVICE} -j SNAT --to ${output}"
					[ "${_testmode}" = 0 ] && \
						${c_iptables} -t nat -A POSTROUTING -o ${MASQ_DEVICE} -j SNAT --to ${output}
				else
					o_echo "  * iptables -t nat -A POSTROUTING -s ${pc} -o ${MASQ_DEVICE} -j SNAT --to ${output}"
					[ "${_testmode}" = 0 ] && \
						${c_iptables} -t nat -A POSTROUTING -s ${pc} -o ${MASQ_DEVICE} -j SNAT --to ${output}
				fi
			}
		done
	fi

	# 출발지 주소 지정에서 사설 IP가 everywhere 이 적용 안될 경우 everywhere 을
	# masq device 로 지정
	if [ "${MASQ_WHOLE_ADJ}" != "1" ]; then
		o_echo "  * iptables -t nat -A POSTROUTING -o ${MASQ_DEVICE} -j SNAT --to ${MASQ_IPADDR}"
		[ "${_testmode}" = 0 ] && \
			${c_iptables} -t nat -A POSTROUTING -o ${MASQ_DEVICE} -j SNAT --to ${MASQ_IPADDR}
	fi

	# Forwarding Rule 이 리얼 IP와 사설 IP간에 잘 통신이 되도록 사설망으로 향상 MASQ 도 설정
	for cdv in ${MASQ_CLIENT_DEVICE}
	do
		TMP_CLIENT=
		WordToUpper ${cdv} TMP_CLIENT

		PRIV_DEVICE=$cdv
		eval "PRIV_IPADDR=\"\$${TMP_CLIENT}_IPADDR\""

		if [ -n "${PRIV_IPADDR}" ]; then
			o_echo "  * iptables -t nat -A POSTROUTING -o ${PRIV_DEVICE} -j SNAT --to ${PRIV_IPADDR}"
			[ "${_testmode}" = 0 ] && \
			${c_iptables} -t nat -A POSTROUTING -o ${PRIV_DEVICE} -j SNAT --to ${PRIV_IPADDR}
		fi
	done
}
