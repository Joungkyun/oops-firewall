# OOPS Firewall ¿¡¼­ »ç¿ëµÇ´Â ÇÔ¼ö
#
# $Id: oops-firewall.h,v 1.3 2004-08-04 15:03:41 oops Exp $
#
# »ç¿ëÀÚ ½ÇÇàÀ» À§ÇÑ ÇÔ¼ö
user_cmd () {
  case "$*" in
    pre)
      USERCHK=$(cat ${CONFS}/user.conf | sed -n '/^%/p')
      USERMENT="USER PRE COMMAND (%) Not Config"
      IFS='%'
      ;;
    post)
      USERCHK=$(cat ${CONFS}/user.conf | sed -n '/^@/p')
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
      uvalue=$(echo ${uvalue} | sed '/\n/d' | sed -e 's/^[^ ]*iptables//g')
      echo "  * ${IPTABLES} ${uvalue}"
      ${IPTABLES} ${uvalue}
    done
  else
    IFS=' '
    case "$*" in
      pre)
        echo $"  * USER PRE COMMAND (%) Not Config"
        ;;
      post)
        echo $"  * USER POST COMMAND (@) Not Config"
        ;;
      *)
        echo $"  * Unknown parameter" > /dev/stderr
    esac
  fi
}

# $1  => RESULT
# $2  => USER COMMENT
# $3  => START COLOR
# $4  => END COLOR
print_result() {
  cend="7;0m"

  if [ "$1" = "0" ]; then
    result="OK" 
    conv_color blue
    MCOL=$?
  else
    result="Fail"
    conv_color red
    MCOL=$?
  fi

  if [ -n "$3" ]; then
    conv_color $3
    MCOL=$?
  fi

  if [ -n "$2" ]; then
    result="$2"
  fi

  cstart="1;${MCOL}m"

  echo -ne "\\033[${GCOL}G"
  echo -n ": "
  echo -ne "[${cstart}${result}[${cend}\n"
}

conv_color() {

  if [ -z "${1}" ]; then
    input=green
  else
    input=${1}
  fi

  case "${input}" in
    gray) MCOLS=30 ;;
    bgray) MCOLS=40 ;;
    red) MCOLS=31 ;;
    bred) MCOLS=41 ;;
    green) MCOLS=32 ;;
    bgreen) MCOLS=42 ;;
    yellow) MCOLS=33 ;;
    byellow) MCOLS=43 ;;
    blue) MCOLS=34 ;;
    bblue) MCOLS=44 ;;
    magenta) MCOLS=35 ;;
    bmagenta) MCOLS=45 ;;
    cyan) MCOLS=36 ;;
    bcyan) MCOLS=46 ;;
    white) MCOLS=37 ;;
    bwhite) MCOLS=47 ;;
    *) MCOLS=32
  esac

  return ${MCOLS}
}

rm_mod() {
  CHKMOD=

  if [ -z "${1}" ] || [ "${1}" = "1" ] ; then
    modchk=$(${lsmod} | ${grep} '^\(ipt_\|ip_\|iptable\)' | ${awk} '{print $1}' | ${grep} -v 'ip_tables\|ip_conntrack')

    if [ -z "${1}" ]; then
      echo
      echo $"  * Netfilter Module Shutdown"
    fi

    for i in ${modchk} ip_conntrack ip_tables
    do
      ${rmmod} ${i} 2> /dev/null
      CHKMOD=$? 
      if [ -z "${1}" ]; then
        echo -n $"  Remove ${i} module"
        print_result ${CHKMOD}
      fi
    done
  else
    ${rmmod} ${1} 2> /dev/null
    CHKMOD=$? 
    echo -n $"  Remove ${1} module"
    print_result ${CHKMOD}
  fi
}

ins_mod() {
  chk=
  load=

  if [ -z "${1}" ]; then
    echo $"  * Load Netfilter Module"

    list="ip_conntrack ip_tables ip_conntrack_ftp"

    for i in ${list}
    do
      msg=
      chk=
      load=
      loads=$(lsmod | grep "^${i}")
      if [ -z "${loads}" ]; then
        ${modprobe} -k ${i} > /dev/null 2>&1
        chk=$?

        if [ "${chk}" != 0 ]; then
          if [ ! -f "/lib/modules/$(${uname} -r)/kernel/net/ipv4/netfilter/${i}" ]; then
            chk=0
            msg="maybe builtin"
          fi
        fi

        echo -n $"  Load ${i} module"
        print_result ${chk} "${msg}"
      else
        echo -n $"  Load ${i} module"
        print_result 1 $"Already Loading"
      fi
    done
  else
    loads=$(lsmod | grep "^${1}")
    if [ -z "${loads}" ]; then
      ${modprobe} -k ${1} > /dev/null 2>&1
      chk=$?

      if [ "${chk}" != 0 ]; then
        if [ ! -f "/lib/modules/$(${uname} -r)/kernel/net/ipv4/netfilter/${1}" ]; then
          chk=0
          msg="maybe builtin"
        fi
      fi

      echo -n $"  Load ${1} module"
      print_result ${chk} "${msg}"
    else
      echo -n $"  Load ${1} module"
      print_result 1 "Already Loading" "yellow"
    fi
  fi
}
