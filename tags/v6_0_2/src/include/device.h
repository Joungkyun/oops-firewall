# Device function
#
# $Id: device.h,v 1.2 2006-12-29 05:45:17 oops Exp $
#

# 네트워크 디바이스 (eth/ppp/bridge) 목록을 얻어오는 함수
# getDeviceList 목록변수명 체크디바이스이름
#
# 목록 변수명은 디바이스 리스트를 가질 변수 이름을 지정
# 체크 디바이스이름을 지정할 경우 해당 디바이스가 존재하면 0을 리턴
# 존재하지 않으면 1을 리턴함으로서 디바이스 존재여부를 체크할 수 있음
#
getDeviceList() {
	devVarName=$1
	devCheckVar=$2
	devGetVar=$(${c_awk} -F ':' "/(eth|ppp|${BRIDGE_NAME}|bond)[0-9]*:/ {print \$1}" /proc/net/dev)

	if [ -z "${devGetVar}" ]; then
		return 1
	else
		if [ -n "${devVarName}" ]; then
			devTmpVarName="${devVarName}=\"${devGetVar}\""
			eval ${devTmpVarName}
		fi
	fi

	if [ -n "${devCheckVar}" ]; then
		devCheckVarOK=$(echo ${devGetVar} | ${c_grep} "${devCheckVar}")
		if [ -n "${devCheckVarOK}" ]; then
			return 0
		else
			return 1
		fi
	fi

	return 0
}

getDeviceCheck() {
	for i in $*
	do
		gDC=0
		for j in ${devlist}
		do
			if [ "${i}" = "${j}" ]; then
				gDC=1
				break
			fi
		done

		[ $gDC -eq 1 ] && return 0
	done

	return 1
}

makeDeviceEnv() {
	devEnvDevName=$1

	devEnvVar_NAME=
	devEnvVar_NUMBER=
	parseDevice "${devEnvDevName}" devEnvVar_NAME devEnvVar_NUMBER

	getDeviceIP "${devEnvDevName}" devEnvVar_IP
	getDeviceMask "${devEnvDevName}" devEnvVar_MASK
	getDeviceNetwork "${devEnvVar_IP}" "${devEnvVar_MASK}" devEnvVar_NW

	devTmpVar="${devEnvVar_NAME}${devEnvVar_NUMBER}"
	devTmpIP="${devTmpVar}_IPADDR=${devEnvVar_IP}"
	devTmpSM="${devTmpVar}_SUBNET=${devEnvVar_MASK}"
	devTmpNW="${devTmpVar}_NET=${devEnvVar_NW}"
	eval ${devTmpIP}
	eval ${devTmpSM}
	eval ${devTmpNW}
}

parseDevice() {
	parseDevDeviceName=$1
	parseDevName=$2
	parseDevNum=$3

	parseTmpName=$(echo ${parseDevDeviceName} | ${c_sed} 's/[0-9]\+//g')
	parseTmpNum=$(echo ${parseDevDeviceName} | ${c_sed} 's/eth\|ppp\|bond//g')

	WordToUpper ${parseTmpName} parseTmpName

	eval "${parseDevName}=\"${parseTmpName}\""
	if [ "$parseTmpName" = "$parseTmpNum" ]; then
		eval "${parseDevNum}=\"\""
	else
		eval "${parseDevNum}=\"${parseTmpNum}\""
	fi
}

getDeviceIP() {
	getDeviceIPDevName=$1
	getDeviceIPVarName=$2

	getDeviceIPTmp=$(${c_ifconfig} ${getDeviceIPDevName} 2> /dev/null | \
					${c_awk} -F : '/inet addr:/ {print $2}' | \
					${c_sed} 's/[ ]\+.*//g')

	if [ -n "${getDeviceIPVarName}" ]; then
		getDeviceIPTmpVar="${getDeviceIPVarName}=\"${getDeviceIPTmp}\""
		eval ${getDeviceIPTmpVar}
	fi
}

getDeviceMask() {
	getDeviceMaskDevName=$1
	getDeviceMaskVarName=$2

	getDeviceMaskTmp=$(${c_ifconfig} ${getDeviceMaskDevName} 2> /dev/null | \
					${c_awk} -F : '/inet addr:/ {print $4}' | \
					${c_sed} 's/[ ]\+.*//g')

	if [ -n "${getDeviceMaskVarName}" ]; then
		getDeviceMaskTmpVar="${getDeviceMaskVarName}=\"${getDeviceMaskTmp}\""
		eval ${getDeviceMaskTmpVar}
	fi
}

getDeviceNetwork() {
	getDeviceNwDevIP=$1
	getDeviceNwDevMask=$2
	getDeviceNwVarName=$3

	getDeviceNwTmp=$(${c_ipcalc} -s -n \
					${getDeviceNwDevIP} \
					${getDeviceNwDevMask} | \
					${c_awk} -F '=' '{print $2}')

	if [ -n "${getDeviceNwVarName}" ]; then
		getDeviceNwTmpVar="${getDeviceNwVarName}=\"${getDeviceNwTmp}\""
		eval ${getDeviceNwTmpVar}
	fi
}

initInterfaceList() {
	intName=$1
	shift
	intList=$*
	TSTR=

	for i in $intList
	do
		WordToUpper $i TMPSTR
		TSTR="${TSTR} ${TMPSTR}"
	done
	TSTR=$(echo ${TSTR} | ${c_sed} 's/^[ ]\+\|[ ]\+$//g')

	eval "$intName=\"${TSTR}\""
}

firewall_wan_check() {
	[ -n "${FIREWALL_WAN}" ] && return

	_donelist=
	for i in BRIDGE_WAN MASQ_DEVICE FORWARD_MASTER
	do
		eval "chkinter=\$${i}"
		[ -z "${chkinter}" ] && continue
		[ -n "$(echo ${_donelist} | ${c_grep} "-${chkinter}-")" ] && continue

		FILEWALL_WAN="${FIREWALL_WAN} ${chkinter}"
		_donelist="${_donelist} -${chkinter}-"
	done

	[ -z "${FIREWALL_WAN}" ] && FIREWALL_WAN="eth0"
	FIREWALL_WAN=$(echo ${FIREWALL_WAN} | ${c_sed} 's/^[ ]\+\|[ ]\+$//g')

	export FIREWALL_WAN
}
