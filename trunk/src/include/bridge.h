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

bridge_dev_check() {
	# Ethernet device check
	[ ! "$($c_grep "${BRIDGE_WANDEV}:" /proc/net/dev 2> /dev/null)" ] && \
		echo $"  * BRIDGE WAN Interface name \"$BRIDGE_WANDEV\" is not found" && \
		echo $"  * BRIDGE MODE go off!" && \
		export BRIDGE_USED=0 && return 1

	[ ! "$($c_grep "${BRIDGE_LOCDEV}:" /proc/net/dev 2> /dev/null)" ] && \
		echo $"  * BRIDGE Local Interface name \"$BRIDGE_LOCDEV\" is not found" && \
		echo $"  * BRIDGE MODE go off!" && \
		export BRIDGE_USED=0 && return 1

	return 0
}

init_bridge() {
	if [ $BRIDGE_SET -eq 0 ] ;then
		o_echo $"  * BRIDGE not support on build time"
		o_echo $"    If you want use this function, isntall bridge-utils package"
		o_echo $"    and, set 1 on BRIDGE_SET default.h and, configure path of"
	    o_echo $"    c_brctl environment on command.h"
		return 1
	fi

	if [ $BRIDGE_USED -eq 0 ]; then
		o_echo $"  * BRIDGE mode don't set"
		return 1
	fi

	BRIDGE_WANIP=
	BRIDGE_WANMASK=
	BRIDGE_WANGW=$($c_route -n 2> /dev/null | $c_awk '/UG/ {print $2}')

	bridge_dev_check
	[ $? -ne 0 ] && return 1

	# bridge interface 등록
	[ ! "$($c_brctl show | $c_grep $BRIDGE_NAME)" ] && \
		o_echo "  * brctl addbr $BRIDGE_NAME" && \
		[ $_testmode -eq 0 ] && $c_brctl addbr $BRIDGE_NAME

	# bridge interface 에 wan device 등록
	[ ! "$($c_brctl show | $c_grep $BRIDGE_WANDEV)" ] && \
		o_echo "  * brctl addif $BRIDGE_NAME $BRIDGE_WANDEV" && \
		[ $_testmode -eq 0 ] && $c_brctl addif $BRIDGE_NAME $BRIDGE_WANDEV

	# bridge interface 에 local device 등록
	[ ! "$($c_brctl show | $c_grep $BRIDGE_LOCDEV)" ] && \
		o_echo "  * brctl addif $BRIDGE_NAME $BRIDGE_LOCDEV" && \
		[ $_testmode -eq 0 ] && $c_brctl addif $BRIDGE_NAME $BRIDGE_LOCDEV

	bridge_wan_info $BRIDGE_NAME

	if [ -z "$BRIDGE_WANIP" ]; then
		# Set promisc mode on real interface
		o_echo "  * ifconfig $BRIDGE_WANDEV 0.0.0.0"
		[ $_testmode -eq 0 ] && \
			$c_ifconfig $BRIDGE_WANDEV 0.0.0.0
		o_echo "  * ifconfig $BRIDGE_LOCDEV 0.0.0.0"
		[ $_testmode -eq 0 ] && \
			$c_ifconfig $BRIDGE_LOCDEV 0.0.0.0

		WordToUpper "$BRIDGE_WANDEV" UPPERNAME
		eval "BRIDGE_WANIP=\$${UPPERNAME}_IPADDR"
		eval "BRIDGE_WANMASK=\$${UPPERNAME}_SUBNET"

		# Set IP on Bridge Interface
		[ -n "$BRIDGE_WANIP" -a -n "$BRIDGE_WANMASK" ] && \
			o_echo "  * ifconfig $BRIDGE_NAME $BRIDGE_WANIP netmask $BRIDGE_WANMASK up" && \
			[ $_testmode -eq 0 ] && \
				$c_ifconfig $BRIDGE_NAME $BRIDGE_WANIP netmask $BRIDGE_WANMASK up

		# Set gateway
		_gw_chk=$($c_route -n | $c_grep UG | $c_awk '{print $2}')

		[ -n "$BRIDGE_WANGW" -a "$BRIDGE_WANGW" != "$_gw_chk" ] && \
			o_echo "  * $c_route add default gw $BRIDGE_WANGW" && \
			[ $_testmode -eq 0 ] && \
				$c_route add default gw $BRIDGE_WANGW

		bridge_wan_info $BRIDGE_NAME
	fi

	# BRIDGE 의 접속 지연 현상을 향상 시키기 위한 옵션
	o_echo "  * ${c_iptables} -A FORWARD -p tcp --tcp-flags SYN,RST SYN \\"
	o_echo "                  -j TCPMSS --clamp-mss-to-pmtu"
	[ "${_testmode}" = 0 ] && \
		${c_iptables} -A FORWARD -p tcp --tcp-flags SYN,RST SYN -j TCPMSS --clamp-mss-to-pmtu

	if [ $MASQ_USED -eq 1 ]; then
		[ -z "${MASQUERADE_WAN}" ] && MASQUERADE_WAN="$BRIDGE_WANDEV"
		[ -z "${MASQUERADE_LOC}" ] && MASQUERADE_LOC="eth1"

		[ "$MASQUERADE_LOC" = "$BRIDGE_WANDEV" ] && \
			o_echo $"  * Value of MASQ Client DEVICE can't same value of BRIDGE WAN device" && \
			o_echo $"  * MASQ mode changed off mode!" && \
			export MASQ_USED=0
		[ "$MASQUERADE_LOC" = "$BRIDGE_LOCDEV" ] && \
			o_echo $"  * Value of MASQ Client DEVICE can't same value of BRIDGE LOC device" && \
			o_echo $"  * MASQ mode changed off mode!" && \
			export MASQ_USED=0

		makeDeviceEnv $BRIDGE_NAME
	fi
}

