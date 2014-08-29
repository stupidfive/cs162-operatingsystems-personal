fail=0
if ./main | grep "[1-9]"
then
    echo "Main passed"
else
    echo "Main Failed"
    fail=1
fi

if ./wc wc | grep "[1-9]"
then
    echo "WC passed"
else
    echo "WC Failed"
    fail=1
fi

exit $fail
