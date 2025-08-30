// define variables
set x := 5;
set y := 10;


set msg := "Counter: ";
set flag := true;

// printing(for now)
print "Initial x = " + x;
print "Initial y = " + y;
print " ";
print msg;

// if-else
if x < y {
    print "x is less than y";
} else {
    print "x is greater or equal y";
}

// ala-loop aka(while)
ala x < 8 {
    print "x inside ala: " + x;
    ++x;
}

print " ";

// for-loop, logical and comparision operators
for(set i := 0; i <= 10; ++i){
    if(i % 2 == 0){
        print i +  ": even";
    } else {
        print i + ": odd";
    }
}

print " ";

for(set i := 0; i <= 10; ++i){
    if(!(i & 1)){
        print i +  ": even";
    } else {
        print i + ": odd";
    }
}

print " ";

set sum := x + y;
set ok := sum > 20;
print "sum = " + sum;
print "ok? " + ok;

++x;
print "x after ++x: " + x;

