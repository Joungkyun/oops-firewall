# $Id$

:sumline
# 줄 마지막이 \ (줄넘김 문자) 일 경우 처리
/\\[ \t]*$/ {
  # 다음 줄을 패턴 스페이스에 넣는다.
  N

  # 줄넘김 문자와 두 라인간의 개행 문자를 삭제하여 한줄로 붙인다.
  s/[ \t]*\\[ \t]*\n[ \t]*/ /g

  # 반복한다.
  t sumline
}

# 주석 제거
s/#.*\|"//g

# 설정값이 아닌 것들 삭제
/^[^=]\+$/d

# 공백 라인 삭제
/^$/d

# 각 라인의 처음 공백을 제거
s/^[ \t]\+//g

# equal mark 사이의 공백 제거
s/[ \t]*=[ \t]*/="/g

# 각 라인 마지막을 처리
s/$/";/g

# 출력
p
