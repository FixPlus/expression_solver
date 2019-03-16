out testsv2\006.ans;
i = 0;

iters = 20;
m = 10;

while(i < iters){
	j = 0;
	while(j < iters){
		m = m + 5;
		j = j + 1;
	}
	m = m /2;
	i = i + 1;
}

print m;