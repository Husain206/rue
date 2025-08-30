fn fack(n){
	if(n <=1){ return 1; }

	return n * fack(n-1);
}

set x;
print "enter n: ";
input x;
print fack(x);
