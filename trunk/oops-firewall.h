# OOPS Firewall ¿¡¼­ »ç¿ëµÇ´Â ÇÔ¼ö
#
# $Id: oops-firewall.h,v 1.10 2004-08-26 14:06:57 oops Exp $
#
# »ç¿ëÀÚ ½ÇÇàÀ» À§ÇÑ ÇÔ¼ö
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

  # 2.4.0 º¸´Ù ÀÛÀ¸¸é ÀÛµ¿ ¸ØÃã
  if [ ${dontpoint} -gt ${version_r} ]; then
    echo $"Enable to use over kernel 2.4.0"
    exit 1
  # 2.4.18 º¸´Ù ÀÛÀ¸¸é mangle table È®Àå »ç¿ë ¾ÈÇÔ
  elif [ ${chkpoint} -gt ${version_r} ]; then
    return 0
  else
    return 1
  fi
}

# ³×Æ®¿öÅ© µð¹ÙÀÌ½º (eth/ppp) ¸ñ·ÏÀ» ¾ò¾î¿À´Â ÇÔ¼ö
# getDeviceList ¸ñ·Ïº¯¼ö¸í Ã¼Å©µð¹ÙÀÌ½ºÀÌ¸§
#
# ¸ñ·Ï º¯¼ö¸íÀº µð¹ÙÀÌ½º ¸®½ºÆ®¸¦ °¡Áú º¯¼ö ÀÌ¸§À» ÁöÁ¤
# Ã¼Å© µð¹ÙÀÌ½ºÀÌ¸§À» ÁöÁ¤ÇÒ °æ¿ì ÇØ´ç µð¹ÙÀÌ½º°¡ Á¸ÀçÇÏ¸é 0À» ¸®ÅÏ
# Á¸ÀçÇÏÁö ¾ÊÀ¸¸é 1À» ¸®ÅÏÇÔÀ¸·Î¼­ µð¹ÙÀÌ½º Á¸Àç¿©ºÎ¸¦ Ã¼Å©ÇÒ ¼ö ÀÖÀ½
#
getDeviceList() {
  devVarName=$1
  devCheckVar=$2
  devGetVar=$(${grep} '\(eth\|ppp\)[0-9]\+:' /proc/net/dev | ${awk} -F ':' '{print $1}')

  if [ -z "${devGetVar}" ]; then
    return 1
  else
    if [ -n "${devVarName}" ]; then
      devTmpVarName="${devVarName}=\"${devGetVar}\""
      eval ${devTmpVarName}
    fi
  fi

  if [ -n "${devCheckVar}" ]; then
    devCheckVarOK=$(echo ${devGetVar} | ${grep} "${devCheckVar}")
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

  parseTmpName=$(echo ${parseDevDeviceName} | ${sed} 's/[0-9]\+//g')
  parseTmpNum=$(echo ${parseDevDeviceName} | ${sed} 's/eth\|ppp//g')

  WordToUpper ${parseTmpName} parseTmpName

  eval "${parseDevName}=\"${parseTmpName}\""
  eval "${parseDevNum}=\"${parseTmpNum}\""
}

getDeviceIP() {
  getDeviceIPDevName=$1
  getDeviceIPVarName=$2

  getDeviceIPTmp=$(${SIFCONFIG} ${getDeviceIPDevName} 2> /dev/null | \
                   ${grep} 'inet addr' | \
                   ${awk} '{print $2}' | \
                   ${sed} -e 's/.*://g')

  if [ -n "${getDeviceIPVarName}" ]; then
    getDeviceIPTmpVar="${getDeviceIPVarName}=\"${getDeviceIPTmp}\""
    eval ${getDeviceIPTmpVar}
  fi
}

getDeviceMask() {
  getDeviceMaskDevName=$1
  getDeviceMaskVarName=$2

  getDeviceMaskTmp=$(${SIFCONFIG} ${getDeviceMaskDevName} 2> /dev/null | \
                   ${grep} 'inet addr' | \
                   ${awk} '{print $4}' | \
                   ${sed} -e 's/.*://g')

  if [ -n "${getDeviceMaskVarName}" ]; then
    getDeviceMaskTmpVar="${getDeviceMaskVarName}=\"${getDeviceMaskTmp}\""
    eval ${getDeviceMaskTmpVar}
  fi
}

getDeviceNetwork() {
  getDeviceNwDevIP=$1
  getDeviceNwDevMask=$2
  getDeviceNwVarName=$3

  getDeviceNwTmp=$(${ipcalc} -s -n ${getDeviceNwDevIP} \
                                   ${getDeviceNwDevMask} | \
                   ${awk} -F '=' '{print $2}')

  if [ -n "${getDeviceNwVarName}" ]; then
    getDeviceNwTmpVar="${getDeviceNwVarName}=\"${getDeviceNwTmp}\""
    eval ${getDeviceNwTmpVar}
  fi
}