clear_bridge() {
	[ -z "$BRIDGE_CONTROL" -o "$BRIDGE_CONTROL" = 0 ] && return

	$c_brctl show 2> /dev/null | $c_grep "$BRIDGE_NAME" >& /dev/null
	cleardev=$?
	$c_ifconfig $BRIDGE_NAME >& /dev/null
	clearifc=$?

	o_echo "  * Unset bridge setting"

	[ $clearifc -eq 0 ] && \
		[ $_testmode -eq 0 ] && $c_ifconfig $BRIDGE_NAME down >& /dev/null

	if [ $cleardev -eq 0 -a $_testmode -eq 0 ]; then
		$c_brctl delif $BRIDGE_NAME $BRIDGE_LOCDEV >& /dev/null
		$c_brctl delif $BRIDGE_NAME $BRIDGE_WANDEV >& /dev/null
		$c_brctl delbr $BRIDGE_NAME >& /dev/null
	fi

	[ $clearifc -eq 0 ] && \
		[ $_testmode -eq 0 ] && /sbin/service network restart &> /dev/null
}

bridge_wan_check() {
	[ $BRIDGE_USED -eq 0 ] && return
	if [ -z "$BRIDGE_WANDEV" -o -z "$BRIDGE_LOCDEV" ]; then
		BRIDGE_USED=0
		return
	fi

	if [ -n "$(echo "${FIREWALL_WAN}" | ${c_grep} "${BRIDGE_WANDEV}")" ]; then
		export FIREWALL_WAN=$(echo "${FIREWALL_WAN}" | ${c_sed} "s/${BRIDGE_WANDEV}/${BRIDGE_NAME}/g")
	fi
}

#
# Local variables:
# tab-width: 4
# c-basic-offset: 4
# End:
# vim: set filetype=sh noet sw=4 ts=4 fdm=marker:
# vim<600: noet sw=4 ts=4:
#
