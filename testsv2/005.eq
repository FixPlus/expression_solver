out testsv2\005.ans;
num1 = 2;
num2 = num1*(1 + 2*(1 && (2 > 0)));
if(num2 == 3*num1){
	num1 = num2/3 - num1;
}
else{
	num1 = num2 - num1;
}

if(num1 == 0){
	print num1; print num2;
}
else{
	print num2; print num1;
}
