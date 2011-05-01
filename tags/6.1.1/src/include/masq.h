# Masq rule function
#
# $Id: masq.h,v 1.2 2006-12-29 05:45:17 oops Exp $
#

add_masq_init() {
	[ "${MASQ_USED}" = "0" ] && return 1

	o_echo
	ins_mod ip_nat_ftp
	o_echo

	o_echo $"    Deprecated Direction Check"
	for i in MASQ_DEVICE MASQ_CLIENT_DEVICE
	do
		eval "masq_chk_direction=\$${i}"
		[ "${i}" = "MASQ_DEVICE" ] && _alter="MASQUERADE_WAN" || _alter="MASQUERADE_LOC"
		if [ -n "${masq_chk_direction}" ]; then
			o_echo -n "    * "
			print_color $"${i} is Deprecated." red
			o_echo -ne " Use ${_alter} in interface.conf\n"
		fi
	done
	o_echo

	if [ -z "${MASQUERADE_WAN}" ]; then
		WordToUpper ${MASQ_DEVICE} MASQ_DEVICE_UPPER
		eval "MASQ_IPADDR=\"\$${MASQ_DEVICE_UPPER}_IPADDR\""
		export MASQUERADE_WAN=${MASQ_DEVICE}
	else
		WordToUpper ${MASQUERADE_WAN} MASQ_DEVICE_UPPER
		eval "MASQ_IPADDR=\"\$${MASQ_DEVICE_UPPER}_IPADDR\""
	fi
  
	if [ -z "${MASQUERADE_LOC}" ]; then
		if [ -z "${MASQ_CLIENT_DEVICE}" ]; then
			export MASQUERADE_LOC=eth1
		else
			export MASQUERADE_LOC=${MASQ_CLIENT_DEVICE}
		fi
	fi
  
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
	add_masq_init
	[ $? -ne 0 ] && return 0

	# MSSQ �ó� ���� ���� ������ ��� ��Ű�� ���� �ɼ�
	o_echo "  * iptables -t nat -A POSTROUTING -p tcp --tcp-flags SYN,RST SYN \\"
	o_echo "             -j TCPMSS --clamp-mss-to-pmtu"
	[ "${_testmode}" = 0 ] && \
		${c_iptables} -t nat -A POSTROUTING -p tcp --tcp-flags SYN,RST SYN -j TCPMSS --clamp-mss-to-pmtu

	# �ܺη� ������ ������� �����ϵ��� ��. ���� ����缳ip:������ּ� �� ���¸�
	# ���ϸ� ����缳ip�� ���� 0 �ϰ�쿡�� everywhere �� ������.
	if [ -n "${MASQ_MATCH_START}" ]; then
		for values in ${MASQ_MATCH_START}
		do
			masqStartCheck ${values} MASQ_WHOLE_ADJ
  
			echo ${values} | {
				IFS=':' read pc output
				if [ "${pc}" = "0" ]; then
					o_echo "  * iptables -t nat -A POSTROUTING -o ${MASQUERADE_WAN} -j SNAT --to ${output}"
					[ "${_testmode}" = 0 ] && \
						${c_iptables} -t nat -A POSTROUTING -o ${MASQUERADE_WAN} -j SNAT --to ${output}
				else
					o_echo "  * iptables -t nat -A POSTROUTING -s ${pc} -o ${MASQUERADE_WAN} \\"
					o_echo "             -j SNAT --to ${output}"
					[ "${_testmode}" = 0 ] && \
						${c_iptables} -t nat -A POSTROUTING -s ${pc} -o ${MASQUERADE_WAN} -j SNAT --to ${output}
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
	fi

	# Forwarding Rule �� ���� IP�� �缳 IP���� �� ����� �ǵ��� �缳������ ��� MASQ �� ����
	for cdv in ${MASQ_CLIENT_DEVICE}
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