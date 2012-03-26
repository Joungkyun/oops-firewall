# $Id$

# removed comment
s/[[:space:]]\+#.*\|"//g

:sumline
# Case that the last character of ther line is '/'
/\\[ \t]*$/ {
	# input next line to patern space.
	N

	# removed next line comment
	s/[[:space:]]\+#.*\|"//g

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

# remove last blanks
s/[[:space:]]\+$//g

# close quote of variable's value
s/$/";/g

# print
p
