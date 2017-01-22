for a in $@
do
  c2ffi $INCLUDE_PATH/$a.h | ./dale-autowrap.pl $a > $a.dt
done

