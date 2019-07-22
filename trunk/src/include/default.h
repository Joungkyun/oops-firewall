# Default variables define
#
# $Id$
#

_ver=$(${c_cat} ${_includes}/version.h)
_gcol=50
_testmode=0
_verbose=0
_noansi=0
_iprange=0

export _ver _gcol _testmode _verbose _noansi _ftppassive _iprange

_tmpifile="/tmp/oops-firewall-tmpi.$$"
_tmpofile="/tmp/oops-firewall-tmpo.$$"
_passiveif="/tmp/oops-firewall-passivei.$$"
_passiveof="/tmp/oops-firewall-passiveo.$$"
_userport="1024:65535"
_allport="1:65535"

# network status
_nN="NEW"
_nE="ESTABLISHED"
_nR="RELATED"
_nI="INVALID"

_logformat="-m limit --limit 1/h --limit-burst 3 -j LOG"

# configration file name list
_configs="interface application filter masq forward tos bridge"

export _tmpifile _tmpofile _passiveif _passiveof
export _userport _nN _nE _nR _nI
export _logformat _configs

[ -f "${_tmpifile}" ]  && rm -rf "${_tmpifile}"  >& /dev/null
[ -f "${_tmpofile}" ]  && rm -rf "${_tmpofile}"  >& /dev/null
[ -f "${_passiveif}" ] && rm -rf "${_passiveif}" >& /dev/null
[ -f "${_passiveof}" ] && rm -rf "${_passiveof}" >& /dev/null

#
# Local variables:
# tab-width: 4
# c-basic-offset: 4
# End:
# vim: filetype=sh noet sw=4 ts=4 fdm=marker
# vim<600: noet sw=4 ts=4:
#
