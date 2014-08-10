number=0

run_suite() {
	local exit_code=$1

	if [ $exit_code -eq 0 ]; then
		local test_suite=(`grep 'TEST_SUCCESS' compile.cpp | awk '{print $2}'`)
	else
		local test_suite=(`grep 'TEST_FAIL'    compile.cpp | awk '{print $2}'`)
	fi
	for test in "${test_suite[@]}"; do
		echo $test >> output_compile.log 2>&1
		$CXX compile.cpp -std=c++11 -lm -lstdc++ -D"$test" >> output_compile.log 2>&1
		if [ $? -eq "$exit_code" ]; then
			echo -ne "\e[1;32m=\e[0m"
		else
			echo -e "\e[1;31m="
			echo -e "Fail\t$test\e[0m"
			cat output_compile.log
		    exit 1
		fi
		let number=number+1
	done
}
echo "" > output_compile.log
run_suite 0
let success=number
run_suite 1
let fail=number-success

echo ""
echo -e "\e[1;32mAll tests passed\e[0m ($success TEST_SUCCESS and $fail TEST_FAIL)"