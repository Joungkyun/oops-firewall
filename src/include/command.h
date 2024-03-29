# Command line variables
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
c_sort="@sort@"

c_iptables="@iptables@"
c_ifconfig="@ifconfig@"

# bridge command
c_route="@route@"

export c_depmod c_lsmod c_rmmod c_modprobe
export c_sed c_grep c_awk c_cat c_uname c_ipcalc c_cut
export c_iptables c_ifconfig c_route c_sort

brute_force_set() {
	local value

	[ -n "${BRUTE_FORCE_FILTER}" ] && \
		o_echo $"  * Brute Force Attack Filter"

	for value in ${BRUTE_FORCE_FILTER}
	do
		echo "${value}" | {
			IFS=':' read b_port b_sec b_hit

			o_echo "    iptables -A INPUT -p tcp --dport ${b_port} -m state --state NEW \\"
			o_echo "             -m recent --set --name OFIRE_${b_port}"
			[ ${_testmode} -eq 0 ] && \
				${c_iptables} -A INPUT -p tcp --dport ${b_port} -m state --state NEW \
							-m recent --set --name OFIRE_${b_port}
			if [ $BRUTE_FORCE_LOG -eq 1 ]; then
				o_echo "    iptables -A INPUT -p tcp --dport ${b_port} -m state --state NEW \\"
				o_echo "             -m recent --update --seconds ${b_sec} --hitcount ${b_hit} --rttl \\"
				o_echo "             --name OFIRE_${b_port} -j LOG --log-prefix SSH_Scan:"
				[ ${_testmode} -eq 0 ] && \
					${c_iptables} -A INPUT -p tcp --dport ${b_port} -m state --state NEW \
								-m recent --update --seconds ${b_sec} --hitcount ${b_hit} --rttl \
								--name OFIRE_${b_port} -j LOG --log-prefix SSH_Scan:
			fi
			o_echo "    iptables -A INPUT -p tcp --dport ${b_port} -m state --state NEW \\"
			o_echo "             -m recent --update --seconds ${b_sec} --hitcount ${b_hit} --rttl \\"
			o_echo "             --name OFIRE_${b_port} -j DROP"
			[ ${_testmode} -eq 0 ] && \
				${c_iptables} -A INPUT -p tcp --dport ${b_port} -m state --state NEW \
							-m recent --update --seconds ${b_sec} --hitcount ${b_hit} --rttl \
							--name OFIRE_${b_port} -j DROP
		}
		o_echo
	done

	#
	# BRIDGE
	#
	[ $BRIDGE_USED -eq 0 ] && return

	[ -n "${BRUTE_FORCE_FILTER}" ] && \
		o_echo $"  * Bridge Brute Force Attack Filter"

	for value in ${BR_BRUTE_FORCE_FILTER}
	do
		echo "${value}" | {
			IFS='|' read src dest portinfo

			iprange_set "${src}" src
			iprange_check "${src}"
			range_chk1=$?

			iprange_set "${dest}" dest
			iprange_check "${dest}"
			range_chk2=$?

			[ ${range_chk1} -eq 1 ] && srcrule="--src-range ${src}" || srcrule="-s ${src}"
			[ ${range_chk2} -eq 1 ] && dstrule="--dst-range ${dst}" || dstrule="-d ${dest}"
			[ ${range_chk1} -eq 1 -o ${range_chk2} -eq 1 ] && rangemod=" -m iprange"

			echo "${portinfo}" | {
				IFS=':' read b_port b_sec b_hit

				o_echo "    iptables -A FORWARD -p tcp${rangemod} \\"
				o_echo "             ${srcrule} ${dstrule} \\"
				o_echo "             --dport ${b_port} -m state --state NEW \\"
				o_echo "             -m recent --set --name OFIRE_${b_port}"
				[ ${_testmode} -eq 0 ] && \
					${c_iptables} -A FORWARD -p tcp${rangemod} \
								${srcrule} ${dstrule} \
								--dport ${b_port} -m state --state NEW \
								-m recent --set --name OFIRE_${b_port}
				if [ $BRUTE_FORCE_LOG -eq 1 ]; then
					o_echo "    iptables -A FORWARD -p tcp${rangemod} \\"
					o_echo "             ${srcrule} ${dstrule} \\"
					o_echo "             --dport ${b_port} -m state --state NEW \\"
					o_echo "             -m recent --update --seconds ${b_sec} --hitcount ${b_hit} --rttl \\"
					o_echo "             --name OFIRE_${b_port} -j LOG --log-prefix SSH_Scan:"
					[ ${_testmode} -eq 0 ] && \
						${c_iptables} -A FORWARD -p tcp${rangemod} \
									${srcrule} ${dstrule} \
					   				--dport ${b_port} -m state --state NEW \
									-m recent --update --seconds ${b_sec} --hitcount ${b_hit} --rttl \
									--name OFIRE_${b_port} -j LOG --log-prefix SSH_Scan:
				fi
				o_echo "    iptables -A FORWARD -p tcp${rangemod} \\"
				o_echo "             ${srcrule} ${dstrule} \\"
				o_echo "             --dport ${b_port} -m state --state NEW \\"
				o_echo "             -m recent --update --seconds ${b_sec} --hitcount ${b_hit} --rttl \\"
				o_echo "             --name OFIRE_${b_port} -j DROP"
				[ ${_testmode} -eq 0 ] && \
					${c_iptables} -A FORWARD -p tcp${rangemod} \
								${srcrule} ${dstrule} \
								--dport ${b_port} -m state --state NEW \
								-m recent --update --seconds ${b_sec} --hitcount ${b_hit} --rttl \
								--name OFIRE_${b_port} -j DROP
			}
		}
		o_echo
	done
}

