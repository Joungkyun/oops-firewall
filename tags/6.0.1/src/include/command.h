# Command line variables
#
# $Id: command.h,v 1.4 2007-01-07 16:02:24 oops Exp $
#

# command line command
c_depmod="@depmod@"
c_lsmod="@lsmod@"
c_rmmod="@rmmod@"
c_modprobe="@modprobe@"
c_sed="@sed@"
c_grep="@grep@"
c_awk="@awk@"
c_cat="@cat@"
c_uname="@uname@"
c_ipcalc="@ipcalc@"
c_cut="@cut@"
c_getopt="@getopt@"

c_iptables="@iptables@"
c_ifconfig="@ifconfig@"

# bridge command
c_brctl="@brctl@"
c_route="@route@"

export c_depmod c_lsmod c_rmmod c_modprobe
export c_sed c_grep c_awk c_cat c_uname c_ipcalc c_cut
export c_iptables c_ifconfig c_brctl c_route

brute_force_set() {
	[ -z "${BRUTE_FORCE_FILTER}" ] && return
	
	o_echo $"  * SSH Brute Force Attack Filter"

	echo "${BRUTE_FORCE_FILTER}" | {
		IFS=':' read b_sec b_hit

		for b_dev in INPUT FORWARD
		do
			o_echo "    iptables -A ${b_dev} -p tcp --dport 22 -m state --state NEW \\"
			o_echo "             -m recent --set --name SSHSCAN"
			[ ${_testmode} -eq 0 ] && \
				${c_iptables} -A ${b_dev} -p tcp --dport 22 -m state --state NEW \
							-m recent --set --name SSHSCAN
			if [ $USE_LOG -eq 1 ]; then
				o_echo "    iptables -A ${b_dev} -p tcp --dport 22 -m state --state NEW \\"
				o_echo "             -m recent --update --seconds ${b_sec} --hitcount ${b_hit} --rttl \\"
				o_echo "             --name SSHSCAN -j LOG --log-prefix SSH_Scan:"
				[ ${_testmode} -eq 0 ] && \
					${c_iptables} -A ${b_dev} -p tcp --dport 22 -m state --state NEW \
								-m recent --update --seconds ${b_sec} --hitcount ${b_hit} --rttl \
								--name SSHSCAN -j LOG --log-prefix SSH_Scan:
			fi
			o_echo "    iptables -A ${b_dev} -p tcp --dport 22 -m state --state NEW \\"
			o_echo "             -m recent --update --seconds ${b_sec} --hitcount ${b_hit} --rttl \\"
			o_echo "             --name SSHSCAN -j DROP"
			[ ${_testmode} -eq 0 ] && \
				${c_iptables} -A ${b_dev} -p tcp --dport 22 -m state --state NEW \
							-m recent --update --seconds ${b_sec} --hitcount ${b_hit} --rttl \
							--name SSHSCAN -j DROP
		done
	}
	o_echo
}


# 사용자 실행을 위한 함수
user_cmd () {
	case "$1" in
		pre) 
			brute_force_set
			USERCHK=$(${c_sed} -n -f ${_includes}/user_pre.sed ${_confdir}/user.conf)
			IFS='%'
			;;
		post)
			USERCHK=$(${c_sed} -n -f ${_includes}/user_post.sed ${_confdir}/user.conf)
			IFS='@'
			;;
	esac
		
	if [ -n "${USERCHK}" ]; then
		for uvalue in ${USERCHK}
		do 
			IFS=' '
			if [ -z "${uvalue}" ]; then
				continue;
			fi
			uvalue=$(echo ${uvalue})
			o_echo "  * ${c_iptables} ${uvalue}"
			[ $_testmode -eq 0 ] && ${c_iptables} ${uvalue}
		done
	else 
		IFS=' '
		case "$1" in
			pre)
				o_echo $"  * USER PRE COMMAND (%) Not Config"
				;;
			post)
				o_echo $"  * USER POST COMMAND (@) Not Config"
				;;
			*)
				o_echo $"  * Unknown parameter" > /dev/stderr
		esac
	fi
}

