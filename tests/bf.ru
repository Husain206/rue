fn bf(code) {
    set tape := array(30000, 0);
    set ptr := 0;

    # prepare bracket matching
    set stack := [];
    set n := len(code);
    set matching := array(n, 0);

    for(set i := 0; i < len(code); i++){
        if(code[i] == "["){
            stack := push(stack, i);
        }
        if(code[i] == "]") {
            if(len(stack) == 0){
            brint("Error: unmatched closing bracket");
            return;
            }
            set lb := stack[len(stack)-1];  
            stack := pop(stack);
             

            matching[lb] := i;
            matching[i] := lb;
        }
    }

    if(len(stack) != 0){
    brint("Error: unmatched opening bracket");
    return;
    }
    set i := 0;
    ala(i < len(code)) {
        set c := code[i];

        if(c == ">") { ptr++; }
        if(c == "<") { ptr--; }

        if(c == "+") { tape[ptr] := tape[ptr] + 1; }
        if(c == "-") { tape[ptr] := tape[ptr] - 1; }

        if(c == ".") { 
            brint(chr(tape[ptr])); 
        }

        if(c == ",") {
            set input_val := inbut();
            if(len(input_val) > 0){
                tape[ptr] := ord(input_val[0]);
            } else {
                tape[ptr] := 0;
            }
        }

        if(c == "[") {
            if(tape[ptr] == 0) { i := matching[i]; }
        }
        if(c == "]") {
            if(tape[ptr] != 0) { i := matching[i]; }
        }

        i := i + 1;
    }
}

set hello := "++++++++[->-[->-[->-[-]<]<]<]>++++++++[<++++++++++>-]<[>+>+<<-]>-.>-----.>";
bf(hello);