layer7_set() {
	[ -n "${LAYER7_FILTER}" ] && \
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

	#
	# BRIDGE
	#
	[ $BRIDGE_USED -eq 0 ] && return

	[ -n "${BR_LAYER7_FILTER}" ] && \
		o_echo $"  * Bridge LAYER7 Filter"

	for value in ${BR_LAYER7_FILTER}
	do

		echo "${value}" | {
			IFS='|' read src dest layerinfo

			iprange_set "${src}" src
			iprange_check "${src}"
			range_chk1=$?

			iprange_set "${dest}" dest
			iprange_check "${dest}"
			range_chk2=$?

			[ ${range_chk1} -eq 1 ] && srcrule="--src-range ${src}" || srcrule="-s ${src}"
			[ ${range_chk2} -eq 1 ] && dstrule="--dst-range ${dst}" || dstrule="-d ${dest}"
			[ ${range_chk1} -eq 1 -o ${range_chk2} -eq 1 ] && rangemod=" -m iprange" || rangemod=

			echo "${layerinfo}" | {
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


					o_echo "    iptables ${pr_table}-A ${l_chain}${rangemod} \\"
					o_echo "             ${srcrule} ${dstrule} \\"
					o_echo "             -m layer7 --l7proto ${l_proto} \\"
					o_echo "             ${pr_dir}-j ${l_action}${pr_mark}"
					[ ${_testmode} -eq 0 ] && \
						${c_iptables} ${pr_table}-A ${l_chain}${rangemod} \
									${srcrule} ${dstrule} \
									-m layer7 --l7proto ${l_proto} \
									${pr_dir}-j ${l_action}${pr_mark}

			}
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

anywhere_set() {
	local var=$1
	local varname=$2

	[ "${var}" != "0/0" -a "${var}" != "anywhere" -a "${var}" != "ANYWHERE" ] && return
	[ -z "${varname}" ] && return

	eval "${varname}=\"0.0.0.0/0\""
}

# Usage:
#        10.10.10.10-20          => 10.10.10.10-10.10.10.20
#        10.10.10.10-15.20       => 10.10.10.10-10.10.15.20
#        10.10.10.10-11.15.20    => 10.10.10.10-10.11.15.20
#        10.10.10.10-11.11.15.20 => 10.10.10.10-11.11.15.20
#
iprange_set() {
	local value=$1
	local varname=$2

	if [ $_iprange -eq 0 ]; then
		anywhere_set "${value}" value
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

	anywhere_set "${primary}" primary
	anywhere_set "${secondary}" secondary

	[ -n "${varname}" ] && unset $varname

	if [ "${primary}" == "${secondary}" ]; then
		[ -n "${varname}" ] && eval "${varname}=\"${primary}\""
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

port_set() {
	local mode=$1
	local val=$2
	local var=$3
	local cvar=$4

	local _port
	local _conn
	local constate="-m state --state"

	_port=${val%%:*}
	_conn=${val##*:}

	[ "${_port}" = "${_conn}" ] && _conn=

	_port=$(echo "${_port}" | ${c_sed} -e 's/-/:/g')

	if [ -n "${var}" ]; then
		[ "${mode}" = "" ] && \
			eval "${var}=\"${_port}\"" || \
			eval "${var}=\" --${mode}port ${_port}\""
		export $var
	fi

	if [ -n "${cvar}" -a -n "${_conn}" ]; then
		eval "${cvar}=\"-m state --state ${_conn}\""
		export $cvar
	fi
}

#
# Local variables:
# tab-width: 4
# c-basic-offset: 4
# End:
# vim: filetype=sh noet sw=4 ts=4 fdm=marker
# vim<600: noet sw=4 ts=4:
#
