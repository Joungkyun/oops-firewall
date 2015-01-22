# IPTables Modules function
#
# $Id: modules.h 360 2015-01-20 10:05:11Z oops $
#

# {{{ ins_mod
ins_mod() {
	chk=
	load=

	if [ -z "${1}" ]; then
		o_echo $"  * Load Netfilter Module"

		list=$(${c_cat} ${_confdir}/modules.list | ${c_sed} 's/#.*//g' | ${c_grep} -v "^$")

		for i in ${list}
		do
			msg=
			chk=
			load=

			mod_file=${i%%:*}
			mod_opts=${i##*:}
			mod_name=${mod_file%%.ko}
			mod_name=${mod_name%%.o}

			[ -z "${mod_file}" ] && mod_file=$i
			[ "${mod_file}" = "${mod_opts}" ] && mod_opts=

			mod_sname=$(echo ${mod_name} | ${c_sed} 's/^\(xt\|nf\|ip\)_//g')
			loads=$(${c_lsmod} | $c_grep "^\(xt\|nf\|ip\)_${mod_sname}")

			if [ -z "${loads}" ]; then
				if [ "${mod_opts}" = "unload" ]; then
					o_echo -n $"    Unload ${mod_file} module"
					print_result 1 "skip" "yellow"
					continue
				fi

				${c_modprobe} ${mod_name} ${mod_opts} > /dev/null 2>&1
				chk=$?

				if [ "${chk}" != 0 ]; then
					if [ ! -f "/lib/modules/$(${c_uname} -r)/kernel/net/netfilter/${mod_file}" ]; then
						if [ ! -f "/lib/modules/$(${c_uname} -r)/kernel/net/ipv4/netfilter/${mod_file}" ]; then
							chk=0
							msg="maybe builtin"
						fi
					fi
				fi

				o_echo -n $"    Load ${mod_name} module"
				[ -n "${mod_opts}" ] && o_echo -n " (${mod_opts})"
				print_result ${chk} "${msg}"
			else
				if [ "${mod_opts}" = "unload" ]; then
					rm_mod "${mod_file}"
					continue
				fi

				o_echo -n $"    Load ${mod_name} module"
				print_result 1 $"Already Loading"
			fi
		done
	else
		loads=$(${c_lsmod} | $c_grep "^${1}")
		if [ -z "${loads}" ]; then
			${c_modprobe} -k ${1} ${2} > /dev/null 2>&1
			chk=$?

			if [ "${chk}" != 0 ]; then
				if [ ! -f "/lib/modules/$(${c_uname} -r)/kernel/net/netfilter/${1}" ]; then
					if [ ! -f "/lib/modules/$(${c_uname} -r)/kernel/net/ipv4/netfilter/${1}" ]; then
						chk=0
						msg="maybe builtin"
					fi
				fi
			fi

			o_echo -n $"    Load ${1} module"
			[ -n "${2}" ] && o_echo -n " (${2})"
			print_result ${chk} "${msg}"
		else
			o_echo -n $"    Load ${1} module"
			print_result 1 $"Already Loading" "yellow"
		fi
	fi
	printf " \b"
}
# }}}

# {{{ rm_mod
rm_mod() {
	CHKMOD=

	if [ -z "${1}" -o "${1}" = "1" ] ; then
		modchk=$(${c_lsmod} | ${c_awk} '/^(ipt_|ip_|iptable_|xt_|nf_|x_tables)/ {print $1}' | \
				${c_grep} -v 'ip_tables\|ip_conntrack')

		if [ -z "${1}" ]; then
			o_echo
			o_echo $"  * Netfilter Module Shutdown"
		fi

		for i in ${modchk} ip_conntrack ip_tables
		do 
			${c_rmmod} ${i} 2> /dev/null
			CHKMOD=$?  
			if [ -z "${1}" ]; then
				o_echo -n $"    Remove ${i} module"
				print_result ${CHKMOD}
			fi
		done
	else
		_mod_name="${1}"
		${c_rmmod} ${_mod_name} 2> /dev/null
		CHKMOD=$? 
		o_echo -n $"    Remove ${_mod_name} module"
		print_result ${CHKMOD}
	fi
}

set_mod_parm() {
	if [ -n "${CONNTBL_MAX}" ]; then
		if [ ${__version_r} -lt 2004023000 ]; then
			echo ${CONNTBL_MAX} > /proc/sys/net/ipv4/ip_conntrack_max
		elif [ ${__version_r} -lt 2006020000 ]; then
			echo ${CONNTBL_MAX} > /proc/sys/net/ipv4/netfilter/ip_conntrack_max
		else
			echo ${CONNTBL_MAX} > /proc/sys/net/netfilter/nf_conntrack_max
		fi
	fi

	# enable modify hashsize after 2.6.14
	[ ${__version_r} -lt 2006014000 ] && return

	if [ -n "${HASHSIZE}" ]; then
		if [ ${__version_r} -lt 2006020000 ]; then
			echo ${HASHSIZE} > /sys/module/ip_conntrack/parameters/hashsize
		else
			echo ${HASHSIZE} > /sys/module/nf_conntrack/parameters/hashsize
		fi
	fi
}
# }}}

#
# Local variables:
# tab-width: 4
# c-basic-offset: 4
# End:
# vim: set filetype=sh noet sw=4 ts=4 fdm=marker:
# vim<600: noet sw=4 ts=4:
#
