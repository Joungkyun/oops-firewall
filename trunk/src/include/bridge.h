# Bridge rule function
#
# $Id$
#

bridge_wan_info() {
	_dev=$1

	$c_ifconfig $_dev >& /dev/null
	if [ $? -ne 0 ]; then
		BRIDGE_WANIP=
		BRIDGE_WANMASK=
		BRIDGE_WANNET=
	else
		eval `$c_ifconfig $_dev 2> /dev/null | \
			$c_awk '/inet addr/ {print $2"\"; "$4"\""}' | \
			$c_sed -e 's/addr:/BRIDGE_WANIP="/g' -e 's/Mask:/BRIDGE_WANMASK="/g'`

		getDeviceNetwork "${BRIDGE_WANIP}" "${BRIDGE_WANMASK}" BRIDGE_WANNET
		getDevicePrefix "${BRIDGE_WANIP}" "${BRIDGE_WANMASK}" BRIDGE_PREFIX
		BRIDGE_SUBNET="${BRIDGE_WANNET}/${BRIDGE_PREFIX}"
		[ -z "${BRG0_NETPX}" -o "${BRG0_NETPX}" = "/" ] && BRG0_NETPX="${BRIDGE_SUBNET}"
	fi

	export BRIDGE_WANIP BRIDGE_WANMASK BRIDGE_WANNET BRIDGE_SUBNET BRG0_NETPX
}

init_bridge() {
	if [ $BRIDGE_USED -eq 0 ]; then
		o_echo $"  * BRIDGE mode don't set"
		return 1
	fi

	${c_iptables} -m physdev --help > /dev/null
	if [ $? -ne 0 ]; then
		o_echo $"  * The version of iptables must be over 1.2.8 or"
		o_echo $"  * must support physdev extension"
		BRIDGE_USED=0
		return 1
	fi

	BRIDGE_WANIP=
	BRIDGE_WANMASK=
	BRIDGE_WANGW=$($c_route -n 2> /dev/null | $c_awk '/UG/ {print $2}')

	[ $? -ne 0 ] && return 1

	bridge_wan_info $BRIDGE_DEVNAME

	# BRIDGE 의 접속 지연 현상을 향상 시키기 위한 옵션
	o_echo "  * ${c_iptables} -A FORWARD -p tcp --tcp-flags SYN,RST SYN \\"
	o_echo "                  -j TCPMSS --clamp-mss-to-pmtu"
	[ "${_testmode}" = 0 ] && \
		${c_iptables} -A FORWARD -p tcp --tcp-flags SYN,RST SYN -j TCPMSS --clamp-mss-to-pmtu

	if [ $MASQ_USED -eq 1 ]; then
		[ -z "${MASQUERADE_WAN}" ] && MASQUERADE_WAN="$BRIDGE_DEVNAME"
		[ -z "${MASQUERADE_LOC}" ] && MASQUERADE_LOC="eth1"

		makeDeviceEnv $BRIDGE_DEVNAME
	fi
}

