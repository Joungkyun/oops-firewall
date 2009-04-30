# $Id: user_pre.sed,v 1.3 2009-04-30 13:33:37 oops Exp $

:sumline
# Case that the last character of ther line is '/'
/\\[ \t]*$/ {
  # input next line to patern space.
  N

  # 
  # remove '/' character and newline
  s/[ \t]*\\[ \t]*[\r\n]\+[ \t]*/ /g

  # repeat Until '/' character don't exists.
  t sumline
}

# remove first space each lines.
s/^[ \t]\+//g

# if iptables command is exists after separator,
# remove iptables command
s/%.*iptables[ \t]/%/g

# removed comment
s/[ \t]*#.*//g

# remove blank line
/^$/d

# print
/^%/p
