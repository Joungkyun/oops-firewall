# Command line variables
#
# $Id: command.h,v 1.6 2007-05-10 17:46:51 oops Exp $
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
	
	o_echo $"  * Brute Force Attack Filter"

	for value in ${BRUTE_FORCE_FILTER}
	do

		echo "${value}" | {
			IFS=':' read b_port b_sec b_hit

			if [ -z "${b_hit}" ]; then
				b_hit=${b_sec}
				b_sec=${b_port}
				b_port=22
			fi

			for b_dev in INPUT FORWARD
			do
				o_echo "    iptables -A ${b_dev} -p tcp --dport ${b_port} -m state --state NEW \\"
				o_echo "             -m recent --set --name SSHSCAN"
				[ ${_testmode} -eq 0 ] && \
					${c_iptables} -A ${b_dev} -p tcp --dport ${b_port} -m state --state NEW \
								-m recent --set --name SSHSCAN
				if [ $BRUTE_FORCE_LOG -eq 1 ]; then
					o_echo "    iptables -A ${b_dev} -p tcp --dport ${b_port} -m state --state NEW \\"
					o_echo "             -m recent --update --seconds ${b_sec} --hitcount ${b_hit} --rttl \\"
					o_echo "             --name SSHSCAN -j LOG --log-prefix SSH_Scan:"
					[ ${_testmode} -eq 0 ] && \
						${c_iptables} -A ${b_dev} -p tcp --dport ${b_port} -m state --state NEW \
									-m recent --update --seconds ${b_sec} --hitcount ${b_hit} --rttl \
									--name SSHSCAN -j LOG --log-prefix SSH_Scan:
				fi
				o_echo "    iptables -A ${b_dev} -p tcp --dport ${b_port} -m state --state NEW \\"
				o_echo "             -m recent --update --seconds ${b_sec} --hitcount ${b_hit} --rttl \\"
				o_echo "             --name SSHSCAN -j DROP"
				[ ${_testmode} -eq 0 ] && \
					${c_iptables} -A ${b_dev} -p tcp --dport ${b_port} -m state --state NEW \
								-m recent --update --seconds ${b_sec} --hitcount ${b_hit} --rttl \
								--name SSHSCAN -j DROP
			done
		}
		o_echo
	done
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