add_br_rule () {
	local types=$*
	local dev=
	local var=

	[ ${BRIDGE_USED} -eq 0 ] && return

	echo ${types} | ${c_grep} "_OUT_" >& /dev/null
	[ $? -eq 0 ] && dev="${BRIDGE_LANDEV}" || dev="${BRIDGE_WANDEV}"

	for i in ${types}
	do
		chk="$(echo ${i} | $c_grep TCP)"
		[ -n "${chk}" ] && proto="tcp" || proto="udp"
		[ -n "${chk}" ] && ment="TCP" || ment="UDP"

		o_echo $"  * BRIDGE ${ment} service"

		eval "var=\$${i}"

		for v in ${var}
		do
			echo ${v} | {
				IFS='|' read source dest

				srcip="${source%%:*}"
				sport="${source#*:}"
				dstip="${dest%%:*}"
				dport="${dest#*:}"

				[ "${srcip}" = "${sport}" ] && sport=""
				[ "${dstip}" = "${dport}" ] && dport=""

				[ -n "${sport}" ] && port_set s ${sport} sport sconn
				[ -n "${dport}" ] && port_set d ${dport} dport dconn

				[ -n "${dconn}" ] && constate="${dconn}"
				[ -n "${sconn}" ] && constate="${sconn}"
				[ -z "${constate}" ] && constate="-m state --state ${_nN}"

				iprange_set ${srcip} srcip
				iprange_set ${dstip} dstip

				iprange_check ${srcip}
				rangechk=$?
				if [ $rangechk -eq 0 ]; then
					iprange_check ${dstip}
					rangechk=$?
				fi

				if [ $rangechk -eq 1 ]; then
					iprange_check ${srcip}
					rangechk=$?
					[ $rangechk -eq 1 ] && srcip="--src-range ${srcip}" || srcip="-s ${srcip}"

					iprange_check ${dstip}
					rangechk=$?
					[ $rangechk -eq 1 ] && dstip="--dst-range ${dstip}" || dstip="-d ${dstip}"

					[ $_verbose -eq 1 ] && {
						o_echo "    iptables -A FORWARD -m physdev --physdev-in ${dev} -p ${proto} -m iprange \\"
						o_echo "             ${srcip}${sport} ${dstip}${dport} \\"
						o_echo "             ${constate} -j ACCEPT"
					}

					[ $_testmode -eq 0 ] && \
						${c_iptables} -A FORWARD -m physdev --physdev-in ${dev} -p ${proto} -m iprange \
									${srcip}${sport} ${dstip}${dport} \
									${constate} -j ACCEPT
				else
					[ $_verbose -eq 1 ] && {
						o_echo "    iptables -A FORWARD -m physdev --physdev-in ${dev} -p ${proto} \\"
						o_echo "             -s ${srcip}${sport} -d ${dstip}${dport} \\"
						o_echo "             ${constate} -j ACCEPT"
					}
					[ $_testmode -eq 0 ] && \
						${c_iptables} -A FORWARD -m physdev --physdev-in ${dev} -p ${proto} \
									-s ${srcip}${sport} -d ${dstip}${dport} ${constate} -j ACCEPT
				fi
			}
		done
		o_echo
	done
}

add_br_icmp() {
	local types=$*

	[ ${BRIDGE_USED} -eq 0 ] && return

	for v in ${types}
	do
		echo ${v} | ${c_grep} "PING" >& /dev/null
		[ $? -eq 0 ] && add_br_ping $v || add_br_trace $v
	done
}

add_br_ping() {
	local types=$*
	local dev=
	local var=

	[ ${BRIDGE_USED} -eq 0 ] && return

	for i in ${types}
	do
		o_echo $"    ==> for bridge ping service"

		eval "var=\$${i}"

		for v in ${var}
		do
			echo ${v} | {
				IFS='|' read srcip dstip

				iprange_set ${srcip} srcip
				iprange_set ${dstip} dstip

				iprange_check ${srcip}
				rangechk=$?
				if [ $rangechk -eq 0 ]; then
					iprange_check ${dstip}
					rangechk=$?
				fi

				if [ $rangechk -eq 1 ]; then
					iprange_check ${srcip}
					rangechk=$?
					[ $rangechk -eq 1 ] && rsrcip="--dst-range ${srcip}" || rsrcip="-d ${srcip}"
					[ $rangechk -eq 1 ] && srcip="--src-range ${srcip}" || srcip="-s ${srcip}"

					iprange_check ${dstip}
					rangechk=$?
					[ $rangechk -eq 1 ] && rdstip="--src-range ${dstip}" || rdstip="-s ${dstip}"
					[ $rangechk -eq 1 ] && dstip="--dst-range ${dstip}" || dstip="-d ${dstip}"

					[ $_verbose -eq 1 ] && {
						o_echo "      iptables -A FORWARD -p icmp --icmp-type echo-request -m iprange \\"
						o_echo "               ${srcip} ${dstip} -j ACCEPT"
						o_echo "      iptables -A FORWARD -p icmp --icmp-type echo-reply -m iprange \\"
						o_echo "               ${rdstip} ${rsrcip} -j ACCEPT"
					}

					[ $_testmode -eq 0 ] && {
						${c_iptables} -A FORWARD -p icmp --icmp-type echo-request -m iprange \
									${srcip} ${dstip} -j ACCEPT
						${c_iptables} -A FORWARD -p icmp --icmp-type echo-reply -m iprange \
									${rdstip} ${rsrcip} -j ACCEPT
					}
				else
					[ $_verbose -eq 1 ] && {
						o_echo "      iptables -A FORWARD -p icmp --icmp-type echo-request \\"
						o_echo "               -s ${srcip} -d ${dstip} -j ACCEPT"
						o_echo "      iptables -A FORWARD -p icmp --icmp-type echo-reply \\"
						o_echo "               -s ${dstip} -d ${srcip} -j ACCEPT"
					}

					[ $_testmode -eq 0 ] && {
						${c_iptables} -A FORWARD -p icmp --icmp-type echo-request \
									-s ${srcip} -d ${dstip} -j ACCEPT
						${c_iptables} -A FORWARD -p icmp --icmp-type echo-reply \
									-s ${dstip} -d ${srcip} -j ACCEPT
					}
				fi
			}
		done
		o_echo
	done
}

