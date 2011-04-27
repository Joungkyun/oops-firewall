# Command line variables
#
# $Id: command.h,v 1.2.2.1 2008-07-17 18:49:06 oops Exp $
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

export c_depmod c_lsmod c_rmmod c_modprobe
export c_sed c_grep c_awk c_cat c_uname c_ipcalc c_cut
export c_iptables c_ifconfig

# 사용자 실행을 위한 함수
user_cmd () {
	case "$*" in
		pre) 
			USERCHK=$(${c_sed} -n -f ${_includes}/user_pre.sed ${_confdir}/user.conf)
			USERMENT="USER PRE COMMAND (%) Not Config"
			IFS='%'
			;;
		post)
			USERCHK=$(${c_sed} -n -f ${_includes}/user_post.sed ${_confdir}/user.conf)
			USERMENT="USER POST COMMAND (@) Not Config"
			IFS='@'
			;;
	esac
		
	if [ ! -z "${USERCHK}" ]; then
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
		case "$*" in
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

iprange_set() {
	value=$1
	varname=$2

	if [ $_iprange -eq 0 ]; then
		[ -n "${varname}" ] && eval "${varname}=\"${value}\""

		#iprange_check $value
		#chk=$?
		#
		#if [ $chk -eq 1 ]; exit 1
		#   o_echo " !!!!!! IP set ${value}, but This kernel don't support iprange extension"
		#   exit 1
		#fi
		return
	fi

	primary=${value%%-*}
	secondary=${value##*-}

	[ -n "${varname}" ] && unset $varname

	if [ "${primary}" == "${secondary}" ]; then
		[ -n "${varname}" ] && eval "${varname}=\"${value}\""
		return
	fi

	dot=$(echo "${secondary}" | ${c_sed} 's/[^.]//g')

	case "$dot" in
		"...")
			prefix=
			;;
		"..")
			prefix=$(echo ${primary} | ${c_sed} 's/[0-9]\+\.[0-9]\+\.[0-9]\+$//g')
			;;
		".")
			prefix=$(echo ${primary} | ${c_sed} 's/[0-9]\+\.[0-9]\+$//g')
			;;
		*)
			prefix=$(echo ${primary} | ${c_sed} 's/[0-9]\+$//g')
	esac

	secondary="${prefix}${secondary}"

	if [ -n "${varname}" ]; then
		eval "${varname}=\"${primary}-${secondary}\""
		export ${varname}
	fi
}
