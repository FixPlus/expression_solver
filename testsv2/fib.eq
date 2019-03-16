out testsv2\fib.ans;
fst = 0;
snd = 1;
iters = 10;

while(iters > 0){
	tmp = fst;
	fst = snd;
	snd = snd + tmp;
	iters = iters - 1;
}

print snd;