# OOPS Firewall ø°º≠ ªÁøÎµ«¥¬ «‘ºˆ
#
# $Id: oops-firewall.h,v 1.8 2004-08-18 11:05:26 oops Exp $
#
# ªÁøÎ¿⁄ Ω««‡¿ª ¿ß«— «‘ºˆ
user_cmd () {
  case "$*" in
    pre)
      USERCHK=$(${cat} ${CONFS}/user.conf | ${sed} -n '/^%/p')
      USERMENT="USER PRE COMMAND (%) Not Config"
      IFS='%'
      ;;
    post)
      USERCHK=$(${cat} ${CONFS}/user.conf | ${sed} -n '/^@/p')
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
      uvalue=$(echo ${uvalue} | ${sed} -e '/\n/d' -e 's/^[^ ]*iptables//g' -e 's/#.*//g')
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

kernelCheck() {
  dontpoint=2004000000
  chkpoint=2004018000
  version_t=$(${uname} -r | ${sed} -e 's/-.*//g' -e 's/[^0-9.]//g')
  rele_t=$(echo ${version_t} | ${awk} -F . '{print $4}')
  patch_t=$(echo ${version_t} | ${awk} -F . '{print $3}')
  minor_t=$(echo ${version_t} | ${awk} -F . '{print $2}')
  major_t=$(echo ${version_t} | ${awk} -F . '{print $1}')

  rele=${rele_t:=0}
  patch_r=${patch_t:=0}
  minor_r=${minor_t:=0}
  major_r=${major_t:=0}

  patch=$[ ${patch_t} * 1000 ]
  minor=$[ ${minor_t} * 1000000 ]
  major=$[ ${major_t} * 1000000000 ]
  version_r=$[ ${major} + ${minor} + ${patch} + ${rele} ]

  # 2.4.0 ∫∏¥Ÿ ¿€¿∏∏È ¿€µø ∏ÿ√„
  if [ ${dontpoint} -gt ${version_r} ]; then
    echo $"Enable to use over kernel 2.4.0"
    exit 1
  # 2.4.18 ∫∏¥Ÿ ¿€¿∏∏È mangle table »Æ¿Â ªÁøÎ æ»«‘
  elif [ ${chkpoint} -gt ${version_r} ]; then
    return 0
  else
    return 1
  fi
}
