# Command line variables
#
# $Id$
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
				o_echo "             -m recent --set --name OFIRE_${b_port}"
				[ ${_testmode} -eq 0 ] && \
					${c_iptables} -A ${b_dev} -p tcp --dport ${b_port} -m state --state NEW \
								-m recent --set --name OFIRE_${b_port}
				if [ $BRUTE_FORCE_LOG -eq 1 ]; then
					o_echo "    iptables -A ${b_dev} -p tcp --dport ${b_port} -m state --state NEW \\"
					o_echo "             -m recent --update --seconds ${b_sec} --hitcount ${b_hit} --rttl \\"
					o_echo "             --name OFIRE_${b_port} -j LOG --log-prefix SSH_Scan:"
					[ ${_testmode} -eq 0 ] && \
						${c_iptables} -A ${b_dev} -p tcp --dport ${b_port} -m state --state NEW \
									-m recent --update --seconds ${b_sec} --hitcount ${b_hit} --rttl \
									--name OFIRE_${b_port} -j LOG --log-prefix SSH_Scan:
				fi
				o_echo "    iptables -A ${b_dev} -p tcp --dport ${b_port} -m state --state NEW \\"
				o_echo "             -m recent --update --seconds ${b_sec} --hitcount ${b_hit} --rttl \\"
				o_echo "             --name OFIRE_${b_port} -j DROP"
				[ ${_testmode} -eq 0 ] && \
					${c_iptables} -A ${b_dev} -p tcp --dport ${b_port} -m state --state NEW \
								-m recent --update --seconds ${b_sec} --hitcount ${b_hit} --rttl \
								--name OFIRE_${b_port} -j DROP
			done
		}
		o_echo
	done
}

layer7_set() {
	[ -z "${LAYER7_FILTER}" ] && return
	
	o_echo $"  * LAYER7 Filter"

	for value in ${LAYER7_FILTER}
	do

		echo "${value}" | {
			IFS=':' read l_table l_chain l_proto l_action l_mark l_dir

			# TABLE checking
			#
			case "${l_table}" in
				"nat")    ;;
				"mangle") ;;
				*)
					if [ -n "${l_table}" ]; then
						o_echo $"     ${value} => ${l_table} is unknown table"
						continue
					fi
			esac

			[ -n "${l_table}" ] && pr_table="-t ${l_table} " || pr_table=

			# Chain Checking
			#
			if [ -z "${l_chain}" ]; then
				o_echo $"     ${value} => CHAIN value must need"
				continue;
			fi

			case "${l_table}" in
				"nat")
					case "${l_chain}" in
						"PREROUTING")  ;;
						"POSTROUTING") ;;
						"OUTPUT")      ;;
						*)
							o_echo $"     ${value} => ${l_chain} can't use with ${l_table}"
							continue
					esac
					;;
				"mangle")
					case "${l_chain}" in
						"PREROUTING")  ;;
						"POSTROUTING") ;;
						"FORWARD")     ;;
						"OUTPUT")      ;;
						"INPUT")       ;;
						*)
							o_echo $"     ${value} => ${l_chain} can't use with ${l_table}"
							continue
					esac
					;;
				*)
					case "${l_chain}" in
						"OUTPUT")  ;;
						"INPUT")   ;;
						"FORWARD") ;;
						*)
							o_echo $"     ${value} => ${l_chain} can't use with ${l_table}"
							continue
					esac
			esac

			# PROTO Checking
			#
			if [ -z "${l_proto}" ]; then
				o_echo $"     ${value} => PROTO must be setting"
				continue
			fi

			# Action Checking
			#
			[ -z "${l_action}" ] && l_action="DROP"

			case "${l_action}" in
				"DROP")   ;;
				"MARK")   ;;
				*)
					o_echo $"     ${value} => action value is used with DROP/MARK"
					continue;
			esac

			# Mark check
			#
			case "${l_action}" in
				"MARK")
					if [ -z "${l_mark}" ]; then
						o_echo $"     ${value} => action MARK is needed mark value"
						continue
					fi
					;;
				"DROP")
					[ -n "${l_mark}" ] && l_mark=
					;;
			esac

			[ -n "${l_mark}" ] && pr_mark=" --set-mark ${l_mark}" || pr_mark=
			[ -n "${l_dir}" ] && pr_dir="--l7dir ${l_dir} " || pr_dir=


				o_echo "    iptables ${pr_table}-A ${l_chain} -m layer7 --l7proto ${l_proto} \\"
				o_echo "             ${pr_dir}-j ${l_action}${pr_mark}"
				[ ${_testmode} -eq 0 ] && \
					${c_iptables} ${pr_table}-A ${l_chain} -m layer7 --l7proto ${l_proto} \
								${pr_dir}-j ${l_action}${pr_mark}

		}
		o_echo
	done
}

# 사용자 실행을 위한 함수
user_cmd () {
	case "$1" in
		pre) 
			brute_force_set
			layer7_set
			USERCHK=$(LANG="C" ${c_sed} -n -f ${_includes}/user_pre.sed ${_confdir}/user.conf)
			IFS='%'
			;;
		post)
			USERCHK=$(LANG="C" ${c_sed} -n -f ${_includes}/user_post.sed ${_confdir}/user.conf)
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
			[ $_testmode -eq 0 ] && ${c_iptables} ${uvalue} || true
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

#
# Local variables:
# tab-width: 4
# c-basic-offset: 4
# End:
# vim: set filetype=sh noet sw=4 ts=4 fdm=marker:
# vim<600: noet sw=4 ts=4:
#
