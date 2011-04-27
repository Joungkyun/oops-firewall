# Device function
#
# $Id: device.h,v 1.1 2005-12-03 19:37:28 oops Exp $
#

# ��Ʈ��ũ ����̽� (eth/ppp) ����� ������ �Լ�
# getDeviceList ��Ϻ����� üũ����̽��̸�
#
# ��� �������� ����̽� ����Ʈ�� ���� ���� �̸��� ����
# üũ ����̽��̸��� ������ ��� �ش� ����̽��� �����ϸ� 0�� ����
# �������� ������ 1�� ���������μ� ����̽� ���翩�θ� üũ�� �� ����
#
getDeviceList() {
	devVarName=$1
	devCheckVar=$2
	devGetVar=$(${c_awk} -F ':' '/(eth|ppp)[0-9]+:/ {print $1}' /proc/net/dev)

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
	parseTmpNum=$(echo ${parseDevDeviceName} | ${c_sed} 's/eth\|ppp//g')

	WordToUpper ${parseTmpName} parseTmpName

	eval "${parseDevName}=\"${parseTmpName}\""
	eval "${parseDevNum}=\"${parseTmpNum}\""
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

