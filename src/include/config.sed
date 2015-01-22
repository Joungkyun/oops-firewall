# $Id: config.sed 316 2013-01-02 21:17:37Z oops $

# removed comment
s/\([[:space:]]\+\)\?#.*\|"//g

:sumline
# Case that the last character of ther line is '/'
/\\[ \t]*$/ {
	# input next line to patern space.
	N

	# removed next line comment
	s/\([[:space:]]\+\)\?#.*\|"//g

	# remove '/' character and newline
	s/[ \t]*\\[ \t]*[\r\n]\+[ \t]*/ /g

	# repeat Until '/' character don't exists.
	t sumline
}

# remove line of direction format
/^[^=]\+$/d

# remove blank line
/^$/d

# remove first white space each lines.
s/^[ \t]\+//g

# remove white space before or after equal mark
s/[ \t]*=[ \t]*/="/g

# rutine of +=
s/+="/+=/g
s/^\([A-Z_]\+\)[ \t]*+=[ \t]*/\1="${\1} /g

# remove last blanks
s/[[:space:]]\+$//g

# close quote of variable's value
s/$/";/g

# print
p
