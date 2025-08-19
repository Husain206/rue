set a = 5;
set b = 9;

// it outputs the result in decimal(for now) instead of binary, but the value is correct :D

    // The result is 00000001
    print "a&b = " + a & b;

    // The result is 00001101
    print "a|b = " + a | b;

    // The result is 00001100
    print "a^b = " + a ^ b;

    // The result is 11111111111111111111111111111010
    // (assuming 32-bit unsigned int) -> cpp shit :D
       print "~a = " + ~a;
    
    // The result is 00010010
    print "b<<1 = " + b << 1;

    // The result is 00000100
    print "b>>1 = " + b >> 1;
