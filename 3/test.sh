make &> /dev/null
if [ $? -ne 0 ]; then
    echo "COMPILATION FAILED. GG"
fi

for f in ./should_fail/*
do
    g++ -c $f &> /dev/null

    if [ $? -eq 0 ]
    then
        echo -e "test $f \e[31mCompiles, but is should not!\e[0m"
    else
        echo -e "test $f \e[32mFails as it should\e[0m"
    fi
done

for f in ./should_work/*
do
    g++ -c $f &> /dev/null

    if [ $? -ne 0 ]
    then
        echo -e "test $f \e[31mFails, but is should not!\e[0m"
    else
        echo -e "test $f \e[32mCompiles as it should\e[0m"
    fi
done
