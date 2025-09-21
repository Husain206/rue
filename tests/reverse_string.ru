fn reverse(s) {
    set result := "";
    for(set i := len(s)-1; i >= 0; i := i - 1) {
        result := result + s[i];
    }
    return result;
}

fn main(args){
    brint(reverse("hello"));
}

