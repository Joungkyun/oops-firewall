# Masq rule function
#
# $Id$
#

add_masq_init() {
	[ "${MASQ_USED}" = "0" ] && return 1

	ins_mod ip_nat_ftp
	o_echo

	o_echo $"  * Deprecated Direction Check"
	for i in MASQ_DEVICE MASQ_CLIENT_DEVICE
	do
		eval "masq_chk_direction=\"\$${i}\""
		[ "${i}" = "MASQ_DEVICE" ] && _alter="MASQUERADE_WAN" || _alter="MASQUERADE_LOC"
		if [ -n "${masq_chk_direction}" ]; then
			o_echo -n "    ==> "
			print_color $"${i} is Deprecated." red
			o_echo -ne $" Use ${_alter} in interface.conf\n"
			return 1
		fi
	done
	o_echo

	if [ -z "${MASQUERADE_WAN}" ]; then
		o_echo -n "  * "
		print_color "MASQUERADE_WAN"
		o_echo $" is not set"
		return 1
	else
		WordToUpper ${MASQUERADE_WAN} MASQ_DEVICE_UPPER
		eval "MASQ_IPADDR=\"\$${MASQ_DEVICE_UPPER}_IPADDR\""
	fi

	if [ -z "${MASQUERADE_LOC}" ]; then
		o_echo -n "  * "
		print_color "MASQUERADE_LOC"
		o_echo $" is not set"
		return 1
	fi

	# enabled packet forwarding
	[ -f "/proc/sys/net/ipv4/ip_forward" ] && \
		echo 1 > /proc/sys/net/ipv4/ip_forward

	return 0
}

#
# ��ü MASQ�� ���� ���θ� Ȯ��
# masqStartCheck üũ��_�����̸� [MASQ_WHOLE_ADJ]
#
masqStartCheck() {
	local VARNAME=$1
	local RETVARNAME=$2
	local cvalue

	[ -z "${VARNAME}" ] && return 0

	eval "cvalue=\"\$${VARNAME}\""

	echo ${cvalue} | ${c_grep} "^\(+\|0:\)" >& /dev/null
	[ $? -ne 0 ] && return 0

	cvalue=$(echo ${cvalue} | ${c_sed} 's/^+//g')
	eval "${VARNAME}=\"${cvalue}\""

	[ -z "${RETVARNAME}" ] && return 1 || eval "${RETVARNAME}=1"
}

#
# Bridge ����, Forwarding table���� MASQ ó���� �����ϵ��� �ϱ� ���� ��
#
bridge_masq_rule() {
	local MASQ_INTERNAL

	[ ${BRIDGE_USED} -eq 0 ] && return 0

	if [ -n "$1" ]; then
		MASQ_INTERNAL=$1
	else
		WordToUpper "${MASQUERADE_LOC}" MASQ_TMP_DEV
		eval "MASQ_INTERNAL=\"\${${MASQ_TMP_DEV}_NET}/\${${MASQ_TMP_DEV}_PREFIX}\""
	fi

	o_echo "  * MASQ configration on Bridge Service"

	o_echo "    iptables -A FORWARD -s ${MASQ_INTERNAL} -p tcp --dport 1:65535 \\"
	o_echo "                        -m state --state NEW,ESTABLISHED,RELATED -j ACCEPT"
	[ "${_testmode}" = 0 ] && \
	${c_iptables} -A FORWARD -s ${MASQ_INTERNAL} -p tcp --dport 1:65535 \
							-m state --state NEW,ESTABLISHED,RELATED -j ACCEPT

	o_echo "    iptables -A FORWARD -s ! ${MASQ_INTERNAL} -p tcp --dport 1:65535 \\"
	o_echo "                        -m state --state ESTABLISHED,RELATED -j ACCEPT"
	[ "${_testmode}" = 0 ] && \
		${c_iptables} -A FORWARD -s ! ${MASQ_INTERNAL} -p tcp --dport 1:65535 \
								-m state --state ESTABLISHED,RELATED -j ACCEPT
}

