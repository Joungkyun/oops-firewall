# Color function
#
# $Id: color.h 302 2012-03-26 16:22:36Z oops $
#

conv_color() {
	[ -z "${1}" ] && __input="green" || __input=${1}

	case "${__input}" in
		gray) __mcols=30 ;;
		bgray) __mcols=40 ;;
		red) __mcols=31 ;;
		bred) __mcols=41 ;;
		green) __mcols=32 ;;
		bgreen) __mcols=42 ;;
		yellow) __mcols=33 ;;
		byellow) __mcols=43 ;;
		blue) __mcols=34 ;;
		bblue) __mcols=44 ;;
		magenta) __mcols=35 ;;
		bmagenta) __mcols=45 ;;
		cyan) __mcols=36 ;;
		bcyan) __mcols=46 ;;
		white) __mcols=37 ;;
		bwhite) __mcols=47 ;;
		*) __mcols=32
	esac

	return ${__mcols}
}

#
# Local variables:
# tab-width: 4
# c-basic-offset: 4
# End:
# vim: set filetype=sh noet sw=4 ts=4 fdm=marker:
# vim<600: noet sw=4 ts=4:
#
