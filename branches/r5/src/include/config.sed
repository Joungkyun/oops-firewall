# $Id$

:sumline
# �� �������� \ (�ٳѱ� ����) �� ��� ó��
/\\[ \t]*$/ {
  # ���� ���� ���� �����̽��� �ִ´�.
  N

  # �ٳѱ� ���ڿ� �� ���ΰ��� ���� ���ڸ� �����Ͽ� ���ٷ� ���δ�.
  s/[ \t]*\\[ \t]*\n[ \t]*/ /g

  # �ݺ��Ѵ�.
  t sumline
}

# �ּ� ����
s/#.*\|"//g

# �������� �ƴ� �͵� ����
/^[^=]\+$/d

# ���� ���� ����
/^$/d

# �� ������ ó�� ������ ����
s/^[ \t]\+//g

# equal mark ������ ���� ����
s/[ \t]*=[ \t]*/="/g

# �� ���� �������� ó��
s/$/";/g

# ���
p