#
# divided host and port for given value
# div_host_port value d|s ret_var_name
#
div_host_port() {
	local ORGV=$1
	local LOC=$2
	local RETV=$3
	local addr
	local port

	addr=${ORGV%:*}
	port=${ORGV#*:}
	port=$(echo "${port}" | ${c_sed} 's/-/:/g')

	[ "${addr}" = "${port}" ] && port="" || port=" --${LOC}port ${port}"

	eval "${RETV}=\"${addr}${port}\""
}

add_masq_rule() {
	add_masq_init
	[ $? -ne 0 ] && return 0

	# MSSQ �ó� ���� ���� ������ ��� ��Ű�� ���� �ɼ�
	o_echo "  * iptables -t nat -A POSTROUTING -p tcp --tcp-flags SYN,RST SYN \\"
	o_echo "             -j TCPMSS --clamp-mss-to-pmtu"
	[ "${_testmode}" = 0 ] && \
		${c_iptables} -t nat -A POSTROUTING -p tcp --tcp-flags SYN,RST SYN -j TCPMSS --clamp-mss-to-pmtu

	# �ܺη� ������ ������� �����ϵ��� ��. ���� ����缳ip:������ּ�[:�������ּ�] �� ���¸�
	# ���ϸ� ����缳ip�� ���� 0 �ϰ�쿡�� everywhere �� ������.
	if [ -n "${MASQ_MATCH_START}" ]; then
		for values in ${MASQ_MATCH_START}
		do
			masqStartCheck values MASQ_WHOLE_ADJ

			echo ${values} | {
				IFS=',' read pc public dest

				div_host_port ${pc} s pc

				[ -n "${dest}" ] && div_host_port ${dest} d dest
				[ -n "${dest}" ] && dest=" -d ${dest}"

				if [ "${pc}" = "0" ]; then
					o_echo "  * iptables -t nat -A POSTROUTING -o ${MASQUERADE_WAN}${dest} -j SNAT --to ${public}"
					[ "${_testmode}" = 0 ] && \
						${c_iptables} -t nat -A POSTROUTING -o ${MASQUERADE_WAN}${dest} -j SNAT --to ${public}
					bridge_masq_rule
				else
					o_echo "  * iptables -t nat -A POSTROUTING -o ${MASQUERADE_WAN} -s ${pc}${dest} \\"
					o_echo "             -j SNAT --to ${public}"
					[ "${_testmode}" = 0 ] && \
						${c_iptables} -t nat -A POSTROUTING -o ${MASQUERADE_WAN} \
									-s ${pc}${dest} -j SNAT --to ${public}
					bridge_masq_rule ${pc}
				fi
			}
		done
	fi

	# ����� �ּ� �������� �缳 IP�� everywhere �� ���� �ȵ� ��� everywhere ��
	# masq device �� ����
	if [ "${MASQ_WHOLE_ADJ}" != "1" ]; then
		o_echo "  * iptables -t nat -A POSTROUTING -o ${MASQUERADE_WAN} \\"
		o_echo "             -j SNAT --to ${MASQ_IPADDR}"
		[ "${_testmode}" = 0 ] && \
			${c_iptables} -t nat -A POSTROUTING -o ${MASQUERADE_WAN} -j SNAT --to ${MASQ_IPADDR}

		bridge_masq_rule
	fi

	# Forwarding Rule �� ���� IP�� �缳 IP���� �� ����� �ǵ��� �缳������ ��� MASQ �� ����
	for cdv in ${MASQUERADE_LOC}
	do
		TMP_CLIENT=
		WordToUpper ${cdv} TMP_CLIENT

		PRIV_DEVICE=$cdv
		eval "PRIV_IPADDR=\"\$${TMP_CLIENT}_IPADDR\""

		if [ -n "${PRIV_IPADDR}" ]; then
			o_echo "  * iptables -t nat -A POSTROUTING -o ${PRIV_DEVICE} \\"
			o_echo "             -j SNAT --to ${PRIV_IPADDR}"
			[ "${_testmode}" = 0 ] && \
			${c_iptables} -t nat -A POSTROUTING -o ${PRIV_DEVICE} -j SNAT --to ${PRIV_IPADDR}
		fi
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