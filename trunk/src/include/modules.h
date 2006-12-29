# IPTables Modules function
#
# $Id: modules.h,v 1.2 2006-12-29 05:45:17 oops Exp $
#

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
			loads=$(${c_lsmod} | $c_grep "^${i}")

			if [ -z "${loads}" ]; then
				${c_modprobe} -k ${i} > /dev/null 2>&1
				chk=$?

				if [ "${chk}" != 0 ]; then
					if [ ! -f "/lib/modules/$(${c_uname} -r)/kernel/net/ipv4/netfilter/${i}" ]; then
						chk=0
						msg="maybe builtin"
					fi
				fi

				o_echo -n $"    Load ${i} module"
				print_result ${chk} "${msg}"
			else
				o_echo -n $"    Load ${i} module"
				print_result 1 $"Already Loading"
			fi
		done
	else
		loads=$(${c_lsmod} | $c_grep "^${1}")
		if [ -z "${loads}" ]; then
			${c_modprobe} -k ${1} > /dev/null 2>&1
			chk=$?

			if [ "${chk}" != 0 ]; then
				if [ ! -f "/lib/modules/$(${c_uname} -r)/kernel/net/ipv4/netfilter/${1}" ]; then
					chk=0
					msg="maybe builtin"
				fi
			fi

			o_echo -n $"    Load ${1} module"
			print_result ${chk} "${msg}"
		else
			o_echo -n $"    Load ${1} module"
			print_result 1 "Already Loading" "yellow"
		fi
	fi
}

rm_mod() {
	CHKMOD=
    
	if [ -z "${1}" -o "${1}" = "1" ] ; then
		modchk=$(${c_lsmod} | ${c_awk} '/^(ipt_|ip_|iptable)/ {print $1}' | \
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
		${c_rmmod} ${1} 2> /dev/null
		CHKMOD=$? 
		o_echo -n $"    Remove ${1} module"
		print_result ${CHKMOD}
	fi
}