WordToUpper() {
  ORGTEXT=$1
  ORGVAR=$2

  toU=
  ORGTMP=$(echo $ORGTEXT | sed 's/\(.\)/\1 /g')

  for i in $ORGTMP
  do
    convVar=
    CharToUpper ${i} convVar
    toU="${toU}${convVar}"
  done

  if [ -n "${ORGVAR}" ]; then
    eval "${ORGVAR}=\"${toU}\""
  fi
}

WordToLower() {
  ORGTEXT=$1
  ORGVAR=$2

  toL=
  ORGTMP=$(echo $ORGTEXT | sed 's/\(.\)/\1 /g')

  for i in $ORGTMP
  do
    convVar=
    CharToLower ${i} convVar
    toL="${toL}${convVar}"
  done

  if [ -n "${ORGVAR}" ]; then
    eval "${ORGVAR}=\"${toL}\""
  fi
}

CharToUpper() {
  ORGCHAR=$1
  UPPERVAR=$2

  case $ORGCHAR in
    a) RETURNCHAR=A ;;
    b) RETURNCHAR=B ;;
    c) RETURNCHAR=C ;;
    d) RETURNCHAR=D ;;
    e) RETURNCHAR=E ;;
    f) RETURNCHAR=F ;;
    g) RETURNCHAR=G ;;
    h) RETURNCHAR=H ;;
    i) RETURNCHAR=I ;;
    j) RETURNCHAR=J ;;
    k) RETURNCHAR=K ;;
    l) RETURNCHAR=L ;;
    m) RETURNCHAR=M ;;
    n) RETURNCHAR=N ;;
    o) RETURNCHAR=O ;;
    p) RETURNCHAR=P ;;
    q) RETURNCHAR=Q ;;
    r) RETURNCHAR=R ;;
    s) RETURNCHAR=S ;;
    t) RETURNCHAR=T ;;
    u) RETURNCHAR=U ;;
    v) RETURNCHAR=V ;;
    w) RETURNCHAR=W ;;
    x) RETURNCHAR=X ;;
    y) RETURNCHAR=Y ;;
    z) RETURNCHAR=Z ;;
    *) RETURNCHAR=$ORGCHAR ;;
  esac

  if [ -n "$UPPERVAR" ]; then
    eval "${UPPERVAR}=\"${RETURNCHAR}\""
  fi
}

CharToLower() {
  ORGCHAR=$1
  LOWERVAR=$2

  case $ORGCHAR in
    A) RETURNCHAR=a ;;
    B) RETURNCHAR=b ;;
    C) RETURNCHAR=c ;;
    D) RETURNCHAR=d ;;
    E) RETURNCHAR=e ;;
    F) RETURNCHAR=f ;;
    G) RETURNCHAR=g ;;
    H) RETURNCHAR=h ;;
    I) RETURNCHAR=i ;;
    J) RETURNCHAR=j ;;
    K) RETURNCHAR=k ;;
    L) RETURNCHAR=l ;;
    M) RETURNCHAR=m ;;
    N) RETURNCHAR=n ;;
    O) RETURNCHAR=o ;;
    P) RETURNCHAR=p ;;
    Q) RETURNCHAR=q ;;
    R) RETURNCHAR=r ;;
    S) RETURNCHAR=s ;;
    T) RETURNCHAR=t ;;
    U) RETURNCHAR=u ;;
    V) RETURNCHAR=v ;;
    W) RETURNCHAR=w ;;
    X) RETURNCHAR=x ;;
    Y) RETURNCHAR=y ;;
    Z) RETURNCHAR=z ;;
    *) RETURNCHAR=$ORGCHAR ;;
  esac

  if [ -n "$LOWERVAR" ]; then
    eval "${LOWERVAR}=\"${RETURNCHAR}\""
  fi
}

masqStartCheck() {
  SCHKTMP=$1
  SCHKTMPNAME=$2

  SCHKTMP1=$(echo ${SCHKTMP} | ${awk} -F ':' '{print $1}')
  if [ "${SCHKTMP1}" = "0" ]; then
    if [ -z "${SCHKTMPNAME}" ]; then
      SCHKTMPNAME=1
    else
      eval "${SCHKTMPNAME}=1"
    fi
  fi
}

parseValue() {
  pV=$1
  pN=$2
  rV=0

  WordToLower ${pV} pV

  if [ -n "${pV}" ]; then
    case ${pV} in
      true) rV=1 ;;
      yes)  rV=1 ;;
      1)    rV=1 ;;
      *)    rV=0 ;;
    esac
  fi

  if [ -n "${pN}" ]; then
    eval "${pN}=${rV}"
  fi

  return $rV
}