add_br_trace() {
	local types=$*
	local dev=
	local var=

	[ ${BRIDGE_USED} -eq 0 ] && return

	for i in ${types}
	do
		o_echo $"    ==> for bridge trace service"

		eval "var=\$${i}"

		for v in ${var}
		do
			echo ${v} | {
				IFS='|' read srcip dstip

				iprange_set ${srcip} srcip
				iprange_set ${dstip} dstip

				iprange_check ${srcip}
				rangechk=$?
				if [ $rangechk -eq 0 ]; then
					iprange_check ${dstip}
					rangechk=$?
				fi

				if [ $rangechk -eq 1 ]; then
					iprange_check ${srcip}
					rangechk=$?
					[ $rangechk -eq 1 ] && rsrcip="--dst-range ${srcip}" || rsrcip=" -d ${srcip}"
					[ $rangechk -eq 1 ] && srcip="--src-range ${srcip}" || srcip=" -s ${srcip}"

					iprange_check ${dstip}
					rangechk=$?
					[ $rangechk -eq 1 ] && rdstip="--src-range ${dstip}" || rdstip=" -s ${dstip}"
					[ $rangechk -eq 1 ] && dstip="--dst-range ${dstip}" || dstip=" -d ${dstip}"

					[ $_verbose -eq 1 ] && {
						o_echo "      iptables -A FORWARD -p icmp --icmp-type time-exceeded -m iprange \\"
						o_echo "               ${rdstip} ${rsrcip} -j ACCEPT"
						o_echo "      iptables -A FORWARD -p icmp --icmp-type port-unreachable -m iprange \\"
						o_echo "               ${rdstip} ${rsrcip} -j ACCEPT"
						# For traceroute
						o_echo "      iptables -A FORWARD -p udp -m iprange\\"
						o_echo "               ${srcip} ${dstip} --dport 33434:33525 -j ACCEPT"
						# For tracepath
						o_echo "      iptables -A FORWARD -p udp -m iprange\\"
						o_echo "               ${srcip} ${dstip} --dport 44444:44624 -j ACCEPT"
					}

					[ ${_testmode} -eq 0 ] && {
						${c_iptables} -A FORWARD -p icmp --icmp-type time-exceeded -m iprange \
									${rdstip} ${rsrcip} -j ACCEPT
						${c_iptables} -A FORWARD -p icmp --icmp-type port-unreachable -m iprange \
									${rdstip} ${rsrcip} -j ACCEPT
						${c_iptables} -A FORWARD -p udp -m iprange ${srcip} ${dstip} --dport 33434:33525 -j ACCEPT
						${c_iptables} -A FORWARD -p udp -m iprange ${srcip} ${dstip} --dport 44444:44624 -j ACCEPT
					}
				else
					[ $_verbose -eq 1 ] && {
						o_echo "      iptables -A FORWARD -p icmp --icmp-type time-exceeded \\"
						o_echo "               -s ${dstip} -d ${srcip} -j ACCEPT"
						o_echo "      iptables -A FORWARD -p icmp --icmp-type port-unreachable \\"
						o_echo "               -s ${dstip} -d ${srcip} -j ACCEPT"
						# For traceroute
						o_echo "      iptables -A FORWARD -p udp\\"
						o_echo "               -s ${srcip} -d ${dstip} --dport 33434:33525 -j ACCEPT"
						# For tracepath
						o_echo "      iptables -A FORWARD -p udp\\"
						o_echo "               -s ${srcip} -d ${dstip} --dport 44444:44624 -j ACCEPT"
					}

					[ ${_testmode} -eq 0 ] && {
						${c_iptables} -A FORWARD -p icmp --icmp-type time-exceeded \
									-s ${dstip} -d ${srcip} -j ACCEPT
						${c_iptables} -A FORWARD -p icmp --icmp-type port-unreachable \
									-s ${dstip} -d ${srcip} -j ACCEPT
						${c_iptables} -A FORWARD -p udp -s ${srcip} -d ${dstip} --dport 33434:33525 -j ACCEPT
						${c_iptables} -A FORWARD -p udp -s ${srcip} -d ${dstip} --dport 44444:44624 -j ACCEPT
					}
				fi
			}
		done
		o_echo
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
