# Check function
#
# $Id: check.h,v 1.1 2005-12-03 19:37:28 oops Exp $
#

isroot() {
	[ "${UID}" != "0" ] && return 1 || return 0
}

kernelCheck() {
	__dontpoint=2004000000
	__chkpoint=2004018000
	__version_t=$(${c_uname} -r | ${c_cut} -d"-" -f1)
	__rele_t=$(echo ${__version_t} | ${c_cut} -d. -f4)
	__patch_t=$(echo ${__version_t} | ${c_cut} -d. -f3)
	__minor_t=$(echo ${__version_t} | ${c_cut} -d. -f2)
	__major_t=$(echo ${__version_t} | ${c_cut} -d. -f1)

	__rele=${__rele_t:=0}
	__patch_r=${__patch_t:=0}
	__minor_r=${__minor_t:=0}
	__major_r=${__major_t:=0}

	__patch=$[ ${__patch_r} * 1000 ]
	__minor=$[ ${__minor_r} * 1000000 ]
	__major=$[ ${__major_r} * 1000000000 ]
	__version_r=$[ ${__major} + ${__minor} + ${__patch} + ${__rele} ]

	# 2.4.0 보다 작으면 작동 멈춤
	if [ ${__dontpoint} -gt ${__version_r} ]; then
		return 2
	# 2.4.18 보다 작으면 mangle table 확장 사용 안함
	elif [ ${__chkpoint} -gt ${__version_r} ]; then
		return 0
	else
		return 1
	fi
}
